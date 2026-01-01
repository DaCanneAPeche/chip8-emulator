add_rules("mode.debug", "mode.release")
add_requires("libsdl")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
target("chip-8")
    set_kind("binary")
    add_files("Source/*.c")
    add_includedirs("Header")
    add_packages("libsdl")

