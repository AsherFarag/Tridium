project "JoltPhysics"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "On"

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

   files
   {
      "./Jolt/**.h",
      "./Jolt/**.hpp",
      "./Jolt/**.cpp",
   }

   includedirs
   {
      "./"
   }

	filter "configurations:Debug"
      defines "JPH_DEBUG_RENDERER"
		symbols "on"

	filter "configurations:Release"
      defines "JPH_DEBUG_RENDERER"
		optimize "on"
		
	filter "configurations:Debug-Editor"
      defines "JPH_DEBUG_RENDERER"
		symbols "on"

	filter "configurations:Release-Editor"
      defines "JPH_DEBUG_RENDERER"
		optimize "on"

	filter "configurations:Shipping"
		optimize "On"