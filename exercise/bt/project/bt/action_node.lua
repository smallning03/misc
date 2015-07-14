
local BT = bt

local Common = BT.import(".common")
local Node = BT.import(".node")

local ObjectClass = Common.Class("ActionNode", Node)

function ObjectClass:initialize(precondition)
    Node.initialize(self, precondition)

    self.m_is_running = false
end

function ObjectClass:onEnter(input)

end

function ObjectClass:onExit(input, state)

end

function ObjectClass:onExecute(input, output)
    return Common.RunningStatus.Finish
end

function ObjectClass:onTransition(input)
    if self.m_is_running then
        self:onExit(input, Common.RunningStatus.Terminal)
        self.m_is_running = false
    end
end

function ObjectClass:onUpdate(input, output)
    if not self.m_is_running then
        self:onEnter(input)
        self.m_is_running = true
    end

    local state = self:onExecute(input, output)
    if state == Common.RunningStatus.Executing then
        return state
    else
        self.m_is_running = false
        self:onExit(input, state)
        return state
    end
end

return ObjectClass
