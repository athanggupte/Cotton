-- ======================================
--           Custom actions
-- ======================================
newaction {
    trigger     = "run-tests",
    description = "Run all unit tests",
    execute = function ()

        if os.istarget "linux" then
            os.execute "./run_tests.sh"
        elseif os.istarget "windows" then
            os.execute "run_tests.bat"
        end

    end
}

newaction {
    trigger     = "clean",
    description = "Clean all build and output files the workspace",
    execute = function ()

        local files_to_del = {
            -- gmake
            "Makefile",
            "*.make",
            -- Visual Studio
            "*.sln",
            "*.vcxproj",
            "*.vcxproj.user",
            "*.vcxproj.filters",
            -- xcode
        }

        local dirs_to_del = {
            outputs = {
                -- Outputs
                "bin",
                "platform/embed",
                "docs",
            },
            intermediate = {
                -- Intermediate Outputs
                "obj",
                "platform/obj",
            }
        }


        os.chdir(_MAIN_SCRIPT_DIR)
        for _,v in ipairs(dirs_to_del.intermediate) do
            os.rmdir(v)
        end
        if _OPTIONS['skip-outputs'] then
            for _,v in ipairs(dirs_to_del.outputs) do
                os.rmdir(v)
            end
        end 

        if os.istarget "linux" then
            for i,v in ipairs(files_to_del) do
                os.execute("find . -type f -name '"..v.."' -delete")
            end
        elseif os.istarget "windows" then
            for i,v in ipairs(files_to_del) do
                os.execute("del /f/s/q \""..v.."\"")
            end
        end
        os.chdir(_WORKING_DIR)

    end
}