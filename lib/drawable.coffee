module.exports = class Drawable
  constructor: (@x, @y, @w, @h, @layer) ->
    @fillStyle = '#000'
    @layer.dango?.drawables.push(this)
  tick: -> null
  draw: ->
    @layer.fillStyle = @fillStyle
    @layer.fillRect(@x, @y, @w, @h)
