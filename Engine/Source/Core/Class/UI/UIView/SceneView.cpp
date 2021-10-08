#include "pch.h"
#include "SceneView.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/Components/Transform.h"
#include "Class/Game/Components/Camera.h"
#include "ExternalImpl/JGImGui.h"
#include "Imgui/imgui.h"
#include "Imgui/ImGuizmo.h"
namespace JG
{


	SceneView::SceneView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/SceneView", 0,
			[&]()
		{
			Open();
		}, nullptr);
	}
	void SceneView::Load()
	{

		LoadIcons();
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		mCurrentGizmoMode = ImGuizmo::WORLD;
		mCurrentCameraMode = CameraMode_3D;
	}
	void SceneView::Initialize()
	{
		mShowGizmo = CreateUniquePtr<Command<GameNode*>>();
		mShowGizmo->Subscribe(this, [&](GameNode* node)
		{
			auto mainCam = Camera::GetMainCamera();
			if (mainCam == nullptr) return;
			if (node == nullptr || node->GetType() != JGTYPE(GameNode)) return;

			i32 snapIndex = 0;
			if (mCurrentGizmoMode & ImGuizmo::TRANSLATE_X ||
				mCurrentGizmoMode & ImGuizmo::TRANSLATE_Y ||
				mCurrentGizmoMode & ImGuizmo::TRANSLATE_Z)
			{
				snapIndex = 0;
			}
			else if (mCurrentGizmoMode & ImGuizmo::ROTATE_X ||
				mCurrentGizmoMode & ImGuizmo::ROTATE_Y ||
				mCurrentGizmoMode & ImGuizmo::ROTATE_Z)
			{
				snapIndex = 1;
			}
			else if (mCurrentGizmoMode & ImGuizmo::SCALE_X ||
				mCurrentGizmoMode & ImGuizmo::SCALE_Y ||
				mCurrentGizmoMode & ImGuizmo::SCALE_Z)
			{
				snapIndex = 2;
			}

			f32 snapValue[3];
			for (int i = 0; i < 3; ++i)
			{
				snapValue[i] = mSnapValue[snapIndex];
			}

			JVector3 location = node->GetTransform()->GetWorldLocation();
			JVector3 rotation = Math::ConvertToRadians(node->GetTransform()->GetWorldRotation());
			JVector3 scale = node->GetTransform()->GetWorldScale();


			JMatrix worldMat;
			ImGuizmo::RecomposeMatrixFromComponents((float*)&location, (float*)&rotation, (float*)&scale, worldMat.GetFloatPtr());
			auto itemMin = ImGui::GetItemRectMin();
			auto itemSize = ImGui::GetItemRectSize();
			ImGuizmo::SetRect(itemMin.x, itemMin.y, itemSize.x, itemSize.y);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetOrthographic(mainCam->IsOrthographic());

			auto view = mainCam->GetViewMatrix();
			auto proj = mainCam->GetProjMatrix();


			ImGui::PushClipRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true);

			bool result = ImGuizmo::Manipulate(
				view.GetFloatPtr(), proj.GetFloatPtr(),
				(ImGuizmo::OPERATION)mCurrentGizmoOperation, (ImGuizmo::MODE)mCurrentGizmoMode,
				worldMat.GetFloatPtr(), nullptr, (mIsSnap) ? snapValue : nullptr, nullptr, nullptr);


			ImGui::PopClipRect();


			JVector3 matrixTranslation, matrixRotation, matrixScale;
			ImGuizmo::DecomposeMatrixToComponents(worldMat.GetFloatPtr(), (float*)&matrixTranslation, (float*)&matrixRotation, (float*)&matrixScale);

			if (result)
			{
				node->GetTransform()->SetWorldLocation(matrixTranslation);
				node->GetTransform()->SetWorldRotation(Math::ConvertToDegrees(matrixRotation));
				node->GetTransform()->SetWorldScale(matrixScale);
			}

		});

	}
	void SceneView::OnGUI()
	{
		ImGuizmo::BeginFrame();

		ImGui::Begin("SceneView", &mOpenGUI);


		auto mainCam = Camera::GetMainCamera();
		bool isOrth = mainCam->IsOrthographic();
		if (isOrth) mCurrentCameraMode = CameraMode_2D;
		else mCurrentCameraMode = CameraMode_3D;



		if (mEnableEditorCameraControll == false)
		{
			if ((ImGui::IsKeyPressed((i32)EKeyCode::Q)))
			{
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			}
			if ((ImGui::IsKeyPressed((i32)EKeyCode::W)))
			{
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			}
			if ((ImGui::IsKeyPressed((i32)EKeyCode::E)))
			{
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			}
		}
		OnGUI_Top();
		OnGUI_Bottom();

		ImGui::End();

		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void SceneView::Destroy()
	{
		mShowGizmo->UnSubscribe(this);
		mShowGizmo = nullptr;
	}
	void SceneView::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		UIView::MakeJson(jsonData);

	}
	void SceneView::LoadJson(SharedPtr<JsonData> jsonData)
	{
		UIView::LoadJson(jsonData);

	}
	void SceneView::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyChangeMainSceneTextureEvent>(EVENT_BIND_FN(&SceneView::ResponseChangeMainSceneTexture));
		dispatcher.Dispatch<NotifySelectedGameNodeInEditorEvent>(EVENT_BIND_FN(&SceneView::ResponseSelectedGameNodeInEditor));
	}
	void SceneView::OnGUI_Top()
	{
		static ImVec4 pressedBtColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		static ImVec2 btSize = ImVec2(20.0f, 20.0f);
		ImGui::BeginChild("SceneView_Top",ImVec2(0, 43.0f), true);
		auto mainCam = Camera::GetMainCamera();
		auto winSize = ImGui::GetWindowSize();

		bool isPushStyle = false;
		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			isPushStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, pressedBtColor);
		}
		if (ImGui::ImageButton(GetIconTextureID(Icon_TRANSLATE), btSize))
		{
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		}
		ImGui::SameLine();
		if (isPushStyle)
		{
			isPushStyle = false;
			ImGui::PopStyleColor();
		}

		if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
		{
			isPushStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, pressedBtColor);
		}
		if (ImGui::ImageButton(GetIconTextureID(Icon_ROTATION), btSize))
		{
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		}ImGui::SameLine();
		if (isPushStyle)
		{
			isPushStyle = false;
			ImGui::PopStyleColor();
		}
		if (mCurrentGizmoOperation == ImGuizmo::SCALE)
		{
			isPushStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, pressedBtColor);
		}
		if (ImGui::ImageButton(GetIconTextureID(Icon_SCALE), btSize))
		{
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		}ImGui::SameLine();
		if (isPushStyle)
		{
			isPushStyle = false;
			ImGui::PopStyleColor();
		}

		ImGui::Dummy(ImVec2(25.0f, 0.0f)); ImGui::SameLine();
		if (mCurrentCameraMode == CameraMode_2D)
		{
			if (ImGui::Button("2D", ImVec2(27, 27)) == true)
			{
				mCurrentCameraMode = CameraMode_3D;
				if (mainCam)
				{
					mainCam->SetOrthographic(false);
				}
			}ImGui::SameLine();
		}
		else
		{
			if (ImGui::Button("3D", ImVec2(27, 27)) == true)
			{
				mCurrentCameraMode = CameraMode_2D;
				if (mainCam)
				{
					mainCam->SetOrthographic(true);
				}
			}ImGui::SameLine();
		}
		
		


		if (mCurrentGizmoMode == ImGuizmo::WORLD)
		{
			if (ImGui::ImageButton(GetIconTextureID(Icon_WORLD), btSize) == true)
			{
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			}ImGui::SameLine();
		}
		else
		{
			if (ImGui::ImageButton(GetIconTextureID(Icon_LOCAL), btSize) == true)
			{
				mCurrentGizmoMode = ImGuizmo::WORLD;
			}ImGui::SameLine();
		}






		isPushStyle = false;
		f32 hw = winSize.x * 0.5f;
		ImGui::SetCursorPosX(hw - 28.0f - ImGui::GetStyle().FramePadding.x);
		if (mCurrentGameControll == Game_Play)
		{
			isPushStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, pressedBtColor);
		}
		if (ImGui::ImageButton(GetIconTextureID(Icon_PLAY), btSize))
		{
			if (mCurrentGameControll == Game_Wait) mCurrentGameControll = Game_Play;
			else if (mCurrentGameControll == Game_Pause) mCurrentGameControll = Game_Play;
			else mCurrentGameControll = Game_Wait;
		}ImGui::SameLine();
		if (isPushStyle)
		{
			isPushStyle = false;
			ImGui::PopStyleColor();
		}
		if (mCurrentGameControll == Game_Pause)
		{
			isPushStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, pressedBtColor);
		}
		if (ImGui::ImageButton(GetIconTextureID(Icon_PAUSE), btSize))
		{
			if (mCurrentGameControll == Game_Play)       mCurrentGameControll = Game_Pause;
			else if (mCurrentGameControll == Game_Pause) mCurrentGameControll = Game_Play;
			else mCurrentGameControll = Game_Wait;
		}ImGui::SameLine();
		if (isPushStyle)
		{
			isPushStyle = false;
			ImGui::PopStyleColor();
		}


		ImGui::AlignTextToFramePadding();
		u64 fps = Application::GetInstance().GetAppTimer()->GetFPS();
		ImGui::Text("FPS : %d", fps); ImGui::SameLine();


	
		auto resolutionStr = ResolutionToString(mCurrentResolution);
		ImGui::SetCursorPosX(winSize.x - 258);
		ImGui::SetNextItemWidth(250.0f);
		ImGui::AlignTextToFramePadding();
		if (ImGui::BeginCombo("##Resolution_Combo", resolutionStr.c_str()) == true)
		{
			for (i32 i = 0; i < Resolution_Max; ++i)
			{
				String str = ResolutionToString(i);
				if (ImGui::Selectable(str.c_str(), false, ImGuiSelectableFlags_None) == true)
				{
					mCurrentResolution = i;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::EndChild();
	}
	void SceneView::OnGUI_Bottom()
	{
		ImGui::BeginChild("SceneView_Bottom", ImVec2(0, 0), true);
		auto winSize = ImGui::GetWindowSize();
		winSize.x -= 16.0f;
		winSize.y -= 18.0f;
		auto sceneTexture = GetSceneTexture();
		auto mainCam = Camera::GetMainCamera();
		if (sceneTexture && sceneTexture->IsValid())
		{
			JVector2 imageSize;

			switch (mCurrentResolution)
			{
			case Resolution_FHD_1920_1080:
				imageSize = GetFitSize(JVector2(winSize.x, winSize.y), 16, 9);
				if (mainCam)
				{
					mainCam->SetResolution(JVector2(1920, 1080));
				}
				break;
			case Resolution_QHD_2560_1440:

				imageSize = GetFitSize(JVector2(winSize.x, winSize.y), 16, 9);
				if (mainCam)
				{
					mainCam->SetResolution(JVector2(2560, 1440));
				}
				break;
			case Resolution_FreeAspect:
				imageSize = JVector2(winSize.x, winSize.y);
				if (mainCam)
				{
					mainCam->SetResolution(JVector2(imageSize.x, imageSize.y));
				}
				break;
			}
			TextureInfo tInfo = sceneTexture->GetTextureInfo();
			auto TexID = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(sceneTexture->GetTextureID());

			ImGui::Image(TexID, ImVec2(imageSize.x, imageSize.y));


			mShowGizmo->Execute(GetSelectedGameNode());
			f32 fixSceneHeight = imageSize.y;
			f32 fixSceneWidth = imageSize.x;
			f32 textureWidth  = tInfo.Width;
			f32 textureHeight = tInfo.Height;
			if (ImGui::IsMouseClicked(0) == true && ImGui::IsItemHovered() && ImGuizmo::IsUsing() == false)
			{
				auto winPos   = ImGui::GetItemRectMin();
				auto mousePos = ImGui::GetMousePos();

				auto standardPos = JVector2(mousePos.x, mousePos.y) - JVector2(winPos.x, winPos.y);
				standardPos.x = (standardPos.x / fixSceneWidth) * textureWidth;
				standardPos.y = (standardPos.y / fixSceneHeight) * textureHeight;

				NotifyEditorSceneOnClickEvent e;
				e.ClickPos = standardPos;
				SendEvent(e);
				JG_CORE_INFO("Scene View MousePos : {0}, {1}", standardPos.x, standardPos.y);
			}
			if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused() == true && mainCam->GetType() == JGTYPE(EditorCamera) && mCurrentCameraMode == 0)
			{
				auto editCamera = static_cast<EditorCamera*>(mainCam);
				f32 zoom = editCamera->GetZoom();
				f32 zoom_restrict = 1.0f;
				if (zoom < 1.0f) zoom_restrict = zoom;
				else zoom_restrict = 1 / zoom;


				f32 wheelDelta = ImGui::GetIO().MouseWheel;
				if (wheelDelta != 0)
				{
					auto winPos = ImGui::GetItemRectMin();
					auto mousePos = ImGui::GetMousePos();
					auto appTimer = Application::GetInstance().GetAppTimer();
					auto tick = appTimer->GetTick();

					auto focusCenter = JVector2(mousePos.x, mousePos.y) - JVector2(winPos.x, winPos.y);

					focusCenter.x = (focusCenter.x / fixSceneWidth);
					focusCenter.y = (focusCenter.y / fixSceneHeight);
					zoom += wheelDelta * tick * 20 * zoom_restrict;
					editCamera->SetZoom(zoom);
					editCamera->SetFocusCenter(focusCenter);
				}
			}







			// 3d 화면 이동
			bool isLeftMouseDown = ImGui::IsMouseDown(1) && ImGui::IsItemHovered();
			if (mEnableEditorCameraControll == false && isLeftMouseDown)
			{
				mMousePos = JVector2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				mPrevMousePos = mMousePos;
			}
			mEnableEditorCameraControll = isLeftMouseDown;
			if (mEnableEditorCameraControll)
			{
				ControllEditorCamera();
			}

		}
		ImGui::EndChild();
	}
	void SceneView::SetSelectedGameNode(GameNode* gameNode)
	{
		mSelectedGameNode = gameNode;
	}
	GameNode* SceneView::GetSelectedGameNode() const
	{
		return mSelectedGameNode;
	}
	void SceneView::SetSceneTexture(SharedPtr<ITexture> sceneTexture)
	{
		mSceneTexture = sceneTexture;
	}
	SharedPtr<ITexture> SceneView::GetSceneTexture() const
	{
		return mSceneTexture;
	}
	bool SceneView::ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e)
	{
		SetSelectedGameNode(e.SelectedGameNode);
		return false;
	}
	bool SceneView::ResponseChangeMainSceneTexture(NotifyChangeMainSceneTextureEvent& e)
	{
		SetSceneTexture(e.SceneTexture);
		return false;
	}
	String SceneView::ResolutionToString(i32 resolution)
	{
		switch (resolution)
		{
		case Resolution_FreeAspect: return "Free Aspect";
		case Resolution_FHD_1920_1080: return "FHD_(1920 x 1080)";
		case Resolution_QHD_2560_1440: return "QHD_(2560 x 1440)";
		default:
			return "None";
		}
	}
	void SceneView::LoadIcons()
	{
		mIcons.resize(MAX_ICON);
		mIcons[Icon_TRANSLATE] = UIManager::GetInstance().GetIcon("Icon_Translation");
		mIcons[Icon_ROTATION] = UIManager::GetInstance().GetIcon("Icon_Rotate");
		mIcons[Icon_SCALE] = UIManager::GetInstance().GetIcon("Icon_Scale");

		mIcons[Icon_WORLD] = UIManager::GetInstance().GetIcon("Icon_World");
		mIcons[Icon_LOCAL] = UIManager::GetInstance().GetIcon("Icon_Local");
		mIcons[Icon_PLAY] = UIManager::GetInstance().GetIcon("Icon_Play");
		mIcons[Icon_PAUSE] = UIManager::GetInstance().GetIcon("Icon_Pause");
		mIcons[Icon_STOP] = UIManager::GetInstance().GetIcon("Icon_Stop");

	}
	ImTextureID SceneView::GetIconTextureID(i32 iconEnum) const
	{
		if (mIcons[iconEnum] && mIcons[iconEnum]->Get() && mIcons[iconEnum]->Get()->IsValid())
		{
			return (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mIcons[iconEnum]->Get()->GetTextureID());
		}


		return (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(ITexture::NullTexture()->GetTextureID());
		
	}
	JVector2 SceneView::GetFitSize(JVector2 originSize, f32 wratio, f32 hratio)
	{
		f32 total = originSize.x + originSize.y;
		f32 x = total * wratio / (wratio + hratio);
		f32 y = total - x;

		if (x > originSize.x)
		{
			x = originSize.x;
			y = x * hratio / wratio;
		}
		else
		{
			y = originSize.y;
			x = y * wratio / hratio;
		}

		return JVector2(x, y);
	}
	void SceneView::ControllEditorCamera()
	{
		auto mainCam = Camera::GetMainCamera();
		if (mainCam == nullptr)
		{
			return;
		}
		// 카메라 이동 시작
		mMousePos = JVector2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);

		JVector2 mouseDelta = JVector2(0.0f, 0.0f);
		if (mMousePos != mPrevMousePos)
		{
			mouseDelta = mMousePos - mPrevMousePos;
			mPrevMousePos = mMousePos;
		}


		static f32 cameraSensitivity = 60.0f;
		static f32 cameraSpeed = 15.0f;
		auto appTimer = Application::GetInstance().GetAppTimer();
		auto tick = appTimer->GetTick();


		auto camTransform = mainCam->GetOwner()->GetTransform();

		if (mCurrentCameraMode == 0)
		{
			f32 offset = cameraSpeed * tick * cameraSpeed;
			JVector3 location = camTransform->GetLocalLocation();
			location.x += (-mouseDelta.x * offset);
			location.y += (mouseDelta.y * offset);
			camTransform->SetLocalLocation(location);
		}
		else
		{
			JVector3 rotation = camTransform->GetLocalRotation();
			rotation.x += mouseDelta.y * cameraSensitivity * tick;
			rotation.y += mouseDelta.x * cameraSensitivity * tick;



			if (rotation.x < 260.0f && rotation.x >= 180.0f)
			{
				rotation.x = std::max<f32>(rotation.x, 260.0f);
			}
			if (rotation.x >= 80.0f && rotation.x < 180.0f)
			{
				rotation.x = std::min<f32>(rotation.x, 80.0f);
			}

		
			
			camTransform->SetLocalRotation(rotation);

			JVector3 location = camTransform->GetLocalLocation();
			f32 offset = cameraSpeed * tick;
			if (ImGui::IsKeyDown((int)EKeyCode::W))
			{
				location += mainCam->GetLook() * offset;
				//loca
			}
			if (ImGui::IsKeyDown((int)EKeyCode::S))
			{
				location -= mainCam->GetLook() * offset;
			}
			if (ImGui::IsKeyDown((int)EKeyCode::A))
			{
				location -= mainCam->GetRight() * offset;
			}
			if (ImGui::IsKeyDown((int)EKeyCode::D))
			{
				location += mainCam->GetRight() * offset;
			}
			if (ImGui::IsKeyDown((int)EKeyCode::Q))
			{
				location.y -= offset;
			}
			if (ImGui::IsKeyDown((int)EKeyCode::E))
			{
				location.y += offset;
			
			}
			if (ImGui::GetIO().MouseWheel != 0.0f)
			{
				JVector3 look = mainCam->GetLook();
				look = look * offset * ImGui::GetIO().MouseWheel;
				location += look;
			}


			camTransform->SetLocalLocation(location);
		}
		

	}
}