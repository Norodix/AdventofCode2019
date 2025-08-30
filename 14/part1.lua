local inspect = require("inspect")
local math = require("math")

if #arg < 1 then
	print("Pass input file as arg1")
	os.exit(-1)
end

-- recipes = {result1 = {amount = 1, ingredients= {ing1 = 1, ing2 = 2}}}
local recipes = {}
-- Add the dummy ore-ore recipe
recipes["ORE"] = {
	amount = 1,
	ingredients = {
		["ORE"] = 1,
	},
}

io.input(arg[1])

for line in io.lines() do
	local ingredients, res = string.match(line, "(.*)=>(.*)")
	local amount, result = string.match(res, "(%d+) (%w+)")
	recipes[result] = {
		["amount"] = tonumber(amount),
		["ingredients"] = {},
	}
	for ingnum, ing in string.gmatch(ingredients, "(%d+) (%w+)") do
		-- print(ing .. " -> " .. ingnum)
		recipes[result]["ingredients"][ing] = ingnum
	end
end

local get_requirements = function(result, leftovers)
	local requirements = {}
	for ing, ingnum in pairs(result) do
		-- account for levtover reagents
		if leftovers[ing] == nil then
			leftovers[ing] = 0
		end
		local sub = math.min(leftovers[ing], ingnum)
		ingnum = ingnum - sub
		leftovers[ing] = leftovers[ing] - sub

		-- we need this reaction n times
		local n = math.ceil(ingnum / recipes[ing].amount)

		-- add leftovers
		local lo = recipes[ing].amount * n - ingnum
		leftovers[ing] = leftovers[ing] + lo

		-- add each precursor of this reaction n times to the requirements
		for k, v in pairs(recipes[ing].ingredients) do
			if requirements[k] == nil then
				requirements[k] = 0
			end
			requirements[k] = requirements[k] + n * v
		end
	end
	return requirements
end

-- print(inspect(recipes))
-- Lets work backwards from 1 fuel

local finished = function(t)
	local f = true
	for k, v in pairs(t) do
		if k ~= "ORE" then
			f = false
		end
	end
	return f
end

local get_ore_count = function(fuel_count)
	local endresult = { ["FUEL"] = fuel_count, ["ORE"] = 0 }
	local intermediate = endresult
	local leftovers = {}
	-- for i = 1, 10 do
	while not finished(intermediate) do
		intermediate = get_requirements(intermediate, leftovers)
	end
	return intermediate["ORE"]
end

print(string.format("For %d fuel you need %d ores", 1, get_ore_count(1)))

-- binary search for the correct amount of fuel you can make with 1 trillion ores
local triore = 1000000000000
local f_count = 1000
while get_ore_count(f_count) < triore do
	f_count = f_count * 2
end

local f_min = f_count / 2
local f_max = f_count

while f_max - f_min > 1 do
	local f_guess = math.floor((f_min + f_max) / 2)
	local guess_ores = get_ore_count(f_guess)
	if guess_ores > triore then
		f_max = f_guess
	else
		f_min = f_guess
	end
end

print(string.format("For %d fuel you need %d ores", f_max, get_ore_count(f_max)))
print(string.format("For %d fuel you need %d ores", f_min, get_ore_count(f_min)))
