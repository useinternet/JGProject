#pragma once
#include "JGCore.h"
#include "Common/Type.h"
#include <shared_mutex>

namespace JG
{
	class IUIView;
	class IPopupUIView;
	class ITexture;
	class IInspectorUI;


	class IAsset;
	template<class T>
	class Asset;


	struct MenuItem
	{
		String ShortCut;
		std::function<void()> Action;
		std::function<bool()> EnableAction;
	};

	struct MenuItemNode
	{
		String    Name;
		u64       Priority = 0;
		bool      IsSperator = false;

		const MenuItemNode* Parent = nullptr;
		UniquePtr<MenuItem> MenuItem;
		mutable bool IsOpen = false;

	private:
		friend class UIManager;
		SortedDictionary<u64, List<UniquePtr<MenuItemNode>>> ChildNodes;
	};
	struct ClipBoard
	{
		u64 ID = 0;
		List<jbyte> Data;
	};

	class UIManager : public IGlobalSingleton<UIManager>
	{
		friend class EditorUISystemLayer;
	public:
		static const u64 DEFAULT_PRIORITY = 1000;
		static const u64 SPERATOR_PRIORITY_DELTA = 10;
		static const char CTRL_SHORTCUT_TOKEN  = '%';
		static const char SHIFT_SHORTCUT_TOKEN = '#';
		static const char ALT_SHORTCUT_TOKEN   = '&';
	private:
		Dictionary<u64, UniquePtr<IUIView>>      mUIViewPool;
		Dictionary<JG::Type, UniquePtr<IPopupUIView>> mPopupUIViewPool;
		Dictionary<JG::Type, UniquePtr<IInspectorUI>> mInspectorUIPool;
		Dictionary<JG::Type, UniquePtr<MenuItemNode>> mUIViewContextMenu;
		UniquePtr<MenuItemNode>  mMainMenuItemRootNode;
		mutable std::shared_mutex   mMutex;


		std::function<bool(Type, bool)> mShowContextMenuFunc;

		
		ClipBoard mClipBoard;
		List<SharedPtr<IAsset>> mIconPool;
		Dictionary<String, Asset<ITexture>*>  mIcons;
		bool mIsLoadIcons = false;
	public:
		UIManager();
		virtual ~UIManager();
	public:
		Asset<ITexture>* GetIcon(const String& iconName);
		void SetClipBoard(u64 ID, void* data, u64 size);
		const ClipBoard& GetClipBoard() const;
	public:
		// µî·Ï
		template<class UIViewType>
		void RegisterUIView(u64 viewID = 0)
		{
			Type type = Type(TypeID<UIViewType>());
			u64 typeID = type.GetID();

			if (viewID != 0)
			{
				HashHelper::Combine(typeID, viewID);
			}
			

			std::lock_guard<std::shared_mutex> lock(mMutex);
			if (mUIViewPool.find(typeID) != mUIViewPool.end())
			{
				return;
			}
			mUIViewPool[typeID] = CreateUniquePtr<UIViewType>();
			mUIViewPool[typeID]->SetViewID(typeID);
		}
		template<class UIPopupViewType>
		void RegisterPopupUIView()
		{
			Type type = Type(TypeID<UIPopupViewType>());
			std::lock_guard<std::shared_mutex> lock(mMutex);

			if (mPopupUIViewPool.find(type) != mPopupUIViewPool.end())
			{
				return;
			}
			mPopupUIViewPool[type] = CreateUniquePtr<UIPopupViewType>();
		}
		template<class InspectorUIType>
		void RegisterInspectorUI()
		{
			auto inspectorUI = CreateUniquePtr<InspectorUIType>();
			Type type = inspectorUI->GetObjectType();
			if (mInspectorUIPool.find(type) != mInspectorUIPool.end())
			{
				return;
			}
			mInspectorUIPool[type] = std::move(inspectorUI);
		}

		template<class UIViewType>
		UIViewType* GetUIView(u64 viewID = 0)
		{
		
			Type type  = Type(TypeID<UIViewType>());
			u64 typeID = type.GetID();
			{
				std::lock_guard<std::shared_mutex> lock(mMutex);
				auto iter = mUIViewPool.find(typeID);
				if (iter == mUIViewPool.end())
				{
					JG_LOG_ERROR("Not Find UIViewType : {0}", type.GetName());
					return nullptr;
				}
				else if(iter->second->IsUniqueView() == false && viewID != 0)
				{
					HashHelper::Combine(typeID, viewID);
				}
				else
				{
					viewID = 0;
				}
			}
			if (viewID != 0)
			{
				RegisterUIView<UIViewType>(viewID);
			}

			return static_cast<UIViewType*>(mUIViewPool[typeID].get());
		}

		template<class UIPopupViewType, class InitData>
		UIPopupViewType* OpenPopupUIView(const InitData& initData, u32 viewID = 0) const
		{
			auto view = GetPopupUIView<UIPopupViewType>();
	
			if (view != nullptr)
			{
				Type type   = Type(TypeID<UIPopupViewType>());
				u64  typeID = type.GetID();
				if (viewID > 0)
				{
					HashHelper::Combine(typeID, viewID);
				}
				IPopupUIView* pInterfaceView = static_cast<IPopupUIView*>(view);
				pInterfaceView->SetViewID(typeID);
				view->Open(initData);
				
			}
			return view;
		}

		template<class UIPopupViewType>
		bool OnContextUIView(u32 viewID) 
		{
			auto view = GetPopupUIView<UIPopupViewType>();
			if (view == nullptr)
			{
				return false;
			}
			IPopupUIView* pInterfaceView = static_cast<IPopupUIView*>(view);
			Type type   = Type(TypeID<UIPopupViewType>());
			u64  typeID = type.GetID();
			if (viewID > 0)
			{
				HashHelper::Combine(typeID, viewID);
			}

			if (view->GetViewID() == typeID && view->IsOpen() && view->GetPopupType() == EPopupType::Context)
			{
				bool result = pInterfaceView->OnGUI();
				if (result == false)
				{
					view->Close();
					return true;
				}
			}
			return false;
		}

		template<class UIPopupViewType>
		UIPopupViewType* GetPopupUIView() const
		{
			Type type = Type(TypeID<UIPopupViewType>());

			std::shared_lock<std::shared_mutex> lock(mMutex);
			auto iter = mPopupUIViewPool.find(type);
			if (iter == mPopupUIViewPool.end())
			{
				JG_LOG_ERROR("Not Find UIModalViewType : {0}", type.GetName());
				return nullptr;
			}
			return static_cast<UIPopupViewType*>(iter->second.get());
		}


		void RegisterMainMenuItem(const String& menuPath, u64 priority,  const std::function<void()>& action, const std::function<bool()> enableAction);
		void RegisterContextMenuItem(const Type& type, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction);
		void BindShowContextMenuFunc(const std::function<bool(Type, bool)>& func);
		bool ShowContextMenu(const Type& type, bool isWhenHoveredItem = true);
		bool ShowInspectorUI(IJGObject* object);
		template<class T>
		bool ForceShowInspectorUI(T* object)
		{
			if (object == nullptr)
			{
				return false;
			}
			if (mInspectorUIPool.find(JGTYPE(T)) == mInspectorUIPool.end())
			{
				return false;
			}
			return mInspectorUIPool[JGTYPE(T)]->OnGUI(object, true);
		}
		void ForEach(const std::function<void(IUIView*)> action);
		void ForEach(
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);
		void ForEach(
			const Type& type,
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);
	private:
		void LoadIcons();
		void OnGUI();
		void OnEvent(IEvent& e);
		void ForEach(
			MenuItemNode* rootNode,
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);

		void RegisterMenuItem(MenuItemNode* rootNode, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction);
		MenuItemNode* FindMenuItemNode(MenuItemNode* parentNode, const String& menuName, u64 default_priority = DEFAULT_PRIORITY);
		MenuItemNode* RegisterMenuNode(MenuItemNode* parentNode, const String& menuName, u64 priority);
		void ExtractPathAndShortcut(const String& menuPath, String* out_path, String* out_shortCut);
	};
}
