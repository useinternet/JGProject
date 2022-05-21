#include "pch.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "AnimationBlendSpaceView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"

#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetHelper.h"
#include "Animation/JGAnimation.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationStateMachine.h"
#include "Animation/AnimationClip.h"
#include "Animation/AnimationBlendSpace.h"
#include "Animation/AnimationParameters.h"


namespace JG
{
	AnimationBlendSpaceView::AnimationBlendSpaceView()
	{
		DisableUniqueView();

	}

	void AnimationBlendSpaceView::Initialize()
	{
		SetMesh(mModelAssetPath.GetValue());
		SetSkeletal(mSkeletoneAssetPath.GetValue());
		SetMaterial(StringHelper::Split(mMaterialAssetPath.GetValue(), ','));


		if (mAnimBlendAsset != nullptr)
		{
			AnimationBlendSpaceStock stock;
			AssetHelper::ReadAsset(EAssetFormat::AnimationBlendSpace, mAnimBlendAsset->GetAssetPath(), [&](SharedPtr<JsonData> jsonData)
			{
				stock.LoadJson(jsonData);
			});
			mBuildData.XParamName = stock.XParamName;
			mBuildData.YParamName = stock.YParamName;
			mBuildData.MinMaxXValue = stock.MinMaxXValue;
			mBuildData.MinMaxYValue = stock.MinMaxYValue;
			mBuildData.ValueXYNumGrid = stock.ValueXYNumGrid;

			f32 xValueWidth = (mBuildData.MinMaxXValue.y - mBuildData.MinMaxXValue.x);
			f32 yValueWidth = (mBuildData.MinMaxYValue.y - mBuildData.MinMaxYValue.x);

			f32 xOneUnit = xValueWidth / mBuildData.ValueXYNumGrid.x;
			f32 yOneUnit = yValueWidth / mBuildData.ValueXYNumGrid.y;

			for (const AnimationBlendSpaceStock::AnimClipData& clipData : stock.AnimClipDatas)
			{
				AnimClipData data;
				data.Name = clipData.Name;
				data.Asset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationClip>(clipData.AssetPath);
				data.Value = clipData.Value;
				data.Speed = clipData.Speed;
				i32 xGrid = (data.Value.x - mBuildData.MinMaxXValue.x) / xOneUnit;
				i32 yGrid = (data.Value.y - mBuildData.MinMaxYValue.x) / yOneUnit;
				i32 key = GetGridKey(JVector2Int(xGrid, yGrid));

				mBuildData.AnimClipDatas[key] = data;
			}
		}
	}

	void AnimationBlendSpaceView::OnGUI()
	{

		JVector2 winSize;
		f32 LeftWidth = mSceneResolution.x + ImGui::GetStyle().FramePadding.x * 4;
		f32 RightWidth = mInspectorWidth + ImGui::GetStyle().FramePadding.x * 4;
		winSize.x = LeftWidth + RightWidth;
		winSize.y = mSceneResolution.y + 130.0F + ImGui::GetStyle().FramePadding.y * 2;
		ImGui::SetWindowSize(ImVec2(winSize.x, winSize.y));
		// Image
		UpdateScene();
		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, LeftWidth);
		LeftOnGUI();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, RightWidth);
		RightOnGUI();
		ImGui::Columns(1);


		if (mAnimController != nullptr)
		{
			mAnimController->GetAnimationParameters()->SetFloat(mBuildData.XParamName, mSampleValue.x);
			mAnimController->GetAnimationParameters()->SetFloat(mBuildData.YParamName, mSampleValue.y);
		}
	}

	void AnimationBlendSpaceView::Destroy()
	{
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
		mAnimBlendAsset = nullptr;
		mSkyBox = nullptr;
		mModel = nullptr;
		mBuildData = AnimationBlendSpaceBuildData();

		if (mAnimController != nullptr)
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
			mAnimController = nullptr;
		}
		mEditorUIScene = nullptr;
	}


	void AnimationBlendSpaceView::Build()
	{
		if (mAnimBlendAsset == nullptr) return;
		AnimationBlendSpaceStock stock;

		stock.Name = mAnimBlendAsset->Get()->GetName();
		stock.XParamName = mBuildData.XParamName;
		stock.YParamName = mBuildData.YParamName;
		stock.MinMaxXValue = mBuildData.MinMaxXValue;
		stock.MinMaxYValue = mBuildData.MinMaxYValue;
		stock.ValueXYNumGrid = mBuildData.ValueXYNumGrid;

		for (auto& _pair : mBuildData.AnimClipDatas)
		{
			AnimClipData& clipData = _pair.second;
			if (clipData.Asset == nullptr || clipData.Asset->IsValid() == false)
			{
				continue;
			}

			AnimationBlendSpaceStock::AnimClipData data;
			data.AssetPath = clipData.Asset->GetAssetPath();
			data.Name	   = clipData.Name;
			data.Value	   = clipData.Value;
			data.Speed = clipData.Speed;
			stock.AnimClipDatas.push_back(data);
		}

		if (AssetHelper::WriteAsset(EAssetFormat::AnimationBlendSpace, mAnimBlendAsset->GetAssetPath(),
			[&](SharedPtr<JsonData> assetJson)
		{
			stock.MakeJson(assetJson);
		}) == false)
		{
			JG_LOG_ERROR("Fail Write Asset");
		}

		RefreshAsset(mAnimBlendAsset->GetAssetPath());
	}

	void AnimationBlendSpaceView::RefreshAsset(const String& assetPath)
	{
		if (mAnimBlendAsset == nullptr)
		{
			mAnimBlendAsset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationBlendSpace>(assetPath);
		}
		else
		{
			AssetDataBase::GetInstance().RefreshAsset(mAnimBlendAsset->GetAssetID(), true);
		}


		if (mAnimController != nullptr)
		{
			JGAnimation::GetInstance().UnRegisterAnimatioinController(mAnimController);
		}
		if (mAnimBlendAsset != nullptr && mAnimBlendAsset->IsValid())
		{
			mAnimController = CreateUniquePtr<AnimationController>();
			mAnimController->GetAnimationParameters()->SetFloat(mAnimBlendAsset->Get()->GetXParamName(), 0.0f);
			mAnimController->AddAnimationBlendSpace(BLEND_SPACE_NAME, mAnimBlendAsset->Get(), EAnimationBlendSpaceFlag::Repeat);
			mAnimController->GetAnimationStateMachine()->
				Begin("Start")
				.MakeAnimationBlendSpaceNode(BLEND_SPACE_NAME, nullptr)
				.ConnectNode("Start", BLEND_SPACE_NAME, nullptr)
				.End();

			JGAnimation::GetInstance().RegisterAnimationController(mAnimController);
		}


	}

	void AnimationBlendSpaceView::LeftOnGUI()
	{
		ImGui::BeginChild("AnimationBlendSpaceView_SceneView");
		if (mEditorUIScene != nullptr)
		{
			mEditorUIScene->OnGUI();
		}
		if (ImGui::Button("Build") == true)
		{
			Build();
		}
		ImGui::EndChild();
	}
	void AnimationBlendSpaceView::RightOnGUI()
	{
		ImGui::BeginChild("AnimationBlendSpaceView_Inspector", ImVec2(0,0), false, ImGuiWindowFlags_NoMove);

		f32 label_Space = ImGui::CalcTextSize("          ").x;

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

		ImGui::Separator();
		ValueParamOnGUI();
		ImGui::Separator();

		ImGui::Text("Sample Value : [ %.3f, %3.f ]", mSampleValue.x, mSampleValue.y);
		if (mSelectedGridKey != -1)
		{
			
			JVector2Int gridPos = GetGridPos(mSelectedGridKey);
			if (gridPos.x <= mBuildData.ValueXYNumGrid.x && gridPos.y <= mBuildData.ValueXYNumGrid.y)
			{
				if (mBuildData.AnimClipDatas.find(mSelectedGridKey) == mBuildData.AnimClipDatas.end())
				{
					f32 xValueWidth = (mBuildData.MinMaxXValue.y - mBuildData.MinMaxXValue.x);
					f32 yValueWidth = (mBuildData.MinMaxYValue.y - mBuildData.MinMaxYValue.x);

					f32 xOneUnit = xValueWidth / mBuildData.ValueXYNumGrid.x;
					f32 yOneUnit = yValueWidth / mBuildData.ValueXYNumGrid.y;


					JVector2Int gridPos = GetGridPos(mSelectedGridKey);
					AnimClipData clipData;
					clipData.Value = JVector2(xOneUnit * gridPos.x, yOneUnit * gridPos.y);
					clipData.Value += JVector2(mBuildData.MinMaxXValue.x, mBuildData.MinMaxYValue.x);

					mBuildData.AnimClipDatas[mSelectedGridKey] = clipData;
				}
				AnimClipData& clipData = mBuildData.AnimClipDatas[mSelectedGridKey];
				ImGui::PushID(clipData.Name.data());
				ImGui::Text("Name"); ImGui::SameLine();
				String inputText = clipData.Name;
				ImGui::SetNextItemWidth(120.0f);
				ImGui::InputText("##AnimClipName_InputText", inputText, inputText);
				if (ImGui::IsItemDeactivated())
				{
					clipData.Name = inputText;
				}
				ImGui::SameLine();
				bool isDelete = ImGui::Button("Delete");
				ImGui::SameLine();
				ImGui::Text("Speed"); ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				ImGui::InputFloat("##AnimClipSpeed_InputFloat", &clipData.Speed);


				ImGui::AssetField_OnGUI("Clip", clipData.Asset == nullptr ? "None" : clipData.Asset->GetAssetName(), EAssetFormat::AnimationClip,
					[&](const String& assetPath)
				{
					clipData.Asset = AssetDataBase::GetInstance().LoadOriginAsset<AnimationClip>(assetPath);
				});
				ImGui::PopID();
				if (isDelete)
				{
					mBuildData.AnimClipDatas.erase(mSelectedGridKey);
				}
			}
			else
			{
				ImGui::Text("None");
				mSelectedGridKey = -1;
			}
		}
		ImGui::Separator();
		CoordinateOnGUI();
		ImGui::EndChild();
	}

	void AnimationBlendSpaceView::ValueParamOnGUI()
	{
		// X Param
		{
			ImGui::Text("X  Name"); ImGui::SameLine();
			String inputText = mBuildData.XParamName;
			ImGui::SetNextItemWidth(150.0F);
			ImGui::InputText("##X_ParamName_InputText", mBuildData.XParamName, inputText);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.XParamName = inputText;
			} ImGui::SameLine();


			ImGui::Text(" Min"); ImGui::SameLine();
			f32 minValue = mBuildData.MinMaxXValue.x;
			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputFloat("##Float_MinXValue", &minValue);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.MinMaxXValue.x = minValue;
				UpdateValueParam();
			}ImGui::SameLine();

			ImGui::Text("Max"); ImGui::SameLine();
			f32 maxValue = mBuildData.MinMaxXValue.y;
			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputFloat("##Float_MaxXValue", &maxValue);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.MinMaxXValue.y = maxValue;
				UpdateValueParam();
			}
		}
		// Y Param
		{
			ImGui::Text("Y  Name"); ImGui::SameLine();
			String inputText = mBuildData.YParamName;
			ImGui::SetNextItemWidth(150.0F);
			ImGui::InputText("##Y_ParamName_InputText", mBuildData.YParamName, inputText);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.YParamName = inputText;
			} ImGui::SameLine();


			ImGui::Text(" Min"); ImGui::SameLine();
			f32 minValue = mBuildData.MinMaxYValue.x;
			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputFloat("##Float_MinYValue", &minValue);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.MinMaxYValue.x = minValue;
				UpdateValueParam();
			}ImGui::SameLine();

			ImGui::Text("Max"); ImGui::SameLine();
			f32 maxValue = mBuildData.MinMaxYValue.y;
			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputFloat("##Float_MaxYValue", &maxValue);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.MinMaxYValue.y = maxValue;
				UpdateValueParam();
			}
		}
		// NumGrid
		{
			ImGui::Text("X Grid Num"); ImGui::SameLine();
			i32 numGrid = mBuildData.ValueXYNumGrid.x;
			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputInt("##XNumGrid", &numGrid, 0, 0);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.ValueXYNumGrid.x = numGrid;
				UpdateValueParam();
			}ImGui::SameLine();

			ImGui::Text(" Y Grid Num"); ImGui::SameLine();
			numGrid = mBuildData.ValueXYNumGrid.y;
			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputInt("##YNumGrid", &numGrid, 0, 0);
			if (ImGui::IsItemDeactivated())
			{
				mBuildData.ValueXYNumGrid.y = numGrid;
				UpdateValueParam();
			}

			mBuildData.ValueXYNumGrid.x = Math::Max(mBuildData.ValueXYNumGrid.x, 1);
			mBuildData.ValueXYNumGrid.y = Math::Max(mBuildData.ValueXYNumGrid.y, 1);
		}
	}

	void AnimationBlendSpaceView::CoordinateOnGUI()
	{
		ImVec2 space = ImVec2(mCoordinateResidualSpace, mCoordinateResidualSpace);
		ImVec2 winPos    = ImGui::GetWindowPos();
		ImVec2 cursorPos = ImGui::GetCursorPos();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 localRectMin = cursorPos + ImGui::GetWindowContentRegionMin() + space;

		ImVec2 rectMin = winPos + localRectMin;
		ImVec2 rectMax = winPos + ImGui::GetWindowContentRegionMax() - space;




		u32 color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		f32 xOneUnit = (rectMax.x - rectMin.x) / mBuildData.ValueXYNumGrid.x;
		ImVec2 start_p1 = rectMin;
		ImVec2 start_p2 = ImVec2(rectMin.x, rectMax.y);
		for (i32 i = 0; i <= mBuildData.ValueXYNumGrid.x; ++i)
		{
			ImVec2 p1 = start_p1 + ImVec2(i * xOneUnit, 0);
			ImVec2 p2 = start_p2 + ImVec2(i * xOneUnit, 0);

			drawList->AddLine(p1, p2, color);
		}

		f32 yOneUnit = (rectMax.y - rectMin.y) / mBuildData.ValueXYNumGrid.y;
		start_p1 = rectMin;
		start_p2 = ImVec2(rectMax.x, rectMin.y);
		for (i32 i = 0; i <= mBuildData.ValueXYNumGrid.y; ++i)
		{
			ImVec2 p1 = start_p1 + ImVec2(0, i * yOneUnit);
			ImVec2 p2 = start_p2 + ImVec2(0, i * yOneUnit);

			drawList->AddLine(p1, p2, color);
		}


		// 
		ImVec2 dotSize = ImVec2(mDotSize, mDotSize);
		u32 dotEmptyColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.3f));
		u32 dotFillColor  = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		u32 dotOutlineColor = ImGui::GetColorU32(ImVec4(1.0f, 0.2f, 0.1f, 1.0f));
		for (i32 x = 0; x <= mBuildData.ValueXYNumGrid.x; ++x)
		{
			for (i32 y = 0; y <= mBuildData.ValueXYNumGrid.y; ++y)
			{
				u32 id = GetGridKey(JVector2Int(x, y));
				ImGui::PushID(id);
				ImVec2 p = ImVec2(x * xOneUnit, y * yOneUnit);
				ImVec2 dotRectMin = rectMin + p - (dotSize * 0.5f);
				ImVec2 dotRectMax = rectMin + p + (dotSize * 0.5f);
				ImVec2 localDotRectMin = localRectMin + p - (dotSize * 0.5f);
				// Find AnimClip
				if (mBuildData.AnimClipDatas.find(id) == mBuildData.AnimClipDatas.end() || 
					mBuildData.AnimClipDatas[id].Asset == nullptr || mBuildData.AnimClipDatas[id].Asset->IsValid() == false)
				{
					drawList->AddRectFilled(dotRectMin, dotRectMax, dotEmptyColor, mDotRounding, ImDrawFlags_RoundCornersAll);
				}
				else
				{
					drawList->AddRectFilled(dotRectMin, dotRectMax, dotFillColor, mDotRounding, ImDrawFlags_RoundCornersAll);
				}
				if (mSelectedGridKey == id)
				{
					drawList->AddRect(dotRectMin, dotRectMax, dotOutlineColor, mDotRounding, ImDrawFlags_RoundCornersAll);
				}
				ImGui::SetCursorPos(localDotRectMin);
				ImGui::Dummy(dotSize);
				if (ImGui::IsItemClicked(1))
				{
					mSelectedGridKey = id;
				}
				ImGui::PopID();
			}
		}





		mSampleValue = SampleValueDotOnGUI(
			JVector2(localRectMin.x, localRectMin.y), 
			JVector2(rectMin.x, rectMin.y), 
			JVector2(rectMax.x, rectMax.y),
			mSampleValue);

		// 각 꼭지점 min max 표시
		{
			String yValueMinStr = std::to_string((i32)mBuildData.MinMaxYValue.x);
			String yValueMaxStr = std::to_string((i32)mBuildData.MinMaxYValue.y);
			String xValueMinStr = std::to_string((i32)mBuildData.MinMaxXValue.x);
			String xValueMaxStr = std::to_string((i32)mBuildData.MinMaxXValue.y);
			ImVec2 regionMin = ImGui::GetWindowContentRegionMin() + space + cursorPos;
			ImVec2 regionMax = ImGui::GetWindowContentRegionMax() - space;
			f32 regionWidth = regionMax.x - regionMin.x;
			f32 regionHeight = regionMax.y - regionMin.y;


			ImVec2 currPos = regionMin;
			ImVec2 textSize = ImGui::CalcTextSize(yValueMinStr.c_str());


			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::SetCursorPos(currPos - ImVec2(textSize.x + 5.0f, 0));
			ImGui::Text(yValueMinStr.c_str());


			textSize = ImGui::CalcTextSize(yValueMaxStr.c_str());
			currPos.y += (regionHeight - textSize.y);
			ImGui::SetCursorPos(currPos - ImVec2(textSize.x + 5.0f, 0));
			ImGui::Text(yValueMaxStr.c_str());
			currPos.y += textSize.y;

			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			textSize = ImGui::CalcTextSize(xValueMinStr.c_str());
			currPos.y += 5.0f;
			ImGui::SetCursorPos(currPos);
			ImGui::Text(xValueMinStr.c_str());


			textSize = ImGui::CalcTextSize(xValueMaxStr.c_str());
			currPos.x += (regionWidth - textSize.x);
			ImGui::SetCursorPos(currPos);
			ImGui::Text(xValueMaxStr.c_str());

			ImGui::PopStyleColor();
		}
	}

	JVector2 AnimationBlendSpaceView::SampleValueDotOnGUI(const JVector2& localRectMin, const JVector2& rectMin, const JVector2& rectMax, const JVector2& value)
	{
		ImVec2 space = ImVec2(mCoordinateResidualSpace, mCoordinateResidualSpace);

		JVector2 result = value;
		f32 rectWidth = rectMax.x - rectMin.x;
		f32 rectHeight = rectMax.y - rectMin.y;
		f32 valueXWidth = mBuildData.MinMaxXValue.y - mBuildData.MinMaxXValue.x;
		f32 valueYWidth = mBuildData.MinMaxYValue.y - mBuildData.MinMaxYValue.x;
		// 
		JVector2 offset;
		if (valueXWidth != 0.0f && valueYWidth != 0.0f)
		{
			offset.x = ((result.x - mBuildData.MinMaxXValue.x) / valueXWidth) * rectWidth;
			
		}
		if (valueYWidth != 0.0f)
		{
			offset.y = ((result.y - mBuildData.MinMaxYValue.x) / valueYWidth) * rectHeight;
		}

		// <->


		ImDrawList* drawList = ImGui::GetWindowDrawList();
		u32 dotColor = ImGui::GetColorU32(ImVec4(1.0f, 0.2f, 0.0f, 1.0f));
		ImVec2 dotSize     = ImVec2(mDotSize, mDotSize);
		ImVec2 dotLocalMin = ImVec2(localRectMin.x, localRectMin.y) + ImVec2(offset.x, offset.y) - (dotSize * 0.5f);
		ImVec2 dotRectMin  = ImVec2(rectMin.x, rectMin.y) + ImVec2(offset.x, offset.y) - (dotSize * 0.5f);
		ImVec2 dotRectMax  = dotRectMin + dotSize;

		ImGui::SetCursorPos(dotLocalMin);
		ImGui::Dummy(dotSize);

		if (ImGui::IsMouseHoveringRect(
			ImVec2(rectMin.x, rectMin.y) - space, 
			ImVec2(rectMax.x, rectMax.y) + space) && ImGui::IsMouseDown(0))
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			ImVec2 valueRatio = mousePos - ImVec2(rectMin.x, rectMin.y);
			valueRatio.x = valueRatio.x / rectWidth;
			valueRatio.y = valueRatio.y / rectHeight;
			result.x = valueRatio.x * valueXWidth;
			result.x += mBuildData.MinMaxXValue.x;

			result.y = valueRatio.y * valueYWidth;
			result.y += mBuildData.MinMaxYValue.x;


		}

		drawList->AddRectFilled(dotRectMin, dotRectMax, dotColor, mDotRounding, ImDrawFlags_RoundCornersAll);


		result.x = Math::Clamp(result.x, mBuildData.MinMaxXValue.x, mBuildData.MinMaxXValue.y);
		result.y = Math::Clamp(result.y, mBuildData.MinMaxYValue.x, mBuildData.MinMaxYValue.y);

		return result;
	}


	void AnimationBlendSpaceView::SetMesh(const String& meshAssetPath)
	{
		mMeshAsset = AssetDataBase::GetInstance().LoadOriginAsset<IMesh>(meshAssetPath);
	}
	void AnimationBlendSpaceView::SetSkeletal(const String& skeletalAssetPath)
	{
		mSkeletoneAsset = AssetDataBase::GetInstance().LoadOriginAsset<Skeletone>(skeletalAssetPath);
	}
	void AnimationBlendSpaceView::SetAnimationBlendSpace(const String& blendSpacePath)
	{
		RefreshAsset(blendSpacePath);
		if (mAnimBlendAsset)
		{
			mModelAssetPath = StorableString("AnimationBlendSpaceView/ModelAssetPath/" + mAnimBlendAsset->GetAssetPath(), "");
			mSkeletoneAssetPath = StorableString("AnimationBlendSpaceView/SkeletoneAssetPath/" + mAnimBlendAsset->GetAssetPath(), "");
			mMaterialAssetPath = StorableString("AnimationBlendSpaceView/MaterialAssetPath/" + mAnimBlendAsset->GetAssetPath(), "");
			SetTitleName(mAnimBlendAsset->GetAssetPath());
		}

	}
	void AnimationBlendSpaceView::SetMaterial(const List<String>& materialAssetPath)
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

	void AnimationBlendSpaceView::UpdateScene()
	{
		// Scene 생성
		static const JVector3 EyePos = JVector3(0, 100.0f, -200.0f);
		if (mEditorUIScene == nullptr)
		{
			EditorUISceneConfig config;
			config.EyePos = EyePos;
			config.Resolution = mSceneResolution;
			config.ImageSize = config.Resolution;
			config.OffsetScale = JVector3(1, 1, 1);
			config.Flags = EEditorUISceneFlags::Fix_RotatePitch;
			config.SkyBox = GraphicsHelper::CreateSkyBox(config.EyePos, config.FarZ, "Asset/Engine/CubeMap/DefaultSky.jgasset");
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
		if (mSkeletoneAsset != nullptr && mSkeletoneAsset->IsValid())
		{
			if (mAnimController != nullptr)
			{
				mAnimController->BindSkeletone(mSkeletoneAsset->Get());
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
	void AnimationBlendSpaceView::UpdateValueParam()
	{

		List<u32> removeList;
		f32 xValueWidth = (mBuildData.MinMaxXValue.y - mBuildData.MinMaxXValue.x);
		f32 yValueWidth = (mBuildData.MinMaxYValue.y - mBuildData.MinMaxYValue.x);

		f32 xOneUnit = xValueWidth / mBuildData.ValueXYNumGrid.x;
		f32 yOneUnit = yValueWidth / mBuildData.ValueXYNumGrid.y;


		for (auto& _pair : mBuildData.AnimClipDatas)
		{
			JVector2Int gridPos = GetGridPos(_pair.first);

			if (gridPos.x >= mBuildData.ValueXYNumGrid.x ||
				gridPos.y >= mBuildData.ValueXYNumGrid.y)
			{
				removeList.push_back(_pair.first);
				continue;
			}
			_pair.second.Value.x = mBuildData.MinMaxXValue.x + xOneUnit * gridPos.x;
			_pair.second.Value.y = mBuildData.MinMaxYValue.x + yOneUnit * gridPos.y;
		}


		for (u32 key : removeList)
		{
			mBuildData.AnimClipDatas.erase(key);
		}

	}
}