local inspect = require("inspect")

if #arg < 1 then
	print("Pass input file as arg1")
	os.exit(-1)
end
local orbits = {}
io.input(arg[1])

for line in io.lines() do
	local parent, child = string.match(line, "(%w*)%)(%w*)")
	-- print(parent .. "->" .. child)
	if orbits[parent] == nil then
		orbits[parent] = { parent = nil, children = {} }
	end
	if orbits[child] == nil then
		orbits[child] = { parent = nil, children = {} }
	end
	table.insert(orbits[parent]["children"], child)
	orbits[child]["parent"] = parent
end

print(inspect(orbits))

local node
local you_path = {}
node = orbits["YOU"]
while node.parent ~= nil do
	table.insert(you_path, 1, node.parent)
	node = orbits[node.parent]
end

local san_path = {}
node = orbits["SAN"]
while node.parent ~= nil do
	table.insert(san_path, 1, node.parent)
	node = orbits[node.parent]
end
print(inspect(you_path))
print(inspect(san_path))

-- remove duplicates
-- this is wrong, but in a good way. it undercuounts by 2 and overcounts by 2

while san_path[1] == you_path[1] do
	table.remove(san_path, 1)
	table.remove(you_path, 1)
end
print(inspect(you_path))
print(inspect(san_path))

print("total count " .. #you_path + #san_path)
