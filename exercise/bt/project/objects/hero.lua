
local lib = require("lib.init")
local Entity = import(".entity")

local ObjectClass = lib.Class("Hero", Entity)

function ObjectClass:initialize(...)
    Entity.initialize(self, ...)
end

return ObjectClass
