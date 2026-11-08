
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

-- Engine
target("MiningEngine")
set_kind("binary")

add_files("MiningEngine/main.cpp")
add_files("MiningEngine/Rendering/Shader.cpp")
add_files("MiningEngine/Rendering/mesh.cpp")
add_files("MiningEngine/Rendering/Camera.cpp")

add_packages("glfw", "glad", "glm")
add_links("opengl32")

set_rundir("$(projectdir)")
