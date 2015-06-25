Drawable = require './drawable'

module.exports = class Roadmap extends Drawable
  roadWidth: 0.2
  roadEpsilon: 0.01
  minNormal: Math.pow(2, -1022)
  constructor: (@rows, @cols, layer) ->
    super(- @roadWidth / 2, - @roadWidth / 2,
      @rows + @roadWidth, @cols + @roadWidth, layer)
  draw: ->
    @layer.canvas.style.backgroundColor = '#333'
    @layer.fillStyle = '#fff'
    for x in [0..@cols]
      @layer.fillRect(@x + x, @y, @roadWidth, @h)
    for y in [0..@rows]
      @layer.fillRect(@x, @y + y, @w, @roadWidth)
    return
  snapToRoad: (x, y) ->
    [cx, cy] = @snapToCross(x, y)
    dx = Math.abs(cx - x)
    dy = Math.abs(cy - y)
    if dx < dy
      return [cx, y]
    else
      return [x, cy]
  snapToCross: (x, y) ->
    return [Math.round(x), Math.round(y)]
  getRoadDir: (x, y) ->
    if @floatNear(Math.round(x), x)
      roadDir = 'y'
    if @floatNear(Math.round(y), y)
      return 'c' if roadDir
      roadDir = 'x'
    return roadDir
  floatNear: (a, b, epsilon=0.001) ->
    return true if a == b
    diff = Math.abs(a - b)
    if a == 0 or b == 0 or diff < @minNormal
      return diff < (epsilon * @minNormal)
    else
      return diff / (Math.abs(a) + Math.abs(b)) < epsilon
  route: (sx, sy, tx, ty) ->
    # 1. Move to the nearest road.
    [rsx, rsy] = @snapToRoad(sx, sy)
    [csx, csy] = @snapToCross(rsx, rsy)
    sourceRoadDir = @getRoadDir(rsx, rsy)
    waypoints = []
    waypoints.push([rsx, rsy])

    [rtx, rty] = @snapToRoad(tx, ty)
    targetRoadDir = @getRoadDir(rtx, rty)
    [ctx, cty] = @snapToCross(rtx, rty)
    if targetRoadDir == 'y'
      ctx = rtx
      cty = if csy < rty then Math.floor(rty) else Math.ceil(rty)
    else
      ctx = if csx < rtx then Math.floor(rtx) else Math.ceil(rtx)
      cty = rty
    # 2. Move along the road to the first turning point.
    if sourceRoadDir == 'y'
      waypoints.push([rsx, cty])
    else
      waypoints.push([ctx, rsy])
    # 3. Move to the nearest cross to the target.
    waypoints.push([ctx, cty])

    if targetRoadDir == 'y'
      if @floatNear(ctx, rsx) and (cty - rsy) * (rty - cty) < 0
        waypoints.pop()
        waypoints.pop() if @floatNear(rsx, ctx)
    else
      if @floatNear(rsy, cty) and (ctx - rsx) * (rtx - ctx) < 0
        waypoints.pop()
        waypoints.pop() if @floatNear(rsy, cty)
    # 4. Move to the nearest point on the road to the target.
    waypoints.push([rtx, rty])
    # 5. Move to the target.
    waypoints.push([tx, ty])

    return waypoints
