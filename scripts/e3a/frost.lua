module('frost', package.seeall)

local function is_winter(turn)
    local season = get_season(turn)
    return season == "calendar::winter"
end

local function freeze(r)
    for i, rn in ipairs(r.adj) do
        -- each region has a chance to freeze
        if rn.terrain=="ocean" and math.mod(rng_int(), 100)<20 then
            rn.terrain = "packice"
        end
    end
end

local function thaw(r)
    r.terrain = "ocean"
    for s in r.ships do
        s.coast = nil
    end
end

function update()
    local turn = get_turn()
    if is_winter(turn) then
        for r in regions() do
            if r.terrain=="glacier" then
                freeze(r)
            end
        end
    elseif is_winter(turn-1) then
        for r in regions() do
            if r.terrain=="packice" then
                thaw(r)
            end
        end
    end
end
