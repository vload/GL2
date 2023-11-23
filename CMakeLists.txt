cmake_minimum_required(VERSION 3.0)

# Set the path to the GLFW folder
set(GLFW ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw-3.3.8)

# Set the path to the GLAD folder
set(GLAD ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad)

# Set the path to the GLM folder
set(GLM ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm)

project(
    GL2
    VERSION 0.1
    DESCRIPTION "The GL trials continue")

# Add the include and library directories for GLFW
include_directories(${GLFW}/include)
link_directories(${GLFW}/lib-vc2022)

# Add the include directory for GLAD
include_directories(${GLAD}/include)

# Add the include directory for GLM
include_directories(${GLM})

# Add the include directory
include_directories(include)

# Add the executable with all sources from src
file(GLOB SOURCES "src/*.cpp")

# Add the glad.c file to the sources
list(APPEND SOURCES ${GLAD}/src/glad.c)

add_executable(main ${SOURCES})

# Link the GLFW library
target_link_libraries(main ${GLFW}/lib-vc2022/glfw3.lib)

# Copy src/shaders to the place where the executable is (cmake wizardry)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders $<TARGET_FILE_DIR:main>/shaders)