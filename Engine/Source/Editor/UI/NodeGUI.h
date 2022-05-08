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
		class NodeEditorDataStorage;
		ENUM_FLAG(ENodeFlags)
		enum class ENodeFlags
		{
			None,
			// 왼쪽에 연결 핀 추가
			LinkInPin  = 0x002,
			// 오른쪽에 연결 핀 추가
			LinkOutPin = 0X004,
			// 제거 불가능
			No_Remove    = 0x008,
			// 이름 변경 불가능
			No_ReName  = 0x010,
			// 오른쪽 연결 핀에 여러 노드 연결 가능
			MultipleOutLinkable = 0x030,
			// 양방향에 연결 핀 추가
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
				f32 LinkLineThick   = 3.0f;
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
			friend class NodeEditorDataStorage;

			NodeEditor* mNodeEditor = nullptr;

			NodeID mID = 0;

			HashSet<NodeID> mInLinkedNodeIDs;
			HashSet<NodeID> mOutLinkedNodeIDs;

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
			void DrawLinks();
			void Draw();
			void Draw_IntermediateLinkNode();
			void Draw_Node();
			void DrawLine(NodeID toID, ImDrawList* draw_list, const JVector2& from, const JVector2& to, f32 factor, f32 thick);
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
			u64 mIDOffset   = 0;
			Node::NodeStyle mNodeStyle;
			SharedPtr<NodeEditorDataStorage> mDataStorage;
		
			Dictionary<NodeID, Node> mNodeDic;

			std::function<void()> mContextMenuFunc;
			JVector2 mWindowSize;
			JVector2 mWindowPos;
			JVector2 mOffset;
		public:
			NodeEditor();
		public:
			NodeID CreateNode(const NodeBuilder& builder);
			void RemoveNode(NodeID id);
			Node*  FindNode(NodeID id);


			void Link(NodeID fromID, NodeID toID);
			void UnLink(NodeID fromID, NodeID toID);

			void BindContextMenuFunc(const std::function<void()>& func);


			const JVector2& GetWindowPos() const {
				return mWindowPos;
			}
			const JVector2& GetWindowSize() const {
				return mWindowSize;
			}
			const JVector2& GetOffset() const {
				return mOffset;
			}

			NodeEditorDataStorage& GetEditorDataStorage();
			Node::NodeStyle& GetNodeStyle() {
				return mNodeStyle;
			}
			void OnGUI(const JVector2& size = JVector2(0.0f,0.0f));
		};
	}
	namespace StateNodeGUI
	{
		// out <-> from
// in <-> in
		using StateNodeID = u64;
		class StateNodeEditorDataStorage;
		ENUM_FLAG(EStateNodeFlags)
		enum class EStateNodeFlags
		{
			None,
			// 제거 불가능
			No_Remove = 0x008,
			// 이름 변경 불가능
			No_ReName = 0x010,
		};

		ENUM_FLAG(EStateNodeState)
		enum class EStateNodeState
		{
			None,
			PinDragging = 0x001,
			Dragging = 0x004,
			ReNaming = 0x008,
		};
		class StateTransition
		{
			StateNodeID mID;


		};
		class StateNode
		{
		public:
			struct StateNodeStyle
			{
				JVector2 NodeRectSize = JVector2(200, 100);
				JVector2 PinNodeRectSize = JVector2(200, 100);
				f32 NodeRectRounding = 10.0f;
				f32 NodeOutlineThick = 3.0f;
				f32 LinkLineThick    = 3.0f;
			};
		private:
			enum EColorStyle
			{
				ColorStyle_NormalOutline,
				ColorStyle_HightlightOutline,
				ColorStyle_NodeBody,
				ColorStyle_Count
			};
			friend class StateNodeEditor;

			StateNodeEditor* mNodeEditor = nullptr;

			StateNodeID mID = 0;
			HashSet<StateNodeID> mLinkedNodeIDs;
			Dictionary<StateNodeID, StateNodeID> mLinkedTransitionDic;
			String mName;


			EStateNodeFlags mFlags = EStateNodeFlags::None;
			EStateNodeState mState = EStateNodeState::None;
			u32 mColors[ColorStyle_Count];


			JVector2 mLocation;
			JVector2 mClickedMouseLocalPos;

			std::function<void(StateNode*)> mCreateContextMenuFunc;
			std::function<void(StateNode*)> mShowContextMenuFunc;
		public:
			StateNode();

			StateNodeID GetID() const {
				return mID;
			}
		private:
			void DrawLinks();
			void Draw();
		};


		class StateNodeBuilder
		{
			friend class StateNodeEditor;
			StateNode  mNode;
		public:
			void SetNodeColor(const Color& color);
			void SetName(const String& name);
			void SetInitLocation(const JVector2& location);
			void SetNodeFlags(EStateNodeFlags flags);

			void SetContextMenuFunc(const std::function<void(StateNode*)>& createFunc, const std::function<void(StateNode*)>& showFunc);
		};


		class StateNodeEditor
		{
			u64 mIDOffset = 0;
			Dictionary<StateNodeID, StateNode> mNodeDic;
			StateNode::StateNodeStyle mNodeStyle;
			std::function<void()> mContextMenuFunc;
			JVector2 mWindowSize;
			JVector2 mWindowPos;
			JVector2 mOffset;
		public:
			StateNodeEditor();
		public:
			StateNodeID CreateNode(const StateNodeBuilder& builder);
			void RemoveNode(StateNodeID id);
			StateNode* FindNode(StateNodeID id);


			void Link(StateNodeID fromID, StateNodeID toID);
			void UnLink(StateNodeID fromID, StateNodeID toID);

			void BindContextMenuFunc(const std::function<void()>& func);


			const JVector2& GetWindowPos() const {
				return mWindowPos;
			}
			const JVector2& GetWindowSize() const {
				return mWindowSize;
			}
			const JVector2& GetOffset() const {
				return mOffset;
			}
			StateNode::StateNodeStyle& GetNodeStyle() {
				return mNodeStyle;
			}
			void OnGUI(const JVector2& size = JVector2(0.0f, 0.0f));
		};
	}

}