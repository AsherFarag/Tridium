project "glad"
	kind "StaticLib"
	language "C"

	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

	includedirs
	{
		"include"
	}

    filter "system:windows"
		systemversion "latest"
        staticruntime "On"

	filter { "system:windows", "configurations:Release" } 
		buildoptions "/MT"