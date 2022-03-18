project "Effekseer"
	language "C++"
	kind "SharedLib"
	targetname "Effekseer"
	targetdir(buildpath("mta"))

	filter {}
		includedirs {
			"../../vendor/effekseer/src/include",
			"../../vendor/effekseer/src/Effekseer",
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}


	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	links { "effekseer_vendor" }

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
