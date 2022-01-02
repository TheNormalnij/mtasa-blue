project "effekseer"
	language "C++"
	kind "StaticLib"
	targetname "effekseer"

	defines { "WIN32", "_WIN32", "_WIN32_WINNT=0x601", "_MSC_PLATFORM_TOOLSET=$(PlatformToolsetVersion)" }

	includedirs {
		"src/include",
		"src/Effekseer",
	}

	vpaths {
		["Headers/*"] = "src/include/**.h",
		["Headers/*"] = "src/**.h",
		["Sources/*"] = "src/**.cpp",
		["*"] = "premake5.lua"
	}

	links {
		"d3dx9", "d3d9"
	}

	files {
		"premake5.lua",
		"*.h",
		"src/Effekseer/**.cpp",
		"src/Effekseer/**.h",
		"src/EffekseerRendererDX9/**.cpp",
		"src/EffekseerRendererDX9/**.h",
		"src/EffekseerMaterialCompiler/**.cpp",
		"src/EffekseerMaterialCompiler/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }