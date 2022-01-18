#include "pch.h"
#include "DevelopView.h"
#include "Application.h"
#include "UI/UIManager.h"
#include "UI/UIView/SceneView.h"
#include "ExternalImpl/JGImGui.h"
#include "Components/Camera.h"

#
#include "Graphics/Renderer.h"
#include "Graphics/PostRenderProcess/PostProcess_Bloom.h"
#include "Graphics/PostRenderProcess/PostProcess_ToneMapping.h"
namespace JG
{



	DevelopView::DevelopView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/Devlop", 0, [&]()
		{
			Open();
		}, nullptr);


	}

	void DevelopView::Load()
	{
		mIsRPHeaderOpen  = StorableBool("DevelopView/RendererParamHeader/IsOpen", false);
		mIsEtcHeaderOpen = StorableBool("DevelopView/EtcParamHeader/IsOpen", false);
	}

	void DevelopView::Initialize()
	{


	}

	void DevelopView::OnGUI()
	{
		ImGui::Begin("Develop", &mOpenGUI);







		if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen) == true)
		{
			Statistics_OnGUI();
		}
		Dev_OnGUI();


		ImGui::End();





		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void DevelopView::Destroy()
	{
	}

	void DevelopView::OnEvent(IEvent& e)
	{
	}

	void DevelopView::Statistics_OnGUI()
	{
		int fps = Application::GetInstance().GetAppTimer()->GetFPS();
		ImGui::Text("FPS : %d", fps);

		ImGui::Text("Draw Total Object : %d", Renderer::Statistics.TotalObjectCount);
		ImGui::Text("Draw Object : %d", Renderer::Statistics.VisibleObjectCount);
		ImGui::Text("Culling Object : %d", Renderer::Statistics.CullingObjectCount);
	}

	void DevelopView::Dev_OnGUI()
	{
		enum 
		{
			Debug_None,
			Debug_Visible_Cluster,
			Debug_Count,
		};
		Camera* mainCamera			  = Camera::GetMainCamera();
		Graphics::Scene* scene		  = nullptr;


		if (mainCamera)
		{
			scene = mainCamera->GetScene();
			auto renderer = scene->GetRenderer();
			if (renderer)
			{
				mRenderParamManager = renderer->GetRenderParamManager();
			}
		}

		InitRenderParamInfo(mRenderParamManager);
		if(mRenderParamManager != nullptr)
		{
			if (ImGui::CollapsingHeader("[Render Texture]", ImGuiTreeNodeFlags_DefaultOpen) == true)
			{
				ImGui::Text("Current Texture : %s", mSelectedTextureParamKey.GetValue().c_str());
				if (ImGui::BeginCombo("##RenderTextureParamCombo", mSelectedTextureParamKey.GetValue().c_str()) == true)
				{
					String selectedTextureParamKey;

					if (ImGui::Selectable("Final",  "Final" == mSelectedTextureParamKey.GetValue()) == true)
					{
						selectedTextureParamKey = "Final";
					}

					mRenderParamManager->ForEach([&](const String& key, SharedPtr<ITexture> tex)
					{
						if (ImGui::Selectable(key.c_str(), key == mSelectedTextureParamKey.GetValue()) == true)
						{
							selectedTextureParamKey = key;
						}
					});
					ImGui::EndCombo();

					if (mSelectedTextureParamKey.GetValue() != selectedTextureParamKey)
					{
						mSelectedTextureParamKey.SetValue(selectedTextureParamKey);
					}

				}
			}
			if (ImGui::CollapsingHeader("[Renderer Params Controller]", ImGuiTreeNodeFlags_DefaultOpen) == true)
			{
				RendererParams_OnGUI();
			}

			SceneView* sceneView = UIManager::GetInstance().GetUIView<SceneView>();
			if (sceneView != nullptr)
			{
				if (mSelectedTextureParamKey.GetValue() == "Final")
				{
					sceneView->SetSceneTexture(nullptr);
				}
				else
				{
					sceneView->SetSceneTexture(RP_Global_Tex::Load(mSelectedTextureParamKey.GetValue(), mRenderParamManager).GetValue());
				}
			}
		}




		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();
	}

	void DevelopView::RendererParams_OnGUI()
	{

		ImGuiTableFlags    tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

		// RenderParam
		{
			ImGuiTreeNodeFlags treeNodeFlags = 0;
			bool isOpen = mIsRPHeaderOpen.GetValue();
			if (isOpen)
			{
				treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
			}
			isOpen = ImGui::TreeNodeEx("[Renderer Params]", treeNodeFlags);
			if (isOpen)
			{
				if (ImGui::BeginTable("[Renderer Params Table]", 2, tableFlags) == true)
				{
					ImGui::TableNextColumn();
					for (auto& _pair : mRendererParmDic)
					{
						ParamInfo_OnGUI<RendererParamInfo>(_pair.first, _pair.second);
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
			mIsRPHeaderOpen.SetValue(isOpen);

		}


		// Param2
		for (auto& _pair : mParam2Dic)
		{
			ImGuiTreeNodeFlags treeNodeFlags = 0;
			String headerName = "[" + _pair.first + "]";
			String tableName  = "[" + _pair.first + " Table ]";

			bool isOpen = mIsP2HeaderOpen[_pair.first].GetValue();
			if (isOpen)
			{
				treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
			}
			isOpen = ImGui::TreeNodeEx(headerName.c_str(), treeNodeFlags);
			if (isOpen)
			{
				if (ImGui::BeginTable(headerName.c_str(), 2, tableFlags) == true)
				{
					ImGui::TableNextColumn();
					for (auto& _infoPair : _pair.second)
					{
						ParamInfo_OnGUI<Param2Info>(_infoPair.first, _infoPair.second);
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
			mIsP2HeaderOpen[_pair.first].SetValue(isOpen);
		}


		for (auto& _pair : mParam3Dic)
		{
			ImGuiTreeNodeFlags treeNodeFlags = 0;
			String headerName = "[" + _pair.first + "]";
			String tableName  = "[" + _pair.first + " Table ]";

			bool is3dOpen = mIsP3Header1Open[_pair.first].GetValue();
			if (is3dOpen)
			{
				treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
			}
			is3dOpen = ImGui::TreeNodeEx(headerName.c_str(), treeNodeFlags);
			if (is3dOpen)
			{
				for (auto& _infoPair : _pair.second)
				{
					Param3Dic_OnGUI(_pair.first, _infoPair.first, _infoPair.second);
				}
				ImGui::TreePop();
			}
			mIsP3Header1Open[_pair.first].SetValue(is3dOpen);
		}

		{
			ImGuiTreeNodeFlags treeNodeFlags = 0;
			bool isOpen = mIsEtcHeaderOpen.GetValue();
			if (isOpen)
			{
				treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
			}
			isOpen = ImGui::TreeNodeEx("[Etc Params]", treeNodeFlags);
			if (isOpen)
			{
				if (ImGui::BeginTable("[Etc Params Table]", 2, tableFlags) == true)
				{
					ImGui::TableNextColumn();
					for (auto& _pair : mEtcParamDic)
					{
						ParamInfo_OnGUI<EtcParamInfo>(_pair.first, _pair.second);
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
			mIsEtcHeaderOpen.SetValue(isOpen);

		}

	}



	void DevelopView::Param3Dic_OnGUI(const String& param1, const String& param2, const Dictionary<String, Param3Info>& dic)
	{
		ImGuiTableFlags    tableFlags    = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
		ImGuiTreeNodeFlags treeNodeFlags = 0;
		bool isOpen = mIsP3Header2Open[param1][param2].GetValue();
		if (isOpen)
		{
			treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
		}
		isOpen = ImGui::TreeNodeEx(param2.c_str(), treeNodeFlags);
		if (isOpen)
		{
			if (ImGui::BeginTable(("[" + param1 + "/" + param2 + " Params Table]").c_str(), 2, tableFlags) == true)
			{
				ImGui::TableNextColumn();
				for (auto& _pair : dic)
				{
					ParamInfo_OnGUI<Param3Info>(_pair.first, _pair.second);
				}
				ImGui::EndTable();
			}

			ImGui::TreePop();
		}
		mIsP3Header2Open[param1][param2].SetValue(isOpen);
	}



	void DevelopView::Param_OnGUI(const String& key, const Type& type)
	{
		if (type == JGTYPE(bool))
		{
			RP_Global_Bool val = RP_Global_Bool::Load(key, mRenderParamManager);
			bool valResult = ImGui::CheckBox((void*)(("##" + key).c_str()), val.GetValue());
			val.SetValue(valResult);
		}
		else if (type == JGTYPE(f32))
		{
			RP_Global_Float val = RP_Global_Float::Load(key, mRenderParamManager);
			f32 valResult = val.GetValue();
			if (ImGui::DragFloat(("##" + key).c_str(), &valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(JVector2))
		{
			RP_Global_Float2 val = RP_Global_Float2::Load(key, mRenderParamManager);
			JVector2 valResult = val.GetValue();
			if (ImGui::DragFloat2(("##" + key).c_str(), (float*)&valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(JVector3))
		{
			RP_Global_Float3 val = RP_Global_Float3::Load(key, mRenderParamManager);
			JVector3 valResult = val.GetValue();
			if (ImGui::DragFloat3(("##" + key).c_str(), (float*)&valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(JVector4))
		{
			RP_Global_Float4 val = RP_Global_Float4::Load(key, mRenderParamManager);
			JVector4 valResult = val.GetValue();
			if (ImGui::DragFloat4(("##" + key).c_str(), (float*)&valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(i32))
		{
			RP_Global_Int val = RP_Global_Int::Load(key, mRenderParamManager);
			i32 valResult = val.GetValue();
			if (ImGui::DragInt(("##" + key).c_str(), &valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(JVector2Int))
		{
			RP_Global_Int2 val = RP_Global_Int2::Load(key, mRenderParamManager);
			JVector2Int valResult = val.GetValue();
			if (ImGui::DragInt2(("##" + key).c_str(), (int*)&valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(JVector3Int))
		{
			RP_Global_Int3 val = RP_Global_Int3::Load(key, mRenderParamManager);
			JVector3Int valResult = val.GetValue();
			if (ImGui::DragInt3(("##" + key).c_str(), (int*)&valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else if (type == JGTYPE(JVector4Int))
		{
			RP_Global_Int4 val = RP_Global_Int4::Load(key, mRenderParamManager);
			JVector4Int valResult = val.GetValue();
			if (ImGui::DragInt4(("##" + key).c_str(), (int*)&valResult) == true)
			{
				val.SetValue(valResult);
			}
		}
		else
		{
			ImGui::Text("None");
		}
	}

	void DevelopView::InitRenderParamInfo(RenderParamManager* rpManager)
	{
		if (rpManager == nullptr)
		{
			return;
		}

	
		rpManager->ForEach([&](const String& key, const RenderParamManager::RenderParamInfo& rpInfo)
		{
			List<String> tokens = StringHelper::Split(key, '/');
			if (tokens.empty())
			{
				return;
			}

			// RendererParam
			if (tokens[0] == "Renderer" && tokens.size() == 2)
			{
				if (mRendererParmDic.find(key) == mRendererParmDic.end())
				{
					RendererParamInfo info;
					info.Key     = key;
					info.VarName = tokens[1];
					info.Type    = rpInfo.GetDataType();
					mRendererParmDic[key] = info;
				}
			}
			else if (tokens.size() == 2)
			{
				if (mParam2Dic[tokens[0]].find(key) == mParam2Dic[tokens[0]].end())
				{
					Param2Info info;
					info.Key	 = key;
					info.Param1  = tokens[0];
					info.VarName = tokens[1];
					info.Type	 = rpInfo.GetDataType();
					mParam2Dic[tokens[0]][key] =  info;
					if (mIsP2HeaderOpen.find(info.Param1) == mIsP2HeaderOpen.end())
					{
						mIsP2HeaderOpen[info.Param1] = StorableBool(GetType().GetName() + "/" + info.Param1 + "/IsOpen", false);
					}
				}
			}
			else if (tokens.size() == 3)
			{
				if (mParam3Dic[tokens[0]][tokens[1]].find(tokens[2]) == mParam3Dic[tokens[0]][tokens[1]].end())
				{
	
					Param3Info info;
					info.Key	 = key;
					info.Param1  = tokens[0];
					info.Param2  = tokens[1];
					info.VarName = tokens[2];
					info.Type = rpInfo.GetDataType();

					mParam3Dic[tokens[0]][tokens[1]][key] = info;

					if (mIsP3Header1Open.find(info.Param1) == mIsP3Header1Open.end())
					{
						mIsP3Header1Open[info.Param1] = StorableBool(GetType().GetName() + "/" + info.Param1 + "_IsOpen", false);
					}
					if (mIsP3Header1Open.find(info.Param1) != mIsP3Header1Open.end())
					{
						if (mIsP3Header2Open[info.Param1].find(info.Param2) == mIsP3Header2Open[info.Param1].end())
						{
							mIsP3Header2Open[info.Param1][info.Param2] = StorableBool(GetType().GetName() + "/" + info.Param1 + "/" + info.Param2 + "/IsOpen", false);
						}
					}

				}
			}
			else
			{
				if (mEtcParamDic.find(key) == mEtcParamDic.end())
				{
					EtcParamInfo info;
					info.Key		  = key;
					info.VarName	  = tokens[tokens.size() - 1];
					info.Type		  = rpInfo.GetDataType();
					mEtcParamDic[key] = info;
				}
			}



		});
	}

}