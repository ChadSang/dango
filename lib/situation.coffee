Drawable = require './drawable'

module.exports = class Situation extends Drawable
  constructor: (@x, @y, layer) ->
    super(@x, @y, 0.2, 0.2, layer)
    @color = '#f00'
    @selected = false
  draw: ->
    @layer.fillStyle = @color
    @layer.beginPath()
    @layer.circle(@x - @w / 2, @y - @h / 2, @w, @h)
    @layer.fill()
    if @selected
      @layer.strokeStyle = '#ff0'
      @layer.beginPath()
      @layer.ellipse(@x - @w / 2, @y - @h / 2, @w, @h)
      @layer.stroke()
  hitTest: (x, y) ->
    left = @x - @w / 2
    top = @y - @h / 2
    right = left + @w
    bottom = top + @h
    return (x >= left and x <= right) and (y >= top and y <= bottom)
  tick: ->
  setTarget: (target) ->
    @target = target
    @waypoints = @roadmap.route(@x, @y, target[0], target[1])
