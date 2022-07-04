#include "pch.h"
#include "AnimationView.h"
#include "Application.h"
#include "Animation/JGAnimation.h"
#include "Animation/AnimationClip.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationParameters.h"
#include "Animation/AnimationTransition.h"
#include "Animation/AnimationStateMachine.h"
#include "Animation/AnimationBlendSpace.h"
#include "Animation/AnimationBlendSpace1D.h"
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
#pragma region Define Build Data
	enum class ENodeType
	{
		Root,
		AnimationClip,
		AnimationBlendSpace1D,
		AnimationBlendSpace,
		Begin,
		Exit,
	};

	struct AnimParamBuildData
	{
		String Name;
		EAnimationParameterType Type;
		List<jbyte> Data;
	};

	struct AnimTransitionConditionBuildData
	{
		String ParamName;
		EAnimationParameterType Type;
		EAnimationCondition Condition;
		List<jbyte> Data;
	};
	struct AnimTransitionBuildData
	{
		StateNodeGUI::StateNodeID From;
		StateNodeGUI::StateNodeID To;
		f32 TransitionDuration = 0.0f;

		bool HasExitTime = false;
		f32  ExitTime = 0.0f;
		List<AnimTransitionConditionBuildData> Conditions;
	};
	struct AnimClipBuildData
	{
		StateNodeGUI::StateNodeID ID;
		SharedPtr<Asset<AnimationClip>> Asset;

		EAnimationClipFlags Flags = EAnimationClipFlags::None;
		f32 Speed = 1.0f;
	};
	struct AnimBlendSpace1DBuildData
	{
		StateNodeGUI::StateNodeID ID;
		EAnimationBlendSpace1DFlag Flags = EAnimationBlendSpace1DFlag::Repeat;
		SharedPtr<Asset<AnimationBlendSpace1D>> Asset;
	};
	struct AnimBlendSpaceBuildData
	{
		StateNodeGUI::StateNodeID ID;
		EAnimationBlendSpaceFlag Flags = EAnimationBlendSpaceFlag::Repeat;
		SharedPtr<Asset<AnimationBlendSpace>> Asset;
	};
#pragma endregion Define Build Data

	// 레이어 기반
	struct AnimationLayerBuildData
	{
		String Name;
		UniquePtr<StateNodeGUI::StateNodeEditor> NodeEditor;
		Dictionary<StateNodeGUI::StateNodeID, AnimTransitionBuildData> AnimTransitionBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimClipBuildData>       AnimClipBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpace1DBuildData>    AnimBlendSpace1DBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, AnimBlendSpaceBuildData>      AnimBlendSpaceBuildDataDic;
		Dictionary<StateNodeGUI::StateNodeID, JVector2> NodeLocationDic;
		Dictionary<String, StateNodeGUI::StateNodeID>   NodeNameDic;


		void InitNodeEditor()
		{
			NodeEditor = CreateUniquePtr<StateNodeGUI::StateNodeEditor>();
			NodeEditor->BindContextMenuFunc([&]()
			{
				UIManager::GetInstance().ShowContextMenu(JGTYPE(AnimationView), false);
			});

			NodeEditor->BindCreateNodeCallBack([&](StateNodeGUI::StateNodeID id)
			{
				StateNodeGUI::StateNode* node = NodeEditor->FindNode(id);
				NodeLocationDic[id] = NodeEditor->FindNode(id)->GetLocation();
				NodeNameDic[node->GetName()] = id;


				switch (node->GetUserData<ENodeType>())
				{
				case ENodeType::AnimationClip:
					AnimClipBuildDataDic[id] = AnimClipBuildData();
					break;
				case ENodeType::AnimationBlendSpace1D:
					AnimBlendSpace1DBuildDataDic[id] = AnimBlendSpace1DBuildData();
					break;
				case ENodeType::AnimationBlendSpace:
					AnimBlendSpaceBuildDataDic[id] = AnimBlendSpaceBuildData();
					break;
				}
			});
			NodeEditor->BindReNameCallBack([&](StateNodeGUI::StateNodeID id, const String& oldName, const String& newName)
			{
				NodeNameDic.erase(oldName);
				NodeNameDic[newName] = id;
			});
			NodeEditor->BindDraggingNodeCallBack([&](StateNodeGUI::StateNodeID id)
			{
				NodeLocationDic[id] = NodeEditor->FindNode(id)->GetLocation();
			});
			NodeEditor->BindLinkNodeCallBack(
				[&](StateNodeGUI::StateNodeID fromID, StateNodeGUI::StateNodeID toID, StateNodeGUI::StateNodeID transID)
			{
				AnimTransitionBuildData& buildData = AnimTransitionBuildDataDic[transID];
				buildData.From = fromID;
				buildData.To = toID;
			});
			NodeEditor->BindRemoveNodeCallBack([&](StateNodeGUI::StateNodeID id)
			{
				StateNodeGUI::StateNode* node = NodeEditor->FindNode(id);
				if (node != nullptr)
				{
					NodeNameDic.erase(node->GetName());
				}
				switch (node->GetUserData<ENodeType>())
				{
				case ENodeType::AnimationClip:
					AnimClipBuildDataDic.erase(id);
					break;
				case ENodeType::AnimationBlendSpace1D:
					AnimBlendSpace1DBuildDataDic.erase(id);
					break;
				case ENodeType::AnimationBlendSpace:
					AnimBlendSpaceBuildDataDic.erase(id);
					break;
				}


				NodeLocationDic.erase(id);
			});
			NodeEditor->BindRemoveTransitionCallBack([&](StateNodeGUI::StateNodeID id)
			{
				AnimTransitionBuildDataDic.erase(id);
			});
		}
		void InitBuildData(const String& name, bool is_base_layer = false)
		{
			Name = name;
			NodeLocationDic.clear();
			CreateRootNode();
			if (is_base_layer == false)
			{
				CreateExitNode();
			}
		}
		void InitBuildData(
			SharedPtr<JsonData> nodeLocationJson, 
			const AnimationAssetStock::AnimationStateMachineInfo& animStateMachineInfo,
			const Dictionary<String, EAnimationParameterType>& paramTypeDic)
		{
			Name = animStateMachineInfo.Name;
			// Node 생성
			NodeLocationDic.clear();
			Dictionary<String, StateNodeGUI::StateNodeID> nodeIDDic; // 
			if (nodeLocationJson != nullptr && nodeLocationJson->IsArray())
			{
				u32 nodeCnt = nodeLocationJson->GetSize();
				for (u32 i = 0; i < nodeCnt; ++i)
				{
					SharedPtr<JsonData> dataJson = nodeLocationJson->GetJsonDataFromIndex(i);

					String   name = dataJson->GetMember("Name")->GetString();
					JVector2 location = dataJson->GetMember("Location")->GetVector2();
					ENodeType type = (ENodeType)dataJson->GetMember("NodeType")->GetUint32();

					nodeIDDic[name] = CreateNode(type, name, location);
				}
			}
			if (nodeIDDic.find(animStateMachineInfo.RootName) == nodeIDDic.end())
			{
				CreateRootNode();
			}
			if (Name != AnimationController::BASE_LAYER_NAME)
			{
				if (nodeIDDic.find(animStateMachineInfo.ExitName) == nodeIDDic.end())
				{
					CreateExitNode();
				}
			}
			for (const AnimationAssetStock::AnimationClipInfo& clipInfo : animStateMachineInfo.AnimClips)
			{
				if (nodeIDDic.find(clipInfo.Name) == nodeIDDic.end())
				{
					nodeIDDic[clipInfo.Name] = CreateNode(ENodeType::AnimationClip, clipInfo.Name, JVector2(100.0, 100.0f));
				}


				AnimClipBuildData buildData;
				buildData.ID = nodeIDDic[clipInfo.Name];
				buildData.Flags = clipInfo.Flags;
				buildData.Speed = clipInfo.Speed;
				buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationClip>(clipInfo.AssetPath);
				AnimClipBuildDataDic[nodeIDDic[clipInfo.Name]] = buildData;
			}
			for (const AnimationAssetStock::AnimationBlendSpace1DInfo& blendSpaceInfo : animStateMachineInfo.AnimBlendSpace1Ds)
			{
				if (nodeIDDic.find(blendSpaceInfo.Name) == nodeIDDic.end())
				{
					nodeIDDic[blendSpaceInfo.Name] = CreateNode(ENodeType::AnimationBlendSpace1D, blendSpaceInfo.Name, JVector2(100.0, 100.0f));
				}
				AnimBlendSpace1DBuildData buildData;
				buildData.ID = nodeIDDic[blendSpaceInfo.Name];
				buildData.Flags = blendSpaceInfo.Flags;
				buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationBlendSpace1D>(blendSpaceInfo.AssetPath);
				AnimBlendSpace1DBuildDataDic[nodeIDDic[blendSpaceInfo.Name]] = buildData;
			}
			for (const AnimationAssetStock::AnimationBlendSpaceInfo& blendSpaceInfo : animStateMachineInfo.AnimBlendSpaces)
			{
				if (nodeIDDic.find(blendSpaceInfo.Name) == nodeIDDic.end())
				{
					nodeIDDic[blendSpaceInfo.Name] = CreateNode(ENodeType::AnimationBlendSpace, blendSpaceInfo.Name, JVector2(100.0, 100.0f));
				}

				AnimBlendSpaceBuildData buildData;
				buildData.ID = nodeIDDic[blendSpaceInfo.Name];
				buildData.Flags = blendSpaceInfo.Flags;
				buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationBlendSpace>(blendSpaceInfo.AssetPath);
				AnimBlendSpaceBuildDataDic[nodeIDDic[blendSpaceInfo.Name]] = buildData;
			}
			// Node 연결
			for (const AnimationAssetStock::AnimationTransitionInfo& linkInfo : animStateMachineInfo.TransitionInfos)
			{
				StateNodeGUI::StateNodeID fromID = nodeIDDic[linkInfo.PrevName];
				StateNodeGUI::StateNodeID toID = nodeIDDic[linkInfo.NextName];

				NodeEditor->Link(fromID, toID);

				StateNodeGUI::StateNode* fromNode = NodeEditor->FindNode(fromID);
				StateNodeGUI::StateNodeID transID = fromNode->GetTransition(toID);
				if (transID == 0) continue;

				AnimTransitionBuildData& buildData = AnimTransitionBuildDataDic[transID];
				buildData.TransitionDuration = linkInfo.TransitionDuration;
				buildData.HasExitTime = linkInfo.HasExitTime;
				buildData.ExitTime = linkInfo.ExitTime;
				for (const AnimationAssetStock::AnimationTransitionConditionInfo& transInfo : linkInfo.Transitions)
				{
					AnimTransitionConditionBuildData condBuildData;
					condBuildData.ParamName = transInfo.ParameterName;
					condBuildData.Data = transInfo.Data;
					condBuildData.Condition = transInfo.Condition;
					condBuildData.Type = paramTypeDic.at(condBuildData.ParamName);

					buildData.Conditions.push_back(condBuildData);
				}
			}
		}

		StateNodeGUI::StateNodeID CreateNode(ENodeType nodeType, const String& name, const JVector2& initPos = JVector2(JG_F32_MAX, JG_F32_MAX))
		{
			switch (nodeType)
			{
			case ENodeType::Root:
				return CreateRootNode();
			case ENodeType::AnimationClip:
				return CreateAnimationClipNode(name, initPos);
			case ENodeType::AnimationBlendSpace1D:
				return CreateAnimationBlendSpace1DNode(name, initPos);
			case ENodeType::AnimationBlendSpace:
				return CreateAnimationBlendSpaceNode(name, initPos);
			default:
				return 0;
			}
		}
		StateNodeGUI::StateNodeID CreateRootNode()
		{
			if (NodeNameDic.find("Root") != NodeNameDic.end())
			{
				return NodeNameDic["Root"];
			}
			StateNodeGUI::StateNodeBuilder nodeBuilder;
			nodeBuilder.SetInitLocation(JVector2(250, 100));
			nodeBuilder.SetName("Root");
			nodeBuilder.SetNodeFlags(
				StateNodeGUI::EStateNodeFlags::No_Remove |
				StateNodeGUI::EStateNodeFlags::No_ReName |
				StateNodeGUI::EStateNodeFlags::RootNode);
			nodeBuilder.SetUserData<ENodeType>(ENodeType::Root);
			return NodeEditor->CreateNode(nodeBuilder);
		}
		StateNodeGUI::StateNodeID CreateExitNode()
		{
			if (NodeNameDic.find("Exit") != NodeNameDic.end())
			{
				return NodeNameDic["Exit"];
			}

			StateNodeGUI::StateNodeBuilder nodeBuilder;
			nodeBuilder.SetInitLocation(JVector2(250, 300));
			nodeBuilder.SetName("Exit");
			nodeBuilder.SetNodeFlags(
				StateNodeGUI::EStateNodeFlags::No_Remove |
				StateNodeGUI::EStateNodeFlags::No_ReName |
				StateNodeGUI::EStateNodeFlags::ExitNode);
			nodeBuilder.SetUserData<ENodeType>(ENodeType::Exit);
			return NodeEditor->CreateNode(nodeBuilder);
		}
		StateNodeGUI::StateNodeID CreateAnimationClipNode(const String& name, const JVector2& initPos)
		{

			StateNodeGUI::StateNodeBuilder nodeBuilder;
			JVector2 offset = NodeEditor->GetOffset();

			if (initPos.x == JG_F32_MAX || initPos.y == JG_F32_MAX)
			{
				ImVec2 mousePos = ImGui::GetMousePos();
				JVector2 winPos = NodeEditor->GetWindowPos();
				nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y) - offset);
			}
			else
			{
				nodeBuilder.SetInitLocation(initPos - offset);
			}

			nodeBuilder.SetName(name);
			nodeBuilder.SetUserData<ENodeType>(ENodeType::AnimationClip);
			return NodeEditor->CreateNode(nodeBuilder);
		}
		StateNodeGUI::StateNodeID CreateAnimationBlendSpace1DNode(const String& name, const JVector2& initPos)
		{
			StateNodeGUI::StateNodeBuilder nodeBuilder;
			JVector2 offset = NodeEditor->GetOffset();

			if (initPos.x == JG_F32_MAX || initPos.y == JG_F32_MAX)
			{
				ImVec2 mousePos = ImGui::GetMousePos();
				JVector2 winPos = NodeEditor->GetWindowPos();
				nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y) - offset);
			}
			else
			{
				nodeBuilder.SetInitLocation(initPos - offset);
			}
			nodeBuilder.SetNodeColor(Color(0.1F, 1.0F, 0.1F, 0.2F));
			nodeBuilder.SetUserData<ENodeType>(ENodeType::AnimationBlendSpace1D);
			nodeBuilder.SetName(name);
			return NodeEditor->CreateNode(nodeBuilder);
		}
		StateNodeGUI::StateNodeID CreateAnimationBlendSpaceNode(const String& name, const JVector2& initPos)
		{
			StateNodeGUI::StateNodeBuilder nodeBuilder;
			JVector2 offset = NodeEditor->GetOffset();

			if (initPos.x == JG_F32_MAX || initPos.y == JG_F32_MAX)
			{
				ImVec2 mousePos = ImGui::GetMousePos();
				JVector2 winPos = NodeEditor->GetWindowPos();
				nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y) - offset);
			}
			else
			{
				nodeBuilder.SetInitLocation(initPos - offset);
			}

			nodeBuilder.SetNodeColor(Color(1.0F, 0.1F, 0.1F, 0.2F));
			nodeBuilder.SetUserData<ENodeType>(ENodeType::AnimationBlendSpace);
			nodeBuilder.SetName(name);
			return NodeEditor->CreateNode(nodeBuilder);
		}
	};
	AnimationView::AnimationView()
	{
		DisableUniqueView();

	}
	void AnimationView::SetAnimation(const String& animationAssetPath)
	{
		String assetPath;
		AssetHelper::GetResourcePath(animationAssetPath, nullptr, &assetPath);
		mModelAssetPath     = StorableString("AnimationView/ModelAssetPath/" + assetPath, "");
		mSkeletoneAssetPath = StorableString("AnimationView/SkeletoneAssetPath/" + assetPath, "");
		mMaterialAssetPath  = StorableString("AnimationView/MaterialAssetPath/" + assetPath, "");
		mNodeSaveData		= StorableString("AnimationView/NodeSaveData/" + assetPath, "");
		RefreshAnimAsset(animationAssetPath);
	}
	void AnimationView::Load()
	{
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/AnimationClip", 0, [&]()
		{
			if (GetCurrentLayerBuildData() == nullptr)
			{
				return;
			}
			CreateNode(GetCurrentLayerBuildData()->Name, ENodeType::AnimationClip, "AnimationClip");
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/AnimationBlendSpace1D", 0, [&]()
		{
			if (GetCurrentLayerBuildData() == nullptr)
			{
				return;
			}
			CreateNode(GetCurrentLayerBuildData()->Name, ENodeType::AnimationBlendSpace1D, "AnimationBlendSpace1D");
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/AnimationBlendSpace", 0, [&]()
		{
			if (GetCurrentLayerBuildData() == nullptr)
			{
				return;
			}
			CreateNode(GetCurrentLayerBuildData()->Name, ENodeType::AnimationBlendSpace, "AnimationBlendSpace");
		}, nullptr);

		mBgColorByParamTypeDic[EAnimationParameterType::Bool]  = Color(0.8f, 0.1f, 0.1f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Float] = Color(0.4f, 0.8f, 0.05f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Int]  = Color(0.05f,0.4f,0.8f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Trigger] = Color(0.8f, 0.1f, 0.8f, 1.0f);
		mBgColorByParamTypeDic[EAnimationParameterType::Unknown] = Color(0.5f, 0.5f, 0.5f, 1.0f);
	}

	void AnimationView::Initialize()
	{
		if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
		{
			AnimationAssetStock stock;
			AssetHelper::ReadAsset(EAssetFormat::Animation, mAnimationAsset->GetAssetPath(), [&](SharedPtr<JsonData> jsonData)
			{
				stock.LoadJson(jsonData);
			});

			mAddedAnimParamNameSet.clear();
			mAnimParamBuildDataList.clear();
			Dictionary<String, EAnimationParameterType> paramTypeDic;
			for (const AnimationAssetStock::ParameterData& paramData : stock.Parameters)
			{
				AnimParamBuildData buildData;
				buildData.Data = paramData.Data;
				buildData.Type = paramData.Type;
				buildData.Name = paramData.Name;
				mAnimParamBuildDataList.push_back(buildData);
				mAddedAnimParamNameSet.insert(buildData.Name);
				paramTypeDic[buildData.Name] = buildData.Type;
			}

			SharedPtr<Json>		json = Json::ToObject(mNodeSaveData.GetValue());
			SharedPtr<JsonData> jsonMain = json->GetMember("Main");
			for (const AnimationAssetStock::AnimationStateMachineInfo& animStateMachineInfo : stock.AnimStateMachineInfos)
			{
				if (mAddedAnimLayerNameSet.find(animStateMachineInfo.Name) != mAddedAnimLayerNameSet.end())
				{
					continue;
				}
				SharedPtr<JsonData> nodeLocationJson = jsonMain->GetMember(animStateMachineInfo.Name);
				SharedPtr<AnimationLayerBuildData> buildData = CreateSharedPtr<AnimationLayerBuildData>();
				buildData->InitNodeEditor();
				buildData->InitBuildData(nodeLocationJson, animStateMachineInfo, paramTypeDic);

				mAddedAnimLayerNameSet.insert(animStateMachineInfo.Name);
				mAnimLayerBuildDataList.push_back(buildData);
			}


			if (mAnimParamBuildDataList.empty() == true)
			{
				SharedPtr<AnimationLayerBuildData> baseLayer = CreateSharedPtr<AnimationLayerBuildData>();
				baseLayer->Name = AnimationController::BASE_LAYER_NAME;
				baseLayer->InitNodeEditor();
				baseLayer->InitBuildData(nullptr, AnimationAssetStock::AnimationStateMachineInfo(), paramTypeDic);


				mCurrentAnimLayerBuildData = baseLayer->Name;
				mAnimLayerBuildDataList.push_back(baseLayer);
			}
			else
			{
				mCurrentAnimLayerBuildData = AnimationController::BASE_LAYER_NAME;
			}
		}
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
		ImGui::SetColumnWidth(2, mRightWidth);
		AnimationInspector_OnGUI();
		AnimationDataEditor_OnGUI();
		ImGui::Columns(1);
	}


	void AnimationView::Destroy()
	{
		SaveNodeLocation();
		mAddedAnimLayerNameSet.clear();
		mAnimLayerBuildDataList.clear();
		mAnimParamBuildDataList.clear();
		mAddedAnimParamNameSet.clear();
		mDataEditMode				 = EDataEditMode::Default;
		mTransitionConditionEditMode = EDataEditMode::Default;
		mAnimState = EAnimState::Editable;
		mEditMode  = EEditMode::AnimationParameters;
		mRenamingLayerBuildData = nullptr;

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

		if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
		{
			RefreshAnimAsset(mAnimationAsset->GetAssetPath());
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimationAsset->Get());
			mAnimationAsset = nullptr;
		}
		mEditorUIScene = nullptr;

		mDataEditMode   			 = EDataEditMode::Default;
		mTransitionConditionEditMode = EDataEditMode::Default;
		mAnimState					 = EAnimState::Editable;
	}

	void AnimationView::AnimationScene_OnGUI()
	{
		ImGui::SetColumnWidth(0, mLeftWidth);
		ImGui::BeginChild("AnimSceneView");

		if (mEditorUIScene != nullptr)
		{
			mEditorUIScene->OnGUI();
		}

		ImGui::Separator();



		f32 label_Space = ImGui::CalcTextSize("        ").x;

		{
			ImGui::AssetField_OnGUI("Mesh", (mMeshAsset != nullptr && mMeshAsset->IsValid()) ? mMeshAsset->GetAssetName() : "None",
				EAssetFormat::SkeletalMesh, [&](const String& assetPath)
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

	void AnimationView::AnimationNodeEditor_OnGUI()
	{
		if (GetCurrentLayerBuildData() == nullptr)
		{
			return;
		}
		ImVec2 winSize = ImGui::GetWindowSize();
		ImGui::SetColumnWidth(1, Math::Max<f32>(winSize.x - (mLeftWidth + mRightWidth), 1));

		// Animation Play && Debugging
		ImGui::BeginChild("Animation Top Menu", ImVec2(0.0f, 55.0f));
		

		if (ImGui::Button("Build", ImVec2(70.0f, 26.0f)) == true)
		{
			if (Build() == false)
			{
				JG_LOG_ERROR("Fail Build Animation Controller");
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


		switch (mAnimState)
		{
		case EAnimState::Playing:
			UpdateFlow();
			break;
		}
		GetCurrentLayerBuildData()->NodeEditor->OnGUI();
	}
	void AnimationView::AnimationInspector_OnGUI()
	{
		SharedPtr<AnimationLayerBuildData> buildData = GetCurrentLayerBuildData();
		if (buildData != nullptr)
		{
			ImGui::BeginChild("StateNodeInspector", ImVec2(mRightWidth, 350.0f));

			StateNodeGUI::StateNodeID selectedNodeID = GetCurrentLayerBuildData()->NodeEditor->GetSelectedNodeID();
			StateNodeGUI::StateNode* node = GetCurrentLayerBuildData()->NodeEditor->FindNode(selectedNodeID);
			if (node != nullptr)
			{
				switch (node->GetUserData<ENodeType>())
				{
				case ENodeType::Root:
					ImGui::Text("Root Node");
					break;
				case ENodeType::AnimationClip:
					buildData->AnimClipBuildDataDic[selectedNodeID].ID = selectedNodeID;
					AnimationClip_OnGUI(buildData->AnimClipBuildDataDic[selectedNodeID]);
					break;
				case ENodeType::AnimationBlendSpace1D:
					buildData->AnimBlendSpace1DBuildDataDic[selectedNodeID].ID = selectedNodeID;
					AnimationBlendSpace1D_OnGUI(buildData->AnimBlendSpace1DBuildDataDic[selectedNodeID]);
					break;
				case ENodeType::AnimationBlendSpace:
					buildData->AnimBlendSpaceBuildDataDic[selectedNodeID].ID = selectedNodeID;
					AnimationBlendSpace_OnGUI(buildData->AnimBlendSpaceBuildDataDic[selectedNodeID]);
					break;
				}
			}
			else if (GetCurrentLayerBuildData()->NodeEditor->FindNodeTransition(selectedNodeID) != nullptr)
			{
				Transition_OnGUI(buildData->AnimTransitionBuildDataDic[selectedNodeID]);
			}

			ImGui::EndChild();
		}
	}
	void AnimationView::AnimationDataEditor_OnGUI()
	{
		SharedPtr<AnimationLayerBuildData> buildData = GetCurrentLayerBuildData();
		if (buildData != nullptr)
		{
			ImGui::BeginChild("Select View", ImVec2(mRightWidth, 30.0f));
			ImGui::Separator();
			if (ImGui::Selectable("Layer", mEditMode == EEditMode::AnimationLayer, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
			{
				mEditMode = EEditMode::AnimationLayer;
			}	ImGui::SameLine();
			if (ImGui::Selectable("Parameters", mEditMode == EEditMode::AnimationParameters, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
			{
				mEditMode = EEditMode::AnimationParameters;
			}
			ImGui::Separator();
			ImGui::EndChild();


			ImGui::BeginChild("DataEditor", ImVec2(mRightWidth, 0.0f));
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Selectable("Default##AnimParamEdit", mDataEditMode == EDataEditMode::Default, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
			{
				mDataEditMode = EDataEditMode::Default;
			}	ImGui::SameLine();
			if (ImGui::Selectable("Move##AnimParamEdit", mDataEditMode == EDataEditMode::Move, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
			{
				mDataEditMode = EDataEditMode::Move;
			}	ImGui::SameLine();
			if (ImGui::Selectable("Delete##AnimParamEdit", mDataEditMode == EDataEditMode::Delete, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
			{
				mDataEditMode = EDataEditMode::Delete;
			}
			ImGui::PopStyleVar();
			ImGui::Separator();


			switch (mEditMode)
			{
			case EEditMode::AnimationLayer:
				AnimationLayerEditor_OnGUI();
				break;
			case EEditMode::AnimationParameters:
				AnimationParamEditor_OnGUI();
				break;
			}
			ImGui::EndChild();
		}

	}

	void AnimationView::AnimationLayerEditor_OnGUI()
	{
		i32 index = 0;
		i32 moveIndex = -1;
		i32 moveDir = 0;
		List<String> removeLayerList;

		if (ImGui::BeginTable("AnimLayerTable", 2, ImGuiTableFlags_BordersH) == true)
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, mTableTypeRowWidth + mTableNameRowWidth);
			ImGui::TableSetupColumn("##Edit", ImGuiTableColumnFlags_WidthStretch, mEditRowWidth);
			ImGui::TableHeadersRow();

			SharedPtr<AnimationLayerBuildData> currLayerBuildData = GetCurrentLayerBuildData();
			bool   isInputTextDeactived = false;
			String inputText;
			for (auto buildData : mAnimLayerBuildDataList)
			{
				ImGui::TableNextColumn();
				ImGui::PushID(index);
				String layerName = buildData->Name;
			
				ImGui::SetNextItemWidth(mTableTypeRowWidth + mTableNameRowWidth);

				bool isSelected = false;
	
				if (currLayerBuildData != nullptr)
				{
					isSelected = layerName == currLayerBuildData->Name;
				}

				if (isSelected && mRenamingLayerBuildData != nullptr && layerName != AnimationController::BASE_LAYER_NAME)
				{
					
					ImGui::InputText("##LayerNameInputText", layerName, inputText);
					if (ImGui::IsItemDeactivated() == true && inputText != AnimationController::BASE_LAYER_NAME)
					{
						isInputTextDeactived = true;
					}
				}
				else if (ImGui::Selectable(layerName.c_str(), isSelected) == true)
				{
					SetCurrentLayerBuildData(layerName);

					if (mRenamingLayerBuildData != nullptr && inputText != AnimationController::BASE_LAYER_NAME)
					{
						isInputTextDeactived = true;
					}
				}
				ImGui::TableNextColumn();


				switch (mDataEditMode)
				{
				case EDataEditMode::Delete:
					if (IsEditable())
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
						if (ImGui::Button("-", ImVec2(50.0f, 0.0)) && buildData->Name != AnimationController::BASE_LAYER_NAME)
						{
							removeLayerList.push_back(buildData->Name);
						}
						ImGui::PopStyleColor();
					}

					break;
				case EDataEditMode::Move:
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



			if (isInputTextDeactived && mRenamingLayerBuildData != nullptr)
			{
				String oldName = mRenamingLayerBuildData->Name;
				UpdateLayerBuildData(oldName, inputText);
				mRenamingLayerBuildData = nullptr;
			}


			if (ImGui::IsKeyPressed((i32)EKeyCode::F2) == true)
			{
				mRenamingLayerBuildData = GetCurrentLayerBuildData().get();
			}


			auto padding = ImGui::GetStyle().FramePadding;
			if (ImGui::Button("+", ImVec2(mRightWidth - (padding.x * 4), 0.0f)) == true && IsEditable())
			{
				i32 cnt = 0;
				while (true)
				{
					String newName = "NewLayer (" + std::to_string(cnt++) + ")";
					if (mAddedAnimLayerNameSet.find(newName) == mAddedAnimLayerNameSet.end())
					{
						SharedPtr<AnimationLayerBuildData> buildData = CreateSharedPtr< AnimationLayerBuildData>();
						buildData->Name = newName;
						buildData->InitNodeEditor();
						buildData->InitBuildData(newName);
						mAddedAnimLayerNameSet.insert(newName);
						mAnimLayerBuildDataList.push_back(buildData);
						break;
					}
				}
			}

			for (const String& removedLayerName : removeLayerList)
			{
				mAddedAnimLayerNameSet.erase(removedLayerName);
				mAnimLayerBuildDataList.erase(std::remove_if(mAnimLayerBuildDataList.begin(), mAnimLayerBuildDataList.end(), [&](SharedPtr<AnimationLayerBuildData> data)
				{
					return data->Name == removedLayerName;
				}));
			}
			if (moveIndex >= 0)
			{
				if (moveDir == ImGuiDir_Up && mAnimLayerBuildDataList.size() > moveIndex && moveIndex > 0)
				{
					SharedPtr<AnimationLayerBuildData> tempData = mAnimLayerBuildDataList[moveIndex - 1];
					mAnimLayerBuildDataList[moveIndex - 1] = mAnimLayerBuildDataList[moveIndex];
					mAnimLayerBuildDataList[moveIndex] = tempData;
				}
				if (moveDir == ImGuiDir_Down && mAnimLayerBuildDataList.size() > (moveIndex + 1))
				{
					SharedPtr<AnimationLayerBuildData> tempData = mAnimLayerBuildDataList[moveIndex + 1];
					mAnimLayerBuildDataList[moveIndex + 1] = mAnimLayerBuildDataList[moveIndex];
					mAnimLayerBuildDataList[moveIndex] = tempData;
				}
			}

	
		}
	}


	void AnimationView::AnimationParamEditor_OnGUI()
	{
		i32 index = 0;
		i32 moveIndex = -1;
		i32 moveDir = 0;
		List<String> removeParamList;

		if (ImGui::BeginTable("ParamEditorTable", 3, ImGuiTableFlags_BordersH) == true)
		{
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, mTableTypeRowWidth);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, mTableNameRowWidth);
			ImGui::TableSetupColumn("##Edit", ImGuiTableColumnFlags_WidthStretch, mEditRowWidth);
			ImGui::TableHeadersRow();


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
						if (ImGui::Selectable(typeName.c_str()) == true && IsEditable())
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
				if (ImGui::IsItemDeactivated() && IsEditable())
				{
					mAddedAnimParamNameSet.erase(data.Name);
					mAddedAnimParamNameSet.insert(paramName);


					UpdateTransitionBuildData(data.Name, paramName);
					data.Name = paramName;
				}



				ImGui::TableNextColumn();


				switch (mDataEditMode)
				{
					// 값편집
				case EDataEditMode::Default:
					AnimationParam_OnGUI(data);
					break;
				case EDataEditMode::Delete:
					if (IsEditable())
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
						if (ImGui::Button("-", ImVec2(50.0f, 0.0)))
						{
							removeParamList.push_back(paramName);
						}
						ImGui::PopStyleColor();
					}

					break;
				case EDataEditMode::Move:
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
		}



		auto padding = ImGui::GetStyle().FramePadding;

		if (ImGui::Button("+", ImVec2(mRightWidth - (padding.x * 4), 0.0f)) == true && IsEditable())
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
	}
	void AnimationView::AnimationParam_OnGUI(AnimParamBuildData& buildData)
	{
	
		ImGui::SetNextItemWidth(mEditRowWidth);

		if (IsEditable() == false && mAnimationAsset != nullptr && mAnimationAsset->IsValid() == true)
		{
			SharedPtr<AnimationParameters> animParams= mAnimationAsset->Get()->GetAnimationParameters();

			switch (buildData.Type)
			{
			case EAnimationParameterType::Bool:
			{
				bool _bool;
				animParams->GetBool(buildData.Name, &_bool);
				ImGui::Checkbox("##CheckBox", &_bool);
				animParams->SetBool(buildData.Name, _bool);
			}
			break;
			case EAnimationParameterType::Trigger:
			{
				bool _bool = false;
				if (ImGui::RadioButton("##RadioButton", &_bool) == true)
				{
					animParams->SetTrigger(buildData.Name);
				}
			}
			break;
			case EAnimationParameterType::Float:
			{
				f32 value = 0.0f;
				animParams->GetFloat(buildData.Name, &value);
				ImGui::InputFloat("##InputFloat", &value);
				if (ImGui::IsItemDeactivated())
				{
					animParams->SetFloat(buildData.Name, value);
				}
			}

			break;
			case EAnimationParameterType::Int:
			{
				i32 value = 0.0f;
				animParams->GetInt(buildData.Name, &value);
				ImGui::InputInt("##InputInt", &value, 0, 0);
				if (ImGui::IsItemDeactivated())
				{
					animParams->SetInt(buildData.Name, value);
				}
			}
			break;
			}
			
		}
		else
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

		
	}
	void AnimationView::Transition_OnGUI(AnimTransitionBuildData& buildData)
	{

		StateNodeGUI::StateNode* fromNode = GetCurrentLayerBuildData()->NodeEditor->FindNode(buildData.From);
		StateNodeGUI::StateNode* toNode = GetCurrentLayerBuildData()->NodeEditor->FindNode(buildData.To);

		ImGui::Text("%s -> %s", 
			fromNode == nullptr ? "None" : fromNode->GetName().c_str(),
			toNode == nullptr ? "None" : toNode->GetName().c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text("TransitionDuration "); ImGui::SameLine();

		f32 transitionDuration = buildData.TransitionDuration;
		ImGui::SetNextItemWidth(200.0f);
		ImGui::InputFloat("##InputText_TransitionDuration", &transitionDuration);
		if (ImGui::IsItemDeactivated())
		{
			buildData.TransitionDuration = transitionDuration;
		}

		if (fromNode != nullptr && fromNode->GetUserData<ENodeType>() == ENodeType::AnimationClip)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("HasExitTime"); ImGui::SameLine();
			ImGui::Checkbox("##HasExitTime_CheckBox", &buildData.HasExitTime);

			if (buildData.HasExitTime)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text("ExitTime"); ImGui::SameLine();
				ImGui::SetNextItemWidth(200.0f);
				ImGui::InputFloat("##ExitTime_InputFloat", &buildData.ExitTime);
			}
		}
		

		ImGui::Separator();
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		if (ImGui::Selectable("Default##TransitionConditionEditMode", mTransitionConditionEditMode == EDataEditMode::Default, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mTransitionConditionEditMode = EDataEditMode::Default;
		}	ImGui::SameLine();
		if (ImGui::Selectable("Move##TransitionConditionEditMode", mTransitionConditionEditMode == EDataEditMode::Move, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mTransitionConditionEditMode = EDataEditMode::Move;
		}	ImGui::SameLine();
		if (ImGui::Selectable("Delete##TransitionConditionEditMode", mTransitionConditionEditMode == EDataEditMode::Delete, ImGuiSelectableFlags_None, ImVec2(70.0f, 25.0f)) == true)
		{
			mTransitionConditionEditMode = EDataEditMode::Delete;
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
					if (ImGui::Selectable(paramBuildData.Name.c_str()) == true && IsEditable())
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
					if (ImGui::Selectable(conditionStr.c_str()) == true && IsEditable())
					{
						condBuildData.Condition = EAnimationCondition::Equal;
					}
				}
				else if (condBuildData.Type == EAnimationParameterType::Trigger)
				{

				}
				else
				{
					for (i32 iParamType = 1; iParamType < (i32)EAnimationCondition::Count; ++iParamType)
					{
						conditionStr = AnimationConditionTypeToString((EAnimationCondition)iParamType);
						if (ImGui::Selectable(conditionStr.c_str()) == true && IsEditable())
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
			case EDataEditMode::Default:
				TransitionConditionValue_OnGUI(condBuildData);
				break;
			case EDataEditMode::Delete:
				if (IsEditable())
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
					if (ImGui::Button("-", ImVec2(50.0f, 0.0)))
					{
						removeIndex = index;
					}
					ImGui::PopStyleColor();
				}
	
				break;
			case EDataEditMode::Move:
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
		if (ImGui::Button("+", ImVec2(mRightWidth - (padding.x * 4), 0.0f)) == true && IsEditable())
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
			if (IsEditable())
			{
				ImGui::Checkbox("##CheckBox", (bool*)buildData.Data.data());
			}
			else
			{
				bool _bool = *(bool*)buildData.Data.data();
				ImGui::Checkbox("##CheckBox", &_bool);
			}
		}
		break;
		case EAnimationParameterType::Float:
		{
			f32 value = *((f32*)buildData.Data.data());
			ImGui::InputFloat("##InputFloat", &value);
			if (ImGui::IsItemDeactivated() && IsEditable())
			{
				memcpy(buildData.Data.data(), &value, sizeof(f32));
			}
		}

		break;
		case EAnimationParameterType::Int:
		{
			i32 value = *((i32*)buildData.Data.data());
			ImGui::InputInt("##InputInt", &value, 0, 0);
			if (ImGui::IsItemDeactivated() && IsEditable())
			{
				memcpy(buildData.Data.data(), &value, sizeof(i32));
			}
		}
		break;
		}
	}
	void AnimationView::AnimationRoot_OnGUI()
	{
	}
	void AnimationView::AnimationClip_OnGUI(AnimClipBuildData& buildData)
	{
		ImGui::PushID(buildData.ID);
		ImGui::Text("AnimationClip");
		ImGui::Text("Name : NodeName");
		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Repeat "); ImGui::SameLine();
		StateNodeGUI::StateNode* node = GetCurrentLayerBuildData()->NodeEditor->FindNode(buildData.ID);
		bool _bool = buildData.Flags & EAnimationClipFlags::Repeat;
		if (ImGui::Checkbox("##CheckBox_Repeat", &_bool) == true && IsEditable())
		{
			if (_bool == false) buildData.Flags = (EAnimationClipFlags)((i32)buildData.Flags & (~(i32)EAnimationClipFlags::Repeat));
			else buildData.Flags = buildData.Flags | EAnimationClipFlags::Repeat;
		}

		ImGui::Text("Speed"); ImGui::SameLine();
		ImGui::InputFloat("##Speed_InputFloat", &buildData.Speed);




		f32 label_Space = ImGui::CalcTextSize("         ").x;

		ImGui::AssetField_OnGUI("AnimationClip", (buildData.Asset != nullptr && buildData.Asset->IsValid()) ? buildData.Asset->GetAssetName() : "None",
			EAssetFormat::AnimationClip, [&](const String& assetPath)
		{
			if (IsEditable())
			{
				buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationClip>(assetPath);
			}
			
		}, label_Space);

		
		ImGui::Separator();
		if (buildData.Asset != nullptr && buildData.Asset->IsValid())
		{
			if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
			{
				SharedPtr<AnimationClipInfo> animClipInfo = mAnimationAsset->Get()->FindAnimationClipInfo(node->GetName());
				if (animClipInfo != nullptr)
				{
					ImGui::ProgressBar(animClipInfo->GetNormalizedTimePos(), ImVec2(-FLT_MIN, 0), animClipInfo->GetName().c_str());
				}

			}
		}
		ImGui::PopID();
	}
	void AnimationView::AnimationBlendSpace1D_OnGUI(AnimBlendSpace1DBuildData& buildData)
	{
		ImGui::PushID(buildData.ID);
		ImGui::Text("AnimationBlendSpace1D");
		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();

		f32 label_Space = ImGui::CalcTextSize("         ").x;
		StateNodeGUI::StateNode* node = GetCurrentLayerBuildData()->NodeEditor->FindNode(buildData.ID);


		ImGui::AssetField_OnGUI("Asset", (buildData.Asset != nullptr && buildData.Asset->IsValid()) ? buildData.Asset->GetAssetName() : "None",
			EAssetFormat::AnimationBlendSpace1D, [&](const String& assetPath)
		{
			if (IsEditable())
			{
				buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationBlendSpace1D>(assetPath);
			}

		}, label_Space);
		ImGui::Separator();
		if (buildData.Asset != nullptr && buildData.Asset->IsValid())
		{
			ImGui::Text("Param Name : %s", buildData.Asset->Get()->GetXParamName().c_str());
			ImGui::Text("Value :  %3.f  ~  %3.f", buildData.Asset->Get()->GetMinValue(), buildData.Asset->Get()->GetMaxValue());

			if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
			{
				SharedPtr<AnimationBlendSpace1DInfo> blendSpace1DInfo = mAnimationAsset->Get()->FindAnimationBlendSpace1DInfo(node->GetName());
				if (blendSpace1DInfo != nullptr)
				{
					const AnimationClipInfo& leftAnimClipInfo = blendSpace1DInfo->GetLeftBlendingAnimationClipInfo();
					const AnimationClipInfo& rightAnimClipInfo = blendSpace1DInfo->GetRightBlendingAnimationClipInfo();
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					f32 regionWidth = ImGui::GetWindowContentRegionWidth();
					f32 progressBarWidth = regionWidth * 0.5f - 5.0f;


					ImGui::Text("Left : %s", leftAnimClipInfo.GetName().c_str());
					ImGui::SameLine(regionWidth * 0.5f + 5.0f);
					ImGui::Text("Right : %s", rightAnimClipInfo.GetName().c_str());
					ImGui::Spacing();
					ImGui::Text("BlendFactor : %f", blendSpace1DInfo->GetBlendFactor());
					ImGui::ProgressBar(leftAnimClipInfo.GetNormalizedTimePos(), ImVec2(progressBarWidth, 0), leftAnimClipInfo.GetName().c_str());
					ImGui::SameLine();
					ImGui::ProgressBar(rightAnimClipInfo.GetNormalizedTimePos(), ImVec2(progressBarWidth, 0), rightAnimClipInfo.GetName().c_str());
				}
				
			}
		}
		ImGui::PopID();
	}
	void AnimationView::AnimationBlendSpace_OnGUI(AnimBlendSpaceBuildData& buildData)
	{
		ImGui::PushID(buildData.ID);
		ImGui::Text("AnimationBlendSpace");
		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();



		f32 label_Space = ImGui::CalcTextSize("         ").x;
		StateNodeGUI::StateNode* node = GetCurrentLayerBuildData()->NodeEditor->FindNode(buildData.ID);


		ImGui::AssetField_OnGUI("Asset", (buildData.Asset != nullptr && buildData.Asset->IsValid()) ? buildData.Asset->GetAssetName() : "None",
			EAssetFormat::AnimationBlendSpace, [&](const String& assetPath)
		{
			if (IsEditable())
			{
				buildData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationBlendSpace>(assetPath);
			}

		}, label_Space);
		ImGui::Separator();
		if (buildData.Asset != nullptr && buildData.Asset->IsValid())
		{
			ImGui::Text("Param X Name : %s", buildData.Asset->Get()->GetXParamName().c_str());
			ImGui::Text("Value :  %3.f  ~  %3.f", buildData.Asset->Get()->GetMinXValue(), buildData.Asset->Get()->GetMaxXValue());


			ImGui::Text("Param Y Name : %s", buildData.Asset->Get()->GetYParamName().c_str());
			ImGui::Text("Value :  %3.f  ~  %3.f", buildData.Asset->Get()->GetMinYValue(), buildData.Asset->Get()->GetMaxYValue());

			
			if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
			{
				SharedPtr<AnimationBlendSpaceInfo> blendSpaceInfo = mAnimationAsset->Get()->FindAnimationBlendSpaceInfo(node->GetName());
				if (blendSpaceInfo != nullptr)
				{
					const AnimationClipInfo& ltInfo = blendSpaceInfo->GetLeftTopBlendingAnimationClipInfo();
					const AnimationClipInfo& lbInfo = blendSpaceInfo->GetLeftBottomBlendingAnimationClipInfo();
					const AnimationClipInfo& rtInfo = blendSpaceInfo->GetRightTopBlendingAnimationClipInfo();
					const AnimationClipInfo& rbInfo = blendSpaceInfo->GetRightBottomBlendingAnimationClipInfo();
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					f32 regionWidth = ImGui::GetWindowContentRegionWidth();
					f32 progressBarWidth = regionWidth * 0.5f - 5.0f;


					ImGui::Text("LeftTop : %s", ltInfo.GetName().c_str());
					ImGui::SameLine(regionWidth * 0.5f + 5.0f);
					ImGui::Text("RightTop : %s", rtInfo.GetName().c_str());
					ImGui::Text("LeftBottom : %s", lbInfo.GetName().c_str());
					ImGui::SameLine(regionWidth * 0.5f + 5.0f);
					ImGui::Text("RightBottom : %s", rbInfo.GetName().c_str());


					ImGui::Spacing();
					ImGui::Text("BlendFactor : %f", blendSpaceInfo->GetBlendFactor());
					ImGui::ProgressBar(ltInfo.GetNormalizedTimePos(), ImVec2(progressBarWidth, 0), ltInfo.GetName().c_str());
					ImGui::SameLine();
					ImGui::ProgressBar(rtInfo.GetNormalizedTimePos(), ImVec2(progressBarWidth, 0), rtInfo.GetName().c_str());

					ImGui::ProgressBar(lbInfo.GetNormalizedTimePos(), ImVec2(progressBarWidth, 0), lbInfo.GetName().c_str());
					ImGui::SameLine();
					ImGui::ProgressBar(rbInfo.GetNormalizedTimePos(), ImVec2(progressBarWidth, 0), rbInfo.GetName().c_str());
				}
				
			
			}
		}

		ImGui::PopID();
	}
	StateNodeGUI::StateNodeID AnimationView::CreateNode(const String& layerName, ENodeType nodeType, const String& name, const JVector2& initPos)
	{
		SharedPtr<AnimationLayerBuildData> currLayerBuildData = GetLayerBuildData(layerName);
		if (currLayerBuildData == nullptr)
		{
			return 0;
		}
		return currLayerBuildData->CreateNode(nodeType, name, initPos);
	}


	
	void AnimationView::UpdateScene()
	{
		if (mMeshAsset == nullptr)
		{
			SetMesh(mModelAssetPath.GetValue());
		}
		if (mSkeletoneAsset == nullptr)
		{
			SetSkeletal(mSkeletoneAssetPath.GetValue());
		}
		if (mMaterialAssetList.empty())
		{
			SetMaterial(StringHelper::Split(mMaterialAssetPath.GetValue(), ','));
		}
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
			
		}

		if (mMeshAsset != nullptr && mMeshAsset->IsValid())
		{
			JBBox boundingBox = mMeshAsset->Get()->GetBoundingBox();

			JVector3 Center = boundingBox.Center();
			mEditorUIScene->SetLocation(Center * -1);

			JVector3 targetVec = JVector3::Normalize(EyePos * -1);
			targetVec.y = Math::Clamp(targetVec.y - 0.15f, 0.0f, targetVec.y);
			mEditorUIScene->SetTargetVector(targetVec);

			if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
			{
				mAnimationAsset->Get()->BindMesh(mMeshAsset->Get());
				mModel->Mesh = mAnimationAsset->Get()->GetBindedMesh();				
				mModel->Flags = Graphics::ESceneObjectFlags::Always_Update_Bottom_Level_AS;
			}
			else {
				mModel->Mesh = mMeshAsset->Get();
			}
		}
		if (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid())
		{
			if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
			{
				mAnimationAsset->Get()->BindSkeletone(mSkeletoneAsset->Get());
			}
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
		mMeshAsset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<IMesh>(meshAssetPath);
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
		for (auto& buildData : mAnimLayerBuildDataList)
		{
			for (auto& _pair : buildData->AnimTransitionBuildDataDic)
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
	}
	void AnimationView::UpdateTransitionBuildData(const String& oldName, const String& newName)
	{
		for (auto& buildData : mAnimLayerBuildDataList)
		{
			for (auto& _pair : buildData->AnimTransitionBuildDataDic)
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

	}
	void AnimationView::RemoveTransitionBuildData(const String& removedName)
	{
		for (auto& buildData : mAnimLayerBuildDataList)
		{
			for (auto& _pair : buildData->AnimTransitionBuildDataDic)
			{
				if (_pair.second.Conditions.empty() == false)
				{
					auto iter = std::remove_if(_pair.second.Conditions.begin(), _pair.second.Conditions.end(),
						[&](const AnimTransitionConditionBuildData& data)
					{
						return data.ParamName == removedName;
					});
					if (iter != _pair.second.Conditions.end())
					{
						_pair.second.Conditions.erase(iter);
					}

				}

			}
		}

	}
	void AnimationView::UpdateLayerBuildData(const String& oldName, const String& newName)
	{
		for (auto& buildData : mAnimLayerBuildDataList)
		{
			if (buildData->Name == oldName)
			{
				buildData->Name = newName;


				if (mCurrentAnimLayerBuildData == oldName)
				{
					mCurrentAnimLayerBuildData = newName;
				}
				mAddedAnimLayerNameSet.erase(oldName);
				mAddedAnimLayerNameSet.insert(newName);

			}
		}



	}
	bool AnimationView::Build()
	{
		AnimationAssetStock assetStock;
	
		for (const AnimParamBuildData& buildData : mAnimParamBuildDataList)
		{
			AnimationAssetStock::ParameterData paramData;
			paramData.Name = buildData.Name;
			paramData.Type = buildData.Type;
			paramData.Data = buildData.Data;

			assetStock.Parameters.push_back(paramData);
		}
		for (auto& buildData : mAnimLayerBuildDataList)
		{
			StateNodeGUI::StateNodeLinkInfo nodeLinkInfo = buildData->NodeEditor->GetNodeLinkInfo();

			AnimationAssetStock::AnimationStateMachineInfo animStateMachineInfo;
			animStateMachineInfo.Name = buildData->Name;

			HashSet<String> cachedNodeName;
			for (auto _pair : buildData->AnimClipBuildDataDic)
			{
				if (_pair.first == nodeLinkInfo.RootNodeID) continue;

				SharedPtr<Asset<AnimationClip>> clipAsset = _pair.second.Asset;
				if (clipAsset == nullptr || clipAsset->IsValid() == false)
				{
					JG_LOG_ERROR("AnimationClipNode's Asset is Null -> Node ID :  {0}", _pair.first);
					return false;
				}
				StateNodeGUI::StateNode* node = buildData->NodeEditor->FindNode(_pair.first);
				if (node == nullptr)
				{
					JG_LOG_ERROR("Can't Find StateNode In NodeEditor: {0}", _pair.first);
					return false;
				}
				if (cachedNodeName.find(node->GetName()) != cachedNodeName.end())
				{
					JG_LOG_ERROR("Is Duplicates StateNodeName In NodeEditor: {0}", _pair.first);
					return false;
				}
				AnimationAssetStock::AnimationClipInfo clipInfo;
				clipInfo.Name = node->GetName();
				clipInfo.AssetPath = clipAsset->GetAssetPath();
				clipInfo.Flags = _pair.second.Flags;
				clipInfo.Speed = _pair.second.Speed;
				cachedNodeName.insert(node->GetName());
				animStateMachineInfo.AnimClips.push_back(clipInfo);
			}
			for (auto _pair : buildData->AnimBlendSpace1DBuildDataDic)
			{
				if (_pair.first == nodeLinkInfo.RootNodeID) continue;

				SharedPtr<Asset<AnimationBlendSpace1D>> blendAsset = _pair.second.Asset;
				if (blendAsset == nullptr || blendAsset->IsValid() == false)
				{
					JG_LOG_ERROR("AnimationBlendSpace1D's Asset is Null -> Node ID :  {0}", _pair.first);
					return false;
				}
				StateNodeGUI::StateNode* node = buildData->NodeEditor->FindNode(_pair.first);
				if (node == nullptr)
				{
					JG_LOG_ERROR("Can't Find StateNode In NodeEditor: {0}", _pair.first);
					return false;
				}
				if (cachedNodeName.find(node->GetName()) != cachedNodeName.end())
				{
					JG_LOG_ERROR("Is Duplicates StateNodeName In NodeEditor: {0}", _pair.first);
					return false;
				}
				AnimationAssetStock::AnimationBlendSpace1DInfo blendSpaceInfo;
				blendSpaceInfo.Name = node->GetName();
				blendSpaceInfo.AssetPath = blendAsset->GetAssetPath();
				blendSpaceInfo.Flags = _pair.second.Flags;
				cachedNodeName.insert(node->GetName());
				animStateMachineInfo.AnimBlendSpace1Ds.push_back(blendSpaceInfo);
			}
			for (auto _pair : buildData->AnimBlendSpaceBuildDataDic)
			{
				if (_pair.first == nodeLinkInfo.RootNodeID) continue;

				SharedPtr<Asset<AnimationBlendSpace>> blendAsset = _pair.second.Asset;
				if (blendAsset == nullptr || blendAsset->IsValid() == false)
				{
					JG_LOG_ERROR("AnimationBlendSpace's Asset is Null -> Node ID :  {0}", _pair.first);
					return false;
				}
				StateNodeGUI::StateNode* node = buildData->NodeEditor->FindNode(_pair.first);
				if (node == nullptr)
				{
					JG_LOG_ERROR("Can't Find StateNode In NodeEditor: {0}", _pair.first);
					return false;
				}
				if (cachedNodeName.find(node->GetName()) != cachedNodeName.end())
				{
					JG_LOG_ERROR("Is Duplicates StateNodeName In NodeEditor: {0}", _pair.first);
					return false;
				}
				AnimationAssetStock::AnimationBlendSpaceInfo blendSpaceInfo;
				blendSpaceInfo.Name = node->GetName();
				blendSpaceInfo.AssetPath = blendAsset->GetAssetPath();
				blendSpaceInfo.Flags = _pair.second.Flags;
				cachedNodeName.insert(node->GetName());
				animStateMachineInfo.AnimBlendSpaces.push_back(blendSpaceInfo);
			}


			StateNodeGUI::StateNode* rootNode = buildData->NodeEditor->FindNode(nodeLinkInfo.RootNodeID);
			if (rootNode == nullptr)
			{
				JG_LOG_ERROR("Can't Find RootNode");
				return false;
			}
			animStateMachineInfo.RootName = rootNode->GetName();
		
			StateNodeGUI::StateNode* exitNode = buildData->NodeEditor->FindNode(nodeLinkInfo.ExitNodeID);
			if (exitNode != nullptr)
			{
				animStateMachineInfo.ExitName = exitNode->GetName();
			}

			// Create Transition
			u32 nodeCnt = nodeLinkInfo.NodeIDList.size();
			for (u32 i = 0; i < nodeCnt; ++i)
			{
				StateNodeGUI::StateNodeID nodeID = nodeLinkInfo.NodeIDList[i];
				List<StateNodeGUI::StateNodeID> linkedNodeIDList = nodeLinkInfo.LinkedNodeIDLists[i];
				StateNodeGUI::StateNode* prevNode = buildData->NodeEditor->FindNode(nodeID);


				for (StateNodeGUI::StateNodeID linkedNodeID : linkedNodeIDList)
				{
					StateNodeGUI::StateNode* nextNode = buildData->NodeEditor->FindNode(linkedNodeID);
					if (nextNode == nullptr)
					{
						JG_LOG_ERROR("Can't Find Node {0}", nodeID);
						return false;
					}


					AnimationAssetStock::AnimationTransitionInfo transInfo;
					transInfo.PrevName = prevNode->GetName();
					transInfo.NextName = nextNode->GetName();

					StateNodeGUI::StateNodeID transitionNodeID = prevNode->GetTransition(nextNode->GetID());
					const AnimTransitionBuildData& tranBuildData = buildData->AnimTransitionBuildDataDic[transitionNodeID];

					transInfo.TransitionDuration = tranBuildData.TransitionDuration;
					transInfo.HasExitTime = tranBuildData.HasExitTime;
					transInfo.ExitTime = tranBuildData.ExitTime;
					for (const AnimTransitionConditionBuildData& condBuildData : tranBuildData.Conditions)
					{
						AnimationAssetStock::AnimationTransitionConditionInfo condInfo;
						condInfo.ParameterName = condBuildData.ParamName;
						condInfo.Condition = condBuildData.Condition;
						condInfo.Data = condBuildData.Data;
						transInfo.Transitions.push_back(condInfo);
					}
					animStateMachineInfo.TransitionInfos.push_back(transInfo);
				}

			}

			assetStock.AnimStateMachineInfos.push_back(animStateMachineInfo);
		}

		

		if (AssetHelper::WriteAsset(EAssetFormat::Animation, mAnimationAsset->GetAssetPath(),
			[&](SharedPtr<JsonData> assetJson)
		{
			assetStock.MakeJson(assetJson);
		}) == false)
		{
			JG_LOG_ERROR("Fail Write Asset");
			return false;
		}

		
		if (mAnimationAsset != nullptr)
		{
			RefreshAnimAsset(mAnimationAsset->GetAssetPath());
			
		}
		if (mSkeletoneAsset == nullptr || mSkeletoneAsset->IsValid() == false)
		{
			JG_LOG_ERROR("Skeleton Asset is Null");
			return false;
		}

		SaveNodeLocation();
		return true;
	}
	void AnimationView::RefreshAnimAsset(const String& assetPath)
	{
		if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimationAsset->Get());
	
		}

		mAnimationAsset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationController>(assetPath);
		if (mAnimationAsset != nullptr)
		{
			AssetDataBase::GetInstance().RefreshAsset(mAnimationAsset->GetAssetID(), true);
			JGAnimation::GetInstance().RegisterAnimationController(mAnimationAsset->Get());
		}
	}
	void AnimationView::Play()
	{
	

	}
	void AnimationView::UpdateFlow()
	{
		// Update Flow
		if (mAnimationAsset == nullptr || mAnimationAsset->IsValid() == false)
		{
			return;
		}
		SharedPtr<AnimationLayerBuildData> currentLayerBuildData = GetCurrentLayerBuildData();

		const AnimationStateFlow& flow = mAnimationAsset->Get()->GetAnimationStateFlow();

		List<StateNodeGUI::StateNodeID> flowList;
		for (const String& nodeName : flow.NodeList)
		{
			if (currentLayerBuildData->NodeNameDic.find(nodeName) == currentLayerBuildData->NodeNameDic.end())
			{
				return;
			}
			StateNodeGUI::StateNodeID id = currentLayerBuildData->NodeNameDic[nodeName];
			flowList.push_back(id);
		}
		currentLayerBuildData->NodeEditor->Flow(flowList);
	}
	void AnimationView::Editable()
	{
		SharedPtr<AnimationLayerBuildData> currentLayerBuildData = GetCurrentLayerBuildData();


		currentLayerBuildData->NodeEditor->Flow(List<StateNodeGUI::StateNodeID>());
		if (mAnimationAsset != nullptr && mAnimationAsset->IsValid())
		{
			mAnimationAsset->Get()->Reset();
		}
		SharedPtr<AnimationParameters> animParams = mAnimationAsset->Get()->GetAnimationParameters();
		for (const AnimParamBuildData& buildData : mAnimParamBuildDataList)
		{
			switch (buildData.Type)
			{
			case EAnimationParameterType::Bool:
				animParams->SetBool(buildData.Name, *((bool*)buildData.Data.data()));
			break;
			case EAnimationParameterType::Float:
				animParams->SetFloat(buildData.Name, *((f32*)buildData.Data.data()));
			break;
			case EAnimationParameterType::Int:
				animParams->SetInt(buildData.Name, *((i32*)buildData.Data.data()));
			break;
			}
		}
	}
	void AnimationView::SaveNodeLocation()
	{
		SharedPtr<Json> json = CreateSharedPtr<Json>();
		// Location 저장
		SharedPtr<JsonData> jsonMain = json->CreateJsonData();

		for (auto buildData : mAnimLayerBuildDataList)
		{
			SharedPtr<JsonData> nodeLocationListJson = jsonMain->CreateJsonData();
			for (auto _nodeLocation_pair : buildData->NodeLocationDic)
			{
				StateNodeGUI::StateNode* node = buildData->NodeEditor->FindNode(_nodeLocation_pair.first);
				if (node == nullptr) continue;

				SharedPtr<JsonData> dataJson = nodeLocationListJson->CreateJsonData();
				dataJson->AddMember("Name", node->GetName());
				dataJson->AddMember("NodeType", (u32)node->GetUserData<ENodeType>());
				dataJson->AddMember("Location", node->GetLocation());
				nodeLocationListJson->AddMember(dataJson);
			}
			jsonMain->AddMember(buildData->Name, nodeLocationListJson);
		}
		json->AddMember("Main", jsonMain);
		mNodeSaveData.SetValue(Json::ToString(json));
	}

	SharedPtr<AnimationLayerBuildData> AnimationView::GetCurrentLayerBuildData() const
	{
		return GetLayerBuildData(mCurrentAnimLayerBuildData);
	}

	void AnimationView::SetCurrentLayerBuildData(const String& layerName)
	{
		if (mAddedAnimLayerNameSet.find(layerName) == mAddedAnimLayerNameSet.end())
		{
			return;
		}
		mCurrentAnimLayerBuildData = layerName;
	}
	SharedPtr<AnimationLayerBuildData> AnimationView::GetLayerBuildData(const String& name) const
	{
		if (mAddedAnimLayerNameSet.find(name) == mAddedAnimLayerNameSet.end())
		{
			return nullptr;
		}
		for (auto buildData : mAnimLayerBuildDataList)
		{
			if (buildData->Name == name)
			{
				return buildData;
			}
		}
		return nullptr;
	}
}