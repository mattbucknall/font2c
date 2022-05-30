add_executable(font2c
        app-arg-parser.cpp
        app-canvas.cpp
        app-char-set.cpp
        app-error.cpp
        app-font.cpp
        app-ft-lib.cpp
        app-glyph.cpp
        app-options.cpp
        app-output-model.cpp
        app-preview.cpp
        main.cpp
)

target_link_libraries(font2c PRIVATE fmt spng ${FREETYPE_LIBRARIES})
target_include_directories(font2c PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_compile_options(font2c PRIVATE ${FREETYPE_CFLAGS_OTHER})
