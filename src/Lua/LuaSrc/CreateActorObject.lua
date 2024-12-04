
Actor = {} -- empty table named entity, initalizing objects like this is common for object oriented programming
Actor.__index = Actor -- tells Lua to look into entity to check for keys to data

--Contructor funciton to create an entity
function Actor:new(name, collisionType, position, scale)
    
  print("name:", name)
  print("collisionType:", collisionType)
  print("position:", position.x, position.y, position.z)
  print("scale:", scale.x, scale.y, scale.z)
  
  
  -- Creates a new local actor with the metatable of actor, enabling inheritance of methods
    local Actor = setmetatable({}, Actor)
      -- Set properties for the new instance
    Actor.name = name or "DefaultName"
    Actor.collisionType = collisionType or 0
    Actor.position = position or  { x = 0, y = 0, z = 0}
    Actor.scale = scale or  { x = 1, y = 1, z = 1}
    return Actor
end

--function Actor:new()
--  -- Creates a new local actor with the metatable of actor, enabling inheritance of methods
-- local Actor = setmetatable({}, Actor)
--   -- Set properties for the new instance
-- Actor.name = "DefaultName"
-- Actor.collisionType =  1
-- Actor.position = { x = 2, y = 0, z = 0}
-- Actor.scale = { x = 1, y = 1, z = 1}
-- return Actor
--end
--