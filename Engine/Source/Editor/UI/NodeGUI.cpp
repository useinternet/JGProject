#include "pch.h"
#include "NodeGUI.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
namespace JG
{
	//-- Util Function
	JVector2 ToJVector2(const ImVec2& i)
	{
		return JVector2(i.x, i.y);
	}
	ImVec2 ToImVec2(const JVector2& v)
	{
		return ImVec2(v.x, v.y);
	}
	namespace StateNodeGUI
	{
		class StateNodeDrawer
		{
		public:
			void DrawNode(
				StateNodeEditor* editor, 
				ImDrawList* draw_list,
				const JVector2& location, 
				bool isFlowNode,
				u32 nodeColor, u32 pinAreaColor, u32 outlineColor, u32 flowOutlineColor)
			{
				StateNode::StateNodeStyle& style = editor->GetNodeStyle();
				JVector2 halfRectSize = style.NodeRectSize * 0.5f;
				JVector2 pinAreaRectSize = style.NodeRectSize + JVector2(style.PinArea, style.PinArea);
				JVector2 halfPinAreaRectSize = pinAreaRectSize * 0.5f;


				JVector2 pinAreaMin = editor->GetWindowPos() + editor->GetOffset() + location - halfPinAreaRectSize;
				JVector2 pinAreaMax = pinAreaMin + pinAreaRectSize;

				draw_list->AddRectFilled(ToImVec2(pinAreaMin), ToImVec2(pinAreaMax),
					pinAreaColor, style.NodeRectRounding, ImDrawFlags_RoundCornersAll);



				JVector2 rectMin = editor->GetWindowPos() + editor->GetOffset() + location - halfRectSize;
				JVector2 rectMax = rectMin + style.NodeRectSize;

				draw_list->AddRectFilled(ToImVec2(rectMin), ToImVec2(rectMax), 
					nodeColor, style.NodeRectRounding, ImDrawFlags_RoundCornersAll);

				draw_list->AddRect(ToImVec2(rectMin), ToImVec2(rectMax),
					outlineColor, style.NodeRectRounding, ImDrawFlags_RoundCornersAll, style.NodeOutlineThick);

				if (isFlowNode)
				{
					draw_list->AddRect(ToImVec2(pinAreaMin), ToImVec2(pinAreaMax),
						flowOutlineColor, style.NodeRectRounding, ImDrawFlags_RoundCornersAll, style.NodeOutlineThick);
				}
			}

			void DrawTransition(
				StateNodeEditor* editor,
				ImDrawList* draw_list,
				const JVector2& from, const JVector2& to, 
				u32 lineColor, u32 rectColor, u32 outlineColor,
				JRect* outTransitionNodeRect,
				bool isFlowState,
				bool islinkingLine, 
				bool isFromNodeIsRootNode)
			{
				StateNodeTransition::StateNodeTransitionStyle& style = editor->GetNodeTransitionStyle();
				StateNode::StateNodeStyle& nodeStyle = editor->GetNodeStyle();

				f32 arrowHeight = 0.88f * style.ArrowSize;
				f32 halfArrowSize = style.ArrowSize * 0.5f;
				JVector2 rectHalfSize = style.NodeRectSize * 0.5f;
				f32 dist = JVector2::Length(to - from);

				if (islinkingLine == false && dist <= nodeStyle.NodeRectSize.y  * 2)
				{
					return;
				}
				//	방향
				f32 offsetDist = 0.0f;
				JVector2 dir = JVector2::Normalize(to - from);
				f32 slope = Math::Abs(dir.y) / Math::Max(Math::Abs(dir.x), 0.000001f);

				{
					JVector2 extraRectSize = (nodeStyle.NodeRectSize + JVector2(nodeStyle.PinArea, nodeStyle.PinArea)) * 1.15f;
					f32 theta = std::atanf(slope);
					f32 conTheta = std::atanf(Math::Abs(extraRectSize.y) / Math::Abs(extraRectSize.x));

					if (theta > conTheta)
					{
						f32 halfHeight = extraRectSize.y * 0.5f;
						offsetDist = halfHeight / Math::Max(std::sinf(theta), 0.000001f);
					}
					else
					{
						f32 halfWidth = extraRectSize.x * 0.5f;
						offsetDist = halfWidth / Math::Max(std::cosf(theta), 0.000001f);
					}
				}
				f32 halfdist = dist * 0.5f - offsetDist;
				JVector2 start = from + editor->GetWindowPos() + editor->GetOffset() + (dir * offsetDist);

				JVector2 end = (islinkingLine) ?
					to + editor->GetWindowPos() + editor->GetOffset() :
					to + editor->GetWindowPos() + editor->GetOffset() - (dir * offsetDist);


				/*

				*/

				JVector2 rectCenter = start + (dir * halfdist);
				JVector2 rectMin = rectCenter - rectHalfSize;
				JVector2 rectMax = rectMin + style.NodeRectSize;

				if (islinkingLine == false)
				{
					JVector2 offset;
					f32 offsetDist = JVector2::Length(style.NodeRectSize) * 0.5f;
					offset = JVector2::Normalize(JVector2(-dir.y, dir.x)) * (offsetDist + halfArrowSize + 2.0f);
					rectMin += offset;
					rectMax += offset;
				}


				if (outTransitionNodeRect)
				{
					*outTransitionNodeRect = JRect(
						rectMin - editor->GetWindowPos() - editor->GetOffset(),
						rectMax - editor->GetWindowPos() - editor->GetOffset());
				}


				// 선분 Offset
				if (islinkingLine == false)
				{
					JVector2 offset = JVector2::Normalize(JVector2(-dir.y, dir.x)) * halfArrowSize;
					start += offset;
					end   += offset;
				}

				draw_list->AddLine(ToImVec2(start), ToImVec2(end - (dir* halfArrowSize)), lineColor, (isFlowState) ? style.FlowLineThick : style.LineThick);

				if (islinkingLine == false && isFromNodeIsRootNode == false)
				{
					draw_list->AddRectFilled(ToImVec2(rectMin), ToImVec2(rectMax), rectColor, style.NodeRectRounding, ImDrawFlags_RoundCornersAll);

					draw_list->AddRect(ToImVec2(rectMin), ToImVec2(rectMax), outlineColor, style.NodeRectRounding, ImDrawFlags_RoundCornersAll, style.NodeOutlineThick);
				}
				


				// Draw Arrow
				{
					JVector2 start_arrow_pos = end - (dir * arrowHeight);
					JVector2 p1 = start_arrow_pos + (halfArrowSize * JVector2::Normalize(JVector2(-dir.y, dir.x)));
					JVector2 p2 = start_arrow_pos + (halfArrowSize * JVector2::Normalize(JVector2(dir.y, -dir.x)));
					JVector2 p3 = start_arrow_pos + (dir * arrowHeight);
					draw_list->AddTriangleFilled(ToImVec2(p1), ToImVec2(p2), ToImVec2(p3), lineColor);
				}
			}
		};
		class StateNodeEditorDataStorage
		{
			StateNodeEditor* mNodeEditor = nullptr;
			StateNodeID mSelectedNodeID = 0;
			StateNodeID mLinkingNodeID  = 0;
			StateNodeID mDraggingNodeID = 0;
			StateNodeID mRenamingNodeID = 0;
			HashSet<u64> mFlowStateNodeIDSet;
			u64 mFinalFlowStateNodeID = 0;
		public:
			StateNodeID TempNodeTransitionID = 0;
			JVector2 TempVector;
			String   TempStr;
			bool IsNodeHoveredInOneFrame = false;
		public:
			StateNodeEditorDataStorage() = default;
			StateNodeEditorDataStorage(StateNodeEditor* editor) : mNodeEditor(editor) {}
		public:
			StateNodeID GetSelectedNode() const {
				return mSelectedNodeID;
			}
			bool IsFlowState(StateNodeID from, StateNodeID to)
			{
				u64 seed = std::hash<u64>()(from + 56789) ^ std::hash<u64>()(to + 12345);
				return mFlowStateNodeIDSet.find(seed) != mFlowStateNodeIDSet.end();
			}
			StateNodeID GetFinalFlowStateNode() {
				return mFinalFlowStateNodeID;
			}
			void SetFlowState(const List<StateNodeID>& flowList)
			{
				mFinalFlowStateNodeID = 0;
				mFlowStateNodeIDSet.clear();
				i32 cnt = flowList.size();
				for (i32 i = 0; i < cnt - 1; ++i)
				{
					StateNodeID from = flowList[i];
					StateNodeID to = flowList[i + 1];
					u64 seed = std::hash<u64>()(from + 56789) ^ std::hash<u64>()(to + 12345);

					mFlowStateNodeIDSet.insert(seed);
				}
				if (cnt > 0)
				{
					mFinalFlowStateNodeID = flowList[cnt - 1];
				}
				
			}
			void SetSelectedNode(StateNodeID id) {
				mSelectedNodeID = id;
			}
			void SetLinkingNode(StateNodeID id)
			{
				mLinkingNodeID = id;

				if (id == 0)
				{
					mNodeEditor->RemoveTransition(TempNodeTransitionID);
				}
				else
				{
					TempNodeTransitionID = mNodeEditor->CreateTransition(id, 0);
				}
			}
			StateNodeID GetLinkingNode() const {
				return mLinkingNodeID;
			}
			void SetDraggingNode(StateNodeID id)
			{
				mDraggingNodeID = id;
				StateNode* node = mNodeEditor->FindNode(id);
				if (node != nullptr)
				{
					TempVector = (ToJVector2(ImGui::GetMousePos()) - mNodeEditor->GetWindowPos() - mNodeEditor->GetOffset()) - node->mLocation;
				}
			}
			StateNodeID GetDraggingNode() const {
				return mDraggingNodeID;
			}
			void SetRenamingNode(StateNodeID id)
			{
				StateNode* prev_node = mNodeEditor->FindNode(mRenamingNodeID);
				if (prev_node != nullptr)
				{
					String oldName = prev_node->mName;
					prev_node->mName = TempStr;
					TempStr = "";
					if (mNodeEditor->mReNameCallBack)
					{
						mNodeEditor->mReNameCallBack(id, oldName, prev_node->mName);
					}
				}
				
				mRenamingNodeID = id;
				StateNode* node = mNodeEditor->FindNode(id);
				if (node != nullptr)
				{
					TempStr = node->mName;
					if (node->mFlags & EStateNodeFlags::No_ReName)
					{
						mRenamingNodeID = 0;
					}
				}
			
			}
			StateNodeID GetRenamingNode() const {
				return mRenamingNodeID;
			}
		};


		StateNodeTransition::StateNodeTransition()
		{
			mColors[ColorStyle_NormalOutline] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
			mColors[ColorStyle_HightlightOutline] = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.2f, 0.4f));
			mColors[ColorStyle_NodeBody]      = ImGui::GetColorU32(ImVec4(0.05f, 0.06f, 0.04f, 1.0F));
			mColors[ColorStyle_LineColor]     = ImGui::GetColorU32(ImVec4(1.0F, 1.0F, 1.0F, 0.7f));
			mColors[ColorStyle_FlowLineColor] = ImGui::GetColorU32(ImVec4(1.0F, 1.0F, 0.5F, 1.0f));
		}
		void StateNodeTransition::OnGUI()
		{
			StateNodeDrawer drawer;

			StateNode* from = mNodeEditor->FindNode(mFromID);
			StateNode* to = mNodeEditor->FindNode(mToID);

			if (from == nullptr)
			{
				return;
			}
			StateNodeEditorDataStorage& dataStorage = mNodeEditor->GetDataStorage();
			if (to != nullptr)
			{
				bool isFlow = dataStorage.IsFlowState(mFromID, mToID);

				JRect transitionNodeRect;
				drawer.DrawTransition(
					mNodeEditor,
					ImGui::GetWindowDrawList(),
					from->GetLocation(),
					to->GetLocation(),
					isFlow ? GetColor(ColorStyle_FlowLineColor) : GetColor(ColorStyle_LineColor),
					GetColor(ColorStyle_NodeBody),
					dataStorage.GetSelectedNode() == GetID() ? 
						GetColor(ColorStyle_HightlightOutline) : 
						GetColor(ColorStyle_NormalOutline), 
					&transitionNodeRect,
					isFlow,
					false, 
					from->GetFlags() & EStateNodeFlags::RootNode);

				if (transitionNodeRect.Width() > 0 && transitionNodeRect.Height() > 0 && (from->GetFlags() & EStateNodeFlags::RootNode) == false)
				{
					JVector2 cursorPos = mNodeEditor->GetOffset() + JVector2(transitionNodeRect.left, transitionNodeRect.top);
					ImGui::SetCursorPos(ToImVec2(cursorPos));
					ImGui::Dummy(ImVec2(transitionNodeRect.Width(), transitionNodeRect.Height()));
					if (ImGui::IsItemClicked())
					{
						dataStorage.SetSelectedNode(GetID());
					}
					if (ImGui::IsItemHovered()) dataStorage.IsNodeHoveredInOneFrame = true;
				}
			}
			else
			{
				JVector2 localMousePos = ToJVector2(ImGui::GetMousePos()) - mNodeEditor->GetWindowPos() - mNodeEditor->GetOffset();
				drawer.DrawTransition(
					mNodeEditor,
					ImGui::GetWindowDrawList(),
					from->GetLocation(),
					localMousePos,
					GetColor(ColorStyle_LineColor),
					GetColor(ColorStyle_NodeBody),
					GetColor(ColorStyle_NormalOutline), 
					nullptr, false, true, from->GetFlags() & EStateNodeFlags::RootNode);
			}
		}
		u32 StateNodeTransition::GetColor(EColorStyle style)
		{
			return mColors[style];
		}

		StateNode::StateNode()
		{
			mColors[ColorStyle_NormalOutline] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
			mColors[ColorStyle_HightlightOutline] = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.2f, 0.4f));
			mColors[ColorStyle_NodeBody] = ImGui::GetColorU32(ImVec4(0.05f, 0.06f, 0.04f, 0.8F));
			mColors[ColorStyle_NodePinArea] = ImGui::GetColorU32(ImVec4(0.5f, 0.51f, 0.52f, 1.0f));
			mColors[ColorStyle_FlowHighlight] = ImGui::GetColorU32(ImVec4(1, 1, 0.5f, 1.0f));
		}
		List<StateNodeID> StateNode::GetTransitionList() const
		{
			List<StateNodeID> result;
			for (auto _pair : mOutLinkedTransitionDic)
			{
				result.push_back(_pair.second);
			}
			return result;
		}
		StateNodeID StateNode::GetTransition(StateNodeID id)
		{
			if (mOutLinkedTransitionDic.find(id) == mOutLinkedTransitionDic.end())
			{
				return 0;
			}
			return mOutLinkedTransitionDic[id];
		}
		void StateNode::OnGUI()
		{
			StateNodeEditorDataStorage& dataStorage = mNodeEditor->GetDataStorage();
			StateNode::StateNodeStyle& style = mNodeEditor->GetNodeStyle();
			JVector2 halfRectSize    = style.NodeRectSize * 0.5f;
			JVector2 pinAreaRectSize = style.NodeRectSize + JVector2(style.PinArea, style.PinArea);
			JVector2 halfPinAreaRectSize = pinAreaRectSize * 0.5f;
			JVector2 mousePos = ToJVector2(ImGui::GetMousePos());
			JVector2 winPos = mNodeEditor->GetWindowPos();
			JVector2 cursorPos;
			bool isClickPinArea  = false;
			bool isClickNodeBody = false;
			bool isF2Pressed = ImGui::IsKeyPressed((i32)EKeyCode::F2);





			// NodeBody
			cursorPos = mNodeEditor->GetOffset() + mLocation - halfRectSize;
			ImGui::SetCursorPos(ToImVec2(cursorPos));
			ImGui::Dummy(ToImVec2(style.NodeRectSize));
			isClickNodeBody = ImGui::IsItemClicked();
			if (ImGui::IsItemHovered()) dataStorage.IsNodeHoveredInOneFrame = true;

			// PinArea
			cursorPos = mNodeEditor->GetOffset() + mLocation - halfPinAreaRectSize;
			ImGui::SetCursorPos(ToImVec2(cursorPos));
			ImGui::Dummy(ToImVec2(pinAreaRectSize));
			isClickPinArea = ImGui::IsItemClicked() && isClickNodeBody == false;
			if (ImGui::IsItemHovered()) dataStorage.IsNodeHoveredInOneFrame = true;







			StateNodeDrawer drawer;
			drawer.DrawNode(
				mNodeEditor,
				ImGui::GetWindowDrawList(),
				mLocation,
				dataStorage.GetFinalFlowStateNode() == GetID(),
				GetColor(ColorStyle_NodeBody),
				GetColor(ColorStyle_NodePinArea),
				dataStorage.GetSelectedNode() == GetID() ? GetColor(ColorStyle_HightlightOutline) : GetColor(ColorStyle_NormalOutline),
				GetColor(ColorStyle_FlowHighlight));



			if (GetID() != dataStorage.GetRenamingNode() && mName.empty() == false)
			{
				JVector2 textSize = ToJVector2(ImGui::CalcTextSize(mName.c_str()));
				JVector2 halfTextSize = textSize * 0.5f;
				cursorPos = mNodeEditor->GetOffset() + mLocation - halfTextSize;
				ImGui::SetCursorPos(ToImVec2(cursorPos));
				ImGui::Text(mName.c_str());
			}


			if (isClickNodeBody)
			{
				dataStorage.SetDraggingNode(GetID());
				dataStorage.SetSelectedNode(GetID());
			}
			if (isF2Pressed && dataStorage.GetSelectedNode() == GetID())
			{
				dataStorage.SetRenamingNode(GetID());
			}
			if (isClickPinArea)
			{
				if (dataStorage.GetLinkingNode() > 0 && dataStorage.GetLinkingNode() != GetID())
				{
					mNodeEditor->Link(dataStorage.GetLinkingNode(), GetID());
					dataStorage.SetLinkingNode(0);
				}
				else
				{
					dataStorage.SetLinkingNode(GetID());
				}
			}
		}

		void StateNode::AddFlags(EStateNodeFlags flags)
		{
			mFlags = mFlags | flags;
		}
		u32 StateNode::GetColor(EColorStyle style)
		{
			return mColors[style];
		}


		void StateNodeBuilder::SetNodeColor(const Color& color)
		{
			mNode.mColors[StateNode::ColorStyle_NodeBody] = ImGui::GetColorU32(ImVec4(color.R, color.G, color.B, color.A));
		}
		void StateNodeBuilder::SetNodePinAreaColor(const Color& color)
		{
			mNode.mColors[StateNode::ColorStyle_NodePinArea] = ImGui::GetColorU32(ImVec4(color.R, color.G, color.B, color.A));
		}
		void StateNodeBuilder::SetName(const String& name)
		{
			mNode.mName = name;
		}
		void StateNodeBuilder::SetInitLocation(const JVector2& location)
		{
			mNode.mLocation = location;
		}
		void StateNodeBuilder::SetNodeFlags(EStateNodeFlags flags)
		{
			mNode.mFlags = flags;
		}
		void StateNodeBuilder::SetContextMenuFunc(const std::function<void(StateNode*)>& createFunc, const std::function<void(StateNode*)>& showFunc)
		{
			mNode.mCreateContextMenuFunc = createFunc;
			mNode.mShowContextMenuFunc   = showFunc;
		}

		StateNodeEditor::StateNodeEditor()
		{
			mDataStorage = CreateSharedPtr<StateNodeEditorDataStorage>(this);
			
		}
		StateNodeID StateNodeEditor::GetSelectedNodeID() const
		{
			return mDataStorage->GetSelectedNode();
		}
		StateNodeID StateNodeEditor::CreateNode(const StateNodeBuilder& builder)
		{
			StateNode newNode = builder.mNode;
			newNode.mID = ++mIDOffset;
			newNode.mNodeEditor = this;
			mNodeDic[newNode.GetID()] = newNode;
			if (mCreateNodeCallBack)
			{
				mCreateNodeCallBack(newNode.mID);
			}
			return newNode.mID;
		}

		void StateNodeEditor::RemoveNode(StateNodeID id)
		{
			StateNode* node = FindNode(id);
			if (node == nullptr || node->mFlags & EStateNodeFlags::No_Remove)
			{
				return;
			}
			if (mDataStorage->GetSelectedNode() == id)
			{
				mDataStorage->SetSelectedNode(0);
			}
			if (mDataStorage->GetLinkingNode() == id)
			{
				mDataStorage->SetLinkingNode(0);
			}
			// 내보내는 링크 노드 연결 해제
			HashSet<StateNodeID> outLinkedNodeIDs = node->mOutLinkedNodeIDs;
			for (StateNodeID outLinkedID : outLinkedNodeIDs)
			{
				UnLink(node->GetID(), outLinkedID);
			}
			mNodeDic.erase(id);

			if (mRemoveNodeCallBack)
			{
				mRemoveNodeCallBack(id);
			}
		}
		StateNode* StateNodeEditor::FindNode(StateNodeID id)
		{
			if (mNodeDic.find(id) == mNodeDic.end())
			{
				return nullptr;
			}
			return &mNodeDic[id];
		}
		StateNodeTransition* StateNodeEditor::FindNodeTransition(StateNodeID id)
		{
			if (mNodeTransitionDic.find(id) == mNodeTransitionDic.end())
			{
				return nullptr;
			}
			return &mNodeTransitionDic[id];
		}

		void StateNodeEditor::Flow(const List<StateNodeID>& flowList)
		{
			mDataStorage->SetFlowState(flowList);
		}


		void StateNodeEditor::Link(StateNodeID fromID, StateNodeID toID)
		{
			StateNode* from = FindNode(fromID);
			StateNode* to = FindNode(toID);
			if (from == nullptr || to == nullptr || from == to)
			{
				return;
			}
			if (to->mFlags & EStateNodeFlags::RootNode)
			{
				mDataStorage->SetLinkingNode(0);
				return;
			}
			if (from->mFlags & EStateNodeFlags::RootNode)
			{
				HashSet<StateNodeID> outlinkedNodeIDs = from->mOutLinkedNodeIDs;
				for (StateNodeID id : outlinkedNodeIDs)
				{
					UnLink(from->GetID(), id);
				}
			}

			from->mOutLinkedNodeIDs.insert(toID);
			mDataStorage->SetLinkingNode(0);

			from->mOutLinkedTransitionDic[toID] = CreateTransition(fromID, toID);
	
			

			if (mLinkNodeCallBack)
			{
				mLinkNodeCallBack(fromID, toID, from->mOutLinkedTransitionDic[toID]);
			}
		}
		void StateNodeEditor::UnLink(StateNodeID fromID, StateNodeID toID)
		{
			StateNode* from = FindNode(fromID);
			if (from == nullptr)
			{
				return;
			}
			StateNode* to = FindNode(toID);
			if (to == nullptr)
			{
				return;
			}
			if (mUnLinkNodeCallBack)
			{
				mUnLinkNodeCallBack(fromID, toID, from->mOutLinkedTransitionDic[toID]);
			}
			from->mOutLinkedNodeIDs.erase(toID);
			RemoveTransition(from->mOutLinkedTransitionDic[toID]);
			from->mOutLinkedTransitionDic.erase(toID);
		}

		void StateNodeEditor::BindContextMenuFunc(const std::function<void()>& func)
		{
			mContextMenuFunc = func;
		}
		void StateNodeEditor::BindCreateNodeCallBack(const std::function<void(StateNodeID)>& callBack)
		{
			mCreateNodeCallBack = callBack;
		}
		void StateNodeEditor::BindDraggingNodeCallBack(const std::function<void(StateNodeID)>& callBack)
		{
			mDraggingNodeCallBack = callBack;
		}
		void StateNodeEditor::BindRemoveNodeCallBack(const std::function<void(StateNodeID)>& callBack)
		{
			mRemoveNodeCallBack = callBack;
		}
		void StateNodeEditor::BindRemoveTransitionCallBack(const std::function<void(StateNodeID)>& callBack)
		{
			mRemoveTransitionCallBack = callBack;
		}
		void StateNodeEditor::BindLinkNodeCallBack(const std::function<void(StateNodeID, StateNodeID, StateNodeID)>& callBack)
		{
			mLinkNodeCallBack = callBack;
		}
		void StateNodeEditor::BindUnLinkCallBack(const std::function<void(StateNodeID, StateNodeID, StateNodeID)>& callBack)
		{
			mUnLinkNodeCallBack = callBack;
		}
		void StateNodeEditor::BindReNameCallBack(const std::function<void(StateNodeID, const String&, const String&)>& callBack)
		{
			mReNameCallBack = callBack;
		}
		void StateNodeEditor::OnGUI(const JVector2& size)
		{
			mDataStorage->IsNodeHoveredInOneFrame = false;
			ImGui::BeginChild((ImGuiID)this, ImVec2(size.x, size.y), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);


			mWindowPos = ToJVector2(ImGui::GetWindowPos());
			mWindowSize = ToJVector2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
			ImVec2 cursorPos = ImGui::GetCursorPos();

			for (auto& _pair : mNodeTransitionDic)
			{
				_pair.second.OnGUI();
			}
			for (auto& _pair : mNodeDic)
			{
				_pair.second.OnGUI();
			}
			ImGui::SetCursorPos(cursorPos);
			UpdateNodeInteraction();
			if (ImGui::IsKeyPressed((int)EKeyCode::Del))
			{
				RemoveNode(mDataStorage->GetSelectedNode());
			}
			if (ImGui::IsMouseDown(2) == true && ImGui::IsWindowHovered())
			{
				mOffset += ToJVector2(ImGui::GetIO().MouseDelta);
			}
			if (mDataStorage->IsNodeHoveredInOneFrame == false)
			{
				if (mContextMenuFunc)
				{
					mContextMenuFunc();
				}
			}

			ImGui::EndChild();
		}

		StateNodeID StateNodeEditor::CreateTransition(StateNodeID fromID, StateNodeID toID)
		{
			if (FindNode(fromID) == nullptr)
			{
				return 0;
			}


			StateNodeTransition transition;
			transition.mID     = ++mIDOffset;
			transition.mFromID = fromID;
			transition.mToID   = toID;
			transition.mNodeEditor = this;

			mNodeTransitionDic[transition.mID] = transition;
			return transition.mID;
		}
		void StateNodeEditor::RemoveTransition(StateNodeID id)
		{
			StateNodeTransition* transition = FindNodeTransition(id);
			if (transition == nullptr)
			{
				return;
			}
			mNodeTransitionDic.erase(id);

			if (mRemoveTransitionCallBack)
			{
				mRemoveTransitionCallBack(id);
			}
		}
		void StateNodeEditor::UpdateNodeInteraction()
		{
			if (mDataStorage->IsNodeHoveredInOneFrame == false)
			{

				if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
				{
					if (mDataStorage->GetLinkingNode() > 0)
					{
						mDataStorage->SetLinkingNode(0);
					}
					else
					{
						mDataStorage->SetSelectedNode(0);
						mDataStorage->SetRenamingNode(0);
					}
				}
				
			}
			
			JVector2 mousePos = ToJVector2(ImGui::GetMousePos());
			JVector2 winPos = GetWindowPos();



			StateNode* renameingNode = FindNode(mDataStorage->GetRenamingNode());
			if (renameingNode != nullptr)
			{
				ImVec2 framePadding = ImGui::GetStyle().FramePadding;
				StateNode::StateNodeStyle& style = GetNodeStyle();
				f32 halfFontHeight = ImGui::GetFontSize() * 0.5f;
				JVector2 rectSize = style.NodeRectSize;
				JVector2 halfRectSize = style.NodeRectSize * 0.5f;
				ImGui::SetNextItemWidth(rectSize.x - (framePadding.x * 2) );
				JVector2 cursorPos = GetOffset() + renameingNode->mLocation - JVector2(halfRectSize.x - framePadding.x, halfFontHeight + ImGui::GetStyle().FramePadding.y);
				ImGui::SetCursorPos(ToImVec2(cursorPos));
				mDataStorage->TempStr.resize(512);
				
				ImGui::InputText("##ReName_InputText", mDataStorage->TempStr.data(), 512);
				if (ImGui::IsItemHovered())
				{
					mDataStorage->SetDraggingNode(0);
				}
				if (ImGui::IsItemDeactivated())
				{
					mDataStorage->SetRenamingNode(0);
				}
			}


			StateNode* draggingNode = FindNode(mDataStorage->GetDraggingNode());
			if (draggingNode != nullptr)
			{
				// State
				if (ImGui::IsMouseDown(0))
				{
					draggingNode->mLocation = (mousePos - winPos - GetOffset()) - mDataStorage->TempVector;
					if (mDraggingNodeCallBack)
					{
						mDraggingNodeCallBack(draggingNode->GetID());
					}
				}
				else mDataStorage->SetDraggingNode(0);
			}
		}
		StateNodeLinkInfo StateNodeEditor::GetNodeLinkInfo() const
		{
			StateNodeLinkInfo info;
			for (const std::pair<StateNodeID, StateNode>& _pair : mNodeDic)
			{
				StateNodeID id		  = _pair.first;
				const StateNode& node = _pair.second;

				List<StateNodeID> linkedNodeIDList;


				if (node.GetFlags() & EStateNodeFlags::RootNode)
				{
					info.RootNodeID = id;
				}
				
				for (StateNodeID linkedNodeID : node.mOutLinkedNodeIDs)
				{
					linkedNodeIDList.push_back(linkedNodeID);
				}
				

				info.NodeIDList.push_back(id);
				info.LinkedNodeIDLists.push_back(linkedNodeIDList);
			}

			return info;
		}
	}
}


