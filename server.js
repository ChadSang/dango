#!/usr/bin/node

var express = require('express');
var path = require('path');
var child_process = require('child_process');
var fs = require('fs');

var app = express();

app.get('/ep', function (req, res) {
  var cp = child_process.spawn('./backend/main');
  cp.stdout.on('data', function (data) {
    res.write(data.toString('utf-8'));
  });
  cp.stdout.on('end', function () {
    res.end();
  });
  cp.on('close', function (code) {
    if (code !== 0) {
      console.log('main process exited with code ' + code);
    }
    res.end();
  });
  cp.stdin.write(JSON.stringify({
    type: 'init', channelCount: 2, maxDistance: 10,
    nodes: [
      {"uuid": "uuid-01", "pos" : [0, 0], "speed" : [0, 0]},
      {"uuid": "uuid-02", "pos" : [0, 0], "speed" : [1, 0]},
      {"uuid": "uuid-03", "pos" : [0, 0], "speed" : [4, 0]},
      {"uuid": "uuid-04", "pos" : [0, 0], "speed" : [5, 0]},
      {"uuid": "uuid-05", "pos" : [9, 0], "speed" : [5, 0]},
      {"uuid": "uuid-06", "pos" : [18, 0], "speed" : [5, 0]},
    ],
  }) + "\r\n");
  cp.stdin.write(JSON.stringify({type: "update", nodes: []}) + "\r\n");
  cp.stdin.write(JSON.stringify({type: "listRoutes"}) + "\r\n");
  cp.stdin.end();
});

app.listen(3333);
