local milk = require "scripts.milk"
local class = require "scripts.class"

local btnup = milk.btnup
local btndown = milk.btndown
local btnleft = milk.btnleft
local btnright = milk.btnright
local btna = milk.btna

local ATTACK_TICK_BUFFER = 8

local PlayerShip = class("PlayerShip")

function PlayerShip:initialize()
    self.x = 0
    self.y = 0
    self.speed = 1
    self.sprite = 0
    self.bulletTimer = 0
    self.hearts = 3
end

local function attack(self, gameplay)
    if milk.ticks > self.bulletTimer then
        milk.play(1, 1, 128)
        gameplay.bulletPool:create(self.x, self.y, -3)
        self.bulletTimer = milk.ticks + ATTACK_TICK_BUFFER
    end
end

local function animate(self)
    if milk.ticks % 12 < 6 then
        self.sprite = 0
    else
        self.sprite = 1
    end
end

-- luacheck: push ignore self
function PlayerShip:load()
    milk.loadsnd(1, "sounds/shoot.wav")
end
-- luacheck: pop

function PlayerShip:checkCollision(other)
    return
        self.x > other.x - 8
        and self.x < other.x + 8
        and self.y < other.y + 8
        and self.y > other.y - 8
end

function PlayerShip:update(gameplay)
    local mvx = 0
    local mvy = 0

    if milk.btn(btnup) then
        mvy = -self.speed
    end
    if milk.btn(btndown) then
        mvy = self.speed
    end
    if milk.btn(btnleft) then
        mvx = -self.speed
    end
    if milk.btn(btnright) then
        mvx = self.speed
    end
    if milk.btn(btna) then
        attack(self, gameplay)
    end

    self.x = self.x + mvx
    self.y = self.y + mvy

    animate(self)
end

function PlayerShip:draw()
    milk.spr(self.sprite, self.x, self.y)
end

return PlayerShip
