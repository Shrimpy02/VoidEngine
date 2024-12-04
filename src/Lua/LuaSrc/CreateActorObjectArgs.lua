
function newObject(name, collisionType, position, scale)
  -- directly return a table representing the actor
  return {
      name = name or "LuaActorObjectArgs",
      collisionType = collisionType or 0,
      position = position or {x = 2, y = 0, z = 0},
      scale = scale or {x = 1, y = 1, z = 1}
  }
end
