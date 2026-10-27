
set_project("MiningEngine")
set_version("0.1.0")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

add_requires("glfw 3.4", {
    configs = {
        glfw_include = "system"
    }
})

target("MiningEngine")
    set_kind("binary")
    add_files("MiningEngine/main.cpp")
    add_packages("glfw")
    add_links("opengl32")
