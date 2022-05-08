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
		Node::Node()
		{
			mColors[ColorStyle_NodeHeader] = ImGui::GetColorU32(ImVec4(1.0f, 0.32f, 0.24f, 0.5f));
			mColors[ColorStyle_NodeBody] = ImGui::GetColorU32(ImVec4(0.05f, 0.06f, 0.04f, 1.0f));
			mColors[ColorStyle_NormalOutline] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
			mColors[ColorStyle_HightlightOutline] = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.2f, 0.4f));
			mColors[ColorStyle_LinkPin] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
			mColors[ColorStyle_LinkLine] = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
		}

		void Node::DrawLinks(
			const JVector2& winPos,
			const JVector2& winSize)
		{
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			f32 linkLineThick = nodeStyle.LinkLineThick;
			JVector2 mousePos = ToJVector2(ImGui::GetMousePos());

			ImDrawList* drawList = ImGui::GetWindowDrawList();
#pragma  region Draw_LinkLine

			if (mState & ENodeState::InPinDragging)
			{
				JVector2 inLinkPinCenter = GetLinkInPinLocation(winPos);

				DrawLine(drawList, inLinkPinCenter, mousePos, -1.0f, linkLineThick);
			}
			if (mState & ENodeState::OutPinDragging)
			{
				JVector2 outLinkPinCenter = GetLinkOutPinLocation(winPos);
				DrawLine(drawList, outLinkPinCenter, mousePos, 1.0f, linkLineThick);
			}
			
			//
			Node* outLinkNode = mNodeEditor->FindNode(mOutLinkedNodeID);
			if (outLinkNode != nullptr)
			{
				DrawLine(drawList, GetLinkOutPinLocation(winPos), outLinkNode->GetLinkInPinLocation(winPos), 1.0f, linkLineThick);
			}
#pragma  endregion
		}

		void Node::Draw(const JVector2& winPos, const JVector2& winSize)
		{
#pragma region Define_Node_Var
			NodeStyle& nodeStyle = mNodeEditor->GetNodeStyle();
			JVector2 framePadding = ToJVector2(ImGui::GetStyle().FramePadding);
			JVector2 nodeRectSize     = nodeStyle.NodeRectSize;
			JVector2 nodeRectHalfSize = nodeRectSize * 0.5f;
			f32 fontSize = ImGui::GetFontSize();
			f32 rectRounding		  = nodeStyle.NodeRectRounding;
		
			f32 nodeOutlineThick  = nodeStyle.NodeOutlineThick;
			f32 nodeHeaderHeight  = nodeStyle.HeaderHeight;
			f32 linkPinRectRounding = nodeStyle.LinkPinRectRounding;
			f32 linkPinRectOutlineThick = nodeStyle.LinkPinRectOutlineThick;
			f32 linkPinRectSize = nodeStyle.LinkPinRectSize;
			f32 linkPinRectHalfSize = linkPinRectSize * 0.5f;
			f32 linkLineThick = nodeStyle.LinkLineThick;
			JVector2 pinFramePadding  = JVector2(nodeOutlineThick, nodeOutlineThick) + JVector2(3.0f, nodeHeaderHeight + 6.0f);
			JVector2 startNodeLocation = winPos + mLocation;
			JVector2 nodeRectMin = startNodeLocation - nodeRectHalfSize;
			JVector2 nodeRectMax = nodeRectMin + nodeRectSize;
			JVector2 mousePos = ToJVector2(ImGui::GetMousePos());

			ImGui::SetCursorPos(ToImVec2(mLocation - nodeRectHalfSize));
			ImGui::Dummy(ImVec2(nodeRectSize.x, nodeRectSize.y));

			bool isNodeClicked = ImGui::IsItemClicked();//&& ImGui::IsItemHovered(ImGuiHoveredFlags_AnyWindow);
			bool isNodeDragging = ImGui::IsItemHovered() && ImGui::IsMouseDown(0);
			bool isAltPresseed = ImGui::GetIO().KeyAlt;
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
				GetID() == mNodeEditor->GetSelectedNode() ? GetColor(ColorStyle_HightlightOutline) : GetColor(ColorStyle_NormalOutline),
				rectRounding, ImDrawFlags_RoundCornersAll, nodeOutlineThick);
#pragma endregion
#pragma region Draw_Node_Pin
			
		
			JVector2 outLinkPinCenter;



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

				isLinkedInPinClicked = ImGui::IsItemClicked();
			}

			pinStartLocation = nodeRectMin + JVector2(nodeRectSize.x, 0.0f);
			pinStartLocation += JVector2(-pinFramePadding.x, pinFramePadding.y);
			if (mFlags & ENodeFlags::LinkOutPin)
			{
				outLinkPinCenter = pinStartLocation  + JVector2(-linkPinRectHalfSize, linkPinRectHalfSize);
				JVector2 rectMin = pinStartLocation + JVector2(-linkPinRectSize, 0.0f);
				JVector2 rectMax = rectMin + JVector2(linkPinRectSize, linkPinRectSize);
				if (mNodeEditor->FindNode(mOutLinkedNodeID) != nullptr || (mState & ENodeState::OutPinDragging))
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
				isLinkedOutPinClicked = ImGui::IsItemClicked();
			}
#pragma  endregion

#pragma region Draw Node Element
			f32 headerTextPaddingY = (nodeHeaderHeight - fontSize) * 0.5f + framePadding.y;
			ImGui::SetCursorPos(ToImVec2(nodeHeaderRectMin - winPos + JVector2(framePadding.x, headerTextPaddingY)));
			ImGui::Text(mHeaderName.c_str());

#pragma  endregion

			if (isLinkedInPinClicked)
			{
				NodeID linkingNodeID  = mNodeEditor->GetLinkingNode();
				Node*  linkingNode    = mNodeEditor->FindNode(linkingNodeID);
				if (linkingNode != nullptr && linkingNodeID != mID)
				{
					u8 linkingMethod = mNodeEditor->GetLinkingMethod();
					if (linkingMethod == LinkingMethod_Out) 
						mNodeEditor->Link(linkingNodeID, mID);
					else isLinkedInPinClicked = false;
						
				}
				else if (linkingNodeID == mID)
				{
					isLinkedInPinClicked = false;
				}
				else 
				{
					isAltPresseed == true ?
						mNodeEditor->UnLink(mOutLinkedNodeID) : 
						mNodeEditor->SetLinkingNode(GetID(), LinkingMethod_In);
				}
			}
			if (isLinkedOutPinClicked)
			{
				NodeID linkingNodeID = mNodeEditor->GetLinkingNode();
				Node* linkingNode    = mNodeEditor->FindNode(linkingNodeID);

				// 
				if (linkingNode != nullptr && linkingNodeID != mID)
				{
					u8 linkingMethod = mNodeEditor->GetLinkingMethod();
					if (linkingMethod == LinkingMethod_In)
						mNodeEditor->Link(linkingNodeID, mID);
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
						mNodeEditor->UnLink(GetID()) :
						mNodeEditor->SetLinkingNode(GetID(), LinkingMethod_Out);
				}
			}

			
			if (isNodeClicked && isLinkedInPinClicked == false && isLinkedOutPinClicked == false)
			{
				mClickedMouseLocalPos = mousePos - winPos - mLocation;
				AddState(ENodeState::Dragging);
				mNodeEditor->SetSelectedNode(mID);
			}



			if (mState & ENodeState::Dragging && ImGui::IsMouseDown(0))
			{
				JVector2 regionMin = ToJVector2(ImGui::GetWindowContentRegionMin()) + nodeRectHalfSize;
				JVector2 regionMax = ToJVector2(ImGui::GetWindowContentRegionMax()) - nodeRectHalfSize;



				mLocation = mousePos - winPos - mClickedMouseLocalPos;
				mLocation.x = Math::Clamp(mLocation.x, regionMin.x, regionMax.x);
				mLocation.y = Math::Clamp(mLocation.y, regionMin.y, regionMax.y);
			}
			else RemoveState(ENodeState::Dragging);


			if (isNodeClicked)
			{
				mNodeEditor->NotifyNodeClick();
			}
			if (isLinkedOutPinClicked || isLinkedInPinClicked)
			{
				mNodeEditor->NofityNodeLinkPinClick();
			}
		}
		void Node::DrawLine(ImDrawList* draw_list, const JVector2& from, const JVector2& to, f32 factor, f32 thick)
		{
			f32 scale = JVector2::LengthSqrd(from - to);
			scale /= 10000.0f;


			f32 tx = Math::Clamp<f32>(320.0f * scale  * factor, -320, 320);
			ImVec2 p1 = ToImVec2(from);

			ImVec2 t1 = ImVec2(tx, 0.0f);
			ImVec2 p2 = ToImVec2(to);
			ImVec2 t2 = ImVec2(tx, 0.0f);
			const int STEPS = 36;
			for (int step = 0; step <= STEPS; step++)
			{
				float t = (float)step / (float)STEPS;
				float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
				float h2 = -2 * t * t * t + 3 * t * t;
				float h3 = t * t * t - 2 * t * t + t;
				float h4 = t * t * t - t * t;
				draw_list->PathLineTo(ImVec2(h1 * p1.x + h2 * p2.x + h3 * t1.x + h4 * t2.x,
					h1 * p1.y + h2 * p2.y + h3 * t1.y + h4 * t2.y));
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
			NodeStyle& nodeStyle	= mNodeEditor->GetNodeStyle();
			JVector2 nodeRectSize	= nodeStyle.NodeRectSize;
			JVector2 nodeRectHalfSize	= nodeRectSize * 0.5f;
			JVector2 startNodeLocation	= winPos + mLocation;
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





		NodeID NodeEditor::CreateNode(const NodeBuilder& builder)
		{
			u64 id = ++mNodeIDOffset;
			mNodeDic[id] = builder.mNode;
			mNodeDic[id].mID = id;
			mNodeDic[id].mNodeEditor = this;
			return id;
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
			if (mSelectedNodeID == id)
			{
				SetSelectedNode(0);
			}
			if (mLinkingNodeID == id)
			{
				SetLinkingNode(0, 0);
			}
			UnLink(id);
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
			Node* to   = FindNode(toID);

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

			// Unlink
			UnLink(fromID);
			from->mOutLinkedNodeID = toID;
			to->mInLinkedNodeIDs.insert(fromID);

			from->RemoveState(ENodeState::OutPinDragging);
			to->RemoveState(ENodeState::InPinDragging);
		}


		void NodeEditor::BindContextMenuFunc(const std::function<void()>& func)
		{
			mContextMenuFunc = func;
		}

		void NodeEditor::UnLink(NodeID fromID)
		{
			Node* from = FindNode(fromID);
			if (from == nullptr || (from->mFlags & ENodeFlags::LinkOutPin) == false)
			{
				return;
			}

			Node* to = FindNode(from->mOutLinkedNodeID);
			if (to != nullptr && (to->mFlags & ENodeFlags::LinkInPin))
			{
				to->mInLinkedNodeIDs.erase(fromID);
			}
			from->mOutLinkedNodeID = 0;
		}
		void NodeEditor::SetLinkingNode(NodeID id, u8 method)
		{

			Node* prev_node = FindNode(mLinkingNodeID);
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

			Node* node = FindNode(id);
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

		void NodeEditor::OnGUI(const JVector2& size)
		{
			mIsSelectNodeInOneFrame = false;
			mIsSelectPinInOneFrame = false;

			ImGui::BeginChild((ImGuiID)this, ImVec2(size.x, size.y), true, ImGuiWindowFlags_NoMove);


			mWindowPos  = ToJVector2(ImGui::GetWindowPos()); 
			mWindowSize = ToJVector2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
			ImVec2 cursorPos = ImGui::GetCursorPos();

			for (auto& _pair : mNodeDic)
			{
				_pair.second.DrawLinks(mWindowPos, mWindowSize);
			}
			for (auto& _pair : mNodeDic)
			{
				_pair.second.Draw(mWindowPos, mWindowSize);
			}
			ImGui::SetCursorPos(cursorPos);

			if (ImGui::IsKeyPressed((int)EKeyCode::Del))
			{
				RemoveNode(GetSelectedNode());
			}

			if (ImGui::IsMouseClicked(0))
			{
				if (mIsSelectPinInOneFrame == false)
				{
					SetLinkingNode(0, 0);
				}
				if (mIsSelectNodeInOneFrame == false)
				{
					SetSelectedNode(0);
				}
			}
			if (mIsSelectNodeInOneFrame == false && mIsSelectPinInOneFrame == false)
			{
				if (mContextMenuFunc)
				{
					mContextMenuFunc();
				}
			}

			ImGui::EndChild();
		}


	}
}