#!/usr/bin/node

var express = require('express');
var path = require('path');
var child_process = require('child_process');
var fs = require('fs');

var app = express();


var cp = child_process.spawn('./backend/Debug/internet_engineering', {
  stdio: ['pipe', 'pipe', process.stderr]
});
var buf = "";
cp.stdout.on('data', function (data) {
  var str = data.toString('utf-8')
  var i = str.indexOf('\n');
  if (i >= 0) {
    outFrame(buf + str.substr(0, i));
    buf = str.substr(i + 1);
  } else {
    buf += str;
  }
});

var lastFrame = '{}';
var onOutFrameCallbacks = [];
var outFrame = function (str) {
  lastFrame = JSON.parse(str);
  onOutFrameCallbacks.forEach(function (cb) {
    cb(lastFrame);
  });
  onOutFrameCallbacks = [];
};

app.post('/message', function (req, res) {
  req.on('data', function (data) {
    cp.stdin.write(data);
  });
  req.on('end', function () {
    cp.stdin.write("\r\n");
  });
  //cp.stdin.write(JSON.stringify(res.body) + "\r\n");
  //cp.stdin.write(JSON.stringify({type: "update", nodes: []}) + "\r\n");
  //cp.stdin.write(JSON.stringify({type: "listRoutes"}) + "\r\n");
  onOutFrameCallbacks.push(function (frame) {
    res.json(frame);
  });
});
app.use('/', express.static(__dirname));

app.listen(3333);
