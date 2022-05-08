#include "pch.h"
#include "NodeGUI.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
namespace JG
{
	JVector2 ToJVector2(const ImVec2& i)
	{
		return JVector2(i.x, i.y);
	}
	ImVec2 ToImVec2(const JVector2& v)
	{
		return ImVec2(v.x, v.y);
	}
	namespace NodeGUI
	{
#pragma region NodeEditorDataStorage
		class NodeEditorDataStorage
		{
			NodeEditor* mNodeEditor = nullptr;
			NodeID mSelectedNodeID = 0;

			NodeID mLinkingNodeID = 0;
			u8     mLinkingMethod = 0; // In 0, Out 1

			NodeID mDraggingNodeID = 0;
			NodeID mRenamingNodeID = 0;
		public:
			String TempStr;
			bool IsNodeHoveredInOneFrame = false;
		public:
			NodeEditorDataStorage() = default;
			NodeEditorDataStorage(NodeEditor* editor) : mNodeEditor(editor) {}
		public:
			NodeID GetSelectedNode() const {
				return mSelectedNodeID;
			}
			void SetSelectedNode(NodeID id) {
				mSelectedNodeID = id;
			}
			void NodeEditorDataStorage::SetLinkingNode(NodeID id, u8 method)
			{

				Node* prev_node = mNodeEditor->FindNode(mLinkingNodeID);
				u8    prev_method = mLinkingMethod;
				if (prev_node != nullptr)
				{
					switch (prev_method)
					{
					case Node::LinkingMethod_In:
						prev_node->RemoveState(ENodeState::InPinDragging);
						break;
					case Node::LinkingMethod_Out:
						prev_node->RemoveState(ENodeState::OutPinDragging);
						break;
					}
				}

				mLinkingNodeID = id;
				mLinkingMethod = method;

				Node* node = mNodeEditor->FindNode(id);
				if (node != nullptr)
				{
					switch (method)
					{
					case Node::LinkingMethod_In:
						node->AddState(ENodeState::InPinDragging);
						break;
					case Node::LinkingMethod_Out:
						node->AddState(ENodeState::OutPinDragging);
						break;
					}
				}
			}
			NodeID GetLinkingNode() const {
				return mLinkingNodeID;
			}
			u8 GetLinkingMethod() const {
				return mLinkingMethod;
			}
			void SetDraggingNode(NodeID id)
			{
				Node* prev_node = mNodeEditor->FindNode(mDraggingNodeID);
				if (prev_node != nullptr)
				{
					prev_node->RemoveState(ENodeState::Dragging);
				}
				mDraggingNodeID = id;
				Node* node = mNodeEditor->FindNode(mDraggingNodeID);
				if (node != nullptr)
				{
					node->AddState(ENodeState::Dragging);
				}
			}
			void SetRenamingNode(NodeID id)
			{
				Node* prev_node = mNodeEditor->FindNode(mRenamingNodeID);
				if (prev_node != nullptr)
				{
					prev_node->mHeaderName = TempStr;
					prev_node->RemoveState(ENodeState::ReNaming);
				}
				mRenamingNodeID = id;
				Node* node = mNodeEditor->FindNode(mRenamingNodeID);
				if (node != nullptr && (node->mFlags & ENodeFlags::No_ReName) == false)
				{
					node->AddState(ENodeState::ReNaming);
				}
			}
		};
#pragma endregion

		Node::Node()
		{
			mColors[ColorStyle_NodeHeader] = ImGui::GetColorU32(ImVec4(1.0f, 0.32f, 0.24f, 0.5f));
			mColors[ColorStyle_NodeBody] = ImGui::GetColorU32(ImVec4(0.05f, 0.06f, 0.04f, 1.0f));
			mColors[ColorStyle_NormalOutline] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
			mColors[ColorStyle_HightlightOutline] = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.2f, 0.4f));
			mColors[ColorStyle_LinkPin] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
			mColors[ColorStyle_LinkLine] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
		}

		void Node::DrawLinks()
		{
			JVector2 offset = mNodeEditor->GetOffset();
			JVector2 winPos = mNodeEditor->GetWindowPos();
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			f32 linkLineThick = nodeStyle.LinkLineThick;
			JVector2 mousePos = ToJVector2(ImGui::GetMousePos()) - offset;

			ImDrawList* drawList = ImGui::GetWindowDrawList();
#pragma  region Draw_LinkLine

			if (mState & ENodeState::InPinDragging)
			{
				JVector2 inLinkPinCenter = GetLinkInPinLocation(winPos);

				DrawLine(0, drawList, inLinkPinCenter, mousePos, -1.0f, linkLineThick);
			}
			if (mState & ENodeState::OutPinDragging)
			{
				JVector2 outLinkPinCenter = GetLinkOutPinLocation(winPos);
				DrawLine(0, drawList, outLinkPinCenter, mousePos, 1.0f, linkLineThick);
			}
			for (NodeID outLinkNodeID : mOutLinkedNodeIDs)
			{
				Node* outLinkNode = mNodeEditor->FindNode(outLinkNodeID);
				if (outLinkNode != nullptr)
				{
					DrawLine(outLinkNodeID, drawList, GetLinkOutPinLocation(winPos), outLinkNode->GetLinkInPinLocation(winPos), 1.0f, linkLineThick);
				}
			}
#pragma  endregion
		}

		void Node::Draw()
		{
			Draw_Node();
		}

		void Node::Draw_Node()
		{
#pragma region Define_Node_Var
			JVector2 offset = mNodeEditor->GetOffset();
			JVector2 winPos = mNodeEditor->GetWindowPos();
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			NodeEditorDataStorage& dataStorage = mNodeEditor->GetEditorDataStorage();
			JVector2 framePadding = ToJVector2(ImGui::GetStyle().FramePadding);
			JVector2 nodeRectSize = nodeStyle.NodeRectSize;
			JVector2 nodeRectHalfSize = nodeRectSize * 0.5f;
			f32 fontSize = ImGui::GetFontSize();
			f32 rectRounding = nodeStyle.NodeRectRounding;

			f32 nodeOutlineThick = nodeStyle.NodeOutlineThick;
			f32 nodeHeaderHeight = nodeStyle.HeaderHeight;
			f32 linkPinRectRounding = nodeStyle.LinkPinRectRounding;
			f32 linkPinRectOutlineThick = nodeStyle.LinkPinRectOutlineThick;
			f32 linkPinRectSize = nodeStyle.LinkPinRectSize;
			f32 linkPinRectHalfSize = linkPinRectSize * 0.5f;
			f32 linkLineThick = nodeStyle.LinkLineThick;
			JVector2 pinFramePadding = JVector2(nodeOutlineThick, nodeOutlineThick) + JVector2(3.0f, nodeHeaderHeight + 6.0f);
			JVector2 startNodeLocation = offset + winPos + mLocation;
			JVector2 nodeRectMin = startNodeLocation - nodeRectHalfSize;
			JVector2 nodeRectMax = nodeRectMin + nodeRectSize;
			JVector2 mousePos = ToJVector2(ImGui::GetMousePos()) - offset;

			ImGui::SetCursorPos(ToImVec2(offset + (mLocation - nodeRectHalfSize)));
			ImGui::Dummy(ImVec2(nodeRectSize.x, nodeRectSize.y));
			if (ImGui::IsItemHovered()) dataStorage.IsNodeHoveredInOneFrame = true;
			bool isNodeClicked = ImGui::IsItemClicked();
			bool isNodeDragging = ImGui::IsItemHovered() && ImGui::IsMouseDown(0);
			bool isAltPresseed = ImGui::GetIO().KeyAlt;
			bool isF2Pressed = ImGui::IsKeyPressed((int)EKeyCode::F2);
			bool isLinkedInPinClicked = false;
			bool isLinkedOutPinClicked = false;



			ImDrawList* drawList = ImGui::GetWindowDrawList();
#pragma endregion
#pragma region Draw_Node
			// [ Node Body Draw ]
			drawList->AddRectFilled(ImVec2(nodeRectMin.x, nodeRectMin.y), ImVec2(nodeRectMax.x, nodeRectMax.y),
				GetColor(ColorStyle_NodeBody), rectRounding, ImDrawFlags_RoundCornersAll);


			// [ Node Header Draw ]
			JVector2 nodeHeaderRectMin = startNodeLocation - nodeRectHalfSize;
			JVector2 nodeHeaderRectMax = nodeRectMin + JVector2(nodeRectSize.x, nodeHeaderHeight);

			drawList->AddRectFilled(ImVec2(nodeHeaderRectMin.x, nodeHeaderRectMin.y), ImVec2(nodeHeaderRectMax.x, nodeHeaderRectMax.y),
				GetColor(ColorStyle_NodeHeader), rectRounding, ImDrawFlags_RoundCornersTop);


			// [ Node Outline Draw ]
			drawList->AddRect(
				ImVec2(nodeRectMin.x, nodeRectMin.y), ImVec2(nodeRectMax.x, nodeRectMax.y),
				GetID() == dataStorage.GetSelectedNode() ? GetColor(ColorStyle_HightlightOutline) : GetColor(ColorStyle_NormalOutline),
				rectRounding, ImDrawFlags_RoundCornersAll, nodeOutlineThick);
#pragma endregion
#pragma region Draw_Node_Pin

			JVector2 pinStartLocation = nodeRectMin;
			pinStartLocation += pinFramePadding;
			if (mFlags & ENodeFlags::LinkInPin)
			{
				JVector2 rectMin = pinStartLocation;
				JVector2 rectMax = pinStartLocation + JVector2(linkPinRectSize, linkPinRectSize);
				if (mInLinkedNodeIDs.empty() == false || (mState & ENodeState::InPinDragging))
				{
					drawList->AddRectFilled(ToImVec2(rectMin), ToImVec2(rectMax), GetColor(ColorStyle_LinkPin),
						rectRounding, ImDrawFlags_RoundCornersAll);
				}
				else
				{
					drawList->AddRect(ToImVec2(rectMin), ToImVec2(rectMax), GetColor(ColorStyle_LinkPin),
						rectRounding, ImDrawFlags_RoundCornersAll, linkPinRectOutlineThick);
				}

				ImGui::SetCursorPos(ToImVec2(rectMin - winPos));
				ImGui::Dummy(ImVec2(linkPinRectSize, linkPinRectSize));
				if (ImGui::IsItemHovered()) dataStorage.IsNodeHoveredInOneFrame = true;
				isLinkedInPinClicked = ImGui::IsItemClicked();
			}

			pinStartLocation = nodeRectMin + JVector2(nodeRectSize.x, 0.0f);
			pinStartLocation += JVector2(-pinFramePadding.x, pinFramePadding.y);
			if (mFlags & ENodeFlags::LinkOutPin)
			{
				JVector2 outLinkPinCenter = pinStartLocation + JVector2(-linkPinRectHalfSize, linkPinRectHalfSize);
				JVector2 rectMin = pinStartLocation + JVector2(-linkPinRectSize, 0.0f);
				JVector2 rectMax = rectMin + JVector2(linkPinRectSize, linkPinRectSize);
				if (mOutLinkedNodeIDs.empty() == false || (mState & ENodeState::OutPinDragging))
				{
					drawList->AddRectFilled(ToImVec2(rectMin), ToImVec2(rectMax), GetColor(ColorStyle_LinkPin),
						linkPinRectRounding, ImDrawFlags_RoundCornersAll);
				}
				else
				{
					drawList->AddRect(ToImVec2(rectMin), ToImVec2(rectMax), GetColor(ColorStyle_LinkPin),
						linkPinRectRounding, ImDrawFlags_RoundCornersAll, linkPinRectOutlineThick);
				}
				ImGui::SetCursorPos(ToImVec2(rectMin - winPos));
				ImGui::Dummy(ImVec2(linkPinRectSize, linkPinRectSize));
				if (ImGui::IsItemHovered()) dataStorage.IsNodeHoveredInOneFrame = true;
				isLinkedOutPinClicked = ImGui::IsItemClicked();
			}
#pragma  endregion
#pragma region Draw Node Element
			// Header ReName

			ImGui::PushID(GetID());
			if (mState & ENodeState::ReNaming)
			{
				ImGui::SetCursorPos(ToImVec2(nodeHeaderRectMin - winPos + JVector2(framePadding.x, 0.0f)));
				ImGui::SetNextItemWidth(nodeRectSize.x);
				ImGui::InputText(("##" + mHeaderName).c_str(), dataStorage.TempStr.data(), 512);
				if (ImGui::IsItemHovered())
				{
					isNodeClicked = false;
				}
				if (ImGui::IsItemDeactivated() == true)
				{
					dataStorage.SetRenamingNode(0);
				}
			}
			else
			{
				f32 headerTextPaddingY = (nodeHeaderHeight - fontSize) * 0.5f + framePadding.y;
				ImGui::SetCursorPos(ToImVec2(nodeHeaderRectMin - winPos + JVector2(framePadding.x, headerTextPaddingY)));
				ImGui::Text(mHeaderName.c_str());
			}
			ImGui::PopID();


#pragma  endregion
#pragma region Node_Interaction
			if (isLinkedInPinClicked)
			{
				NodeID linkingNodeID = dataStorage.GetLinkingNode();
				Node* linkingNode = mNodeEditor->FindNode(linkingNodeID);
				if (linkingNode != nullptr && linkingNodeID != mID)
				{
					u8 linkingMethod = dataStorage.GetLinkingMethod();
					if (linkingMethod == LinkingMethod_Out)
					{
						mNodeEditor->Link(linkingNodeID, mID);
					}
					else isLinkedInPinClicked = false;
				}
				else if (linkingNodeID == mID)
				{
					isLinkedInPinClicked = false;
				}
				else
				{
					if (isAltPresseed == true)
					{
						HashSet<NodeID> inLinkedNodeIDs = mInLinkedNodeIDs;
						for (NodeID inLinkID : inLinkedNodeIDs)
						{
							mNodeEditor->UnLink(inLinkID, GetID());
						}
					}
					else
					{
						dataStorage.SetLinkingNode(GetID(), LinkingMethod_In);
					}
				}
			}
			if (isLinkedOutPinClicked)
			{
				NodeID linkingNodeID = dataStorage.GetLinkingNode();
				Node* linkingNode = mNodeEditor->FindNode(linkingNodeID);

				// 
				if (linkingNode != nullptr && linkingNodeID != mID)
				{
					u8 linkingMethod = dataStorage.GetLinkingMethod();
					if (linkingMethod == LinkingMethod_In)
						mNodeEditor->Link(mID, linkingNodeID);
					else
						isLinkedOutPinClicked = false;
				}
				else if (linkingNodeID == mID)
				{
					isLinkedOutPinClicked = false;
				}
				else
				{
					isAltPresseed == true ?
						mNodeEditor->UnLink(GetID(), 0) :
						dataStorage.SetLinkingNode(GetID(), LinkingMethod_Out);
				}
			}


			if (isNodeClicked && isLinkedInPinClicked == false && isLinkedOutPinClicked == false)
			{
				mClickedMouseLocalPos = mousePos - winPos - mLocation;
				dataStorage.SetDraggingNode(GetID());
				dataStorage.SetSelectedNode(GetID());
			}

			if (mState & ENodeState::Dragging && ImGui::IsMouseDown(0))
			{
				mLocation = mousePos - winPos - mClickedMouseLocalPos;
			}
			else RemoveState(ENodeState::Dragging);
			if (isF2Pressed && dataStorage.GetSelectedNode() == GetID())
			{
				dataStorage.SetRenamingNode(GetID());
				dataStorage.TempStr = mHeaderName;
				dataStorage.TempStr.resize(512);
			}
#pragma endregion
		}
		void Node::DrawLine(NodeID toID, ImDrawList* draw_list, const JVector2& from, const JVector2& to, f32 factor, f32 thick)
		{
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			NodeEditorDataStorage& dataStorage = mNodeEditor->GetEditorDataStorage();
			JVector2 offset = mNodeEditor->GetOffset();

			f32 scale = JVector2::LengthSqrd(from - to);
			scale /= 10000.0f;


			f32 tx = Math::Clamp<f32>(320.0f * scale * factor, -320, 320);
			ImVec2 p1 = ToImVec2(offset + from);

			ImVec2 t1 = ImVec2(tx, 0.0f);
			ImVec2 p2 = ToImVec2(offset + to);
			ImVec2 t2 = ImVec2(tx, 0.0f);

			ImVec2 intermediateNodePos;
			const int STEPS = 36;
			for (int step = 0; step <= STEPS; step++)
			{
				float t = (float)step / (float)STEPS;
				float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
				float h2 = -2 * t * t * t + 3 * t * t;
				float h3 = t * t * t - 2 * t * t + t;
				float h4 = t * t * t - t * t;
				ImVec2 p = ImVec2(h1 * p1.x + h2 * p2.x + h3 * t1.x + h4 * t2.x,
					h1 * p1.y + h2 * p2.y + h3 * t1.y + h4 * t2.y);
				draw_list->PathLineTo(p);
				if (step == STEPS / 2)
				{
					intermediateNodePos = p;
				}
			}

			draw_list->PathStroke(GetColor(ColorStyle_LinkLine), false, thick);
		}

		u32 Node::GetColor(EColorStyle style)
		{
			return mColors[style];
		}
		void Node::SetColor(EColorStyle style, u32 color)
		{
			mColors[style] = color;
		}
		void Node::AddState(ENodeState state)
		{
			mState = mState | state;
		}
		void Node::AddFlags(ENodeFlags flags)
		{
			mFlags = mFlags | flags;
		}
		void Node::RemoveState(ENodeState state)
		{
			mState = (ENodeState)((i32)mState & (~(i32)state));
		}

		JVector2 Node::GetLinkInPinLocation(const JVector2& winPos) const
		{
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			JVector2 nodeRectSize = nodeStyle.NodeRectSize;
			JVector2 nodeRectHalfSize = nodeRectSize * 0.5f;
			JVector2 startNodeLocation = winPos + mLocation;
			JVector2 nodeRectMin = startNodeLocation - nodeRectHalfSize;
			JVector2 nodeRectMax = nodeRectMin + nodeRectSize;
			f32 linkPinRectSize = nodeStyle.LinkPinRectSize;
			f32 linkPinRectHalfSize = linkPinRectSize * 0.5f;
			JVector2 pinFramePadding = GetPinFramePadding();
			JVector2 inLinkPinCenter = nodeRectMin + pinFramePadding + JVector2(linkPinRectHalfSize, linkPinRectHalfSize);
			return inLinkPinCenter;
		}

		JVector2 Node::GetLinkOutPinLocation(const JVector2& winPos) const
		{
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			JVector2 nodeRectSize = nodeStyle.NodeRectSize;
			JVector2 nodeRectHalfSize = nodeRectSize * 0.5f;
			JVector2 startNodeLocation = winPos + mLocation;
			JVector2 nodeRectMin = startNodeLocation - nodeRectHalfSize;
			f32 linkPinRectSize = nodeStyle.LinkPinRectSize;
			f32 linkPinRectHalfSize = linkPinRectSize * 0.5f;
			JVector2 pinFramePadding = GetPinFramePadding();


			JVector2 outLinkPinCenter = nodeRectMin + JVector2(nodeRectSize.x, 0.0f);
			outLinkPinCenter += (JVector2(-pinFramePadding.x, pinFramePadding.y) + JVector2(-linkPinRectHalfSize, linkPinRectHalfSize));
			return outLinkPinCenter;
		}

		JVector2 Node::GetPinFramePadding() const
		{
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			f32 nodeOutlineThick = nodeStyle.NodeOutlineThick;
			f32 nodeHeaderHeight = nodeStyle.HeaderHeight;
			JVector2 pinFramePadding = JVector2(nodeOutlineThick, nodeOutlineThick) + JVector2(3.0f, nodeHeaderHeight + 6.0f);
			return pinFramePadding;
		}





		void NodeBuilder::SetHeader(const String& name, const Color& color)
		{
			mNode.mHeaderName = name;
			if (color.A > 0.0f)
			{
				mNode.SetColor(Node::ColorStyle_NodeHeader, ImGui::GetColorU32(ImVec4(color.R, color.G, color.B, color.A)));
			}

		}

		void NodeBuilder::SetNodeColor(const Color& color)
		{
			mNode.SetColor(Node::ColorStyle_NodeBody, ImGui::GetColorU32(ImVec4(color.R, color.G, color.B, color.A)));
		}

		void NodeBuilder::SetName(const String& name)
		{
			mNode.mName = name;
		}


		void NodeBuilder::SetNodeFlags(ENodeFlags flags)
		{
			mNode.mFlags = flags;
		}

		void NodeBuilder::SetContextMenuFunc(const std::function<void(Node*)>& createFunc, const std::function<void(Node*)>& showFunc)
		{
			mNode.mCreateContextMenuFunc = createFunc;
			mNode.mShowContextMenuFunc = showFunc;
		}

		void NodeBuilder::SetInitLocation(const JVector2& location)
		{
			mNode.mLocation = location;
		}




		NodeEditor::NodeEditor()
		{
			mDataStorage = CreateUniquePtr<NodeEditorDataStorage>(this);
		}
		NodeID NodeEditor::CreateNode(const NodeBuilder& builder)
		{
			u64 id = ++mIDOffset;
			mNodeDic[id] = builder.mNode;
			mNodeDic[id].mID = id;
			mNodeDic[id].mNodeEditor = this;
			return id;
		}
		NodeEditorDataStorage& NodeEditor::GetEditorDataStorage()
		{
			return *mDataStorage;
		}
		void NodeEditor::RemoveNode(NodeID id)
		{
			Node* node = FindNode(id);
			if (node == nullptr)
			{
				return;
			}
			if (node->mFlags & ENodeFlags::No_Remove)
			{
				return;
			}
			if (mDataStorage->GetSelectedNode() == id)
			{
				mDataStorage->SetSelectedNode(0);
			}
			if (mDataStorage->GetLinkingNode() == id)
			{
				mDataStorage->SetLinkingNode(0, 0);
			}
			HashSet<NodeID> inLinkedNodeIDs = node->mInLinkedNodeIDs;
			for (NodeID inLinkID : inLinkedNodeIDs)
			{
				UnLink(inLinkID, node->GetID());
			}
			UnLink(id, 0);
			mNodeDic.erase(id);
		}
		Node* NodeEditor::FindNode(NodeID id)
		{
			if (mNodeDic.find(id) == mNodeDic.end())
			{
				return nullptr;
			}
			return &mNodeDic[id];
		}

		void NodeEditor::Link(NodeID fromID, NodeID toID)
		{
			Node* from = FindNode(fromID);
			Node* to = FindNode(toID);



			if (from == nullptr || (from->mFlags & ENodeFlags::LinkOutPin) == false)
			{
				return;
			}
			if (to == nullptr || (to->mFlags & ENodeFlags::LinkInPin) == false)
			{
				return;
			}
			if (from == to)
			{
				return;
			}



			if ((from->mFlags & ENodeFlags::MultipleOutLinkable) == false)
			{
				UnLink(fromID, 0);
			}

			from->mOutLinkedNodeIDs.insert(toID);
			to->mInLinkedNodeIDs.insert(fromID);

			from->RemoveState(ENodeState::OutPinDragging);
			to->RemoveState(ENodeState::InPinDragging);

			mDataStorage->SetLinkingNode(0, 0);
		}


		void NodeEditor::BindContextMenuFunc(const std::function<void()>& func)
		{
			mContextMenuFunc = func;
		}

		void NodeEditor::UnLink(NodeID fromID, NodeID toID)
		{
			Node* from = FindNode(fromID);
			if (from == nullptr || (from->mFlags & ENodeFlags::LinkOutPin) == false)
			{
				return;
			}
			if (toID == 0)
			{
				for (NodeID outNodeID : from->mOutLinkedNodeIDs)
				{
					Node* to = FindNode(outNodeID);
					if (to != nullptr && (to->mFlags & ENodeFlags::LinkInPin))
					{
						to->mInLinkedNodeIDs.erase(fromID);
					}
				}
				from->mOutLinkedNodeIDs.clear();
			}
			else
			{
				Node* to = FindNode(toID);
				if (to != nullptr && (to->mFlags & ENodeFlags::LinkInPin))
				{
					to->mInLinkedNodeIDs.erase(fromID);
				}
				from->mOutLinkedNodeIDs.erase(toID);
			}
		}


		void NodeEditor::OnGUI(const JVector2& size)
		{
			mDataStorage->IsNodeHoveredInOneFrame = false;
			ImGui::BeginChild((ImGuiID)this, ImVec2(size.x, size.y), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);


			mWindowPos = ToJVector2(ImGui::GetWindowPos());
			mWindowSize = ToJVector2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
			ImVec2 cursorPos = ImGui::GetCursorPos();

			for (auto& _pair : mNodeDic)
			{
				_pair.second.DrawLinks();
			}
			for (auto& _pair : mNodeDic)
			{
				_pair.second.Draw();
			}
			ImGui::SetCursorPos(cursorPos);


			if (ImGui::IsKeyPressed((int)EKeyCode::Del))
			{
				RemoveNode(mDataStorage->GetSelectedNode());
			}
			if (ImGui::IsMouseDown(2) == true && ImGui::IsWindowHovered())
			{
				mOffset += ToJVector2(ImGui::GetIO().MouseDelta);


				JG_LOG_INFO("Test Offset {0} , {1}", mOffset.x, mOffset.y);
			}
			if (ImGui::IsMouseClicked(0) && mDataStorage->IsNodeHoveredInOneFrame == false)
			{
				if (mDataStorage->GetLinkingNode() > 0)
				{
					mDataStorage->SetLinkingNode(0, 0);
				}
				else
				{
					mDataStorage->SetSelectedNode(0);
					mDataStorage->SetRenamingNode(0);
				}
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
	}


	namespace StateNodeGUI
	{
		void StateNode::DrawLinks()
		{

		}
		void StateNode::Draw()
		{

		}

		void StateNodeBuilder::SetNodeColor(const Color& color)
		{

		}
		void StateNodeBuilder::SetName(const String& name)
		{

		}
		void StateNodeBuilder::SetInitLocation(const JVector2& location)
		{

		}
		void StateNodeBuilder::SetNodeFlags(EStateNodeFlags flags)
		{

		}
		void StateNodeBuilder::SetContextMenuFunc(const std::function<void(StateNode*)>& createFunc, const std::function<void(StateNode*)>& showFunc)
		{

		}

		StateNodeID StateNodeEditor::CreateNode(const StateNodeBuilder& builder)
		{
			return 0;
		}
		void StateNodeEditor::RemoveNode(StateNodeID id)
		{

		}
		StateNode* StateNodeEditor::FindNode(StateNodeID id)
		{
			return nullptr;
		}


		void StateNodeEditor::Link(StateNodeID fromID, StateNodeID toID)
		{

		}
		void StateNodeEditor::UnLink(StateNodeID fromID, StateNodeID toID)
		{

		}

		void StateNodeEditor::BindContextMenuFunc(const std::function<void()>& func)
		{

		}
	}
}


