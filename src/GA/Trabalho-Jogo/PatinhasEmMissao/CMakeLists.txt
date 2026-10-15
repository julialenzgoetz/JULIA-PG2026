cmake_minimum_required(VERSION 3.16)
project(PatinhasEmMissao VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# As bibliotecas estão incluídas: não se baixa nada durante a compilação.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
option(PATINHAS_HEADLESS_ONLY "Somente para testes Linux sem servidor grafico" OFF)
if(PATINHAS_HEADLESS_ONLY)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
endif()
add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL)

add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)
add_library(game_logic STATIC src/Game.cpp)
target_include_directories(game_logic PUBLIC src)

add_executable(PatinhasEmMissao src/main.cpp src/Renderer.cpp)
target_include_directories(PatinhasEmMissao PRIVATE third_party third_party/glm)
target_link_libraries(PatinhasEmMissao PRIVATE game_logic glfw glad ${CMAKE_DL_LIBS})
target_compile_definitions(PatinhasEmMissao PRIVATE PATINHAS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
if(MSVC)
    target_compile_options(PatinhasEmMissao PRIVATE /W4 /utf-8)
    target_compile_options(game_logic PRIVATE /W4 /utf-8)
else()
    target_compile_options(PatinhasEmMissao PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(game_logic PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(MINGW)
    # Evita depender de DLLs de libstdc++, libgcc e winpthread no Windows.
    target_link_options(PatinhasEmMissao PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Recursos ficam junto ao binário (incluindo Debug/Release no Visual Studio).
add_custom_command(TARGET PatinhasEmMissao POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:PatinhasEmMissao>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:PatinhasEmMissao>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:PatinhasEmMissao>/config)

include(CTest)
if(BUILD_TESTING)
    add_executable(game_tests tests/game_tests.cpp)
    target_link_libraries(game_tests PRIVATE game_logic)
    add_test(NAME regras_do_jogo COMMAND game_tests ${CMAKE_SOURCE_DIR}/config/game.cfg)
endif()
