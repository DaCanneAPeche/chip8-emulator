add_rules("mode.debug", "mode.release")
add_requires("libsdl2", "libsdl2_ttf")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
target("chip-8")
    set_kind("binary")
    add_files("Source/*.c")
    add_includedirs("Header")
    add_packages("libsdl2", "libsdl2_ttf")

    after_build(function (target)
      if not os.exists("$(buildir)/$(plat)/$(arch)/$(mode)/assets") then
        os.ln("../../../../assets", "$(buildir)/$(plat)/$(arch)/$(mode)/assets")
      end
      if not os.exists("$(buildir)/$(plat)/$(arch)/$(mode)/exemple_programs") then
        os.ln("../../../../exemple_programs", "$(buildir)/$(plat)/$(arch)/$(mode)/exemple_programs")
      end
    end)

