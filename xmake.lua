
set_project("MiningEngine")
set_version("0.1.0")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

-- Dependencies
add_requires("glfw 3.4")

add_requires("glad 2.0.8", {
    configs = {
        gl_version = "3.3",
        gl_profile = "core"
    }})

add_requires("glm")
add_requires("tinyobjloader v2.0.0rc13")
add_requires("stb")

---Engine---

-- Engine Core
target("MiningEngine.Core")
set_kind("static")

add_files("src/Core/**.cpp")
add_headerfiles("src/Core/**.h", {install = false})
add_headerfiles("src/Core/**.hpp", {install = false})

add_packages("glfw", "glad", "glm", "tinyobjloader", "stb", {public = true})
add_links("opengl32", {public = true})
add_includedirs("src/Core", {public = true})

target_end()

-- Editor
target("MiningEngine.Editor")
set_kind("binary")

add_files("src/Editor/**.cpp")
add_headerfiles("src/Editor/**.h", {install = false})
add_headerfiles("src/Editor/**.hpp", {install = false})

add_deps("MiningEngine.Core")

add_extrafiles("assets/**")
set_rundir("$(projectdir)")

target_end()