#pragma once
#include "JGCore.h"



struct ImDrawList;
namespace JG
{
	namespace NodeGUI
	{
		// out <-> from
		// in <-> in
		using NodeID = u64;

		ENUM_FLAG(ENodeFlags)
		enum class ENodeFlags
		{
			None,
			LinkInPin  = 0x002,
			LinkOutPin = 0X004,
			No_Remove  = 0x008,
			LinkPin = LinkInPin | LinkOutPin
		};

		ENUM_FLAG(ENodeState)
		enum class ENodeState
		{
			None,
			InPinDragging = 0x001,
			OutPinDragging = 0x002,
			Dragging = 0x004,
			ReNaming = 0x008,
		};



		class Node
		{
		public:
			struct NodeStyle
			{
				JVector2 NodeRectSize = JVector2(200, 100);
				f32 HeaderHeight = 25.0f;
				f32 NodeRectRounding = 10.0f;
				f32 NodeOutlineThick = 3.0f;
				f32 LinkPinRectRounding	   = 5.0f;
				f32 LinkPinRectOutlineThick = 2.0f;
				f32 LinkPinRectSize = 10.0f;
				f32 LinkLineThick = 3.0f;
			};
			
		private:
			enum EColorStyle
			{
				ColorStyle_NormalOutline,
				ColorStyle_HightlightOutline,
				ColorStyle_NodeBody,
				ColorStyle_NodeHeader,
				ColorStyle_LinkPin,
				ColorStyle_LinkLine,
				ColorStyle_Count
			};
			enum ELinkingMethod
			{
				LinkingMethod_In,
				LinkingMethod_Out
			};

			enum class EPinType
			{
				Data,
				Connect
			};


			friend class NodeEditor;
			friend class NodeBuilder;

			NodeEditor* mNodeEditor = nullptr;

			NodeID mID = 0;

			HashSet<NodeID> mInLinkedNodeIDs;
			NodeID mOutLinkedNodeID = 0;

			String mName;
			String mHeaderName;
			u32    mHeaderColor;


			ENodeFlags mFlags = ENodeFlags::None;
			ENodeState mState = ENodeState::None;
			u32 mColors[ColorStyle_Count];
		

			JVector2 mLocation;
			JVector2 mClickedMouseLocalPos;

			std::function<void(Node*)> mCreateContextMenuFunc;
			std::function<void(Node*)> mShowContextMenuFunc;
		public:
			Node();

			NodeID GetID() const {
				return mID;
			}
		private:
			void DrawLinks(
				const JVector2& winPos,
				const JVector2& winSize);
			void Draw(
				const JVector2& winPos,
				const JVector2& winSize);
			void DrawLine(ImDrawList* draw_list, const JVector2& from, const JVector2& to,f32 factor, f32 thick);
			u32  GetColor(EColorStyle style);
			void SetColor(EColorStyle style, u32 color);
			void AddState(ENodeState state);
			void AddFlags(ENodeFlags flags);
			void RemoveState(ENodeState state);


			JVector2 GetLinkInPinLocation(const JVector2& winPos) const;
			JVector2 GetLinkOutPinLocation(const JVector2& winPos) const;
			JVector2 GetPinFramePadding() const;
		};


		class NodeBuilder
		{
			friend class NodeEditor;
			Node  mNode;
		public:
			void SetHeader(const String& name, const Color& color = Color(0.0f, 0.0f, 0.0f, -1.0f));
			void SetNodeColor(const Color& color);
			void SetName(const String& name);
			void SetInitLocation(const JVector2& location);
			void SetNodeFlags(ENodeFlags flags);

			void SetContextMenuFunc(const std::function<void(Node*)>& createFunc, const std::function<void(Node*)>& showFunc);
		};

		class NodeEditor
		{
			u64 mNodeIDOffset   = 0;
			Node::NodeStyle mNodeStyle;

			NodeID mLinkingNodeID = 0;
			u8     mLinkingMethod   = 0; // In 0, Out 1


			bool mIsSelectNodeInOneFrame = false;
			bool mIsSelectPinInOneFrame  = false;


			NodeID mSelectedNodeID = 0;
			Dictionary<NodeID, Node> mNodeDic;

			std::function<void()> mContextMenuFunc;
			JVector2 mWindowSize;
			JVector2 mWindowPos;
		public:
			NodeID CreateNode(const NodeBuilder& builder);
			void RemoveNode(NodeID id);
			Node*  FindNode(NodeID id);


			void Link(NodeID fromID, NodeID toID);
			void UnLink(NodeID fromID);

			void BindContextMenuFunc(const std::function<void()>& func);

			NodeID GetSelectedNode() const {
				return mSelectedNodeID;
			}
			void SetSelectedNode(NodeID id) {
				mSelectedNodeID = id;
			}

			NodeID GetLinkingNode() const {
				return mLinkingNodeID;
			}
			u8 GetLinkingMethod() const {
				return mLinkingMethod;
			}
			const JVector2& GetWindowPos() const {
				return mWindowPos;
			}
			const JVector2& GetWindowSize() const {
				return mWindowSize;
			}
			void SetLinkingNode(NodeID id, u8 method);
			void NotifyNodeClick() {
				mIsSelectNodeInOneFrame = true;
			}
			void NofityNodeLinkPinClick() {
				mIsSelectPinInOneFrame = true;
			}
			Node::NodeStyle& GetNodeStyle() {
				return mNodeStyle;
			}
			void OnGUI(const JVector2& size = JVector2(0.0f,0.0f));
		};
	}


}