-- =========================================================
-- GLOBAL SETTINGS (Must be outside the target)
-- =========================================================
set_xmakever("2.8.0")

-- Define global rules and configurations
add_rules("mode.debug", "mode.release")

-- Global policy to force module scans
set_policy("build.c++.modules", true)

option("enable_vulkan")
    set_default(false) -- Turned off by default to save compile time!
    set_showmenu(true)
    set_description("Compile and link the Vulkan rendering module")

-- =========================================================
-- TARGET DEFINITION
-- =========================================================
target("BTC")
    set_kind("binary")
    set_languages("c++23")
    add_cxflags("-municode")
    -- Force the output straight into a completely flat root build directory
    set_targetdir("$(projectdir)/build")
    
    -- Keep object tracking separate so debug/release switches don't clash
    set_objectdir("$(projectdir)/build/.objs/$(plat)/$(arch)/$(mode)") 

    -- Enforce C++23 Module resolution tree scanning
    set_policy("build.c++.modules", true)

    -- Use the platform default toolchain (configured via terminal config commands)
    set_toolchains("gcc")
    if is_mode("release") then 
        add_cxflags("-O2")
        add_cxxflags("-O2")
    else 
        add_cxflags("-g")
        add_cxxflags("-g")
        add_cxflags("-O0")
        add_cxxflags("-O0")
    end
    -- =========================
    -- CONFIGURATION MACROS
    -- =========================
    if is_mode("release") then
        add_defines("KTM_RELEASE")
    else
        add_defines("KTM_DEBUG")
    end
    
    -- Enforce sol2 Lua 5.3 compilation framework globally
    add_defines("SOL_LUA_VERSION=503")

    -- =========================
    -- FILES (.ixx/.cppm modules will automatically map here)
    -- =========================
    add_files("src/modules/*.cppm" , "src/modules/UI.Compiler/*.cppm", {public = true})
    add_files("src/main.cpp")
    add_files("src/_deps_cpp/*.cpp")
    add_files("src/include_cpp/*.cpp")

    -- Compile raw C dependency files independently without throwing C++ module errors
    if has_config("enable_vulkan") then
        add_files("src/_deps_cpp/*.c")
    else
        add_files("src/_deps_cpp/gl.c")
    end

    -- =========================
    -- INCLUDES
    -- =========================
    add_includedirs(
        "include",
        "_deps",
        "_deps/lua",
        "_deps/glad",
        "_deps/glfw",
        "_deps/imgui",
        "_deps/imgui/backends",
        "_deps/lua/src"  -- wherever lua.h/lauxlib.h/lualib.h live
    )

    -- =========================
    -- LINKING & LIBRARIES
    -- =========================
    -- Local project directories MUST be searched first to avoid picking up MSYS2's Lua 5.4/5.5
    add_ldflags("$(projectdir)/_deps/lua/src/liblua.a")
    add_linkdirs("libs")
    add_linkdirs("C:/msys64/ucrt64/lib")      -- System paths searched LAST
    
    add_links("glfw3")
    
    -- System links are automatically ignored on non-Windows platforms
    add_syslinks("gdi32", "opengl32", "winmm", "user32", "shell32")

    -- =========================================================
    -- POST-BUILD ASSETS & RUNTIME DEPENDENCY PIPELINE
    -- =========================================================
    after_build(function (target)
        local build_assets = path.join(target:targetdir(), "assets")
        local build_ui     = path.join(target:targetdir(), "UI")
        local build_msdf   = path.join(build_assets, "msdf-gen")
        local build_docs = path.join(target:targetdir(), "_docs")
        os.mkdir(build_assets)
        os.mkdir(build_ui)
        os.mkdir(build_msdf)
        os.mkdir(build_docs)

        -- Copy runtime library binaries next to execution path targets
        if os.isfile("libs/glfw3.dll") then
            os.cp("libs/glfw3.dll", target:targetdir())
        end

        local proj_assets   = path.join(os.projectdir(), "assets")
        local backup_assets = path.join(os.projectdir(), "assets_backup")

        local proj_ui       = path.join(os.projectdir(), "UI")
        local backup_ui     = path.join(os.projectdir(), "UI_backup")

        local proj_msdf     = path.join(os.projectdir(), "_deps", "msdf-atlas-gen")

        local proj_docs  = path.join(os.projectdir(), "_docs")

        local use_backup = false

        if is_mode("release") then
            -- Assets
            if os.isdir(proj_assets) then
                os.cp(path.join(proj_assets, "*"), build_assets)
            end

            -- UI
            if os.isdir(proj_ui) then
                os.cp(path.join(proj_ui, "*"), build_ui)
            end

            -- MSDF Atlas Generator
            if os.isdir(proj_msdf) then
                os.cp(path.join(proj_msdf, "*"), build_msdf)
            end

            -- _docs
            if os.isdir(proj_docs) then
                os.cp(path.join(proj_docs, "*"), build_docs)
            end
        else
            if use_backup then
                if os.isdir(backup_assets) then
                    os.cp(path.join(backup_assets, "*"), build_assets)
                end

                if os.isdir(backup_ui) then
                    os.cp(path.join(backup_ui, "*"), build_ui)
                end
            else
                if os.isdir(proj_assets) then
                    os.cp(path.join(proj_assets, "*"), build_assets)
                end

                if os.isdir(proj_ui) then
                    os.cp(path.join(proj_ui, "*"), build_ui)
                end
            end

            -- MSDF Atlas Generator
            if os.isdir(proj_msdf) then
                os.cp(path.join(proj_msdf, "*"), build_msdf)
            end

            -- _docs
            if os.isdir(proj_docs) then
                os.cp(path.join(proj_docs, "*"), build_docs)
            end
        end
    end)
