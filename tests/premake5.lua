--[[
    The Google test project without main function.
    To be used for tests which require a custom main function
--]]
project "gtest"
    location "../dependencies/googletest"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    files {
        "%{prj.location}/googletest/include/**.h",
        -- "%{prj.location}/googlemock/include/**.h",
        "%{prj.location}/googletest/src/gtest-all.cc",
    }

    includedirs {
        "%{prj.location}/googletest/include",
        "%{prj.location}/googletest",
    }
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "On"


project "Tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/"..outputdir.."/%{prj.name}")
    objdir ("%{wks.location}/obj/"..outputdir.."/%{prj.name}")

    files {
        "src/**.cpp",
        "src/**.h",
    }

    includedirs {
        "%{wks.location}/src/",
        "%{wks.location}/include/Cotton",
        "%{wks.location}/dependencies/googletest/googletest/include",
    }

    links {
        "Cotton",
        "gtest"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "system:linux"
        links {
            "pthread",
        }
