#pragma once
#include "JGCore.h"
#include "Common/Type.h"
#include <shared_mutex>

namespace JG
{
	class IUIView;
	class IPopupUIView;
	class ITexture;

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

	class UIManager : public GlobalSingleton<UIManager>
	{
		friend class Application;
	public:
		static const u64 DEFAULT_PRIORITY = 1000;
		static const u64 SPERATOR_PRIORITY_DELTA = 10;
		static const char CTRL_SHORTCUT_TOKEN  = '%';
		static const char SHIFT_SHORTCUT_TOKEN = '#';
		static const char ALT_SHORTCUT_TOKEN   = '&';
	private:
		Dictionary<JG::Type, UniquePtr<IUIView>>      mUIViewPool;
		Dictionary<JG::Type, UniquePtr<IPopupUIView>> mPopupUIViewPool;
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
		~UIManager();
	public:
		Asset<ITexture>* GetIcon(const String& iconName);
		void SetClipBoard(u64 ID, void* data, u64 size);
		const ClipBoard& GetClipBoard() const;
	public:
		// µî·Ï
		template<class UIViewType>
		void RegisterUIView()
		{
			Type type = Type(TypeID<UIViewType>());


			std::lock_guard<std::shared_mutex> lock(mMutex);
			if (mUIViewPool.find(type) != mUIViewPool.end())
			{
				return;
			}
			mUIViewPool[type]        = CreateUniquePtr<UIViewType>();
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

		template<class UIViewType>
		UIViewType* GetUIView() const
		{
			Type type = Type(TypeID<UIViewType>());

			std::shared_lock<std::shared_mutex> lock(mMutex);
			auto iter = mUIViewPool.find(type);
			if (iter == mUIViewPool.end())
			{
				JG_CORE_ERROR("Not Find UIViewType : {0}", type.GetName());
				return nullptr;
			}
			return static_cast<UIViewType*>(iter->second.get());
		}

		template<class UIPopupViewType, class InitData>
		UIPopupViewType* OpenPopupUIView(const InitData& initData) const
		{
			auto view = GetPopupUIView<UIPopupViewType>();
			if (view != nullptr)
			{
				view->Open(initData);
			}
			return view;
		}

		template<class UIPopupViewType>
		bool OnContextUIView() const 
		{
			auto view = GetPopupUIView<UIPopupViewType>();
			if (view != nullptr && view->IsOpen() && view->GetPopupType() == EPopupType::Context)
			{
				bool result = (static_cast<IPopupUIView*>(view))->OnGUI();
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
				JG_CORE_ERROR("Not Find UIModalViewType : {0}", type.GetName());
				return nullptr;
			}
			return static_cast<UIPopupViewType*>(iter->second.get());
		}


		void RegisterMainMenuItem(const String& menuPath, u64 priority,  const std::function<void()>& action, const std::function<bool()> enableAction);
		void RegisterContextMenuItem(const Type& type, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction);
		void BindShowContextMenuFunc(const std::function<bool(Type, bool)>& func);
		bool ShowContextMenu(const Type& type, bool isWhenHoveredItem = true);
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
