local inspect = require("inspect")

if #arg < 1 then
	print("Pass input file as arg1")
end
local orbits = {}
io.input(arg[1])

for line in io.lines() do
	local parent, child = string.match(line, "(%w*)%)(%w*)")
	-- print(parent .. "->" .. child)
	if orbits[parent] == nil then
		orbits[parent] = {}
	end
	if orbits[child] == nil then
		orbits[child] = {}
	end
	orbits[parent][child] = 1
end

print(inspect(orbits))

local sum = 0
local get_depth
get_depth = function(obj, depth)
	sum = sum + depth
	for k in pairs(obj) do
		get_depth(orbits[k], depth + 1)
	end
end

get_depth(orbits["COM"], 0)
print(sum)
