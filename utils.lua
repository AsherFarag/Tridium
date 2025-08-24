function get_filename(url)
	local str = url
	local temp = ""
	local result = ""
	for i = str:len(), 1, -1 do
		if str:sub(i,i) ~= "/" then
			temp = temp..str:sub(i,i)
		else
			break
		end
	end
	for j = temp:len(), 1, -1 do
		result = result..temp:sub(j,j)
	end
	return result
end

function get_file_extension(url)
    local dotIndex = url:match(".*%.([^%.]+)$")
    if dotIndex then
        return dotIndex
    else
        return nil
    end
end

function is_static_lib_file(url)
	static_lib_file_extensions = { "a", "lib" }
	file_extension = get_file_extension(url)
	for _, extension in ipairs(static_lib_file_extensions) do
		if extension == file_extension then
			return true
		end
	end
	return false
end

function is_shared_lib_file(url)
	static_lib_file_extensions = { "so", "dll", "jar" }
	file_extension = get_file_extension(url)
	for _, extension in ipairs(static_lib_file_extensions) do
		if extension == file_extension then
			return true
		end
	end
	return false
end

function table.contains(table, element)
	for _, value in pairs(table) do
		if value == element then
			return true
		end
	end
	return false
end

function table.contains_key(table, element)
	for key, _ in pairs(table) do
		if key == element then
			return true
		end
	end
	return false
end

function table.length(table)
	local count = 0
	for _ in pairs(table) do
		count = count + 1 
	end
	return count
end

function table.append(table, source)
	for key, val in pairs(source) do
		table[key] = val
	end
end

-- This will look through a folder and construct a table listing found libraries.
-- Structure of result is as follows:
-- table LibraryTable
-- {
--    table LibraryTableEntry...
-- }
-- 
-- table LibraryTableEntry
-- {
--    path: string
--    include_path: string
--    lib_path: string
--    source_only: bool
--    Debug_Win64: LinkInfo
--    Debug_ARM64: LinkInfo
--    Release_Win64: LinkInfo
--    Release_ARM64: LinkInfo
--    Shipping_Win64: LinkInfo
--    Shipping_ARM64: LinkInfo
-- }
--
-- table LinkInfo
-- {
--    path: string
--    config: string
--    platform: string
--    static: table
--    shared: table
-- }
function find_libraries(root)
	-- Create LibraryTable
	local library_table = {}
	
	-- Iterate through dependency folders and create entries for each dependency found.
	for _, dependency_folder in ipairs(os.matchdirs(path.join(root, "*"))) do
		local library_name = get_filename(dependency_folder)
		library_table[library_name] = {}
		
		-- Set the path to the library.
		library_table[library_name].path = path.getabsolute(dependency_folder)
		
		-- Set the include directory
		local include_dir = path.join(dependency_folder, "include")
		if os.isdir(include_dir) then
			library_table[library_name].include_path = path.getabsolute(include_dir)
		else
			library_table[library_name].include_path = nil
		end
		
		-- Set the lib directory
		local lib_dir = path.join(dependency_folder, "lib")
		if os.isdir(lib_dir) then
			library_table[library_name].lib_path = path.getabsolute(lib_dir)
		else
			library_table[library_name].lib_path = nil
		end

		-- We want to fill out the LinkInfo entries if the lib_path exists.
		local function create_link_info_entry(config_platform_pair)
		
			-- Initialise the config_platform entry.
			library_table[library_name][config_platform_pair] = {}
			
			-- We should extract the config and platform from the config_platform pair.
			local config, platform = string.match(config_platform_pair, "([^_]+)_([^_]+)")
			library_table[library_name][config_platform_pair].config = config
			library_table[library_name][config_platform_pair].platform = platform
			
			-- If lib folder exists, then we want to set the lib info.
			if library_table[library_name].lib_path ~= nil then
				local link_info_entry_dir = path.join(lib_dir, config_platform_pair)
				if os.isdir(link_info_entry_dir) then
					library_table[library_name][config_platform_pair].path = path.getabsolute(link_info_entry_dir)
					library_table[library_name][config_platform_pair].static = {}
					library_table[library_name][config_platform_pair].shared = {}
					
					-- Search through all files in directory and add to lists.
					local lib_file_paths = os.matchfiles(path.join(link_info_entry_dir, "*"))
					
					for _, lib_file_path in ipairs(lib_file_paths) do
						-- Let's get the file name of the lib file.
						local lib_file_name = get_filename(lib_file_path)
						
						-- Add the file name to the right list.
						if is_static_lib_file(lib_file_name) then
							table.insert(library_table[library_name][config_platform_pair].static, lib_file_name)
						elseif is_shared_lib_file(lib_file_name) then
							table.insert(library_table[library_name][config_platform_pair].shared, lib_file_name)
						end
					end
				end
			end
		end
		
		create_link_info_entry("Debug_Win64")
		create_link_info_entry("Debug_ARM64")
		create_link_info_entry("Release_Win64")
		create_link_info_entry("Release_ARM64")
		create_link_info_entry("Shipping_Win64")
		create_link_info_entry("Shipping_ARM64")
	end
	return library_table
end

-- Will add any dependencies by name found in the depependency list. Library table is the result of 'find_libraries'.
function add_dependencies(root, dependencies, headers_only)
	-- Get library table.
	local library_table = find_libraries(root)

	-- Reset filter.
	filter {}

	-- We need to go through all dependencies requested and figure out which ones we can add.
	for _, dependency_name in ipairs(dependencies) do
		if not table.contains_key(library_table, dependency_name) then
			print(string.format("Dependency '%s' not found.", dependency_name))
			goto continue
		end
		
		-- Let's get all header and source files.
		local header_files = {}
		table.append(header_files, os.matchfiles(path.join(library_table[dependency_name].include_path, "**.h")))
		table.append(header_files, os.matchfiles(path.join(library_table[dependency_name].include_path, "**.hpp")))
		table.append(header_files, os.matchfiles(path.join(library_table[dependency_name].include_path, "**.inl")))
		
		local source_files = {}
		table.append(source_files, os.matchfiles(path.join(library_table[dependency_name].include_path, "**.c")))
		table.append(source_files, os.matchfiles(path.join(library_table[dependency_name].include_path, "**.cpp")))
		
		local has_header_files = table.length(header_files) > 0
		local has_source_files = table.length(source_files) > 0
		
		-- Let's create a function that can add each profile.
		-- Decision tree:
		-- For the given profile, we want to check to see if there's a folder for libs for that combination.
		-- -- If there are libs for the given profile:
		-- -- -- Add static libs to input.
		-- -- -- Add commands to copy shared libs to output.
		-- -- If there are no libs:
		-- -- -- We want to 
		
		local function add_dependency(library_table_entry, profile)
			local function add_sub_dependency(config_name, platform_name)
				-- We want to set the filter that we're modifying.
				filter { string.format("configurations:%s", config_name), string.format("platforms:%s", platform_name) }
			
				-- We want to first add the includedirs. These are in the form of 'include_path' and 'include_path/[dependency]'
				includedirs { library_table_entry.include_path }
				includedirs { string.format("%s/%s", library_table_entry.include_path, dependency_name) }
			
				-- We want to add the header files always.
				if headers_only == false then
					files { header_files }
				end
				
				-- If we have lib files, we should add the directory to it.
				local has_lib_files = library_table_entry[profile] ~= nil
				if has_lib_files then
					
					-- We should add the path to the lib file.
					libdirs { library_table_entry[profile].path }
					
					-- We want to link all static libs.
					if library_table_entry[profile].static ~= nil then
						for _, lib_file_to_link in ipairs(library_table_entry[profile].static) do
							links { lib_file_to_link }
						end
					end
					
					-- For all shared libs, we want to add a post process command to copy this to the output folder.
					if library_table_entry[profile].shared ~= nil then
						for _, lib_file_to_link in ipairs(library_table_entry[profile].shared) do
							postbuildcommands {
								string.format("xcopy /Q /E /Y /I \"%s\" \"$(OutDir)\"", path.translate(path.join(path.join(library_table_entry.lib_path, profile), lib_file_to_link)))
							}
						end
					end
					
				-- If not, we want to add the source files.
				elseif headers_only == false then
					files { source_files }
				end
				
				-- Reset filter.
				filter {}
			end
			client_config = string.format("%s Client", library_table_entry[profile].config)
			editor_config = string.format("%s Editor", library_table_entry[profile].config)
			server_config = string.format("%s Server", library_table_entry[profile].config)
			add_sub_dependency(client_config, library_table_entry[profile].platform)
			add_sub_dependency(editor_config, library_table_entry[profile].platform)
			add_sub_dependency(server_config, library_table_entry[profile].platform)
		end
		
		add_dependency(library_table[dependency_name], "Debug_Win64")
		add_dependency(library_table[dependency_name], "Debug_ARM64")
		add_dependency(library_table[dependency_name], "Release_Win64")
		add_dependency(library_table[dependency_name], "Release_ARM64")
		add_dependency(library_table[dependency_name], "Shipping_Win64")
		add_dependency(library_table[dependency_name], "Shipping_ARM64")
		
		::continue::
	end
	
	-- Reset filter.
	filter{}
end

function set_ndk_path(ndk_root, vcxproj)
	local tag = "<VS_NdkRoot>" .. ndk_root .. "</VS_NdkRoot>"

	local f = io.open(vcxproj, "r")
	if not f then
		--print("Could not open file for patching: " .. vcxproj)
		return
	end

	local content = f:read("*all")
	f:close()

	-- Skip if VS_NdkRoot already present
	if content:find("<VS_NdkRoot>") then
		--print("VS_NdkRoot already set, skipping patch.")
		return
	end

	-- Insert into first PropertyGroup Label="Globals"
	local new_content = content:gsub(
		'(<PropertyGroup.-Label="Globals".->)',
		'%1\n    ' .. tag,
		1 -- only the first occurrence
	)

	if new_content == content then
		--print("No Globals PropertyGroup found, could not insert NDK path.")
		return
	end

	-- Save the modified file
	local out = io.open(vcxproj, "w")
	if not out then
		--print("Could not open file for writing: " .. vcxproj)
		return
	end
	out:write(new_content)
	out:close()

	--print("NDK path inserted into: " .. vcxproj)
end

function attach_ndk_path_patcher(premake, ndk_root, vcxproj)

	premake.override(premake.action, "call", 
		function(base, action)
			base(action)
			local workspace_path = workspace().location
			local ndk_local = path.join(workspace_path, ndk_root)
			local vcx_local = path.join(workspace_path, vcxproj)
			--print(ndk_local)
			--print(vcx_local)
			set_ndk_path(ndk_local, vcx_local)
		end
	)
end
