local srcpath = config.source_dir
local respath = srcpath .. '/' .. config.game .. '/res/'
local paths = { config.game..'/scripts/?.lua';'lunit/?.lua','external/lunit/?.lua','scripts/?.lua';'scripts/?' }

tests = {'common'}
for idx, test in pairs(tests) do
  tests[idx] = srcpath .. '/scripts/tests/' .. test .. '.lua'
end

for idx, path in pairs(paths) do
  package.path = srcpath .. '/' .. path .. ';' .. package.path
end

read_xml(respath..'config-'..config.game..'.xml', respath..'catalog-'..config.game..'.xml')

require "init"
