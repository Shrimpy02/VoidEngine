#pragma once
#include <Camera.h>
#include <SceneGraph.h>
#include <memory>
#include <Controller.h>
#include <Mesh.h>
#include <ActorController.h>
#include <CameraController.h>
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"

class Scene
{
public:
    // ---------- Global Variables --------------

    SceneGraph mSceneGraph;
    CameraActor mSceneCamera{ "SceneCamera" };
    Window* mWindow = nullptr;



private:
    // ---------- Local Variables --------------

    MeshActor* mCube0{ nullptr };
    MeshActor* mCube1{ nullptr };
    MeshActor* mCube2{ nullptr };
    class Shader* mShader{ nullptr };

    PointLightActor* mPointLightActor0{ nullptr };
    PointLightActor* mPointLightActor1{ nullptr };
    DirectionalLightActor* mDirectionalLightActor{ nullptr };

    std::shared_ptr<ActorController> mActorController;
    std::shared_ptr<CameraController> mCameraController;

    glm::vec3 down{ 0,-1,0 };

    // ImGui UI Variables
    // ------------------------------------

    int mMainSelectionIndex = 0;
    int mOldSelectionIndex;
    int mCurrentSelectionIndex;
    float mCurrentUniformScale = 1;
    float mOldUniformScale = 1;

    glm::vec3 mActorOriginalScale;
    const float mItemWidth = 80.0f;

	bool mShouldShowWireFrame = false;
    bool mShouldDrawCollisionDebugMesh = false;
	bool mCanControlActor = false;
    bool mIsUniformScale = false;

    int shint = 0;
protected:

    std::shared_ptr<IController> mActiveController{ nullptr };

public:
    // ---------- Global functions --------------

    explicit Scene(const std::string& _name, Window* _window);
    virtual ~Scene() = default;

    // Removes the ability to:
    // copy
    Scene(const Scene&) = delete;
    // copy ref
    Scene& operator=(const Scene&) = delete;
    // move
    Scene(Scene&&) = delete;
    // move ref
    Scene& operator=(Scene&&) = delete;
    // Because this class is explicit.

    virtual void LoadContent();
    virtual void UnloadContent();

    void UpdateInputController(float _dt);
    void UpdateSceneGraph(Actor* _actor, float _dt, Transform _globalTransform = Transform{});
    void RenderSceneGraph(Actor* _actor, float _dt, Transform _globalTransform = Transform{});
    void Update(float _dt);
    void Render(float _dt);
    void RenderUI();
	void imgui_WorldObjectSettings();
    void imguiSub_WorldDetails(Actor* _aptr);
    void imguiSub_Collision(IBounded* _cptr);
    void imguiSub_Light(Light* _lptr);
	void imgui_Logger();

    void HandleCollision();

    void FramebufferSizeCallback(class Window* _window, int _width, int _height);
    void MouseMoveCallback(class Window* _window, double _xpos, double _ypos);
    void MouseButtonCallback(class Window* _window, int _button, int _action, int _mods);
    void MouseScrollCallback(class Window* _window, double _xoffset, double _yoffset);
    void CharCallback(class Window* _window, unsigned int codepoint);
	void KeyCallback(class Window* _window, int _key, int _scancode, int _action, int _mods);

private:
    // ---------- Local functions --------------

    void BindDirectionalLights();
    void BindPointLights();
    void BindCamera();

public:
    // ---------- Getters / setters / Adders --------------

    // Adders
   
    // Setters
    void SetController(const std::shared_ptr<IController>& _controller) { mActiveController = _controller; }
    void SetWindow(class Window* _window) { mWindow = _window;  }

    // Getters
    std::shared_ptr<IController> GetController() const { return mActiveController; }

};

