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
		symbols "on"

	filter "configurations:Release"
		optimize "on"
		
	filter "configurations:Debug-Editor"
		symbols "on"

	filter "configurations:Release-Editor"
		optimize "on"

	filter "configurations:Shipping"
		optimize "On"