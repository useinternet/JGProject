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

		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		mCurrentGizmoMode = ImGuizmo::LOCAL;
	}
	void SceneView::Load()
	{

	}
	void SceneView::Initialize()
	{
		auto viewModel = GetViewModel();
		viewModel->SetMinSize(JVector2(820, 620));
		viewModel->ShowGizmo->Subscribe(viewModel, [&](GameNode* node)
		{
			auto mainCam = Camera::GetMainCamera();

			if (mEnableEditorCameraControll == false) {
				if (ImGui::IsKeyPressed((int)EKeyCode::Q))
					mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
				if (ImGui::IsKeyPressed((int)EKeyCode::W))
					mCurrentGizmoOperation = ImGuizmo::ROTATE;
				if (ImGui::IsKeyPressed((int)EKeyCode::E))
					mCurrentGizmoOperation = ImGuizmo::SCALE;
			}

		
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


			JVector3 location = node->GetTransform()->GetLocalLocation();
			JVector3 rotation = Math::ConvertToRadians(node->GetTransform()->GetLocalRotation());
			JVector3 scale = node->GetTransform()->GetScale();
			JMatrix worldMat;
			ImGuizmo::RecomposeMatrixFromComponents((float*)&location, (float*)&rotation, (float*)&scale, worldMat.GetFloatPtr());
			auto itemMin = ImGui::GetItemRectMin();
			auto itemSize = ImGui::GetItemRectSize();
			ImGuizmo::SetRect(itemMin.x, itemMin.y, itemSize.x, itemSize.y);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetOrthographic(mainCam->IsOrthographic());
	
			//ImGuizmo::SetGizmoSizeClipSpace
			auto view = mainCam->GetViewMatrix();
			auto proj = mainCam->GetProjMatrix();


			ImGui::PushClipRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true);
			bool result = ImGuizmo::Manipulate(
				view.GetFloatPtr(), proj.GetFloatPtr(), 
				(ImGuizmo::OPERATION)mCurrentGizmoOperation, (ImGuizmo::MODE)mCurrentGizmoMode,
				worldMat.GetFloatPtr(), nullptr , (mIsSnap) ? snapValue : nullptr, nullptr, nullptr);
			ImGui::PopClipRect();
			JVector3 matrixTranslation, matrixRotation, matrixScale;
			ImGuizmo::DecomposeMatrixToComponents(worldMat.GetFloatPtr(), (float*)&matrixTranslation, (float*)&matrixRotation, (float*)&matrixScale);
			node->GetTransform()->SetLocalLocation(matrixTranslation);
			node->GetTransform()->SetLocalRotation(Math::ConvertToDegrees(matrixRotation));
			node->GetTransform()->SetScale(matrixScale);

		});

	}
	void SceneView::OnGUI()
	{
		auto viewModel = GetViewModel();
		ImGuizmo::BeginFrame();

		ImGui::Begin("SceneView", &mOpenGUI);
		

		auto minSize = viewModel->GetMinSize();
		auto currSize = ImGui::GetWindowSize();
	


		if (currSize.x <= minSize.x || currSize.y <= minSize.y)
		{
			ImGui::SetWindowSize(ImVec2(minSize.x, minSize.y));
		}


		auto mainCam = Camera::GetMainCamera();



		ImGui::Text("T"); ImGui::SameLine(); 
		if (ImGui::RadioButton("##TranslateRadioButton", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		{
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		}ImGui::SameLine();

		ImGui::Text("R"); ImGui::SameLine();
		if (ImGui::RadioButton("##RotateRadioButton", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		{
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		}ImGui::SameLine();

		ImGui::Text("S"); ImGui::SameLine();
		if (ImGui::RadioButton("##ScalingRadioButton", mCurrentGizmoOperation == ImGuizmo::SCALE))
		{
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		}ImGui::SameLine();

		
		std::string name = "";


		// Gizmo 모드
		{
			if (mCurrentGizmoMode == ImGuizmo::LOCAL) name = "Local";
			else name = "World";

			if (ImGui::Button(name.c_str()) == true)
			{
				mCurrentGizmoMode = !mCurrentGizmoMode;
			}ImGui::SameLine();
		}

		// Gizmo Camera Mode
		{
			auto mainCam = Camera::GetMainCamera();

			if (mCurrentCameraMode == 0)
			{
				name = "2D";
				mainCam->GetOwner()->GetTransform()->SetLocalRotation(JVector3(0, 0, 0));
				auto location = mainCam->GetOwner()->GetTransform()->GetLocalLocation(); location.z = -10;
				mainCam->GetOwner()->GetTransform()->SetLocalLocation(location);
			}
			else name = "3D";

			if (ImGui::Button(name.c_str()) == true)
			{
				mCurrentCameraMode = !mCurrentCameraMode;
			}ImGui::SameLine();

			if (mainCam != nullptr)
			{
				mainCam->SetOrthographic(!mCurrentCameraMode);
			}
		}

		// Gizmo Snap
		{
			if (ImGui::RadioButton("Snap", mIsSnap))
			{
				mIsSnap = !mIsSnap;
			} ImGui::SameLine();
			ImGui::SetNextItemWidth(210.0f);
			ImGui::InputFloat3("##Snap_InputFloat", mSnapValue); ImGui::SameLine();
		}
		{
			ImGui::Text("FPS : %d", Application::GetInstance().GetAppTimer()->GetFPS());

		}

		// Camera Settings
		
		// SceneTexture
		f32 sceneRatio = 0.0f;
		f32 fixSceneHeight = 0.0f;
		f32 fixSceneWidth  = 0.0f;
		f32 textureWidth = 0.0f;
		f32 textureHeight = 0.0f;
		auto sceneTexture = viewModel->GetSceneTexture();
		if (sceneTexture != nullptr && sceneTexture->IsValid())
		{
			auto textureInfo = sceneTexture->GetTextureInfo();
			sceneRatio     = (f32)textureInfo.Width / (f32)textureInfo.Height;
			fixSceneHeight = 650;
			fixSceneWidth  = fixSceneHeight * sceneRatio;
			textureWidth   = textureInfo.Width;
			textureHeight  = textureInfo.Height;
			auto textureID = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(sceneTexture->GetTextureID());
			ImGui::Image(textureID, ImVec2(fixSceneWidth, fixSceneHeight), ImVec2(0,0), ImVec2(0.9999f, 0.9999f));
			viewModel->ShowGizmo->Execute(viewModel->GetSelectedGameNode());
		}






		// Picking & 2D & 이동 모드 일시 화면 이동
		if (ImGui::IsMouseClicked(0) == true && ImGui::IsWindowHovered()) 
		{
			auto winPos   = ImGui::GetItemRectMin();
			auto mousePos = ImGui::GetMousePos();

			auto standardPos = JVector2(mousePos.x, mousePos.y) - JVector2(winPos.x ,winPos.y);
			standardPos.x = (standardPos.x / fixSceneWidth) * textureWidth;
			standardPos.y = (standardPos.y / fixSceneHeight) * textureHeight;

			viewModel->OnClick(standardPos, 0);
			JG_CORE_INFO("Scene View MousePos : {0}, {1}", standardPos.x, standardPos.y);
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

		// 카메라 줌 아웃
		// 
		if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused() == true && mainCam->GetType() == JGTYPE(EditorCamera) && mCurrentCameraMode == 0)
		{
			auto editCamera = static_cast<EditorCamera*>(mainCam);
			f32 zoom = editCamera->GetZoom();
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

				JG_CORE_INFO("focusCenter : {0}", focusCenter.ToString())
				zoom += wheelDelta * tick * 20;
				editCamera->SetZoom(zoom);
				editCamera->SetFocusCenter(focusCenter);
			}
		}


		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void SceneView::Destroy()
	{
		auto viewModel = GetViewModel();
		viewModel->ShowGizmo->UnSubscribe(viewModel);
	}
	void SceneView::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		UIView<SceneViewModel>::MakeJson(jsonData);
		jsonData->AddMember("GizmoOperation", mCurrentGizmoOperation);
		jsonData->AddMember("GizmoMode", mCurrentGizmoMode);
		jsonData->AddMember("CameraMode", mCurrentCameraMode);


		jsonData->AddMember("SnapValue0", mSnapValue[0]);
		jsonData->AddMember("SnapValue1", mSnapValue[1]);
		jsonData->AddMember("SnapValue2", mSnapValue[2]);
	}
	void SceneView::LoadJson(SharedPtr<JsonData> jsonData)
	{
		UIView<SceneViewModel>::LoadJson(jsonData);
		auto val = jsonData->GetMember("GizmoOperation");
		if (val)
		{
			mCurrentGizmoOperation = val->GetInt32();
		}
		val = jsonData->GetMember("GizmoMode");
		if (val)
		{
			mCurrentGizmoMode = val->GetInt32();
		}
		val = jsonData->GetMember("CameraMode");
		if (val)
		{
			mCurrentCameraMode = val->GetInt32();
		}

		val = jsonData->GetMember("SnapValue0");
		if (val)
		{
			mSnapValue[0] = val->GetFloat();
		}
		val = jsonData->GetMember("SnapValue1");
		if (val)
		{
			mSnapValue[1] = val->GetFloat();
		}
		val = jsonData->GetMember("SnapValue2");
		if (val)
		{
			mSnapValue[2] = val->GetFloat();
		}
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


		static f32 cameraSensitivity = 30.0f;
		static f32 cameraSpeed = 15.0f;
		auto appTimer = Application::GetInstance().GetAppTimer();
		auto tick = appTimer->GetTick();


		auto camTransform = mainCam->GetOwner()->GetTransform();

		if (mCurrentCameraMode == 0)
		{
			f32 offset = cameraSpeed * tick * 10;
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