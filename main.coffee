Map = require './lib/map'

mapContainer = document.querySelector('.map')
map = new Map(mapContainer)
map.draw()

window.addEventListener 'resize', (e) ->
  map.resize()

selectedCar = null
carRouteList = document.querySelector('.car-route-list')
carRouteTemplate = carRouteList.querySelector('.template')
updateCarInfo = ->
  return unless selectedCar
  pos = "X=#{selectedCar.x.toFixed(2)}, Y=#{selectedCar.y.toFixed(2)}"
  document.querySelector('.car-pos').textContent = pos
  if not clock?
    map.backend.listRoutes()
map.backend.onRoutes = (routes) ->
  return unless selectedCar
  for node in routes.nodes
    if node.uuid == selectedCar.uuid
      selectedRoutes = node.routes
      break
  carRouteList.innerHTML = ''
  if selectedRoutes
    for route in selectedRoutes
      item = carRouteTemplate.cloneNode(true)
      item.querySelector('.dest-dist').textContent = route.dist.toFixed(2)
      channelDisp = "==#{route.channel}==>"
      item.querySelector('.dest-channel').textContent = channelDisp
      item.querySelector('.dest-uuid').textContent = route.dest
      destCar = null
      for car in map.cars
        if car.uuid == route.dest
          destCar = car
          break
      return unless destCar
      item.querySelector('.dest-color').style.backgroundColor = destCar.color
      carRouteList.appendChild(item)


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
    if selectedCar
      car.selected = false for car in map.cars
      selectedCar.selected = true
      document.querySelector('.infobar').classList.add('active')
      document.querySelector('.car-color').style.backgroundColor =
        selectedCar.color
      document.querySelector('.car-uuid').textContent = selectedCar.uuid
      updateCarInfo()
  else if selectedCar
    #target = map.roadmap.snapToRoad(target[0], target[1])
    return if target[0] < 0 or target[1] < 0
    return if target[0] > 5 or target[1] > 5
    selectedCar.setTarget(target)
  if not clock
    map.draw()

mapContainer.addEventListener 'contextmenu', (e) -> e.preventDefault()

clock = null
play = ->
  pauseButton.classList.remove('active')
  playButton.classList.add('active')
  if not clock?
    clock = setInterval (->
      map.tick()
      updateCarInfo()
    ), 100
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
  updateCarInfo()

play()
