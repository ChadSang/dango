Map = require './lib/map'

mapContainer = document.querySelector('.map')
map = new Map(mapContainer)
map.draw()

window.addEventListener 'resize', (e) ->
  map.resize()

selectedCar = null
mapContainer.addEventListener 'mousedown', (e) ->
  e.preventDefault()
  vw = mapContainer.offsetWidth
  vh = mapContainer.offsetHeight
  target = map.viewport.invTransformPos(e.offsetX, e.offsetY, vw, vh)
  if e.which == 1
    for i in [(map.cars.length-1)..0]
      car = map.cars[i]
      if car.hitTest(target[0], target[1])
        car.selected = true
        selectedCar = car
        break
    car.selected = false for car in map.cars
    selectedCar.selected = true
  else if selectedCar
    #target = map.roadmap.snapToRoad(target[0], target[1])
    return if target[0] < 0 or target[1] < 0
    return if target[0] > 5 or target[1] > 5
    selectedCar.setTarget(target)
  if not clock
    map.draw()

mapContainer.addEventListener 'contextmenu', (e) -> e.preventDefault()

clock = setInterval (-> map.tick()), 100
play = ->
  pauseButton.classList.remove('active')
  playButton.classList.add('active')
  if not clock?
    clock = setInterval (-> map.tick()), 100
pause = ->
  pauseButton.classList.add('active')
  playButton.classList.remove('active')
  if clock?
    clearInterval(clock)
    clock = null

playButton = document.querySelector('.btn-play')
pauseButton = document.querySelector('.btn-pause')
nextButton = document.querySelector('.btn-next')

playButton.addEventListener 'click', play

pauseButton.addEventListener 'click', pause

nextButton.addEventListener 'click', ->
  pause()
  map.tick()
