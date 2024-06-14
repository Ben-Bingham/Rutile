workspace "Rutile-Renderer"
	configurations { "Debug", "Release" }
	platforms "x64"

	filter "platforms:x64"
		architecture "x64"
	filter "configurations:Debug"
		symbols "On"
	filter {}

	filter "configurations:Release"
		optimize "On"
	filter {}

	startproject "Rutile"

project "Rutile"
	kind "ConsoleApp"
	language "C++"

	cppdialect "C++20"

	targetdir ("%{wks.location}/build/bin/%{prj.name}")
	objdir ("%{wks.location}/build/bin-int/%{prj.name}")

	filter "configurations:Debug"
		symbols "On"
	filter {}
	
	filter "configurations:Release"
		optimize "On"
	filter {}

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		"src"
	}
