function use_xmastree(u, amount)
  u.region:set_key("xm06", true)
  u:use_pooled("xmastree", amount)
  local msg = message.create("usepotion")
  msg:set_unit("unit", u)
  msg:set_resource("potion", "xmastree")
  msg:send_region(u.region)
  return 0
end

function update_xmas2006()
  local turn = get_turn()
  local season = get_season(turn)
  if season == "calendar::winter" then
    print("it is " .. season .. ", the christmas trees do their magic")
    local msg = message.create("xmastree_effect")
    for r in regions() do
      if r:get_key("xm06") then
        trees = r:get_resource("tree")
        if trees*0.1>=1 then
          r:set_resource("tree", trees * 1.1)
          msg:send_region(r)
        end
        if clear then
        end
      end
    end
  else
    local prevseason = get_season(turn-1)
    if prevseason == "calendar::winter" then
      -- we celebrate knut and kick out the trees.
      for r in regions() do
        if r:get_key("xm06") then
          r:set_key("xm06", false)
        end
      end
    end
  end
end

function xmas2006()
  if get_gamename() == "Eressea" then
    if not get_key("xm06") then
      print("Es weihnachtet sehr (2006)")
      set_key("xm06", true)
      for f in factions() do
        f:add_item("xmastree", 1)
        f:add_notice("santa2006")
      end
    end
  end
end

-- xmas2006()
