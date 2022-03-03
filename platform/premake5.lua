local GCC
local PY

if _OPTIONS["generate-platform-code"] then
    if os.ishost("windows") then
        GCC = _OPTIONS["gccpath"] or os.getenv("GCC_PATH")
        if not GCC then 
            print "GCC path could not be found in environment variables. Please provide the path to gcc using --gccpath=PATH or set the environment variable GCC_PATH"
            os.exit(1)
        end
        GCC = path.join(GCC, "gcc")
        PY = "python "
    else
        GCC = "gcc"
        PY = "./"
    end
    print("Using GCC : '" .. GCC .. "'")
end

if _ACTION == "gmake2" or _ACTION == "gmake" then
gen_module = require("generate_makefile")
mkfile_str = gen_module.generate_makefile(string.format("%s_%s", TARGET_ARCH, TARGET_PLATFORM))
mkfile = io.open("ContextEmbedded.make", "w")
mkfile:write(mkfile_str)

project "ContextEmbedded"
    kind "Makefile"

    buildcommands {
        "@make -f ContextEmbedded.make"
    }
    buildoutputs {
        "embed/context__%{TARGET_ARCH}_%{TARGET_PLATFORM}.c"
    }
    cleancommands {
        "@make -f ContextEmbedded.make clean"
    }


else -- vs20xx, xcode
project "ContextEmbedded"
    kind "Utility"

    files {
        "get_context__%{TARGET_ARCH}_%{TARGET_PLATFORM}.S",
        "set_context__%{TARGET_ARCH}_%{TARGET_PLATFORM}.S",
        "swap_context__%{TARGET_ARCH}_%{TARGET_PLATFORM}.S",
    }

    prebuildmessage 'Creating output directories...'
    prebuildcommands {
        '{MKDIR} obj embed',
    }

    postbuildmessage "Amalgamating generated source files..."
    postbuildcommands {
        "%{PY}%{wks.location}/scripts/amalgamate_embedded_files.py %{TARGET_ARCH}_%{TARGET_PLATFORM} embed"
    }

    filter "files:*.S"
        buildmessage "Compiling %{file.relpath}"

        buildcommands {
            '{MKDIR} obj embed',
            GCC .. ' -x assembler-with-cpp -o "obj/%{file.basename}.obj" -c "%{file.relpath}"',
            "%{PY}%{wks.location}/scripts/extract_objdump_from_file.py obj/%{file.basename}.obj %{file.basename} --strip-symbol"
        }

        buildoutputs {
            "embed/%{file.basename}.c",
        }

end