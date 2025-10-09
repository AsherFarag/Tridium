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

-- ==========================================================
-- Helper: copydir (safe recursive copy)
-- ==========================================================
function copydir(src, dst)
	if not os.isdir(src) then
		print("Error: Source directory '" .. src .. "' does not exist.")
		return
	end

	if not os.isdir(dst) then
		local ok, err = pcall(os.mkdir, dst)
		if not ok then
			print("Error creating destination directory '" .. dst .. "': " .. tostring(err))
			return
		end
	end

	local entries = os.matchfiles(path.join(src, "*"))
	for _, file in ipairs(entries) do
		local filename = path.getname(file)
		local destFile = path.join(dst, filename)
		local success, err = pcall(os.copyfile, file, destFile)
		if not success then
			print("Error copying file: " .. file .. " -> " .. destFile .. "\nReason: " .. tostring(err))
		end
	end

	local subDirs = os.matchdirs(path.join(src, "*"))
	for _, dir in ipairs(subDirs) do
		local dirname = path.getname(dir)
		local subDst = path.join(dst, dirname)
		copydir(dir, subDst)
	end
end

-- ==========================================================
-- Action: install-templates
-- ==========================================================
newaction {
	trigger     = "install-templates",
	description = "Install local Visual Studio item templates for Tridium Engine",
	execute = function()
		local repoRoot = os.getcwd()
		local sourceTemplates = path.join(repoRoot, "Extra", "Templates")

		local userProfile = os.getenv("USERPROFILE")
		if not userProfile then
			print("Error: USERPROFILE environment variable not found.")
			return
		end

		local userTemplates = path.join(userProfile, "Documents", "Visual Studio 2022", "Templates", "ItemTemplates", "Tridium")
		os.mkdir(userTemplates)

		if not os.isdir(sourceTemplates) then
			print("Error: Source directory '" .. sourceTemplates .. "' does not exist. Cannot install templates.")
			return
		end

		print("Building zipped Visual Studio templates...")
		local folders = os.matchdirs(path.join(sourceTemplates, "*"))
		for _, dir in ipairs(folders) do
			local templateName = path.getname(dir)
			local zipPath = path.join(sourceTemplates, templateName .. ".zip")

			local cmd = string.format(
				'powershell -Command "Compress-Archive -Path \\"%s\\*\\" -DestinationPath \\"%s\\" -Force"',
				dir, zipPath
			)
			os.execute(cmd)
			print("  Created " .. zipPath)
		end

		print("\nMoving templates to Visual Studio directory...")
		local zips = os.matchfiles(path.join(sourceTemplates, "*.zip"))
		for _, zipFile in ipairs(zips) do
			local dest = path.join(userTemplates, path.getname(zipFile))
			os.copyfile(zipFile, dest)
			os.remove(zipFile)
			print("  Installed " .. path.getname(zipFile))
		end

		print("\nInstalled Tridium templates to:")
		print("  " .. userTemplates)
		print("Restart Visual Studio to refresh templates.")
	end
}


-- ==========================================================
-- Action: uninstall-templates
-- ==========================================================
newaction {
	trigger     = "uninstall-templates",
	description = "Uninstall local Visual Studio item templates for Tridium Engine",
	execute = function()
		local userProfile = os.getenv("USERPROFILE")
		if not userProfile then
			print("Error: USERPROFILE environment variable not found.")
			return
		end

		local userTemplates = path.join(userProfile, "Documents", "Visual Studio 2022", "Templates", "ItemTemplates", "Tridium")

		if os.isdir(userTemplates) then
			print("Removing templates from: " .. userTemplates)
			local ok, err = pcall(os.rmdir, userTemplates)
			if not ok then
				print("Error removing templates directory: " .. tostring(err))
			else
				print("Uninstalled Tridium templates from: " .. userTemplates)
			end
		else
			print("No Tridium templates found to uninstall at: " .. userTemplates)
		end
	end
}