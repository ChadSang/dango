Viewport = require './viewport'
Drawable = require './drawable'
Car = require './car'
Situation = require './situation'
Roadmap = require './roadmap'
Backend = require './backend'

module.exports = class Map
  constructor: (elem) ->
    elem = document.querySelector(elem) if typeof elem == 'string'
    @container = elem
    @viewport = new Viewport(-1.1, -1.1, 7.2, 7.2)
    @layers = {}
    @resetLayers()
    @backend = new Backend()
    @roadmap = new Roadmap(5, 5, @layers.road)
    carColors = ['#3fa', '#fae', '#ecf', '#f33', '#3f3', '#33f', '#f0d', '#df0']
    @cars = []
    for i in [0...10]
      [x, y] = @roadmap.snapToRoad(Math.random() * 5, Math.random() * 5)
      @cars.push(new Car(x, y, @layers.car, @roadmap))
      @cars[i].uuid = i.toString()
      @cars[i].color = carColors[i % carColors.length]
    @backend.onChannels = @onChannels.bind(this)
    @backend.onRoutes = @onRoutes.bind(@)
    @backend.init @cars
  createLayer: (layerName) ->
    if layerName and @layers[layerName]
      context = @layers[layerName]
      dango = context.dango
      canvas = context.canvas
      context.resetTransform()
      context.clearRect(0, 0, canvas.width, canvas.height)
    else
      canvas = document.createElement('canvas')
      @container.appendChild(canvas)
    canvas.width = canvas.offsetWidth
    canvas.height = canvas.offsetHeight
    context = canvas.getContext('2d')
    @viewport.transformContext(context)
    context.dango = dango ? {drawables: []}
    if layerName
      @layers[layerName] = context
    return context
  clearLayer: (layer) ->
    if typeof layer == 'string'
      layer = @layers[layer]
    layer.save()
    layer.resetTransform()
    layer.clearRect(0, 0, layer.canvas.width, layer.canvas.height)
    layer.restore()
    return
  resetLayers: ->
    @createLayer('road')
    @createLayer('car')
    @createLayer('situation')
    @createLayer('conn')
    @createLayer('message')
    return
  draw: ->
    @clearLayer('car')
    for _, layer of @layers
      for drawable in layer.dango.drawables
        drawable.draw()
    return
  tick: ->
    @clearLayer('car')
    for car in @cars
      car.tick()
      car.draw()
    @backend.updateCars @cars
    if Math.random() < 0.1
      s = new Situation(Math.random() * 5, Math.random() * 5, @layers.situation)
      s.draw()
  onChannels: (frame) ->
    @clearLayer('conn')
    for i in [0...frame.channels.length]
      channel = frame.channels[i]
      continue if not channel
      for [from, to] in channel
        from = @cars[parseInt(from)]
        to = @cars[parseInt(to)]
        @drawConnection(from, to, i)
    @backend.listRoutes()
    return
  onRoutes: -> null
  drawConnection: (carA, carB, channel) ->
    layer = @layers.conn
    channelColors = ['#00f', '#f00', '#0f0', '#ff0', '#f0f', '#0ff']
    channelLineDash = [[], [0.1], [0.05], [0.1, 0.05], [0.05, 0.1], [0.1, 0.1, 0.05, 0.05]]
    layer.strokeStyle = channelColors[channel]
    layer.lineWidth = 0.05
    layer.setLineDash(channelLineDash[channel])
    layer.beginPath()
    layer.moveTo(carA.x, carA.y)
    layer.lineTo(carB.x, carB.y)
    layer.stroke()
    layer.setLineDash([])

  resize: ->
    @resetLayers()
    @draw()
