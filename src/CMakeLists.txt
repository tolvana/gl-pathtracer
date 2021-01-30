cmake_minimum_required(VERSION 3.16)

set(MAIN_FILES
    main.cpp
)

set(SOURCE_FILES
    windowcontext.cpp
    windowcontext.hpp
    pathtracer.cpp
    pathtracer.hpp
    scene.cpp
    scene.hpp
    shaders.hpp
)

add_executable(pt ${MAIN_FILES} ${SOURCE_FILES})

target_link_libraries(pt OpenGL::GL)
target_link_libraries(pt GLEW)
target_link_libraries(pt sfml-window)
target_link_libraries(pt sfml-system)

target_compile_options(pt PRIVATE -Wall)
