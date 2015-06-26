http = require 'http'

module.exports = class Backend
  constructor: ->
    @onFrameCallbacks = []
  send: (frame, callback) ->
    cb = (res) =>
      buf = ""
      res.on 'data', (data) -> buf += data
      res.on 'end', =>
        frame = try JSON.parse(buf)
        return unless frame
        callback?(frame)
        switch frame.type
          when 'channels'
            @onChannels(frame)
          when 'routes'
            @onRoutes(frame)

    req = http.request({method: 'POST', path: '/message'}, cb)
    req.end(JSON.stringify(frame))
  nodes: (cars) ->
    for car in cars
      {uuid: car.uuid, pos: [car.x * 100, car.y * 100], speed: [car.sx * 100, car.sy * 100]}
  init: (cars, cb) ->
    frame =
      type: 'init'
      nodeCount: cars.length
      maxDistance: 3 * 100
      channelCount: 6
      nodes: @nodes(cars)
    @send(frame, cb)
  updateCars: (cars, cb) ->
    frame =
      type: 'update'
      nodes: @nodes(cars)
    @send(frame, cb)
  listRoutes: (cb) ->
    @send({type: 'listRoutes'}, cb)
