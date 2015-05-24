
local Set = require("lib.tiled.set")

local Tile = {}
Tile.__index = Tile

local function new(class, set, id, offset_x, offset_y)
    local image
    if set then
        image = set.image
    end

    local quad
    if set then
        quad = set.quads[id]
    end

    return setmetatable({
        x = 0,
        y = 0,
        set = set,
        id = id,
        offset_x = offset_x,
        offset_y = offset_y,
        image = image,
        quad = quad,
    }, Tile)
end

function Tile:move(dx, dy)
    self.x = self.x + dx
    self.y = self.y + dy
end

function Tile:draw()
    if self.set then
        love.graphics.draw(self.image, self.quad, self.offset_x + self.x, self.offset_y + self.y)
    end
end

-- the module
return setmetatable({new = new},
    {__call = function(_, ...) return new(...) end})