#include "pch.h"
#include "AnimationView.h"
#include "Application.h"
#include "Animation/JGAnimation.h"
#include "Animation/AnimationClip.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationParameters.h"
#include "Animation/AnimationTransition.h"
#include "Animation/AnimationStateMachine.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetHelper.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"
#include "UI/NodeGUI.h"
#include "UI/UIManager.h";

namespace JG
{
	AnimationView::AnimationView()
	{
		DisableUniqueView();

	}
	void AnimationView::SetAnimation(const String& animatinoAssetPath)
	{
		String assetPath;
		AssetHelper::GetResourcePath(animatinoAssetPath, nullptr, &assetPath);

		mModelAssetPath = StorableString("AnimationView/ModelAssetPath/" + assetPath, "");
		mSkeletoneAssetPath = StorableString("AnimationView/SkeletoneAssetPath/" + assetPath, "");
		mMaterialAssetPath = StorableString("AnimationView/MaterialAssetPath/" + assetPath, "");

	}
	void AnimationView::Load()
	{
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/AnimationClip", 0, [&]()
		{
			CreateAnimationClipNode();
			JG_LOG_INFO("Create Node");
		}, nullptr);

		mBgColorByParamTypeDic[EAnimationParameterType::Bool]  = Color(0.8f, 0.1f, 0.1f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Float] = Color(0.4f, 0.8f, 0.05f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Int]  = Color(0.05f,0.4f,0.8f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Unknown] = Color(0.5f, 0.5f, 0.5f, 1.0f);
	}

	void AnimationView::Initialize()
	{
		mNodeEditor = CreateUniquePtr<StateNodeGUI::StateNodeEditor>();
		mNodeEditor->BindContextMenuFunc([&]()
		{
			UIManager::GetInstance().ShowContextMenu(GetType(), false);
		});
		mNodeEditor->BindLinkNodeCallBack(
			[&](StateNodeGUI::StateNodeID fromID, StateNodeGUI::StateNodeID toID, StateNodeGUI::StateNodeID transID)
		{
			AnimTransitionBuildData& buildData = mAnimTransitionBuildDataDic[transID];
			buildData.From = fromID;
			buildData.To = toID;
		});
		mNodeEditor->BindRemoveNodeCallBack([&](StateNodeGUI::StateNodeID id)
		{
			mAnimClipBuildDataDic.erase(id);
		});
		mNodeEditor->BindRemoveTransitionCallBack([&](StateNodeGUI::StateNodeID id)
		{
			mAnimTransitionBuildDataDic.erase(id);
		});


		SetMesh(mModelAssetPath.GetValue());
		SetSkeletal(mSkeletoneAssetPath.GetValue());
		SetMaterial(StringHelper::Split(mMaterialAssetPath.GetValue(), ','));
		CreateRootNode();

		// 임시
		mAnimController = CreateUniquePtr<AnimationController>();
		JGAnimation::GetInstance().RegisterAnimationController(mAnimController);
	}

	void AnimationView::OnGUI()
	{
		UpdateScene();
		ImVec2 winSize = ImGui::GetWindowSize();
		f32 minWinWidth = Math::Max(winSize.x, mLeftWidth + mRightWidth + 10.0f);
		ImGui::SetWindowSize(ImVec2(minWinWidth, winSize.y));
		ImGui::Columns(3);
		// 애니메이션 뷰
		AnimationScene_OnGUI();
		ImGui::NextColumn();
		AnimationNodeEditor_OnGUI();
		ImGui::NextColumn();
		AnimationInspector_OnGUI();
		ImGui::Columns(1);
	}


	void AnimationView::Destroy()
	{
		mAnimClipBuildDataDic.clear();
		mAnimTransitionBuildDataDic.clear();
		mAnimParamBuildDataList.clear();
		mAddedAnimParamNameSet.clear();

		mAnimParamEditMode = EEditMode::Default;
		mTransitionConditionEditMode = EEditMode::Default;


		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			mModelAssetPath.SetValue(mMeshAsset->GetAssetPath());
		}
		if (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid())
		{
			mSkeletoneAssetPath.SetValue(mSkeletoneAsset->GetAssetPath());
		}

		String materialAssetPathStr;
		for (SharedPtr<Asset<IMaterial>> material : mMaterialAssetList)
		{
			if (material != nullptr && material->IsValid())
			{
				materialAssetPathStr += material->GetAssetPath() + ",";
			}
		}
		mMaterialAssetPath.SetValue(materialAssetPathStr);
		mMaterialAssetList.clear();
		mSkyBox = nullptr;
		mModel = nullptr;

		if (mAnimController != nullptr)
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
			mAnimController = nullptr;
		}
		mEditorUIScene = nullptr;
		mNodeEditor = nullptr;
	}

	void AnimationView::OnEvent(IEvent& e)
	{


	}
	void AnimationView::AnimationScene_OnGUI()
	{
		ImGui::SetColumnWidth(0, mLeftWidth);
		ImGui::BeginChild("AnimSceneView");

		if (mEditorUIScene != nullptr)
		{
			mEditorUIScene->OnGUI();
		}

		//f32 fraction = mAnimController->GetAnimationStateMachine()->Get
		ImGui::ProgressBar(1.0f);



		ImGui::Separator();



		f32 label_Space = ImGui::CalcTextSize("        ").x;

		{
			ImGui::AssetField_OnGUI("Mesh", (mMeshAsset != nullptr && mMeshAsset->IsValid()) ? mMeshAsset->GetAssetName() : "None",
				EAssetFormat::Mesh, [&](const String& assetPath)
			{
				SetMesh(assetPath);
			}, label_Space);
		}

		{
			List<String> inputText;
			for (SharedPtr<Asset<IMaterial>> material : mMaterialAssetList)
			{
				if (material == nullptr)
				{
					inputText.push_back("None");
				}
				else
				{
					inputText.push_back(material->GetAssetName());
				}
			}
			ImGui::AssetField_List_OnGUI("Material", inputText, EAssetFormat::Material,
				[&](int i, const String& assetPath)
			{
				SharedPtr<Asset<IMaterial>> originMaterial = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(assetPath);
				if (originMaterial != nullptr)
				{
					mMaterialAssetList[i] = AssetDataBase::GetInstance().LoadReadWriteAsset<IMaterial>(originMaterial->GetAssetID());
				}
			},
				[&]()
			{
				mMaterialAssetList.push_back(nullptr);
			},
				[&]()
			{
				mMaterialAssetList.pop_back();
			}, label_Space);
		}

		{
			ImGui::AssetField_OnGUI("Skeletal", (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid()) ? mSkeletoneAsset->GetAssetName() : "None",
				EAssetFormat::Skeletal, [&](const String& assetPath)
			{
				SetSkeletal(assetPath);
			}, label_Space);
		}
		ImGui::EndChild();
	
	}
	void AnimationView::AnimationInspector_OnGUI()
	{
		// Param Controller 
		ImGui::SetColumnWidth(2, mRightWidth);
		ImGui::BeginChild("StateNodeInspector", ImVec2(mRightWidth, 350.0f));


		StateNodeGUI::StateNodeID selectedNodeID = mNodeEditor->GetSelectedNodeID();
		StateNodeGUI::StateNode* node = mNodeEditor->FindNode(selectedNodeID);
		if (node != nullptr)
		{
			if (node->GetFlags() & StateNodeGUI::EStateNodeFlags::RootNode)
			{
				ImGui::Text("Root Node");
			}
			else
			{
				mAnimClipBuildDataDic[selectedNodeID].ID = selectedNodeID;
				AnimationClip_OnGUI(mAnimClipBuildDataDic[selectedNodeID]);
			}
			
		}
		else if (mNodeEditor->FindNodeTransition(selectedNodeID) != nullptr)
		{
			Transition_OnGUI(mAnimTransitionBuildDataDic[selectedNodeID]);
		}

		ImGui::EndChild();
		ImGui::BeginChild("ParamEditor", ImVec2(mRightWidth, 0.0f));
		ImGui::Separator();

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		if (ImGui::Selectable("Default##AnimParamEdit", mAnimParamEditMode == EEditMode::Default, ImGuiSelectableFlags_None, ImVec2(70.0f,25.0f)) == true)
		{
			mAnimParamEditMode = EEditMode::Default;
		}	ImGui::SameLine();
		if (ImGui::Selectable("Move##AnimParamEdit", mAnimParamEditMode == EEditMode::Move, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mAnimParamEditMode = EEditMode::Move;
		}	ImGui::SameLine();
		if (ImGui::Selectable("Delete##AnimParamEdit", mAnimParamEditMode == EEditMode::Delete, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mAnimParamEditMode = EEditMode::Delete;
		}
		ImGui::PopStyleVar();
		ImGui::Separator();


		
		ImGui::BeginTable("ParamEditorTable", 3, ImGuiTableFlags_BordersH);


		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, mTableTypeRowWidth);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, mTableNameRowWidth);
		ImGui::TableSetupColumn("##Edit", ImGuiTableColumnFlags_WidthStretch, mEditRowWidth);
		ImGui::TableHeadersRow();

		i32 index = 0;
		i32 moveIndex = -1;
		i32 moveDir = 0;
		List<String> removeParamList;
		for (AnimParamBuildData& data : mAnimParamBuildDataList)
		{
			ImGui::TableNextColumn();
	
			String paramName = data.Name;
			EAnimationParameterType paramType = data.Type;
		
		
			
			ImGui::PushID(index);

		
			ImGui::SetNextItemWidth(mTableTypeRowWidth);
			//-- Type GUI --
			String type = AnimationParameterTypeToString(paramType);

			Color bgColor = GetBgColor(paramType);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(bgColor.R, bgColor.G, bgColor.B, bgColor.A));
			bool openCombo = ImGui::BeginCombo("##TypeComboBox", type.c_str());
			ImGui::PopStyleColor();
			if (openCombo)
			{
				for (i32 iParamType = 1; iParamType < (i32)EAnimationParameterType::Count; ++iParamType)
				{
					String typeName = AnimationParameterTypeToString((EAnimationParameterType)iParamType);
					if (ImGui::Selectable(typeName.c_str()) == true)
					{
						data.Type = (EAnimationParameterType)iParamType;
						u32 dataSize = 0;
						switch (data.Type)
						{
						case EAnimationParameterType::Bool:
							dataSize = 1;
							break;
						case EAnimationParameterType::Int:
						case EAnimationParameterType::Float:
							dataSize = 4;
							break;
						}
						if (dataSize > 0)
						{
							data.Data.resize(dataSize, 0);
						}

						UpdateTransitionBuildData(data.Name, data.Type);
					}
				}
				ImGui::EndCombo();
			}
	
			//--------------


			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(mTableNameRowWidth);

			// -- Param Name GUI -- 

			ImGui::InputText("##InputText", &paramName);
			if (ImGui::IsItemDeactivated())
			{
				mAddedAnimParamNameSet.erase(data.Name);
				mAddedAnimParamNameSet.insert(paramName);
				

				UpdateTransitionBuildData(data.Name, paramName);
				data.Name = paramName;
			}



			ImGui::TableNextColumn();


			switch (mAnimParamEditMode)
			{
				// 값편집
			case EEditMode::Default:
				AnimationParam_OnGUI(data);
				break;
			case EEditMode::Delete:
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("-", ImVec2(50.0f, 0.0)))
				{
					removeParamList.push_back(paramName);
				}
				ImGui::PopStyleColor();
				break;
			case EEditMode::Move:
				if (ImGui::ArrowButton("##UpArrow_AnimParamEdit", ImGuiDir_Up))
				{
					moveIndex = index;
					moveDir = ImGuiDir_Up;
				}ImGui::SameLine();
				if (ImGui::ArrowButton("##DownArrow_AnimParmEdit", ImGuiDir_Down))
				{
					moveIndex = index;
					moveDir = ImGuiDir_Down;
				}
				break;
			}
			
			++index;
			ImGui::PopID();
		}

		ImGui::EndTable();
		auto padding = ImGui::GetStyle().FramePadding;

		if (ImGui::Button("+", ImVec2(mRightWidth - (padding.x * 4), 0.0f)) == true)
		{
			i32 cnt = 0;
			while (true)
			{
				String newName = "NewParam (" + std::to_string(cnt++) + ")";
				if (mAddedAnimParamNameSet.find(newName) == mAddedAnimParamNameSet.end())
				{
					mAddedAnimParamNameSet.insert(newName);
					AnimParamBuildData buildData;
					buildData.Name = newName;
					buildData.Type = EAnimationParameterType::Bool;
					buildData.Data.resize(1, 0);
					mAnimParamBuildDataList.push_back(buildData);
					break;
				}
			}
			
		}

		for (const String& removedParamName : removeParamList)
		{
			mAddedAnimParamNameSet.erase(removedParamName);
			mAnimParamBuildDataList.erase(std::remove_if(mAnimParamBuildDataList.begin(), mAnimParamBuildDataList.end(), [&](const AnimParamBuildData& data)
			{
				return data.Name == removedParamName;
			}));

			RemoveTransitionBuildData(removedParamName);

		}
		if (moveIndex >= 0)
		{
			if (moveDir == ImGuiDir_Up && mAnimParamBuildDataList.size() > moveIndex && moveIndex > 0)
			{
				AnimParamBuildData tempData = mAnimParamBuildDataList[moveIndex - 1];
				mAnimParamBuildDataList[moveIndex - 1] = mAnimParamBuildDataList[moveIndex];
				mAnimParamBuildDataList[moveIndex] = tempData;
			}
			if (moveDir == ImGuiDir_Down && mAnimParamBuildDataList.size() > (moveIndex + 1))
			{
				AnimParamBuildData tempData = mAnimParamBuildDataList[moveIndex + 1];
				mAnimParamBuildDataList[moveIndex + 1] = mAnimParamBuildDataList[moveIndex];
				mAnimParamBuildDataList[moveIndex] = tempData;
			}
		}

		ImGui::EndChild();
	}
	void AnimationView::AnimationNodeEditor_OnGUI()
	{
		ImVec2 winSize = ImGui::GetWindowSize();
		ImGui::SetColumnWidth(1, Math::Max<f32>(winSize.x - (mLeftWidth + mRightWidth), 1));

		// Animation Play && Debugging
		ImGui::BeginChild("Animation Top Menu", ImVec2(0.0f, 30.0f));
		

		if (ImGui::Button("Build", ImVec2(70.0f, 26.0f)) == true)
		{
			SharedPtr<AnimationController> newController = CreateSharedPtr<AnimationController>();
			JGAnimation::GetInstance().RegisterAnimationController(newController);
			if (Build(newController))
			{
				JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
				mAnimController = newController;
			}
			else
			{
				JGAnimation::GetInstance().UnRegisterAnimatioinController(newController);
			}
			
		}; ImGui::SameLine();

		if (mAnimState == EAnimState::Playing) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.1f, 1.0f));
		bool isClickPlayButton = ImGui::Button(mAnimState == EAnimState::Editable ? "Play" : "Pause", ImVec2(70.0f, 26.0f));
		if (mAnimState == EAnimState::Playing) ImGui::PopStyleColor();
		if(isClickPlayButton)
		{
			switch (mAnimState)
			{
			case EAnimState::Editable:
				Play();
				mAnimState = EAnimState::Playing;
				break;
			case EAnimState::Playing:
				Editable();
				mAnimState = EAnimState::Editable;
				break;
			}
		} ImGui::SameLine();
		
		ImGui::Separator();
		ImGui::EndChild();

		mNodeEditor->OnGUI();
	}
	void AnimationView::AnimationParam_OnGUI(AnimParamBuildData& buildData)
	{
		String typeStr = AnimationParameterTypeToString(buildData.Type);
		ImGui::SetNextItemWidth(mEditRowWidth);
		switch (buildData.Type)
		{
		case EAnimationParameterType::Bool:
		{
			ImGui::Checkbox("##CheckBox", (bool*)buildData.Data.data());
		}
		break;
		case EAnimationParameterType::Float:
		{
			f32 value = *((f32*)buildData.Data.data());
			ImGui::InputFloat("##InputFloat", &value);
			if (ImGui::IsItemDeactivated())
			{
				memcpy(buildData.Data.data(), &value, sizeof(f32));
			}
		}

		break;
		case EAnimationParameterType::Int:
		{
			i32 value = *((i32*)buildData.Data.data());
			ImGui::InputInt("##InputInt", &value, 0, 0);
			if (ImGui::IsItemDeactivated())
			{
				memcpy(buildData.Data.data(), &value, sizeof(i32));
			}
		}
		break;
		}
	}
	void AnimationView::Transition_OnGUI(AnimTransitionBuildData& buildData)
	{

		StateNodeGUI::StateNode* fromNode = mNodeEditor->FindNode(buildData.From);
		StateNodeGUI::StateNode* toNode = mNodeEditor->FindNode(buildData.To);

		ImGui::Text("%s -> %s", 
			fromNode == nullptr ? "None" : fromNode->GetName().c_str(),
			toNode == nullptr ? "None" : toNode->GetName().c_str());

		ImGui::Separator();
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		if (ImGui::Selectable("Default##TransitionConditionEditMode", mTransitionConditionEditMode == EEditMode::Default, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mTransitionConditionEditMode = EEditMode::Default;
		}	ImGui::SameLine();
		if (ImGui::Selectable("Move##TransitionConditionEditMode", mTransitionConditionEditMode == EEditMode::Move, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mTransitionConditionEditMode = EEditMode::Move;
		}	ImGui::SameLine();
		if (ImGui::Selectable("Delete##TransitionConditionEditMode", mTransitionConditionEditMode == EEditMode::Delete, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mTransitionConditionEditMode = EEditMode::Delete;
		}
		ImGui::PopStyleVar();
		ImGui::Separator();
		ImGui::BeginTable("ParamEditorTable", 3, ImGuiTableFlags_BordersH);
		ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, mTransitionParamNameRowWidth);
		ImGui::TableSetupColumn("Condition", ImGuiTableColumnFlags_WidthFixed, mTransitionConditionRowWidth);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, mTransitionValueRowWidth);
		ImGui::TableHeadersRow();
		ImVec2 padding = ImGui::GetStyle().FramePadding;
		
		i32 index		= 0;
		i32 removeIndex = -1;
		i32 moveIndex	= -1;
		i32 moveDir		= 0;
		for (AnimTransitionConditionBuildData& condBuildData : buildData.Conditions)
		{
			ImGui::PushID(index);

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(mTransitionParamNameRowWidth);
			//-- Param GUI --

			Color bgColor = GetBgColor(condBuildData.Type);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(bgColor.R, bgColor.G, bgColor.B, bgColor.A));
			bool openCombo = ImGui::BeginCombo("##ParameterComboBox", condBuildData.ParamName.c_str());
			ImGui::PopStyleColor();
			if (openCombo)
			{
				for (AnimParamBuildData& paramBuildData : mAnimParamBuildDataList)
				{
					if (ImGui::Selectable(paramBuildData.Name.c_str()) == true)
					{
						condBuildData.ParamName = paramBuildData.Name;
						condBuildData.Type = paramBuildData.Type;
						condBuildData.Data.resize(paramBuildData.Data.size(), 0);
					}
				}
				ImGui::EndCombo();
			}
	

			//--------------


			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(mTransitionConditionRowWidth);
			// -- Condition GUI -- 
			String conditionStr = AnimationConditionTypeToString(condBuildData.Condition);
			if (ImGui::BeginCombo("##ConditionComboBox", conditionStr.c_str()))
			{
				if (condBuildData.Type == EAnimationParameterType::Bool)
				{
					conditionStr = AnimationConditionTypeToString(EAnimationCondition::Equal);
					if (ImGui::Selectable(conditionStr.c_str()) == true)
					{
						condBuildData.Condition = EAnimationCondition::Equal;
					}
				}
				else
				{
					for (i32 iParamType = 1; iParamType < (i32)EAnimationCondition::Count; ++iParamType)
					{
						conditionStr = AnimationConditionTypeToString((EAnimationCondition)iParamType);
						if (ImGui::Selectable(conditionStr.c_str()) == true)
						{
							condBuildData.Condition = (EAnimationCondition)iParamType;
						}
					}
				}

				ImGui::EndCombo();
			}
			//--------------


			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(mTransitionValueRowWidth);
			switch (mTransitionConditionEditMode)
			{
				// 값편집
			case EEditMode::Default:
				TransitionConditionValue_OnGUI(condBuildData);
				break;
			case EEditMode::Delete:
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("-", ImVec2(50.0f, 0.0)))
				{
					removeIndex = index;
				}
				ImGui::PopStyleColor();
				break;
			case EEditMode::Move:
				if (ImGui::ArrowButton("##UpArrow_TransitionEdit", ImGuiDir_Up))
				{
					moveIndex = index;
					moveDir = ImGuiDir_Up;
				} ImGui::SameLine();
				if (ImGui::ArrowButton("##DownArrow_TransitionEdit", ImGuiDir_Down))
				{
					moveIndex = index;
					moveDir = ImGuiDir_Down;
				}
				break;
			}


			index += 1;
			ImGui::PopID();
		}

		ImGui::EndTable();
		if (ImGui::Button("+", ImVec2(mRightWidth - (padding.x * 4), 0.0f)) == true)
		{
			AnimTransitionConditionBuildData newData;
			newData.ParamName = "None";
			buildData.Conditions.push_back(newData);
		}
		if (removeIndex >= 0)
		{
			buildData.Conditions.erase(buildData.Conditions.begin() + removeIndex);
		}
		if (moveIndex >= 0)
		{
			if (moveDir == ImGuiDir_Up && buildData.Conditions.size() > moveIndex && moveIndex > 0)
			{
				AnimTransitionConditionBuildData tempData = buildData.Conditions[moveIndex - 1];
				buildData.Conditions[moveIndex - 1] = buildData.Conditions[moveIndex];
				buildData.Conditions[moveIndex] = tempData;
			}
			if (moveDir == ImGuiDir_Down && buildData.Conditions.size() > (moveIndex + 1))
			{
				AnimTransitionConditionBuildData tempData = buildData.Conditions[moveIndex + 1];
				buildData.Conditions[moveIndex + 1] = buildData.Conditions[moveIndex];
				buildData.Conditions[moveIndex] = tempData;
			}
		}

	}
	void AnimationView::TransitionConditionValue_OnGUI(AnimTransitionConditionBuildData& buildData)
	{
		String typeStr = AnimationParameterTypeToString(buildData.Type);
		switch (buildData.Type)
		{
		case EAnimationParameterType::Bool:
		{
			ImGui::Checkbox("##CheckBox", (bool*)buildData.Data.data());
		}
		break;
		case EAnimationParameterType::Float:
		{
			f32 value = *((f32*)buildData.Data.data());
			ImGui::InputFloat("##InputFloat", &value);
			if (ImGui::IsItemDeactivated())
			{
				memcpy(buildData.Data.data(), &value, sizeof(f32));
			}
		}

		break;
		case EAnimationParameterType::Int:
		{
			i32 value = *((i32*)buildData.Data.data());
			ImGui::InputInt("##InputInt", &value, 0, 0);
			if (ImGui::IsItemDeactivated())
			{
				memcpy(buildData.Data.data(), &value, sizeof(i32));
			}
		}
		break;
		}
	}
	void AnimationView::AnimationClip_OnGUI(AnimClipBuildData& buildData)
	{
		ImGui::PushID(buildData.ID);
		ImGui::Text("Name : NodeName");
		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Repeat "); ImGui::SameLine();

		bool _bool = buildData.Flags & EAnimationClipFlags::Repeat;
		if (ImGui::Checkbox("##CheckBox_Repeat", &_bool) == true)
		{
			if (_bool == false) buildData.Flags = (EAnimationClipFlags)((i32)buildData.Flags & (~(i32)EAnimationClipFlags::Repeat));
			else buildData.Flags = buildData.Flags | EAnimationClipFlags::Repeat;
		}


		f32 label_Space = ImGui::CalcTextSize("         ").x;

		ImGui::AssetField_OnGUI("AnimationClip", (buildData.Asset != nullptr && buildData.Asset->IsValid()) ? buildData.Asset->GetAssetName() : "None",
			EAssetFormat::AnimationClip, [&](const String& assetPath)
		{
			buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationClip>(assetPath);
		}, label_Space);

		ImGui::PopID();
	}
	void AnimationView::CreateRootNode()
	{
		StateNodeGUI::StateNodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(250, 100));
		nodeBuilder.SetName("Root");
		nodeBuilder.SetNodeFlags(
			StateNodeGUI::EStateNodeFlags::No_Remove | 
			StateNodeGUI::EStateNodeFlags::No_ReName | 
			StateNodeGUI::EStateNodeFlags::RootNode);
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::CreateAnimationClipNode()
	{
		JVector2 offset = mNodeEditor->GetOffset();
		ImVec2 mousePos = ImGui::GetMousePos();
		JVector2 winPos = mNodeEditor->GetWindowPos();
		StateNodeGUI::StateNodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y) - offset);
		nodeBuilder.SetName("AnimationClip");
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::UpdateScene()
	{
		static const JVector3 EyePos = JVector3(0, 100.0f, -200.0f);
		if (mEditorUIScene == nullptr)
		{
			EditorUISceneConfig config;
			config.EyePos = EyePos;
			config.Resolution = mSceneResolution;
			config.ImageSize = config.Resolution;
			config.OffsetScale = JVector3(1, 1, 1);
			config.Flags   = EEditorUISceneFlags::Fix_RotatePitch;
			config.SkyBox  = GraphicsHelper::CreateSkyBox(config.EyePos, config.FarZ, "Asset/Engine/CubeMap/DefaultSky.jgasset");
			mEditorUIScene = CreateUniquePtr<EditorUIScene>(config);
		}

		if (mModel == nullptr)
		{
			mModel = CreateSharedPtr<Graphics::StaticRenderObject>();
			mModel->Flags = Graphics::ESceneObjectFlags::Always_Update_Bottom_Level_AS;
		}

		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			JBBox boundingBox = mMeshAsset->Get()->GetBoundingBox();

			JVector3 Center = boundingBox.Center();
			mEditorUIScene->SetLocation(Center * -1);

			JVector3 targetVec = JVector3::Normalize(EyePos * -1);
			targetVec.y = Math::Clamp(targetVec.y - 0.15f, 0.0f, targetVec.y);
			mEditorUIScene->SetTargetVector(targetVec);

			if (mAnimController != nullptr)
			{
				mAnimController->BindMesh(mMeshAsset->Get());
				mModel->Mesh = mAnimController->GetBindedMesh();
			}
			else mModel->Mesh = mMeshAsset->Get();
		}
		mModel->MaterialList.clear();
		for (SharedPtr<Asset<IMaterial>> material : mMaterialAssetList)
		{
			if (material == nullptr || material->IsValid() == false)
			{
				continue;
			}
			mModel->MaterialList.push_back(material->Get());
		}
		mEditorUIScene->SetModel(mModel);
	}

	void AnimationView::SetMesh(const String& meshAssetPath)
	{
		mMeshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(meshAssetPath);
	}
	void AnimationView::SetSkeletal(const String& skeletalAssetPath)
	{
		mSkeletoneAsset = AssetDataBase::GetInstance().LoadOriginAsset<Skeletone>(skeletalAssetPath);
	}
	void AnimationView::SetMaterial(const List<String>& materialAssetPath)
	{
		mMaterialAssetList.clear();
		for (const String& path : materialAssetPath)
		{
			SharedPtr<Asset<IMaterial>> originMaterial = AssetDataBase::GetInstance().LoadOriginAsset<IMaterial>(path);
			if (originMaterial == nullptr)
			{
				continue;
			}

			mMaterialAssetList.push_back(AssetDataBase::GetInstance().LoadReadWriteAsset<IMaterial>(originMaterial->GetAssetID()));

		}
	}
	void AnimationView::UpdateTransitionBuildData(const String& paramName, EAnimationParameterType type)
	{
		for (auto& _pair : mAnimTransitionBuildDataDic)
		{
			for (AnimTransitionConditionBuildData& condBuildData : _pair.second.Conditions)
			{
				if (condBuildData.ParamName == paramName)
				{
					condBuildData.Type = type;
				}
			}
		}
	}
	void AnimationView::UpdateTransitionBuildData(const String& oldName, const String& newName)
	{
		for (auto& _pair : mAnimTransitionBuildDataDic)
		{
			for (AnimTransitionConditionBuildData& condBuildData : _pair.second.Conditions)
			{
				if (condBuildData.ParamName == oldName)
				{
					condBuildData.ParamName = newName;
				}
			}
		}
	}
	void AnimationView::RemoveTransitionBuildData(const String& removedName)
	{
		for (auto& _pair : mAnimTransitionBuildDataDic)
		{
			_pair.second.Conditions.erase(std::remove_if(_pair.second.Conditions.begin(), _pair.second.Conditions.end(),
				[&](const AnimTransitionConditionBuildData& data)
			{
				return data.ParamName == removedName;
			}));
		}
	}

	bool AnimationView::Build(SharedPtr<AnimationController> newController)
	{
		if (mSkeletoneAsset == nullptr || mSkeletoneAsset->IsValid() == false)
		{
			JG_LOG_ERROR("Skeleton Asset is Null");
			return false;
		}
		newController->BindSkeletone(mSkeletoneAsset->Get());

		if (mMeshAsset == nullptr || mMeshAsset->IsValid() == false)
		{
			JG_LOG_ERROR("Mesh Asset is Null");
			return false;
		}
		newController->BindMesh(mMeshAsset->Get());

		if (mMaterialAssetList.empty() == true)
		{
			JG_LOG_WARN("Material Asset is Empty");
		}
		

		// Animation Parameter 설정
		SharedPtr<AnimationParameters> animParam = newController->GetAnimationParameters();
		for (const AnimParamBuildData& buildData : mAnimParamBuildDataList)
		{
			bool isSuccess = false;
			switch (buildData.Type)
			{
			case EAnimationParameterType::Bool:
				isSuccess = animParam->SetBool(buildData.Name, *((bool*)buildData.Data.data()));
				break;
			case EAnimationParameterType::Float:
				isSuccess = animParam->SetFloat(buildData.Name, *((f32*)buildData.Data.data()));
				break;
			case EAnimationParameterType::Int:
				isSuccess = animParam->SetInt(buildData.Name, *((i32*)buildData.Data.data()));
				break;
			}
			if (isSuccess == false)
			{
				JG_LOG_ERROR("Fail Build Animation Parmeters : {0}", buildData.Name);
				return false;
			}
		}

		

		for (auto _pair : mAnimClipBuildDataDic)
		{
			SharedPtr<Asset<AnimationClip>> clipAsset = _pair.second.Asset;
			if (clipAsset == nullptr || clipAsset->IsValid() == false)
			{
				JG_LOG_ERROR("AnimationClipNode's Asset is Null -> Node ID :  {0}", _pair.first);
				return false;
			}
			StateNodeGUI::StateNode* node = mNodeEditor->FindNode(_pair.first);
			if (node == nullptr)
			{
				JG_LOG_ERROR("Can't Find StateNode In NodeEditor: {0}", _pair.first);
				return false;
			}
			newController->AddAnimationClip(node->GetName(), clipAsset->Get(), _pair.second.Flags, true);
		}
		

		SharedPtr<AnimationStateMachine> stateMachine = newController->GetAnimationStateMachine();
		StateNodeGUI::StateNodeLinkInfo nodeLinkInfo  = mNodeEditor->GetNodeLinkInfo();

		StateNodeGUI::StateNode* rootNode = mNodeEditor->FindNode(nodeLinkInfo.RootNodeID);
		if (rootNode == nullptr)
		{
			JG_LOG_ERROR("Can't Find RootNode");
			return false;
		}
		stateMachine->Begin(rootNode->GetName());

		// Add Clip Node
		for (StateNodeGUI::StateNodeID nodeID : nodeLinkInfo.NodeIDList)
		{
			if (nodeLinkInfo.RootNodeID == nodeID) continue;

			StateNodeGUI::StateNode* clipNode = mNodeEditor->FindNode(nodeID);
			if (clipNode == nullptr)
			{
				JG_LOG_ERROR("Can't Find Node {0}", nodeID);
				return false;
			}
			
			bool isSuccessMakeAnimClipNode = false;
			stateMachine->MakeAnimationClipNode(clipNode->GetName(),[&](AnimationClipInfo* clipInfo)
			{
				isSuccessMakeAnimClipNode = true;
			});
			if (isSuccessMakeAnimClipNode == false)
			{
				JG_LOG_ERROR("Fail Make Animation Clip Node {0}", clipNode->GetName());
				return false;
			}
		}

		// Create Transition
		u32 nodeCnt = nodeLinkInfo.NodeIDList.size();
		for (u32 i = 0; i < nodeCnt; ++i)
		{
			StateNodeGUI::StateNodeID nodeID = nodeLinkInfo.NodeIDList[i];
			List<StateNodeGUI::StateNodeID> linkedNodeIDList = nodeLinkInfo.LinkedNodeIDLists[i];
			StateNodeGUI::StateNode* prevNode = mNodeEditor->FindNode(nodeID);


			for (StateNodeGUI::StateNodeID linkedNodeID : linkedNodeIDList)
			{
				StateNodeGUI::StateNode* nextNode = mNodeEditor->FindNode(linkedNodeID);
				if (nextNode == nullptr)
				{
					JG_LOG_ERROR("Can't Find Node {0}", nodeID);
					return false;
				}
				bool isSuccessConnectNode = false;
				stateMachine->ConnectNode(prevNode->GetName(), nextNode->GetName(),
					[&](AnimationTransition* transition)
				{
					StateNodeGUI::StateNodeID transitionNodeID = prevNode->GetTransition(nextNode->GetID());
					if (transitionNodeID == 0 || mAnimTransitionBuildDataDic.find(transitionNodeID) == mAnimTransitionBuildDataDic.end())
					{
						return;
					}

					const AnimTransitionBuildData& tranBuildData = mAnimTransitionBuildDataDic[transitionNodeID];
					for (const AnimTransitionConditionBuildData& condBuildData : tranBuildData.Conditions)
					{
						switch (condBuildData.Type)
						{
						case EAnimationParameterType::Bool:
							transition->AddCondition_Bool(condBuildData.ParamName, *((bool*)condBuildData.Data.data()));
							break;
						case EAnimationParameterType::Float:
							transition->AddCondition_Float(condBuildData.ParamName, *((f32*)condBuildData.Data.data()), condBuildData.Condition);
							break;
						case EAnimationParameterType::Int:
							transition->AddCondition_Int(condBuildData.ParamName, *((i32*)condBuildData.Data.data()), condBuildData.Condition);
							break;
						default:
							JG_LOG_ERROR("Transition's Parameter Type is Unknown");
							return;
						}
					}

					isSuccessConnectNode = true;
				});
				if (isSuccessConnectNode == false)
				{
					JG_LOG_ERROR("Fail ConnectNode  {0}  ->  {1}", prevNode->GetName(), nextNode->GetName());
					return false;
				}
			}

		}


		stateMachine->End();
		return true;
	}
	void AnimationView::Play()
	{
		// 처음부터 플레이
		// NodeEditor 락
		// 
	}
	void AnimationView::Editable()
	{
		// NodeEditor unlock
	}
}