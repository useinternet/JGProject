#pragma once
#include "JGCore.h"



struct ImDrawList;
namespace JG
{
	namespace StateNodeGUI
	{
		using StateNodeID = u64;
		class StateNodeEditorDataStorage;


		ENUM_FLAG(EStateNodeFlags)
		enum class EStateNodeFlags
		{
			None,
			// 제거 불가능
			No_Remove = 0x001,
			// 이름 변경 불가능
			No_ReName = 0x002,
			//
			RootNode  = 0x004,
		};

		class StateNodeTransition
		{
		public:
			struct StateNodeTransitionStyle
			{
				// 
				JVector2 NodeRectSize = JVector2(30, 30);
				f32 NodeRectRounding = 3.0f;
				f32 NodeOutlineThick = 3.0f;
				f32 LineThick = 3.0f;
				f32 FlowLineThick = 5.0f;
				f32 ArrowSize = 15.0f;
			};
		private:
			enum EColorStyle
			{
				ColorStyle_NormalOutline,
				ColorStyle_HightlightOutline,
				ColorStyle_NodeBody,
				ColorStyle_LineColor,
				ColorStyle_FlowLineColor,
				ColorStyle_Count
			};


			friend class StateNodeEditor;

			StateNodeEditor* mNodeEditor = nullptr;
			StateNodeID mID;

			StateNodeID mFromID;
			StateNodeID mToID;

			u32 mColors[ColorStyle_Count];
		public:
			StateNodeTransition();
		public:
			StateNodeID GetID() const {
				return mID;
			}
			StateNodeID GetFromID() const {
				return mFromID;
			}
			StateNodeID GetToID() const {
				return mToID;
			}
		private:
			void OnGUI();
			u32 GetColor(EColorStyle style);
		};


		class StateNode
		{
		public:
			struct StateNodeStyle
			{
				JVector2 NodeRectSize = JVector2(140, 70);
				f32 NodeRectRounding = 10.0f;
				f32 NodeOutlineThick = 3.0f;
				f32 PinArea = 30.0f;
			};
		private:
			enum EColorStyle
			{
				ColorStyle_NormalOutline,
				ColorStyle_HightlightOutline,
				ColorStyle_NodeBody,
				ColorStyle_NodePinArea,
				ColorStyle_FlowHighlight,
				ColorStyle_Count
			};
			friend class StateNodeEditor;
			friend class StateNodeBuilder;
			friend class StateNodeEditorDataStorage;

			StateNodeEditor* mNodeEditor = nullptr;

			StateNodeID mID = 0;
			String      mName;


			HashSet<StateNodeID> mOutLinkedNodeIDs;
			Dictionary<StateNodeID, StateNodeID> mOutLinkedTransitionDic;
			


			EStateNodeFlags mFlags = EStateNodeFlags::None;
			u32 mColors[ColorStyle_Count];


			JVector2 mLocation;
			JVector2 mClickedMouseLocalPos;

			std::function<void(StateNode*)> mCreateContextMenuFunc;
			std::function<void(StateNode*)> mShowContextMenuFunc;

			List<jbyte> mUserData;
		public:
			StateNode();

			const String& GetName() const {
				return mName;
			}
			StateNodeID GetID() const {
				return mID;
			}
			const JVector2& GetLocation() const {
				return mLocation;
			}
			EStateNodeFlags GetFlags() const {
				return mFlags;
			}
			List<StateNodeID> GetTransitionList() const;
			StateNodeID GetTransition(StateNodeID id);

			template<class T>
			T GetUserData() const {
				return *((T*)mUserData.data());
			}
		private:
			void OnGUI();
			void AddFlags(EStateNodeFlags flags);
			u32 GetColor(EColorStyle style);


		};


		class StateNodeBuilder
		{
			friend class StateNodeEditor;
			StateNode  mNode;
		public:
			void SetNodeColor(const Color& color);
			void SetNodePinAreaColor(const Color& color);
			void SetName(const String& name);
			void SetInitLocation(const JVector2& location);
			void SetNodeFlags(EStateNodeFlags flags);

			template<class T>
			void SetUserData(const T& userData) 
			{
				mNode.mUserData.resize(sizeof(T));
				memcpy(mNode.mUserData.data(), &userData, sizeof(T));
			}
			void SetContextMenuFunc(const std::function<void(StateNode*)>& createFunc, const std::function<void(StateNode*)>& showFunc);
		};

		struct StateNodeLinkInfo
		{
			StateNodeID RootNodeID = 0;
			List<StateNodeID> NodeIDList;
			List<List<StateNodeID>> LinkedNodeIDLists;
		};
		class StateNodeEditor
		{
			friend class StateNodeEditorDataStorage;

			u64 mIDOffset = 0;
			StateNodeTransition::StateNodeTransitionStyle mNodeTransitionStyle;
			StateNode::StateNodeStyle mNodeStyle;
			Dictionary<StateNodeID, StateNode> mNodeDic;
			Dictionary<StateNodeID, StateNodeTransition> mNodeTransitionDic;


			JVector2 mWindowSize;
			JVector2 mWindowPos;
			JVector2 mOffset;

			SharedPtr<StateNodeEditorDataStorage> mDataStorage;
			std::function<void()> mContextMenuFunc;
			std::function<void(StateNodeID)> mCreateNodeCallBack;
			std::function<void(StateNodeID)> mDraggingNodeCallBack;
			std::function<void(StateNodeID)> mRemoveNodeCallBack;
			std::function<void(StateNodeID)> mRemoveTransitionCallBack;
			std::function<void(StateNodeID, StateNodeID, StateNodeID)> mLinkNodeCallBack;
			std::function<void(StateNodeID, StateNodeID, StateNodeID)> mUnLinkNodeCallBack;
			std::function<void(StateNodeID, const String&, const String&)> mReNameCallBack;
		public:
			StateNodeEditor();
		public:
			StateNodeID GetSelectedNodeID() const;
			StateNodeID CreateNode(const StateNodeBuilder& builder);
			void RemoveNode(StateNodeID id);
		
			StateNode* FindNode(StateNodeID id);
			StateNodeTransition* FindNodeTransition(StateNodeID id);
			void Flow(const List<StateNodeID>& flowList);
			void Link(StateNodeID fromID, StateNodeID toID);
			void UnLink(StateNodeID fromID, StateNodeID toID);
		
			void BindContextMenuFunc(const std::function<void()>& func);
			void BindCreateNodeCallBack(const std::function<void(StateNodeID)>& callBack);
			void BindDraggingNodeCallBack(const std::function<void(StateNodeID)>& callBack);
			void BindRemoveNodeCallBack(const std::function<void(StateNodeID)>& callBack);
			void BindRemoveTransitionCallBack(const std::function<void(StateNodeID)>& callBack);
			void BindLinkNodeCallBack(const std::function<void(StateNodeID, StateNodeID, StateNodeID)>& callBack);
			void BindUnLinkCallBack(const std::function<void(StateNodeID, StateNodeID, StateNodeID)>& callBack);
			void BindReNameCallBack(const std::function<void(StateNodeID, const String&, const String&)>& callBack);


			void OnGUI(const JVector2& size = JVector2(0.0f, 0.0f));
			StateNodeID CreateTransition(StateNodeID fromID, StateNodeID toID);
			void RemoveTransition(StateNodeID transitionID);
		private:
			void UpdateNodeInteraction();
		public:
			const JVector2& GetWindowPos() const {
				return mWindowPos;
			}
			const JVector2& GetWindowSize() const {
				return mWindowSize;
			}
			const JVector2& GetOffset() const {
				return mOffset;
			}
			StateNodeTransition::StateNodeTransitionStyle& GetNodeTransitionStyle() {
				return mNodeTransitionStyle;
			}
			StateNode::StateNodeStyle& GetNodeStyle() {
				return mNodeStyle;
			}
			StateNodeEditorDataStorage& GetDataStorage() {
				return *mDataStorage;
			}
			StateNodeLinkInfo GetNodeLinkInfo() const;
		};
	}

}