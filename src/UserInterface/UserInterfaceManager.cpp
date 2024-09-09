
// Includes
#include <UserInterface/UserInterfaceManager.h>
#include <Core/Shader.h>
#include <Levels/Level.h>
#include <LevelActors/SceneGraph.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/VisualActor.h>
#include <LevelActors/CameraActor.h>
#include <LevelActors/GraphActor.h>
#include <LevelActors/DebugActor.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Texture.h>
#include <RenderElements/Material.h>
#include <Actor.h>
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>
#include <Utilities/Logger.h>
#include <Utilities/Defines.h>
#include <Levels/LevelManager.h>
#include <Controllers/ActorController.h>
#include <Core/WindowManager.h>

#include <ModelLoader/AssimpLoader.h>

// Additional Includes


//  --------------------------- Constructors ----------------------------

UserInterfaceManager::UserInterfaceManager(std::shared_ptr<WindowManager> _inWindowManager)
	: mWindowManager(_inWindowManager)
{
}

UserInterfaceManager::~UserInterfaceManager()
{
	// ImGui shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//  --------------------------- Global ----------------------------

void UserInterfaceManager::Init(GLFWwindow* _glfwWindow)
{
	mController = std::make_shared<ActorController>(mWindowManager,shared_from_this());
	mLevelManager = std::make_shared<LevelManager>(mController);
	ImGuiInit(_glfwWindow);
	InitImages();

}

void UserInterfaceManager::InitImages()
{
	mLogoIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_VoidLogo_Icon.png"));
	mFolderIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_Folder_Icon.png"));
	mJPGIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_JPG_Icon.png"));
	mPNGIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_PNG_Icon.png"));
	mFBXIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_FBX_Icon.png"));
	mLVLIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_Lvl_Icon.png"));
	mErrorIcon = Texture::LoadImage(SOURCE_DIRECTORY("EngineAssets/EngineIcons/Temp_ERROR_Icon.png"));
}

void UserInterfaceManager::LoadContent()
{
	if (mLevelManager)
	{
		mLevelManager->LoadContent();
		mDefaultShader = mLevelManager->GetDefaultShader();
		mController->GetLevelManager(mLevelManager);
	}
}

void UserInterfaceManager::StartFrame()
{
	// Tell`s ImGui this is the first frame of the render loop
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UserInterfaceManager::Update(float _dt)
{
	if (mController)
		mController->ProcessInput(_dt);

	if (mLevelManager)
		mLevelManager->Update(_dt);
}

void UserInterfaceManager::Render()
{
	RenderUI();
}

void UserInterfaceManager::EndFrame()
{
	// Tell`s ImGui this is the last frame of the render loop
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Handle platform windows (for multi-viewport)
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void UserInterfaceManager::FrameBufferCallback(GLFWwindow* _window, int _width, int _height)
{
	ResizeFrameBuffer(_width,_height);
}

void UserInterfaceManager::MouseMoveCallback(GLFWwindow* _window, double _xPos, double _yPos)
{
	// Actor Controller if using viewport
	if (mController && mViewportHasFocusAndHover)
		mController->HandleMouseMove( _xPos, _yPos);
}

void UserInterfaceManager::MouseScrollCallback(GLFWwindow* _window, double _xOffset, double _yOffset)
{
	// Passes all scroll callbacks to ImGui
	ImGui_ImplGlfw_ScrollCallback(_window, _xOffset, _yOffset);

	if (mController && mViewportHasFocusAndHover)
		mController->HandleMouseScroll(_xOffset, _yOffset);
}

void UserInterfaceManager::CharCallback(GLFWwindow* _window, unsigned int _codePoint)
{
	// Passes all character keyboard callbacks to ImGui and stops glfw from using them (a,b,c so on)
	ImGui_ImplGlfw_CharCallback(_window, _codePoint);

	if (mController && mViewportHasFocusAndHover)
		mController->HandleChar(_codePoint);
}

void UserInterfaceManager::KeyCallback(GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods)
{
	// Passes all key-keyboard callbacks to ImGui and stops glfw from using them (backspace, enter so on)
	ImGui_ImplGlfw_KeyCallback(_window, _key, _scanCode, _action, _mods);
	//if (ImGui::GetIO().WantCaptureMouse) return;

	if (mController && mViewportHasFocusAndHover)
		mController->HandleKeyboard(_key, _scanCode, _action, _mods);
}

void UserInterfaceManager::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods, double _cursorPosX, double _cursorPosY)
{
	// Passes all mouse button callbacks to ImGui and stops glfw from using them
	ImGui_ImplGlfw_MouseButtonCallback(_window, _button, _action, _mods);

	if (mController && mViewportHasFocusAndHover)
		mController->HandleViewportClick(_button, _action, _mods, _cursorPosX, _cursorPosY);

	if (mController && mViewportHasFocusAndHover)
		mController->HandleMouseButton(_button, _action, _mods);
}

bool UserInterfaceManager::IsViewportFocusedAndHovered()
{
	return mViewportHasFocusAndHover;
}

//  --------------------------- Local ----------------------------

void UserInterfaceManager::ImGuiInit(GLFWwindow* _glfwWindow)
{
	// Init ImGui ------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Imgui
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport // Disabled for now because of overhead

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(_glfwWindow, false);
	ImGui_ImplOpenGL3_Init("#version 130");

	CreateViewportTexture();
}

void UserInterfaceManager::CreateViewportTexture()
{
	glGenFramebuffers(1, &mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	// Generate texture
	glGenTextures(1, &mViewportTexture);
	glBindTexture(GL_TEXTURE_2D, mViewportTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mViewportWidth, mViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach texture to frameBuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mViewportTexture, 0);

	// Create and attach depth buffer

	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mViewportWidth, mViewportHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

	// Error check
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Framebuffer not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind frameBuffer
}

void UserInterfaceManager::RenderUI()
{
	// Demo window for inspiration and explanation
	//ImGui::ShowDemoWindow();
	// Create dock space for UI windows
	ui_DockSpace();
	
	// Renders level to texture for viewport
	RenderLevelToTexture();

	// Render individual UI components
	ui_CreateActors();
	ui_FileExplorer();
	ui_ViewPort();
	ui_WorldProperties();
	ui_DebugProperties();
	ui_ContentBrowser();
	ui_ContentProperties();
	ui_Log();
	ui_Console();
}

void UserInterfaceManager::RenderLevelToTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(mLevelManager)
		mLevelManager->Render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void UserInterfaceManager::ResizeFrameBuffer(int newWidth, int newHeight)
{
	mViewportWidth = newWidth;
	mViewportHeight = newHeight;

	LOG("Resize");

	glViewport(0, 0, mViewportWidth, mViewportHeight);

	// Bind the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	// Resize the texture
	glBindTexture(GL_TEXTURE_2D, mViewportTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mViewportWidth, mViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	// Resize the depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mViewportWidth, mViewportHeight);

	// Check if the frame buffer is complete
	static bool dontCheckFirstFrame = false;
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE && dontCheckFirstFrame)
	{
		LOG_ERROR("Framebuffer not complete!");
		dontCheckFirstFrame = true;
	}

	// Unbind the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImVec2 UserInterfaceManager::CalculateAspectRatioSize(ImVec2 availableSize, float aspectRatio)
{
	float windowAspectRatio = availableSize.x / availableSize.y;
	if (windowAspectRatio > aspectRatio)
	{
		// Window is wider than the desired aspect ratio, adjust width
		float newWidth = availableSize.y * aspectRatio;
		return ImVec2(newWidth, availableSize.y);
	}
	else
	{
		// Window is taller than the desired aspect ratio, adjust height
		float newHeight = availableSize.x / aspectRatio;
		return ImVec2(availableSize.x, newHeight);
	}
}


void UserInterfaceManager::ui_DockSpace()
{

	// Get the main ImGui viewport
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Set the next window position and size to match the viewport (GLFW window)
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	// Create the root ImGui window
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// Adjust to fill the entire screen
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	// Begin the root dock space window
	ImGui::Begin("RootDockSpaceWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(2);

	ui_Sub_MainMenuBar();

	//Create dock space inside the root window
	const ImGuiID dockSpace_id = ImGui::GetID("RootDockSpace");
	ImGui::DockSpace(dockSpace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);


	// Initialize the dock space layout if it's not already set up
	if (!ImGui::DockBuilderGetNode(dockSpace_id)) // Only build docking layout if it doesn't exist
	{
		ImGui::DockBuilderRemoveNode(dockSpace_id); // Clear previous layout
		ImGui::DockBuilderAddNode(dockSpace_id, ImGuiDockNodeFlags_None); // Add a new dockspace node

		ImGuiID dock_main_id = dockSpace_id;
		const ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);
		const ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
		const ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

		// Dock the windows
		ImGui::DockBuilderDockWindow("LevelViewport", dock_main_id);
		ImGui::DockBuilderDockWindow("Content Browser", dock_id_left);
		ImGui::DockBuilderDockWindow("Properties", dock_id_left);
		ImGui::DockBuilderDockWindow("File Explorer", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Create Actor", dock_id_right);
		ImGui::DockBuilderDockWindow("Debug Properties", dock_id_right);
	
		ImGui::DockBuilderFinish(dockSpace_id);
	}

	ImGui::End();
}

void UserInterfaceManager::ui_Sub_MainMenuBar()
{
	// Create the menu bar
	if (ImGui::BeginMenuBar())
	{
		ui_Element_Image(mLogoIcon, mIconSize);

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N")) { /* New action */ }
			if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Open action */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Save action */ }

			if (ImGui::MenuItem("Exit", "Alt+F4")) {
				mController->ExitApplication();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Option 1")) { /* Option 1 action */ }
			if (ImGui::MenuItem("Option 2")) { /* Option 2 action */ }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			// Window Settings
			// ----------------------------
			bool windowed = mWindowManager->GetWindowMode() == WindowMode::WM_Windowed ? true : false;
			bool borderless = mWindowManager->GetWindowMode() == WindowMode::WM_BoarderLess ? true : false;;
			bool fullScreen = mWindowManager->GetWindowMode() == WindowMode::WM_FullScreen ? true : false;;

			if (ImGui::MenuItem("Windowed", "F9", &windowed))
				mWindowManager->SetWindowed();

			if (ImGui::MenuItem("Borderless Windowed", "F10", nullptr, &borderless))
				mWindowManager->SetBoarderLessWindow();

			if (ImGui::MenuItem("FullScreen", "F11", &fullScreen))
				mWindowManager->SetFullScreen();

			ImGui::Separator();

			// Window Visibilities
			// ----------------------------
			if (ImGui::MenuItem("Create Actor", nullptr, mIsCreateActorWindowOpen))
				mIsCreateActorWindowOpen = !mIsCreateActorWindowOpen;
			
			if (ImGui::MenuItem("View Port", nullptr, mIsViewportWindowOpen))
				mIsViewportWindowOpen = !mIsViewportWindowOpen;

			if (ImGui::MenuItem("World Properties", nullptr, mIsWorldPropertiesWindowOpen))
				mIsWorldPropertiesWindowOpen = !mIsWorldPropertiesWindowOpen;

			if (ImGui::MenuItem("Debug Properties", nullptr, mIsDebugPropertiesWindowOpen))
				mIsDebugPropertiesWindowOpen = !mIsDebugPropertiesWindowOpen;

			if (ImGui::MenuItem("Content Browser", nullptr, mIsContentBrowserWindowOpen))
				mIsContentBrowserWindowOpen = !mIsContentBrowserWindowOpen;

			if (ImGui::MenuItem("Content Properties", nullptr, mIsContentPropertiesWindowOpen))
				mIsContentPropertiesWindowOpen = !mIsContentPropertiesWindowOpen;

			if (ImGui::MenuItem("File Explorer", nullptr, mIsFileExplorerWindowOpen))
				mIsFileExplorerWindowOpen = !mIsFileExplorerWindowOpen;

			if (ImGui::MenuItem("Log", nullptr, mIsLogWindowOpen))
				mIsLogWindowOpen = !mIsLogWindowOpen;

			if (ImGui::MenuItem("Console", nullptr, mIsConsoleWindowOpen))
				mIsConsoleWindowOpen = !mIsConsoleWindowOpen;

				ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About")) { /* About dialog */ }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void UserInterfaceManager::ui_CreateActors()
{
	if (!mIsCreateActorWindowOpen) return;

	if (ImGui::Begin("Create Actor", &mIsCreateActorWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		// Get the available space in the window
		ImVec2 availableSize = ImGui::GetContentRegionAvail();

		// Determine the number of rows and columns
		int numColumns = 2; // Number of columns for buttons
		int numRows = (IM_ARRAYSIZE(mActorNames) + numColumns - 1) / numColumns;

		// Calculate the size of each button to fit as a square
		float buttonSize = fminf(availableSize.x / numColumns, availableSize.y / numRows) - 10; // Leave space for margins

		// Iterate through each item to create button and drag source
		for (int n = 0; n < IM_ARRAYSIZE(mActorNames); n++)
		{
			// Set up button
			ImGui::PushID(n);
			if (ImGui::Button(mActorNames[n], ImVec2(buttonSize, buttonSize)))
			{
				// Button click logic goes here
				//std::shared_ptr<BaseActor> baseActor = std::make_shared<BaseActor>("CreatedCube", Mesh::CreateCube(nullptr));
				//mLevelManager->AddActorToLevel(baseActor);
			}

			// Set up the drag source
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of the item
				ImGui::SetDragDropPayload("DND_CREATE_ITEM", &n, sizeof(int));

				// Display a preview during the drag operation
				ImGui::Text("Creating %s", mActorNames[n]);

				ImGui::EndDragDropSource();
			}
			ImGui::PopID();

			// Move to the next column
			if ((n + 1) % numColumns != 0)
			{
				ImGui::SameLine();
			}
		}
	}

	ImGui::End();
	
}


void UserInterfaceManager::ui_FileExplorer()
{
	if (!mIsFileExplorerWindowOpen) return;

	if (ImGui::Begin("File Explorer", &mIsFileExplorerWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		// Navigate up in the directory structure
		if (ImGui::Button("Up")) {
			std::string temp = std::filesystem::path(mCurrentDirectoryPath).parent_path().string();
			if (temp != GetCoreFilePath())
				mCurrentDirectoryPath = temp;
		}
		ImGui::SameLine();

		char buff[100] = { 0 };
		ImGui::InputText("Search", buff, IM_ARRAYSIZE(buff));
		ImGui::Text("Current Path: %s", mCurrentDirectoryPath.c_str());
		ImGui::Separator();

		// Gets the available size for the child window and button spacing
		const ImVec2 windowSize = ImGui::GetContentRegionAvail();

		if (ImGui::BeginChild("xxcd", windowSize))
		{
			std::vector<FileItem> items = GetDirectoryContents(mCurrentDirectoryPath);

			float availableSpace = windowSize.x;

			// Remove button background style for file explorer
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));         // Transparent button background
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));  // Transparent hover color
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));   // Transparent active (click) color

			for (int i = 0; i < items.size(); i++)
			{
				ImGui::PushID(i);

				// Make item + name a vertical group
				ImGui::BeginGroup();

				std::string fullPath = mCurrentDirectoryPath + "/" + items[i].mName;
				const char* fullItemDirectory = static_cast<const char*>(fullPath.c_str());

				// If item is a folder, click: go into its directory
				if (items[i].mIsDirectory)
				{
					if (ImGui::ImageButton((void*)(intptr_t)mFolderIcon->GetTextureID(), ImVec2(mItemIconSize, mItemIconSize), ImVec2(1.0f, 1.0f), ImVec2(0.0f, 0.0f)))
						mCurrentDirectoryPath = (std::filesystem::path(mCurrentDirectoryPath) / items[i].mName).string();

					// If item is jpg, click: 
				}
				else if (HasSuffix((items[i].mName).c_str(), "jpg")) {


					if (ImGui::ImageButton((void*)(intptr_t)Texture::Load(fullItemDirectory)->GetTextureID(), ImVec2(mItemIconSize, mItemIconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
					{
						
					}
					DrawBoarderAroundImage();

					// Set up the drag source
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						// Set payload to carry the index of the item
						ImGui::SetDragDropPayload("DND_DRAG_JPG", fullPath.c_str(), fullPath.size() + 1);

						// Display a preview during the drag operation
						ImGui::Text("Creating %s", items[i].mName.c_str());

						ImGui::EndDragDropSource();
					}

				}	// If item is png, click:
				else if (HasSuffix((items[i].mName).c_str(), "png")) {

					if (ImGui::ImageButton((void*)(intptr_t)Texture::Load(fullItemDirectory)->GetTextureID(), ImVec2(mItemIconSize, mItemIconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
					{
						
					}
					DrawBoarderAroundImage();

					// Set up the drag source
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						// Set payload to carry the index of the item
						ImGui::SetDragDropPayload("DND_DRAG_PNG", fullPath.c_str(), fullPath.size() + 1);

						// Display a preview during the drag operation
						ImGui::Text("Creating %s", items[i].mName.c_str());

						ImGui::EndDragDropSource();
					}

				}	// If item is fbx, click: drag into level
				else if (HasSuffix((items[i].mName).c_str(), "fbx")) {

					if (ImGui::ImageButton((void*)(intptr_t)mFBXIcon->GetTextureID(), ImVec2(mItemIconSize, mItemIconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
					{

					}

					// Set up the drag source
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						// Set payload to carry the index of the item
						ImGui::SetDragDropPayload("DND_DRAG_FBX", fullPath.c_str(), fullPath.size() + 1);

						// Display a preview during the drag operation
						ImGui::Text("Creating %s", items[i].mName.c_str());

						ImGui::EndDragDropSource();
					}

				}	// If item is lvl, click: open level
				else if (HasSuffix((items[i].mName).c_str(), "lvl")) {

					if (ImGui::ImageButton((void*)(intptr_t)mLVLIcon->GetTextureID(), ImVec2(mItemIconSize, mItemIconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
						LOG("lvl");

				}	// If item is undefined
				else {

					if (ImGui::ImageButton((void*)(intptr_t)mErrorIcon->GetTextureID(), ImVec2(mItemIconSize, mItemIconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
						LOG("error");

				}

				// Center the item name text horizontally under the icon
				if (items[i].mName.length() <= mMaxDisplayCharacters)
				{
					ImVec2 textSize = ImGui::CalcTextSize(items[i].mName.c_str());
					float textOffsetX = (mItemIconSize - textSize.x) * 0.5f;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffsetX);
					ImGui::TextWrapped("%s", items[i].mName.c_str());

					// Remove end of name if too long and append '..' to the end of it
				}
				else {
					std::string temp = items[i].mName.c_str();
					temp.erase(temp.begin() + mMaxDisplayCharacters, temp.end());
					temp.append("..");
					ImVec2 textSize = ImGui::CalcTextSize(temp.c_str());
					float textOffsetX = (mItemIconSize - textSize.x) * 0.5f;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffsetX);
					ImGui::TextWrapped("%s", temp.c_str());
				}

				ImGui::EndGroup();

				// Manage new row and spacing for items
				availableSpace -= mItemIconSize + mItemIconSpacing;
				if (availableSpace > ((mItemIconSize + mItemIconSpacing) * 2))
					ImGui::SameLine();
				else
					availableSpace = windowSize.x;

				ImGui::PopID();
			}

			// Revert the style changes
			ImGui::PopStyleColor(3);
		}
		ImGui::EndChild();

	}
	ImGui::End();


}

void UserInterfaceManager::ui_ViewPort()
{
	if (!mIsViewportWindowOpen) return;

	if(ImGui::Begin("LevelViewport", &mIsViewportWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		// Viewport Render
		// -------------------------------------------
		// Get`s size of viewport imgui window
		const ImVec2 availableSize = ImGui::GetContentRegionAvail(); // Get the available space in the window
		const int newWidth = static_cast<int>(availableSize.x);
		const int newHeight = static_cast<int>(availableSize.y);

		// Resize scene texture to render at the correct resolution dynamically based on window size
		if (newWidth != mViewportWidth || newHeight != mViewportHeight)
		{
			ResizeFrameBuffer(newWidth, newHeight);

			// Enable this to make cameras aspect ratio reflect viewport size
			//if (mLevelManager)
			//	mLevelManager->FrameBufferSizeCallback(newWidth, newHeight);
		}

		// Sets the texture and inverts it for glfw
		ImVec2 uv0 = ImVec2(0.0f, 1.0f); // Top-left
		ImVec2 uv1 = ImVec2(1.0f, 0.0f); // Bottom-right
		ImGui::Image((void*)(intptr_t)mViewportTexture, availableSize, uv0, uv1);

		mMouseCursorPosition = ImGui::GetMousePos();

		if (ImGui::BeginDragDropTarget())
		{
			if (mIsCreateActorWindowOpen)
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_CREATE_ITEM"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					const int payload_n = *(const int*)payload->Data;

					// Handle the creation of the actor based on the dropped item
					if (payload_n >= 0)
					{
						static int cubeNum = 0;
						static int pyramidNum = 0;
						static int planeNum = 0;
						static int sphereNum = 0;
						if (strcmp(mActorNames[payload_n], "Cube") == 0)
						{
							cubeNum++;
							std::shared_ptr<BaseActor> baseActor = std::make_shared<BaseActor>("CreatedCube" + std::to_string(cubeNum), Mesh::CreateCube(nullptr, false,"CreatedCube" + std::to_string(cubeNum) + "_mat"));
							mLevelManager->AddActorToLevel(baseActor);
						}
						else if (strcmp(mActorNames[payload_n], "Pyramid") == 0)
						{
							pyramidNum++;
							std::shared_ptr<BaseActor> baseActor = std::make_shared<BaseActor>("CreatedPyramid" + std::to_string(pyramidNum), Mesh::CreatePyramid(nullptr, false,"CreatedPyramid" + std::to_string(pyramidNum) + "_mat"));
							mLevelManager->AddActorToLevel(baseActor);
						}
						else if (strcmp(mActorNames[payload_n], "Plane") == 0)
						{
							planeNum++;
							std::shared_ptr<BaseActor> baseActor = std::make_shared<BaseActor>("CreatedPlane" + std::to_string(planeNum), Mesh::CreatePlane(nullptr, false,"CreatedPlane" + std::to_string(planeNum) + "_mat"));
							mLevelManager->AddActorToLevel(baseActor);
						}
						else if (strcmp(mActorNames[payload_n], "Sphere") == 0)
						{
							sphereNum++;
							std::shared_ptr<BaseActor> baseActor = std::make_shared<BaseActor>("CreatedSphere" + std::to_string(sphereNum), Mesh::CreateSphere(nullptr,2, false, "CreatedSphere" + std::to_string(sphereNum) + "_mat"));
							mLevelManager->AddActorToLevel(baseActor);
						}
						else if (strcmp(mActorNames[payload_n], "Directional-Light") == 0)
						{
							std::shared_ptr<DirectionalLightActor> baseActor = std::make_shared<DirectionalLightActor>("DirectionalLight");
							mLevelManager->AddActorToLevel(baseActor);
						}
						else if (strcmp(mActorNames[payload_n], "Point-Light") == 0)
						{
							std::shared_ptr<PointLightActor> baseActor = std::make_shared<PointLightActor>("PointLight");
							mLevelManager->AddActorToLevel(baseActor);
						}
					}
				}
			}

			if (mIsFileExplorerWindowOpen)
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_FBX"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						const char* fileName = strrchr(fullPath, '/');

						if (fileName) {
							fileName++;
						}
						else {
							fileName = fullPath;
						}

						// Create the FBX actor using the provided path
						std::shared_ptr<Actor> fbxActor = std::make_shared<Actor>(fileName);
						AssimpLoader::Load(fullPath, fbxActor);
						mLevelManager->AddActorToLevel(fbxActor);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		mViewportHasFocusAndHover = ImGui::IsWindowFocused() && ImGui::IsWindowHovered() ? true : false;
		mViewportHasHover = ImGui::IsWindowHovered();

		mViewportPos = ImGui::GetWindowPos();
	}
	ImGui::End();
}

void UserInterfaceManager::ui_LevelSelect()
{



}

void UserInterfaceManager::ui_WorldProperties()
{
	if (!mIsWorldPropertiesWindowOpen) return;

	// Starts window, set to true.
	// If window is closed this becomes false and code inside is not run. 
	if (ImGui::Begin("World Properties", &mIsWorldPropertiesWindowOpen, ImGuiWindowFlags_NoCollapse) && mDefaultShader)
	{
		// Decides if world is rendered as wire frame or solid mesh.
		// ---------------------------------------------------------
		ImGui::Checkbox("Wire-Frame Mode", &mShouldShowWireFrame);

		if(mLevelManager)
			mLevelManager->ShadersDrawWireFrame(mShouldShowWireFrame);

		ImGui::Separator();

		// Decides if world physics enabled
		// ---------------------------------------------------------
		if(ImGui::Button("Toggle world gravity", ImVec2(150, 25)))
		{
			// Toggle bool
			mWorldGravityEnabled = !mWorldGravityEnabled ? true : false;
			
			std::vector<std::shared_ptr<Actor>> tempActors;
			GetSceneActors(mLevelManager->GetActiveLevel()->mSceneGraph, tempActors);

			for(std::shared_ptr<Actor> actor : tempActors)
			{
				if(actor->GetPhysicsComponent())
				{
					actor->GetPhysicsComponent()->SetGravityEnabled(mWorldGravityEnabled);
					actor->GetPhysicsComponent()->ResetForces();
				}
			}
		}

		ImGui::SameLine();
		if (mWorldGravityEnabled)
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "Enabled");
		else
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Disabled");
		ImGui::Separator();


	}
	ImGui::End();
}


void UserInterfaceManager::ui_DebugProperties()
{
	if (!mIsDebugPropertiesWindowOpen) return;

	if (ImGui::Begin("Debug Properties", &mIsDebugPropertiesWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		// Static init
		// in array values the array nums (10) is the x axis, while its content is the y axis.
		// So it remembers and gets the average from the last 10 seconds in this case. 
		static float values[10] = {};
		static int index = 0;

		// counts the amount of frames processed this round
		numFrames++;

		// Gets the current time
		double currentTime = ImGui::GetTime();
		// Calculates the elapsed time 
		double elapsedTime = currentTime - mOldTime;
		// Each second this if function should tick. (0.004 inaccuracy).
		if (elapsedTime >= 1.0)
		{
			// assigns 
			values[index] = (float)numFrames;
			// once max array size is reached offset is reset to 0
			index = (index + 1) % IM_ARRAYSIZE(values);
			// resets frames and mOldTime to continue the loop
			numFrames = 0;
			mOldTime = currentTime;
		}

		// Find the average FPS value over the 10 seconds of sample fps numbers
		float average = 0.0f;
		int divNum = 1;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
		{
			// Does not include 0 or high values since they may inflate average before values are filled in.
			if (values[n] > 0 && values[n] < 10000)
			{
				average += values[n];
				divNum++;
			}
		}

		// Calc the average
		average /= (float)divNum;

		// Write average into char array for dynamic text rendering
		char overlay[32];
		sprintf_s(overlay, "Avg fps %f", average);

		// Sets text pos 50 right and draw
		ImGui::SetCursorPosX(50);
		ImGui::Text(overlay);

		// Draws the graph
		ImGui::PlotLines("##FPSPlot", values, IM_ARRAYSIZE(values), (int)mOldTime, "", -1.0f, 1.0f, ImVec2(200, 80.0f));

		// Section to create descriptive lines on graph
		ImGui::Text(" | "); ImGui::SameLine();
		ImGui::SetCursorPosX(100);
		ImGui::Text(" | "); ImGui::SameLine();
		ImGui::SetCursorPosX(200);
		ImGui::Text(" | ");

		ImGui::Text("10 sec"); ImGui::SameLine();
		ImGui::SetCursorPosX(90);
		ImGui::Text("5 sec"); ImGui::SameLine();
		ImGui::SetCursorPosX(190);
		ImGui::Text("now");

		ImGui::Separator();

		// Handles visibility of collision debug mesh
		// ------------------------------------------
		ImGui::Checkbox("Show Collision debug mesh", &mShouldDrawCollisionDebugMesh);
		std::vector<std::shared_ptr<Actor>> tempActors;
		mLevelManager->GetActiveLevel()->mSceneGraph->Query<BaseActor>(tempActors);

		for (std::shared_ptr<Actor> actor : tempActors)
		{
			std::shared_ptr<BaseActor> mA = std::dynamic_pointer_cast<BaseActor>(actor);
			if (mA)
			{
				if (mShouldDrawCollisionDebugMesh && mA->mCollisionMesh)
					mA->mCollisionMesh->SetIsVisible(true);
				else
					mA->mCollisionMesh->SetIsVisible(false);
			}
		}

		ImGui::Separator();

	}
	ImGui::End();
}

void UserInterfaceManager::ui_ContentBrowser()
{
	if (!mIsContentBrowserWindowOpen) {
		mContentSelectedActor = nullptr;
		return;
	}

	if (ImGui::Begin("Content Browser", &mIsContentBrowserWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		//Shows all actors in scene, based on selection can take control
		// Note only shows the parent actors so to speak as you cannot control children
		// ----------------------------------------------------------------------------
		std::vector<std::shared_ptr<Actor>> tempSceneActors;
	
		GetSceneActors(mLevelManager->GetActiveLevel()->mSceneGraph, tempSceneActors);

		// Calculate the available space for the list box
		ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

		// Use BeginListBox to create a resizable list box
		if (ImGui::BeginListBox("##LB", listBoxSize)) {

			int iterator = 0;
			
			for(std::shared_ptr<Actor> actors : tempSceneActors)
			{
				// Check the depth level of the actor in the hierarchy
				int hierarchyLevel = actors->GetHierarchyLevel();

				// Indent based on hierarchy level
				ImGui::Indent(hierarchyLevel * ImGui::GetStyle().IndentSpacing);


				iterator++;
				//bool selected = false;
				if(ImGui::Selectable(actors->GetTag().c_str()))
				{
					mContentSelectedActor = actors;
				}
			
				// Create a right-click context menu for this item
				std::string contextMenuID = "ItemContextMenu_" + std::to_string(iterator);
				if (ImGui::BeginPopupContextItem(contextMenuID.c_str()))
				{
					if (ImGui::MenuItem("Delete")) {
			
						auto it = std::find(tempSceneActors.begin(), tempSceneActors.end(), actors);
						if (it != tempSceneActors.end())
							tempSceneActors.erase(it);

						mLevelManager->RemoveActorFromLevel(actors);
						mContentSelectedActor = nullptr;
						ImGui::EndPopup();
						break;
					}
					if (ImGui::MenuItem("Rename")) {

						mOpenRenamePopup = true;

					}
					ImGui::EndPopup();
				}
				// Unindent after rendering the actor
				ImGui::Unindent(hierarchyLevel * ImGui::GetStyle().IndentSpacing);
			}
		}
		ImGui::EndListBox();

	}
	ImGui::End();

	if(mOpenRenamePopup)
		ImGui::OpenPopup("RenamePopup");

	// Handle the Rename Popup
	if (ImGui::BeginPopup("RenamePopup"))
	{
		char newItemName[100] = { 0 }; // Buffer to store new item name

		ImGui::Text("Rename Item:");
		ImGui::InputText("NewName", newItemName, IM_ARRAYSIZE(newItemName));

		if (ImGui::Button("OK"))
		{
			if (mContentSelectedActor)
				mContentSelectedActor->RenameActor(newItemName);
			
			mOpenRenamePopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			mOpenRenamePopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (mOldContentSelectedActor != mContentSelectedActor)
	{
		mCanControlActor = false;
		if (mLevelManager->GetActiveLevel()->mActiveCamera)
			mController->SetActorToControl(mLevelManager->GetActiveLevel()->mActiveCamera);
	}
	mOldContentSelectedActor = mContentSelectedActor;
}

void UserInterfaceManager::ui_ContentProperties()
{
	if (!mIsContentPropertiesWindowOpen) return;

	if (ImGui::Begin("Content Properties", &mIsContentPropertiesWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		if (!mContentSelectedActor)
		{
			ImGui::End();
			return;
		}

		// Decides actor control
		// ----------------------------------------------
		if (mContentSelectedActor == mController->GetRefToControlledActor())
			mCanControlActor = true;

		ImGui::Checkbox("Control Selected Actor", &mCanControlActor);
		if (mCanControlActor)
		{
			// If new actor is camera, set it to the level camera
			if (std::shared_ptr<CameraActor> newCamActor = std::dynamic_pointer_cast<CameraActor>(mContentSelectedActor))
			{
				mLevelManager->GetActiveLevel()->mActiveCamera = newCamActor;
			}

			// Decides how fast the actor that is controlled moves
			// ----------------------------------------------
			float tempActorMoveSpeed = mController->GetMovementSpeed();
			ImGui::Text("Actor Movement Speed"); ImGui::SameLine(); ImGui::SetNextItemWidth(mItemWidth); ImGui::InputFloat("##AMS", &tempActorMoveSpeed);
			mController->SetMovementSpeed(tempActorMoveSpeed);
			mController->SetActorToControl(mContentSelectedActor);

			if(std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mContentSelectedActor))
			{
				
			} else
			{

				// Decides if a camera should snap to selected actor
				// ----------------------------------------------
				ImGui::Checkbox("Snap camera to actor", &mSnapCameraToActor);
				if (mSnapCameraToActor && mContentSelectedActor != mLevelManager->GetActiveLevel()->mActiveCamera)
				{
					// Decides what camera should snap
					// ----------------------------------------------
					mLevelManager->GetActiveLevel()->mActiveCamera->mSnappedToActor = mContentSelectedActor;
					mLevelManager->GetActiveLevel()->mActiveCamera->SetGlobalPosition(mContentSelectedActor->GetGlobalPosition() + glm::vec3(0, 2, 7));
					mController->SnapCameraToControlledActor(mLevelManager->GetActiveLevel()->mActiveCamera);

				}
				else {

					mSnapCameraToActor = false;
					mLevelManager->GetActiveLevel()->mActiveCamera->mSnappedToActor = nullptr;
					mController->UnSnapCameraToControlledActor();
				}
				// Reset control to level camera when chaneging object
			}

		}
		else {
			if (mLevelManager->GetActiveLevel()->mActiveCamera)
			{
				mController->SetActorToControl(mLevelManager->GetActiveLevel()->mActiveCamera);
				mController->UnSnapCameraToControlledActor();
			}
			mSnapCameraToActor = false;
		}

		// Resets Actor control back to camera when changing selected item
		

		// Handles all local Sub UI for Actor world settings
		// ----------------------------------------------
		if (mContentSelectedActor)
			uiSub_WorldProperties(mContentSelectedActor);

		// Handles all local Sub UI for Mesh world settings
		// ----------------------------------------------
		std::shared_ptr<BaseActor> basePtr = std::dynamic_pointer_cast<BaseActor>(mContentSelectedActor);
		std::shared_ptr<VisualActor> visualPtr = std::dynamic_pointer_cast<VisualActor>(mContentSelectedActor);
		if (basePtr && basePtr->GetMesh())
			uiSub_MeshProperties(basePtr->GetMesh());
		else if(visualPtr && visualPtr->GetMesh())
			uiSub_MeshProperties(visualPtr->GetMesh());

		// Handles all local Sub UI for component settings
		// ----------------------------------------------
		if (mContentSelectedActor)
			uiSub_ComponentProperties(mContentSelectedActor);

		// Handles all local Sub UI for camera settings
		// ----------------------------------------------
		std::shared_ptr<CameraActor> cameraPtr = std::dynamic_pointer_cast<CameraActor>(mContentSelectedActor);
		if (cameraPtr)
			uiSub_CameraProperties(cameraPtr);

		// Handles all local Sub UI for Actor Collision settings
		// ----------------------------------------------
		std::shared_ptr<IBounded> collisionPtr = std::dynamic_pointer_cast<IBounded>(mContentSelectedActor);
		if (collisionPtr)
			uiSub_CollisionProperties(collisionPtr);

		// Handles all local Sub UI for Actor Light settings
		// ----------------------------------------------
		std::shared_ptr<Light> lightptr = std::dynamic_pointer_cast<Light>(mContentSelectedActor);
		if (lightptr)
			uiSub_LightProperties(lightptr);
		
	}
	ImGui::End();
}

void UserInterfaceManager::uiSub_WorldProperties(std::shared_ptr<Actor> _inActor)
{
	// Actor specific details
	// ----------------------
	ImGui::Text("Actor Details");
	ImGui::Separator();

	// Can edit the x, y, z position for selected actor
	// ----------------------------------------------
	glm::vec3 currentActorsPosition = _inActor->GetGlobalPosition();

	ImGui::Text("World Position: ");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##PX", &currentActorsPosition.x);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##PY", &currentActorsPosition.y);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##PZ", &currentActorsPosition.z);

	_inActor->SetGlobalPosition(currentActorsPosition);

	// Can edit the x, y, z rotation for selected actor
	// ----------------------------------------------
	glm::quat currentActorsRotation = _inActor->GetGlobalRotation();

	// Convert quaternion to Euler angles
	glm::vec3 actorRotationInEulerAngles = glm::degrees(glm::eulerAngles(currentActorsRotation));
	float pitch = actorRotationInEulerAngles.x;
	float yaw = actorRotationInEulerAngles.y;
	float roll = actorRotationInEulerAngles.z;

	const float maxPitch = (float)89.9; // Maximum pitch angle to avoid gimbal lock
	pitch = glm::clamp(pitch, -maxPitch, maxPitch);

	ImGui::Text("World Rotation: ");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pitch");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##RX", &pitch);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "Yaw ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##RY", &yaw);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 1, 1), "Roll");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##RZ", &roll);

	// Convert Euler angles back to quaternion
	glm::vec3 backActorsEulerAngles = glm::radians(glm::vec3(pitch, yaw, roll));
	_inActor->SetGlobalRotation(glm::quat(backActorsEulerAngles));

	// Can edit the x, y, z scale for selected actor
	// ----------------------------------------------
	ImGui::Text("World Scale: ");
	ImGui::SameLine();
	// Toggle Uniform scaling
	ImGui::Checkbox("Uniform Scale", &mIsUniformScale);

	if (mIsUniformScale) {

		// sets new uniform
		ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "Uniform Scale");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##US", &mCurrentUniformScale);

		// if there is a change in uniform value
		if (mCurrentUniformScale != mOldUniformScale)
		{
			// set scale of actor to its current scale * uniform
			// should only proc once since Old is set to current immediately after
			_inActor->SetGlobalScale(mActorOriginalScale * mCurrentUniformScale);

			mOldUniformScale = mCurrentUniformScale;
		}

	}
	else {

		glm::vec3 currentActorsScale = _inActor->GetGlobalScale();
		mCurrentUniformScale = 1;

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##SX", &currentActorsScale.x);

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##SY", &currentActorsScale.y);

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##SZ", &currentActorsScale.z);

		_inActor->SetGlobalScale(glm::vec3(currentActorsScale.x, currentActorsScale.y, currentActorsScale.z));
		mActorOriginalScale = _inActor->GetGlobalScale();
	}

	// Debug visualize barycentric coordinates 
	// -----------------------------------
	if (_inActor->GetPhysicsComponent() && _inActor->GetPhysicsComponent()->HasSurfaceReference()) {

		ImGui::Checkbox("Show barycentric location", &mShowBarycentricLocation);

		// If checkbox enabled
		if (mShowBarycentricLocation)
		{
			std::vector<glm::vec3> debugMeshPoints = _inActor->GetPhysicsComponent()->GetDebugSurfaceBarycentricPoints();

			// If debugActor does not exist create new, otherwise update its visual mesh
			if (!mBarycentricDebugActor)
			{
				std::string name = _inActor->GetTag() + "_BarycentricDebug";

				mBarycentricDebugActor = std::make_shared<DebugActor>(name,false, debugMeshPoints);
				mLevelManager->GetActiveLevel()->AddActorToSceneGraph(mBarycentricDebugActor);
			}
			else {

				mBarycentricDebugActor->UpdateVisualMesh(debugMeshPoints);

			}

			// If checkbox disabled remove debug actor reference
		}
		else {

			if (mBarycentricDebugActor)
			{
				mLevelManager->GetActiveLevel()->RemoveActorFromSceneGraph(mBarycentricDebugActor);
				mBarycentricDebugActor = nullptr;
				mShowBarycentricLocation = false;
			}
		}

		// Same if switched selection
	}
	else {

		if (mBarycentricDebugActor)
		{
			mLevelManager->GetActiveLevel()->RemoveActorFromSceneGraph(mBarycentricDebugActor);
			mBarycentricDebugActor = nullptr;
			mShowBarycentricLocation = false;
		}

	}
}

void UserInterfaceManager::uiSub_MeshProperties(std::shared_ptr<Mesh> _inMesh)
{

	// Mesh display
	// -----------------------------------
	ImGui::Text("Mesh Details");
	ImGui::Separator();

	ImGui::Checkbox("ShowMesh", &mShowMesh);
	_inMesh->SetIsVisible(mShowMesh);

	ImGui::Text("Material textures: ");

	// Diffuse texture --------
	if(_inMesh->GetMaterial()->GetTexture(Material::DIFFUSE))
	{
		ImGui::Image((void*)(intptr_t)_inMesh->GetMaterial()->GetTexture(Material::DIFFUSE)->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		ImGui::SameLine();
		if (mIsFileExplorerWindowOpen)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_JPG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::DIFFUSE, Texture::Load(fullPath));
					}
				}
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_PNG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::DIFFUSE, Texture::Load(fullPath));
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	} else {
		ImGui::Image((void*)(intptr_t)mJPGIcon->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		ImGui::SameLine();
	}
	

	// Specular texture --------
	if(_inMesh->GetMaterial()->GetTexture(Material::SPECULAR))
	{
		ImGui::Image((void*)(intptr_t)_inMesh->GetMaterial()->GetTexture(Material::SPECULAR)->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		ImGui::SameLine();
		if (mIsFileExplorerWindowOpen)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_JPG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::SPECULAR, Texture::Load(fullPath));
					}
				}
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_PNG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::SPECULAR, Texture::Load(fullPath));
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
	else {
		ImGui::Image((void*)(intptr_t)mJPGIcon->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		ImGui::SameLine();
	}
	

	// Normal texture --------
	if(_inMesh->GetMaterial()->GetTexture(Material::NORMAL))
	{
		ImGui::Image((void*)(intptr_t)_inMesh->GetMaterial()->GetTexture(Material::NORMAL)->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		ImGui::SameLine();
		if (mIsFileExplorerWindowOpen)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_JPG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::NORMAL, Texture::Load(fullPath));
					}
				}
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_PNG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::NORMAL, Texture::Load(fullPath));
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	} else {
		ImGui::Image((void*)(intptr_t)mJPGIcon->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		ImGui::SameLine();
	}
	
	// Alpha texture --------
	if(_inMesh->GetMaterial()->GetTexture(Material::ALPHA))
	{
		ImGui::Image((void*)(intptr_t)_inMesh->GetMaterial()->GetTexture(Material::ALPHA)->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
		if (mIsFileExplorerWindowOpen)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_JPG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::ALPHA, Texture::Load(fullPath));
					}
				}
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DRAG_PNG"))
				{
					// Check if the payload is a string (full path)
					if (payload->DataSize > 0)
					{
						const char* fullPath = static_cast<const char*>(payload->Data);

						_inMesh->GetMaterial()->SetTexture(Material::ALPHA, Texture::Load(fullPath));
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	} else {
		ImGui::Image((void*)(intptr_t)mJPGIcon->GetTextureID(), ImVec2(50.f, 50.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		DrawBoarderAroundImage();
	}

}

void UserInterfaceManager::uiSub_ComponentProperties(std::shared_ptr<Actor> _inActor)
{

	// Component display
	// -----------------------------------
	ImGui::Text("Component Details");
	ImGui::Separator();
	std::vector<std::shared_ptr<Component>> actorComponents(_inActor->GetComponents());
	std::vector<const char*> componentNames;
	for (std::shared_ptr<Component> Component : actorComponents) {
		componentNames.push_back(Component->GetTag().c_str());
	}

	ImGui::ListBox("##LBC", &mComponentSelectionIndex, componentNames.data(), (int)componentNames.size());

	if(_inActor->GetPhysicsComponent())
	{
		if (_inActor->GetPhysicsComponent() == _inActor->GetComponents()[mComponentSelectionIndex])
		{
			mEnableGravity = _inActor->GetPhysicsComponent()->IsGravityEnabled();
			ImGui::Checkbox("Gravity", &mEnableGravity);
			if (mEnableGravity)
				_inActor->GetPhysicsComponent()->SetGravityEnabled(mEnableGravity);
			else
				_inActor->GetPhysicsComponent()->SetGravityEnabled(mEnableGravity);

			if (ImGui::Button("ResetForces"))
			{
				_inActor->GetPhysicsComponent()->ResetForces();
			}
		}
	}

	if (_inActor->GetAIComponent())
	{
		if (_inActor->GetAIComponent() == _inActor->GetComponents()[mComponentSelectionIndex])
		{

			// Edit movement speed of Ai actor
			// ----------------------------------------------
			float currentActorsAIMovementSpeed = _inActor->GetAIComponent()->GetMovementSpeed();
			ImGui::Text("AI MovementSpeed: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(mItemWidth);
			ImGui::InputFloat("##AIX", &currentActorsAIMovementSpeed);
			_inActor->GetAIComponent()->SetMovementSpeed(currentActorsAIMovementSpeed);
		}
	}
}

void UserInterfaceManager::uiSub_CameraProperties(std::shared_ptr<CameraActor> _inCamera)
{
}

void UserInterfaceManager::uiSub_CollisionProperties(std::shared_ptr<IBounded> _inCollision)
{
	// Handles all collision setting logic with
	// ImGui and shared_ptr of the object selected
	ImGui::Text("Collision Details");
	ImGui::Separator();
	// Shows if actor is colliding 
	// ----------------------------------------------
	ImGui::Text("Is Colliding: "); ImGui::SameLine();
	if (_inCollision->GetIsColliding())
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "True");
	else
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "False");

	// Edit collision type
	// ----------------------------------------------
	const char* typeItems[] = { "STATIC","DYNAMIC","KINETIC" };
	int currentTypeItem = 0;

	currentTypeItem = static_cast<int>(_inCollision->mCollisionProperties.mType);

	ImGui::Text("Collision Type");
	ImGui::Combo("##TB", &currentTypeItem, typeItems, IM_ARRAYSIZE(typeItems));

	_inCollision->mCollisionProperties.SetCollisionType(static_cast<CollisionType>(currentTypeItem));

	// Edit collision response
	// ----------------------------------------------
	const char* responseItems[] = { "BLOCK","OVERLAP","IGNORE" };
	int currentResponseItem = 0;

	currentResponseItem = static_cast<int>(_inCollision->mCollisionProperties.mResponse);

	ImGui::Text("Collision Response");
	ImGui::Combo("##RB", &currentResponseItem, responseItems, IM_ARRAYSIZE(responseItems));

	_inCollision->mCollisionProperties.SetCollisionResponse(static_cast<CollisionResponse>(currentResponseItem));

	// Edit collision base
	// ----------------------------------------------
	const char* baseItems[] = { "AABB","BoundingSphere" };
	int currentbaseItem = 0;

	currentbaseItem = static_cast<int>(_inCollision->mCollisionProperties.mBase);

	ImGui::Text("Collision Base");
	ImGui::Combo("##BB", &currentbaseItem, baseItems, IM_ARRAYSIZE(baseItems));

	_inCollision->mCollisionProperties.SetCollisionBase(static_cast<CollisionBase>(currentbaseItem));

	// Edit collision sub base
	// ----------------------------------------------
	const char* subBaseItems[] = { "NoSubBase","ConvexHul" };
	int currentSubBaseItem = 0;

	currentSubBaseItem = static_cast<int>(_inCollision->mCollisionProperties.mSubBase);

	ImGui::Text("Collision SubBase");
	ImGui::Combo("##SB", &currentSubBaseItem, subBaseItems, IM_ARRAYSIZE(subBaseItems));

	_inCollision->mCollisionProperties.SetCollisionSubBase(static_cast<CollisionSubBase>(currentSubBaseItem));
}

void UserInterfaceManager::uiSub_LightProperties(std::shared_ptr<Light> _inLight)
{
	// Handles all collision setting logic with
	// ImGui and shared_ptr of the object selected
	
	ImGui::Text("Light Details");
	ImGui::Separator();
	// Edit ambient light strength
	// ----------------------------------------------
	float ambientStrength = _inLight->mAmbient.x;
	ImGui::Text("Ambient Strength: "); ImGui::SameLine(); ImGui::InputFloat(" ", &ambientStrength);
	_inLight->mAmbient.x = ambientStrength;
	_inLight->mAmbient.y = ambientStrength;
	_inLight->mAmbient.z = ambientStrength;
	
	// Edit light Color
	// ----------------------------------------------
	glm::vec3 lightColor = _inLight->mColor;
	float colorValues[3] = { lightColor.x,lightColor.y,lightColor.z };
	ImGui::Text("Light Color: "); ImGui::SameLine(); ImGui::ColorEdit3("  ", &colorValues[0]);
	_inLight->mColor.x = colorValues[0];
	_inLight->mColor.y = colorValues[1];
	_inLight->mColor.z = colorValues[2];
}

void UserInterfaceManager::ui_Log()
{
	if (!mIsLogWindowOpen) return;

	if (ImGui::Begin("Log", &mIsLogWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("tmp - Log");

	}
	ImGui::End();
}

void UserInterfaceManager::ui_Console()
{
	if (!mIsConsoleWindowOpen) return;

	if (ImGui::Begin("Console", &mIsConsoleWindowOpen, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("tmp - Console");

	}
	ImGui::End();
}

void UserInterfaceManager::ui_Element_Image(std::shared_ptr<Texture> _image, ImVec2 _imageSize, ImVec2 _uv0, ImVec2 _uv1, bool _centreVertically)
{
	if(_image)
	{
		// Centered vertically image
		if(_centreVertically)
		{
			const float frameHeight = ImGui::GetFrameHeight();
			const float verticalPadding = (frameHeight - _imageSize.y) / 2.0f;
			const ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosY(cursorPos.y + verticalPadding);

			ImGui::Image((void*)(intptr_t)_image->GetTextureID(), _imageSize, _uv0, _uv1);

			ImGui::SetCursorPosX(cursorPos.x + _imageSize.x + ImGui::GetStyle().ItemSpacing.x);
			ImGui::SameLine();

			// Un-centered vertically image
		} else {

			ImGui::Image((void*)(intptr_t)_image->GetTextureID(), _imageSize, _uv0, _uv1);
			ImGui::SameLine();
		}
	}
}

bool UserInterfaceManager::HasSuffix(const std::string& _fileName, const std::string& _suffix)
{
	// Ensure _fullName is long enough to contain the _suffix
	if (_fileName.length() >= _suffix.length()) {
		// Compare the end of _fullName with _suffix
		return (_fileName.compare(_fileName.length() - _suffix.length(), _suffix.length(), _suffix) == 0);
	}
	return false;
}

void UserInterfaceManager::DrawBoarderAroundImage(ImVec4 _color)
{

	// Get the current position and size of the image
	ImVec2 image_pos = ImGui::GetItemRectMin(); // Top-left corner of the image
	ImVec2 image_size = ImGui::GetItemRectSize(); // Size of the image

	// Calculate the rectangle corners
	ImVec2 rect_min = image_pos;
	ImVec2 rect_max = ImVec2(image_pos.x + image_size.x, image_pos.y + image_size.y);

	// Set the color and thickness of the border
	ImU32 border_color = ImGui::GetColorU32(_color); // Red color
	float border_thickness = 2.0f;

	// Draw the border
	ImGui::GetWindowDrawList()->AddRect(rect_min, rect_max, border_color, 0.0f, 0, border_thickness);

}

std::string UserInterfaceManager::GetFilePath()
{
	std::string directoryPath = std::filesystem::current_path().string();

	directoryPath = std::filesystem::path(directoryPath).parent_path().string();
	directoryPath = std::filesystem::path(directoryPath).parent_path().string();
	directoryPath = std::filesystem::path(directoryPath).parent_path().string();

	return (std::filesystem::path(directoryPath) / "UserAssets").string();
}

std::string UserInterfaceManager::GetCoreFilePath()
{
	std::string directoryPath = std::filesystem::current_path().string();

	directoryPath = std::filesystem::path(directoryPath).parent_path().string();
	directoryPath = std::filesystem::path(directoryPath).parent_path().string();
	directoryPath = std::filesystem::path(directoryPath).parent_path().string();

	return directoryPath;
}

void UserInterfaceManager::GetSceneActors(std::shared_ptr<Actor> _sceneGraph, std::vector<std::shared_ptr<Actor>>& _actorHolder)
{
	for (std::shared_ptr<Actor> _actor : _sceneGraph->GetChildren())
	{
		_actorHolder.push_back(_actor);

		GetSceneActors(_actor, _actorHolder);
	}
}

std::vector<FileItem> UserInterfaceManager::GetDirectoryContents(const std::string& _path)
{
	std::vector<FileItem> items;
	for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::directory_iterator(_path))) {
		items.push_back({ entry.path().filename().string(), entry.is_directory() });
	}
	return items;
}
