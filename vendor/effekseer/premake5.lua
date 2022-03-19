project "Effekseer"
	language "C++"
	kind "StaticLib"
	targetdir(buildpath("mta"))
	targetname "effekseer"

	defines { "WIN32", "_WIN32", "_WIN32_WINNT=0x601", "_MSC_PLATFORM_TOOLSET=$(PlatformToolsetVersion)", "__EFFEKSEER_USE_LIBPNG__", "__EFFEKSEER_RENDERER_INTERNAL_LOADER__" }
	links { "libpng", "zlib", "d3dx9", "d3d9" }

	includedirs {
		"src/include",
		"src/Effekseer",
		"../libpng"
	}

	vpaths {
		["Headers/*"] = "src/include/**.h",
		["Headers/*"] = "src/**.h",
		["Sources/*"] = "src/**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"src/Effekseer/**.cpp",
		"src/Effekseer/**.h",
		"src/EffekseerRendererCommon/**.cpp",
		"src/EffekseerRendererCommon/**.h",
		"src/EffekseerRendererDX9/**.cpp",
		"src/EffekseerRendererDX9/**.h",
		"src/EffekseerMaterialCompiler/**.cpp",
		"src/EffekseerMaterialCompiler/**.h",
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }