Drawable = require './drawable'

module.exports = class Situation extends Drawable
  constructor: (@x, @y, layer) ->
    super(@x, @y, 0.2, 0.2, layer)
    @color = ['#f00', '#0e0', '#00f'][Math.floor(Math.random() * 3)]
    @selected = false
    @handled = false
  draw: ->
    return if @handled
    @layer.fillStyle = @color
    @layer.beginPath()
    @layer.ellipse(@x, @y, @w / 2, @h / 2, 0, 0, 2 * Math.PI)
    @layer.fill()
    @layer.strokeStyle = '#ff0'
    @layer.lineWidth = 0.02
    @layer.stroke()
  hitTest: (x, y) ->
    left = @x - @w / 2
    top = @y - @h / 2
    right = left + @w
    bottom = top + @h
    return (x >= left and x <= right) and (y >= top and y <= bottom)
  handledBy: (car) ->
    @handled = true
  tick: ->
  setTarget: (target) ->
    @target = target
    @waypoints = @roadmap.route(@x, @y, target[0], target[1])
