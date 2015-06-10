#!/usr/bin/node

var path = require('path');
var child_process = require('child_process');
var fs = require('fs');
var glob = require('glob');
var chalk = require('chalk');
Object.getPrototypeOf(chalk.blue).write = function (str) {
  arguments[0] = this(str);
  console.log.apply(console, arguments);
}

var runTest = function (test, cb) {
  var cp = child_process.spawn('./backend/main');
  var frame = 0;
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
  cp.on('close', function (code) {
    if (code !== 0) {
      return cb(new Error("Main process exited with code " + code));
    } else if (frame === test.frames.length) {
      return cb(null, frame);
    } else {
      return cb(new Error("Missing Frame " + frame + "!"));
    }
  });
  var matches = function (obj, cond) {
    if (cond === Array) {
      return Array.isArray(obj);
    } else if (Array.isArray(cond)) {
      if (!Array.isArray(obj)) return false;
      if (obj.length !== cond.length) return false;
      for (var i = 0; i < cond.length; i++) {
        if (!matches(obj[i], cond[i])) return false;
      }
      return true;
    } else if (typeof cond === 'object') {
      if (typeof cond !== 'object') return false;
      for (var key in cond) {
        if (!matches(obj[key], cond[key])) return false;
      }
      return true;
    } else {
      return obj === cond;
    }
  };
  var outFrame = function (str) {
    try {
      var data = JSON.parse(str);
    } catch (_) {
      chalk.red.write("%d %s", frame, str);
      return cb(new Error("Invalid JSON!"));
    }
    if (frame > test.frames.length) {
      chalk.red.write("%d %s", frame, str);
      return cb(new Error("Unexpected EXTRA frame at end: " + data));
    }
    if (!matches(data, test.frames[frame].out)) {
      chalk.red.write("%d ACTUAL: %s", frame, str);
      chalk.green.write("%d EXPECT: %j", frame, test.frames[frame].out);
      return cb(new Error("Frame not matching in " + frame));
    }
    chalk.cyan.write("%d %s", frame, str);
    frame++;
    if (frame < test.frames.length) {
      inFrame(test.frames[frame]['in']);
    } else {
      cp.stdin.end();
    }
  };
  var inFrame = function (data) {
    cp.stdin.write(JSON.stringify(data) + "\r\n");
  };
  inFrame(test.frames[frame]['in']);
};

var files = require('glob').sync("tests/*.test");
var fileCount = 0;

var processFile = function (file) {
  if (!file) return;
  chalk.blue.write('Processing test file: %s...', file);
  var test = {frames: []};
  var frame = -1;
  var buf = "";
  var flush = function () {
    if (!buf) return;
    var data = eval('(' + buf + ')');
    if (dir === "in") {
      if (frame >= 0 && !test.frames[frame].out) {
        test.frames[frame].out = {status: 'ok'};
      }
      frame++;
      test.frames.push({});
    }
    test.frames[frame][dir] = data;
    buf = "";
  };
  fs.readFileSync(file, 'utf-8').split("\n").forEach(function (line) {
    prefix = line.substr(0, 2);
    if (prefix === "//") {
      return;
    } else if (prefix === "=>") {
      flush();
      dir = "in";
      buf += line.substr(2);
    } else if (prefix === "<=") {
      flush();
      dir = "out";
      buf += line.substr(2);
    } else {
      buf += line;
    }
  });
  flush();
  if (frame >= 0 && !test.frames[frame].out) {
    test.frames[frame].out = {status: 'ok'};
  }
  chalk.gray.write('---------------------');
  runTest(test, function (err, frameCount) {
    if (err) {
      chalk.red.write(err);
      throw err;
    }
    console.log('');
    chalk.green.write('Success! (%d frame(s))', frameCount);
    chalk.bold.write('=====================');
    console.log('\r\n');
    fileCount++;
    processFile(files[fileCount]);
  });
};

processFile(files[fileCount]);
