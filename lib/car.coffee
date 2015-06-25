Drawable = require './drawable'

module.exports = class Car extends Drawable
  constructor: (@x, @y, layer, @roadmap) ->
    super(@x, @y, 0.2, 0.2, layer)
    @sx = 0
    @sy = 0
    @speed = 0.1
    @waypoints = []
    @color = '#afc'
    @target = null
  draw: ->
    @layer.fillStyle = @color
    @layer.fillRect(@x - @w / 2, @y - @h / 2, @w, @h)
    @layer.setLineDash([@speed / 2])
    @layer.strokeStyle = @color
    @layer.lineWidth = 0.02
    @layer.beginPath()
    @layer.moveTo(@x, @y)
    for waypoint in @waypoints
      @layer.lineTo(waypoint[0], waypoint[1])
    @layer.stroke()
    @layer.setLineDash([])
  adjustedSpeed: (source, target, speed) ->
    dist = target - source
    if dist >= speed
      speed
    else if dist <= -speed
      -speed
    else
      dist
  tick: ->
    waypoints = @waypoints
    dest = waypoints[0]
    while dest
      xEqual = @roadmap.floatNear(dest[0], @x)
      yEqual = @roadmap.floatNear(dest[1], @y)
      break if not xEqual or not yEqual
      waypoints.shift()
      dest = waypoints[0]
    if not dest
      @sx = 0
      @sy = 0
    else
      if not xEqual
        @sx = @adjustedSpeed(@x, dest[0], @speed)
        @sy = 0
      else
        @sx = 0
        @sy = @adjustedSpeed(@y, dest[1], @speed)
    @x += @sx
    @y += @sy
  setTarget: (target) ->
    @target = target
    @waypoints = @roadmap.route(@x, @y, target[0], target[1])
