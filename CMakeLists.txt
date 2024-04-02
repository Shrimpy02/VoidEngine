# CMakeList.txt : CMake project for 3DProgEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("3DProgEngine")

include_directories(includes)
include_directories(includes/imgui)
include_directories(src)
include_directories(libs/assimp/include)

#Add IMGUI headers
set( IMGUI_HEADERS
    includes/imgui/imgui.h
    includes/imgui/imconfig.h
    includes/imgui/imgui_impl_opengl3.h
    includes/imgui/imgui_impl_glfw.h
    includes/imgui/imgui_impl_opengl3_loader.h
)

#Add IMGUI sources
set( IMGUI_SOURCE
    includes/imgui/imgui.cpp
    includes/imgui/imgui_demo.cpp
    includes/imgui/imgui_draw.cpp
    includes/imgui/imgui_widgets.cpp
    includes/imgui/imgui_impl_opengl3.cpp
    includes/imgui/imgui_impl_glfw.cpp
    includes/imgui/imgui_tables.cpp
 "src/Logger.cpp")

set (HEADER_FILES
  includes/stb_image.h
  src/Camera.h
  src/Window.h
  src/Application.h
  src/Material.h
  src/Mesh.h
  src/Renderer.h
  src/Scene.h
  src/Texture.h
  src/Actor.h
  src/Types.h  
  src/Transform.h
  src/Tag.h
  src/Vertex.h
  src/SceneGraph.h
  src/Controller.h 
  src/CameraController.h 
  src/Collision.h 
  src/AABB.h 
  src/ActorController.h
  src/Lights/Lights.h 
  src/Lights/DirectionalLight.h 
  src/Lights/PointLight.h  
  src/Defines.h "src/ModelLoader/AssimpLoader.h" "src/ModelLoader/AssimpUtility.h" "src/ModelLoader/Shared.h" "src/Logger.h")

set (SRC_FILES
  3DProgEngine.cpp
  includes/glad.c
  src/Shader.cpp
  src/Transform.cpp
  src/Camera.cpp
  src/Window.cpp
  src/Application.cpp
  src/Texture.cpp 
  src/Material.cpp     
  src/Mesh.cpp
  src/Scene.cpp
  src/SceneGraph.cpp
  src/Actor.cpp
  src/Collision.cpp
  src/CameraController.cpp
  src/ActorController.cpp
 "src/ModelLoader/AssimpLoader.cpp" "src/AABB.cpp")

add_subdirectory(libs/glfw-3.3.9)

#set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libs/assimp)

# Add source to this project's executable.
add_executable (3DProgEngine ${IMGUI_HEADERS} ${IMGUI_SOURCE} ${HEADER_FILES} ${SRC_FILES})

#target_link_libraries(3DProg-Main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Libs/glfw3.lib)
target_link_libraries(3DProgEngine glfw)
target_link_libraries(3DProgEngine assimp)
#target_link_libraries(3DProgEngine fbx)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 3DProgEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
