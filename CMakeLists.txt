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
    src/Components/PhysicsComponent.cpp
    "src/OutDated/SceneActors.cpp" 
    "src/OutDated/BoundingSphere.cpp" "src/Core/SMath.cpp" "src/Components/AIComponent.cpp" "src/SkyBox/Skybox.cpp" "src/Levels/LevelManager.cpp"  "src/LevelActors/SceneGraph.cpp" "src/UserInterface/UserInterfaceManager.cpp"   "src/Collision/Collision.cpp"  "src/LevelActors/BaseActor.cpp" "src/LevelActors/VisualActor.cpp"  "src/LevelActors/GraphActor.cpp" "src/RenderElements/GraphPoint.cpp" "src/LevelActors/DebugActor.cpp")

set (HEADER_FILES
  includes/stb_image.h
  "src/LevelActors/CameraActor.h"
  src/Core/Window.h
  src/Core/Application.h
  "src/RenderElements/Material.h"
  "src/RenderElements/Mesh.h"
  src/Renderer.h
  "src/OutDated/Scene.h"
  "src/RenderElements/Texture.h"
  src/Actor.h
  src/Utilities/Types.h
  src/Transform.h
  "src/Core/Tag.h"
  "src/RenderElements/Vertex.h"
  src/Controllers/Controller.h
  "src/OutDated/CameraController.h"
  src/Collision/Collision.h 
  "src/OutDated/AABB.h" 
  src/Controllers/ActorController.h
  src/Lights/Lights.h 
  src/Lights/DirectionalLight.h 
  src/Lights/PointLight.h  
  src/Utilities/Defines.h
  src/ModelLoader/AssimpLoader.h
  src/ModelLoader/AssimpUtility.h
  src/ModelLoader/Shared.h
  src/Utilities/Logger.h
  src/Components/PhysicsComponent.h
  src/Components/Component.h
  "src/OutDated/SceneActors.h"
   "src/Core/SMath.h" "src/Components/AIComponent.h" "src/SkyBox/Skybox.h"  "src/Levels/LevelManager.h" "src/Levels/Level.h" "src/LevelActors/SceneGraph.h" "src/UserInterface/UserInterfaceManager.h"   "src/Collision/CollisionProperties.h"  "src/LevelActors/BaseActor.h" "src/LevelActors/VisualActor.h"  "src/LevelActors/GraphActor.h" "src/RenderElements/GraphPoint.h" "src/RenderElements/GraphVertex.h" "src/LevelActors/DebugActor.h" "src/RenderElements/DebugVertex.h")

set (SRC_FILES
  3DProgEngine.cpp
  includes/glad.c
  src/Core/Shader.cpp
  "src/LevelActors/CameraActor.cpp"
  src/Core/Window.cpp
  src/Core/Application.cpp
  "src/RenderElements/Texture.cpp" 
  "src/RenderElements/Material.cpp"     
  "src/RenderElements/Mesh.cpp"
  "src/OutDated/Scene.cpp"
  src/Actor.cpp
  "src/OutDated/CameraController.cpp"
  src/Controllers/ActorController.cpp
  src/ModelLoader/AssimpLoader.cpp
  "src/OutDated/AABB.cpp"
)

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
