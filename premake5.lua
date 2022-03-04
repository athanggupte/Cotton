include "scripts/options.lua"
include "scripts/actions.lua"

workspace "Cotton"
    architecture "x64"
    configurations {
        "Debug",
        "Release"
    }

local os_platform_Map = {
    windows = 'win',
    linux = 'unix',
    macosx = 'unix'
}

TARGET_ARCH = _OPTIONS['target-arch']
TARGET_PLATFORM = _OPTIONS['target-platform'] or os_platform_Map[_TARGET_OS]

print('Target arch: '..TARGET_ARCH)
print('Target platform: '..TARGET_PLATFORM)

if _OPTIONS["generate-platform-code"] then
group "Platform"
include "platform"
end

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group ""
project "Cotton"
    location ""
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/"..outputdir.."/%{prj.name}")
    objdir ("%{wks.location}/obj/"..outputdir.."/%{prj.name}")

    files {
        "src/**.cpp",
        "src/**.h",
        "include/Cotton/**.h",
        "platform/embed/context__%{TARGET_ARCH}_%{TARGET_PLATFORM}.c",
    }

    includedirs {
        "src/",
        "include/Cotton",
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "On"


if _OPTIONS["skip-tests"] == "false" then
group "testing"
include "tests"
end