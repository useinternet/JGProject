#include "pch.h"
#include "GameInspectorUI.h"





#include "UI/UIManager.h"
#include "UI/UIView/InspectorView.h"
#include "UI/ContextUI/ComponentFinderContextView.h"
namespace JG
{
	void GameNodeInspectorUI::OnGUI_Impl(GameNode* obj)
	{
		const float width    = ImGui::GetWindowWidth();
		const float wpadding = ImGui::GetStyle().FramePadding.x;
		ImGui::Dummy(ImVec2(0, 1.0f));
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name"); ImGui::SameLine();

		// 게임 노드 이름
		char objName[128] = { 0, };
		strcpy(objName, obj->GetName().c_str());
		const float input_width = width * 0.4f;
		ImGui::SetNextItemWidth(input_width);
		if (ImGui::InputText(("##GameNode InputName"), objName, 128))
		{
			obj->SetName(objName);
		}

		// 레이어
		const float combo_width = width * 0.2f;
		ImGui::SameLine(width - ImGui::CalcTextSize("Layer").x - wpadding * 2 - combo_width);
		ImGui::Text("Layer"); ImGui::SameLine();
		ImGui::SetNextItemWidth(combo_width);
		ImGui::SameLine(width - combo_width - wpadding);
		if (ImGui::BeginCombo("##Layer Combo Box", obj->GetLayer().c_str()))
		{
			GameLayerManager::GetInstance().ForEach([&](const String& layerName)
			{
				bool _bool = false;
				if (ImGui::Selectable(layerName.c_str(), &_bool))
				{
					obj->SetLayer(layerName);
				}
			});
			ImGui::EndCombo();
		}


		ImGui::Dummy(ImVec2(0, 1.0f));
		ImGui::Separator();
		List<GameComponent*> removeComList;
		obj->ForEach([&](GameComponent* com)
		{
			bool is_open = true;
			ImGui::Spacing();
			auto id = com->GetName() + "##" + std::to_string((ptraddr)com);
			if (ImGui::CollapsingHeader(
				id.c_str(), &is_open, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) == true)
			{
				UIManager::GetInstance().ShowInspectorUI(com);
			}

			if (is_open == false)
			{
				removeComList.push_back(com);
			}
		});

		for (auto& com : removeComList)
		{
			obj->Destroy(com);
		}

		ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();
		auto padding = ImGui::GetStyle().FramePadding;
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - (padding.x * 4), 0)) == true)
		{
			UIManager::GetInstance().OpenPopupUIView<ComponentFinderContextView>(ComponentFinderInitData());
		}
		if (UIManager::GetInstance().OnContextUIView<ComponentFinderContextView>())
		{
			auto comFinder     = UIManager::GetInstance().GetPopupUIView<ComponentFinderContextView>();
			auto selectedType  = comFinder->GetSelectedComponent();
			auto inspectorView = UIManager::GetInstance().GetUIView<InspectorView>();
			if (inspectorView)
			{
				inspectorView->SelectComponentType(selectedType);
			}
		}

	}
	void GameWorldInspectorUI::OnGUI_Impl(GameWorld* obj)
	{
		ImGui::Dummy(ImVec2(0, 1.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name : "); ImGui::SameLine();
		ImGui::Text(obj->GetName().c_str()); ImGui::SameLine();
		ImGui::Text("Instance : "); ImGui::SameLine();
		ImGui::Text(std::to_string((u64)this).c_str());
		ImGui::Dummy(ImVec2(0, 1.0f));
		ImGui::Separator();


		if (ImGui::CollapsingHeader("Physcis", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) == true)
		{
			auto gravity = obj->GetGravity();
			i32 label_space = ImGui::CalcTextSize("Gravity").x;
			if (ImGui::Vector3_OnGUI("Gravity", gravity, label_space) == true)
			{
				obj->SetGravity(gravity);
			}
		}
	}
	void TransformInspectorUI::OnGUI_Impl(Transform* obj)
	{
		float padding = ImGui::CalcTextSize("Location").x - ImGui::CalcTextSize("Scale").x;

		auto location = obj->GetLocalLocation();
		auto rotation = obj->GetLocalRotation();
		auto scale    = obj->GetLocalScale();


		f32 label_width = ImGui::CalcTextSize("Location").x;


		if (ImGui::Vector3_OnGUI("Location", location, label_width) == true)
		{
			obj->SetLocalLocation(location);
		}
		if (ImGui::Vector3_OnGUI("Rotation", rotation, label_width) == true)
		{
			obj->SetLocalRotation(rotation);
		}
		if (ImGui::Vector3_OnGUI("Scale", scale, label_width) == true)
		{
			obj->SetLocalScale(scale);
		}


	}
	void CameraInspectorUI::OnGUI_Impl(Camera* obj)
	{
		JVector2 resolution = obj->GetResolution();
		f32 fov = obj->GetFOV();
		Color color = obj->GetClearColor();
		f32 nearZ = obj->GetNearZ();
		f32 farZ = obj->GetFarZ();
		bool isOrth = obj->IsOrthographic();

		f32 label_width = ImGui::CalcTextSize("Field of View").x;

		ImGui::Vector2_OnGUI("Resolution", resolution, label_width);
		ImGui::Float_OnGUI("Field of View", fov, label_width);
		ImGui::Float_OnGUI("NearZ", nearZ, label_width);
		ImGui::Float_OnGUI("FarZ", farZ, label_width);
		ImGui::Bool_OnGUI("Orthographic", isOrth, label_width);
		ImGui::Color4_OnGUI("ClearColor", color, label_width);



		obj->SetFOV(fov);
		obj->SetClearColor(color);
		obj->SetFarZ(farZ);
		obj->SetNearZ(nearZ);
		obj->SetOrthographic(isOrth);
		obj->SetResolution(resolution);
	}
	void EditorCameraInspectorUI::OnGUI_Impl(EditorCamera* obj)
	{
		UIManager::GetInstance().ForceShowInspectorUI<Camera>(obj);
	}
	void PointLightInspectorUI::OnGUI_Impl(PointLight* obj)
	{
		ImGui::Spacing();

		Color color = obj->GetColor();
		f32 intensity = obj->GetIntensity();
		f32 range = obj->GetRange();
		f32 attRange = obj->GetAttRange();



		f32 label_width = ImGui::CalcTextSize("Intensity").x;
		ImGui::Color3_OnGUI("Color", color, label_width);
		ImGui::Float_OnGUI("Intensity", intensity, label_width, 0.0f);
		ImGui::Float_OnGUI("Range", range, label_width, 0.0f);
		ImGui::Float_OnGUI("AttRange", attRange, label_width, 0.0f);

		obj->SetColor(Color(color.R, color.G, color.B, 1.0f));
		obj->SetIntensity(intensity);
		obj->SetRange(range);
		obj->SetAttRange(attRange);
	}
	void DirectionalLightInspectorUI::OnGUI_Impl(DirectionalLight* obj)
	{
		ImGui::Spacing();

		Color color   = obj->GetColor();
		f32 intensity = obj->GetIntensity();
		f32 distance  = obj->GetDistance();
		f32 label_width = ImGui::CalcTextSize("Intensity").x;
		ImGui::Color3_OnGUI("Color", color, label_width);
		ImGui::Float_OnGUI("Intensity", intensity, label_width, 0.0f);
		ImGui::Float_OnGUI("Distance", distance, label_width, 0.0f);

		obj->SetColor(Color(color.R, color.G, color.B, 1.0f));
		obj->SetIntensity(intensity);
		obj->SetDistance(distance);
	}
	void SkyDomeInspectorUI::OnGUI_Impl(SkyDome* obj)
	{

	}
	void SpriteRendererInspectorUI::OnGUI_Impl(SpriteRenderer* obj)
	{
		ImGui::AlignTextToFramePadding();

		auto color = obj->GetColor();

		f32 label_width = ImGui::CalcTextSize("Sprite").x;

		ImGui::Color4_OnGUI("Color", color, label_width);


		if (obj->GetSprite())
		{
			ImGui::AssetField_OnGUI("Sprite ", obj->GetSprite()->GetAssetName(), EAssetFormat::Texture, [&](const String& path)
			{
				obj->SetSprite(path);
			}, label_width);

		}
		else
		{
			ImGui::AssetField_OnGUI("Sprite ", "None", EAssetFormat::Texture, [&](const String& path)
			{
				obj->SetSprite(path);
			}, label_width);
		}

		obj->SetColor(color);
	}
	void StaticMeshRendererInspectorUI::OnGUI_Impl(StaticMeshRenderer* obj)
	{
		f32 label_width = ImGui::CalcTextSize("Materials").x;



		String inputText = "None";
		if (obj->GetMesh())
		{
			inputText = obj->GetMesh()->GetAssetName();
		}
		ImGui::AssetField_OnGUI("Mesh", inputText, EAssetFormat::Mesh, [&](const std::string& path)
		{
			obj->SetMesh(path);
		}, label_width);


		List<String> inputTextList;
		obj->ForEach([&](Asset<IMaterial>* m)
		{
			if (m == nullptr) {
				inputTextList.push_back("None");
			}
			else
			{
				inputTextList.push_back(m->GetAssetName());
			}
		});
		ImGui::AssetField_List_OnGUI("Materials", inputTextList, EAssetFormat::Material,
			[&](int index, const std::string& path)
		{
			obj->SetMaterial(index, path);
		}, [&]()
		{
			obj->AddMaterial("");
		}, [&]()
		{
			obj->PopMaterial();
		}, label_width);
	}



}