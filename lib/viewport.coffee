module.exports = class Viewport
  constructor: (@x, @y, @w, @h) -> null

  transformContext: (context) ->
    context.resetTransform()
    context.scale(context.canvas.height / @w, context.canvas.height / @h)
    context.translate(-@x, -@y)
    return context

  transformPos: (x, y, vw, vh) ->
    return [x * vh / @w - @x, y * vh / @h - @y]

  invTransformPos: (vx, vy, vw, vh) ->
    return [vx * @w / vh + @x, vy * @h / vh + @y]
