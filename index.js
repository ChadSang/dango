var rows = 10;
var cols = 10;
var roadCanvas = document.querySelector('.road-canvas');
var contactCanvas = document.querySelector('.contact-canvas');

var gridToPx = function (gridX, gridY) {
  return [
    gridX / cols * roadCanvas.offsetWidth,
    gridY / rows * roadCanvas.offsetHeight
  ];
};

var pxToGrid = function (pxX, pxY) {
  var gridX = Math.round(pxX / roadCanvas.offsetWidth * cols);
  var gridY = Math.round(pxY / roadCanvas.offsetHeight * rows);
  return [gridX, gridY];
};

roadCanvas.width = roadCanvas.offsetWidth;
roadCanvas.height = roadCanvas.offsetHeight;
contactCanvas.width = contactCanvas.offsetWidth;
contactCanvas.height = contactCanvas.offsetHeight;
var roadWidth = 12;
var ctx = roadCanvas.getContext('2d');
var drawRoad = function () {

  ctx.fillStyle = '#fff';
  ctx.fillRect(0, 0, roadCanvas.offsetWidth, roadCanvas.offsetHeight);

  for (var x = 0; x < rows; x++) {
    for (var y = 0; y < cols; y++) {
      ctx.fillStyle = '#999';
      var begin = gridToPx(x, y);
      var end = gridToPx(x + 1, y + 1);
      var dx = (end[0] - begin[0])
      var dy = (end[1] - begin[1])
      begin[0] += dx * 0.1
      begin[1] += dy * 0.1
      roadWidth = dy * 0.1;
      end[0] = dx * 0.9
      end[1] = dy * 0.9
      ctx.fillRect.apply(ctx, begin.concat(end));
    }
  }
};

var Car = {
  pos: [3, 2],
  target: [3, 2],
  color: '#afc',

  center: function () {
    var begin = gridToPx.apply(null, this.pos);
    return [begin[0] + roadWidth / 2, begin[1] + roadWidth / 2];
  },

  create: function (pos, color) {
    var car = Object.create(Car);
    car.pos = pos;
    car.target = pos;
    car.color = color;
    return car;
  },
  draw: function () {
    this.move(this.pos);
    this.retarget(this.target);
  },
  move: function (newPos) {
    var begin = gridToPx.apply(null, this.pos);
    var end = [roadWidth, roadWidth];
    ctx.fillStyle = '#fff';
    ctx.fillRect.apply(ctx, begin.concat(end));

    this.pos = newPos;
    var begin = gridToPx.apply(null, this.pos);
    var end = [roadWidth, roadWidth];
    ctx.fillStyle = this.color;
    ctx.fillRect.apply(ctx, begin.concat(end));
  },

  retarget: function (newTarget) {
    var begin = gridToPx.apply(null, this.target);
    var end = [roadWidth, roadWidth];
    ctx.fillStyle = '#fff';
    ctx.fillRect.apply(ctx, begin.concat(end));

    this.target = newTarget;
    var begin = gridToPx.apply(null, this.target);
    var end = [roadWidth, roadWidth];
    ctx.fillStyle = this.color;
    ctx.fillRect.apply(ctx, begin.concat(end));
    ctx.fillStyle = 'rgba(255, 255, 255, 0.5)';
    ctx.fillRect.apply(ctx, begin.concat(end));
  },

  tick: function () {
    var target = this.target;
    var newPos = [].concat(this.pos);
    if (target[0] < newPos[0]) {
      newPos[0] -= 0.1;
    } else if (target[0] > newPos[0]) {
      newPos[0] += 0.1;
    } else if (target[1] < newPos[1]) {
      newPos[1] -= 0.1;
    } else if (target[1] > newPos[1]) {
      newPos[1] += 0.1;
    }

    var dim = 0;
    var delta = 0.05;
    if (Math.abs(target[0] - newPos[0]) < delta) {
      newPos[0] = target[0];
      dim++;
    }
    if (Math.abs(target[1] - newPos[1]) < delta) {
      newPos[1] = target[1];
      dim++;
    }
    this.retarget(this.target);
    this.move(newPos);

    if (dim === 2) {
      this.emit('done');
    }
  },

  dist: function (target) {
    var xx = Math.pow(target.pos[0] - this.pos[0], 2);
    var yy = Math.pow(target.pos[1] - this.pos[1], 2);
    return Math.sqrt(xx + yy);
  },

  on: function (event, handler) {
    if (!this._handlers[event]) this._handlers[event] = [];
    this._handlers[event].push(handler);
  },

  emit: function (event) {
    var that = this;
    var handlers = this._handlers[event];
    if (handlers) handlers.forEach(function (handler) {
      handler.call(that);
    });
  },

  _handlers: {},
};

var drawContact = function (c1, c2) {
  var contact = contactCanvas.getContext('2d');
  if (c1.dist(c2) > 3) return;
  contact.strokeStyle = '#f00';
  contact.save();
  contact.beginPath();
  contact.moveTo.apply(contact, c1.center());
  contact.lineTo.apply(contact, c2.center());
  contact.stroke();
  contact.restore();
};

contactCanvas.addEventListener('mousedown', function (e) {
  var car = cars[0];
  if (e.which !== 1) car = cars[1];
  var newTarget = pxToGrid(e.clientX, e.clientY);
  car.retarget(newTarget);
  console.log(e);
});

contactCanvas.addEventListener('contextmenu', function (e) {
  e.preventDefault();
});

drawRoad();
var cars = [Car.create([2, 3], '#afc'), Car.create([4, 4], '#fca')];
for (var i = 0; i < 20; i++) {
  var target = [Math.floor(Math.random() * cols), Math.floor(Math.random() * rows)];
  cars.push(Car.create(target, '#0a0'));
}
var rantarget = function (car) {
  var target = [Math.floor(Math.random() * cols), Math.floor(Math.random() * rows)];
  car.retarget(target);
};
cars.forEach(function (car) {
  car.draw();
  rantarget(car);
  car.on('done', function () {
    console.log(this);
    rantarget(this);
  });
});
setInterval(function() {
  cars.forEach(function (car) { car.tick(); });
  var contact = contactCanvas.getContext('2d');
  contact.clearRect(0, 0, contactCanvas.width, contactCanvas.height);
  for (var i = 0; i < 20; i++) {
    for (var j = i + 1; j < 20; j++) {
      drawContact(cars[i], cars[j]);
    }
  }
}, 100);
