project "JoltPhysics"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"

   targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")

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

   filter "configurations:Debug-Editor"
      runtime "Debug"
      symbols "on"

   filter "configurations:Release-Editor"
      runtime "Release"
      optimize "on"

   filter "configurations:Debug"
      runtime "Debug"
      symbols "on"

   filter "configurations:Release"
      runtime "Release"
      optimize "on"

   filter "configurations:Shipping"
      runtime "Release"
      optimize "Full"