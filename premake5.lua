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

	defines {
		"GLEW_STATIC"
	}

	includedirs {
		"src",
		"3rdParty/GLEW/include",
		"3rdParty/GLFW/include",
		"3rdParty/stb"
	}

	libdirs {
        "3rdParty/GLFW/lib-vc2022",
        "3rdParty/GLEW/lib/Release/x64"
    }

	links {
        "glew32s",
        "opengl32",
        "glfw3"
    }
