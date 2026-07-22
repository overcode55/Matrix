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
    set_targetdir("$(projectdir)/build")
    
    set_objectdir("$(projectdir)/build/.objs/$(mode)") 

    set_policy("build.c++.modules", true)

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
    if is_mode("release") then
        add_defines("KTM_RELEASE")
    else
        add_defines("KTM_DEBUG")
    end
    
    add_defines("SOL_LUA_VERSION=503")

    add_files("src/**.c")
    add_files("src/**.cpp")
    add_files("src/**.cppm")

    if has_config("enable_vulkan") then
        add_defines("MATRIX_USE_VULKAN");
    end
    
    add_includedirs(
        "include",
        "_deps",
        "_deps/lua",
        "_deps/glad",
        "_deps/glfw",
        "_deps/imgui",
        "_deps/imgui/backends",
        "_deps/lua/src"  
    )

    -- =========================
    -- LINKING & LIBRARIES
    -- =========================
    -- Local project directories MUST be searched first to avoid picking up MSYS2's Lua 5.4/5.5
    add_ldflags("$(projectdir)/_deps/lua/src/liblua.a")
    add_linkdirs("_libs")
    add_linkdirs("C:/msys64/ucrt64/lib")      -- System paths searched LAST
    
    add_links("glfw3")
    
    -- System links are automatically ignored on non-Windows platforms
    add_syslinks("gdi32", "opengl32", "winmm", "user32", "shell32")

    -- =========================================================
    -- POST-BUILD ASSETS & RUNTIME DEPENDENCY PIPELINE
    -- =========================================================
    after_build(function (target)
        local build_assets = path.join(target:targetdir(), "assets")
        local build_msdf   = path.join(build_assets, "msdf-gen")
        local build_docs = path.join(target:targetdir(), "_docs")
        local build_libs = path.join(target:targetdir(), "_libs")
        os.mkdir(build_assets)
        os.mkdir(build_msdf)
        os.mkdir(build_docs)
        os.mkdir(build_libs)


        local proj_assets   = path.join(os.projectdir(), "assets")

        local proj_msdf     = path.join(os.projectdir(), "_deps", "msdf-atlas-gen")

        local proj_docs     = path.join(os.projectdir(), "_docs")

        local proj_libs     = path.join(os.projectdir(), "_libs")

        -- Assets
        if os.isdir(proj_assets) then
            os.cp(path.join(proj_assets, "*"), build_assets)
        end

        -- MSDF Atlas Generator
        if os.isdir(proj_msdf) then
            os.cp(path.join(proj_msdf, "*"), build_msdf)
        end

        -- _docs
        if os.isdir(proj_docs) then
            os.cp(path.join(proj_docs, "*"), build_docs)
        end

        -- _libs
        if os.isdir(proj_libs) then
            os.cp(path.join(proj_libs, "*"), build_libs)
        end
    end)
