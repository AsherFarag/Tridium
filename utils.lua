function tableContains(table, element)
   for _, value in pairs(table) do
       if value == element then
           return true
       end
   end
   return false
end

-- Include the dependency's include directory
function includeDependency( dep )
	if dep.includeDir ~= nil then
		includedirs { dep.includeDir }
	end
end

-- Include the dependency's source files
function includeDependencySource( dep )
	if dep.sourceDir ~= nil then
		files { path.join(dep.sourceDir, "**.h"), path.join(dep.sourceDir, "**.cpp"), path.join(dep.sourceDir, "**.c"), path.join(dep.sourceDir, "**.hpp") }
	end
end

-- Link the dependency's library files
function linkDependency( dep, isDebug )
	if isDebug then
		if dep.debugLibDir ~= nil then
			libdirs { dep.debugLibDir }
		elseif dep.libDir ~= nil then
			libdirs { dep.libDir }
		end

		if dep.debugLibName ~= nil then
			links { dep.debugLibName }
		elseif dep.libName ~= nil then
			links { dep.libName }
		end
	else
		if dep.libDir ~= nil then
			libdirs { dep.libDir }
		end

		if dep.libName ~= nil then
			links { dep.libName }
		end
	end
end

-- This will register dependencies based on a list of dependencies and the current configurations.
-- dependenciesList: The list of dependencies to register (see Dependencies.lua for structure)
-- configurations: The list of configurations to consider (e.g. { "Debug", "Release", "Shipping" })
function registerDependencies( dependenciesList, configurations )

	-- Check if we are in a debug configuration
	local isDebug = tableContains(configurations, "Debug")

	-- Get the current target system (windows, linux, macos, etc.)
	local system = os.target()
	system = system:lower()

	for depName, dep in pairs(dependenciesList) do
		-- Check if the dependency is used in the current configurations
		if not dep.configs or tableContains(dep.configs, configurations) then

			-- Include the dependency's include directory
			includeDependency( dep )

			-- Include the dependency's source files (if any) so that they are built as part of the project
			includeDependencySource( dep )

			-- Platform specific configurations
			local platformConfig = dep[system]

			if platformConfig then
				linkDependency( platformConfig, isDebug )
			end
		end
	end
end