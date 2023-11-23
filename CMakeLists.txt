cmake_minimum_required(VERSION 3.0)

# Set the path to the GLFW folder
set(GLFW ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw-3.3.8)

# Set the path to the GLAD folder
set(GLAD ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad)

# Set the path to the GLM folder
set(GLM ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm)

# Set the path to the ImGui folder
set(IMGUI ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui)

project(
    GL2
    VERSION 0.2
    DESCRIPTION "The GL trials continue")
    
# Add the include directory
include_directories(include)

# Add the executable with all sources from src
file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

# Add the include and library directories for GLFW
include_directories(${GLFW}/include)
link_directories(${GLFW}/lib-vc2022)
# Link the GLFW library
target_link_libraries(main ${GLFW}/lib-vc2022/glfw3.lib)

# Add the include directory for GLAD
include_directories(${GLAD}/include)
# Add the glad.c file to the sources
target_sources(main PRIVATE ${GLAD}/src/glad.c)

# Add the include directory for GLM
include_directories(${GLM})

# Add the sources for ImGui
file(GLOB IMGUI_SOURCES ${IMGUI}/*.cpp ${IMGUI}/backends/imgui_impl_glfw.cpp ${IMGUI}/backends/imgui_impl_opengl3.cpp)
file(GLOB IMGUI_HEADERS ${IMGUI}/*.h ${IMGUI}/backends/imgui_impl_glfw.h ${IMGUI}/backends/imgui_impl_opengl3.h)
target_sources(main PUBLIC ${IMGUI_HEADERS})
target_sources(main PUBLIC ${IMGUI_SOURCES})
include_directories(${IMGUI})

# Copy src/shaders to the place where the executable is (cmake wizardry)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders $<TARGET_FILE_DIR:main>/shaders)