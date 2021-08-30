#include "pch.h"
#include "UIManager.h"
#include "Class/UI/UIView.h"
#include "Application.h"
#include "Graphics/Resource.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	UIManager::UIManager()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::OnGUI,
			[&]() -> EScheduleResult
		{
			OnGUI();
			return EScheduleResult::Continue;
		});
		Scheduler::GetInstance().ScheduleOnceByFrame(0, SchedulePriority::BeginSystem,
			[&]()-> EScheduleResult
		{
			if(mIsLoadIcons == false)
			{
				LoadIcons();
			}
	
			return EScheduleResult::Continue;
		});
	}
	UIManager::~UIManager()
	{
		for (auto& _pair : mUIViewPool)
		{
			if (_pair.second->IsOpen())
			{
				_pair.second->Close();
			}
		}
		mUIViewPool.clear();
	}
	Asset<ITexture>* UIManager::GetIcon(const String& iconName)
	{
		if (mIsLoadIcons == false)
		{
			LoadIcons();
		}


		auto iter = mIcons.find(iconName);
		if (iter == mIcons.end())
		{
			return nullptr;
		}
		return iter->second;
	}
	void UIManager::SetClipBoard(u64 ID, void* data, u64 size)
	{
		mClipBoard.ID = ID;
		mClipBoard.Data.clear();
		mClipBoard.Data.resize(size);
		memcpy(mClipBoard.Data.data(), data, size);
	}
	const ClipBoard& UIManager::GetClipBoard() const
	{
		return mClipBoard;
	}
	void UIManager::RegisterMainMenuItem(const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction)
	{
		if (mMainMenuItemRootNode == nullptr)
		{
			mMainMenuItemRootNode = CreateUniquePtr<MenuItemNode>();
			mMainMenuItemRootNode->Name = "Root";
			mMainMenuItemRootNode->IsOpen = true;
		}
		RegisterMenuItem(mMainMenuItemRootNode.get(), menuPath, priority, action, enableAction);
	}
	void UIManager::RegisterContextMenuItem(const Type& type, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction)
	{
		if (mUIViewPool.find(type) == mUIViewPool.end())
		{
			return;
		}

		auto iter = mUIViewContextMenu.find(type);
		if (iter == mUIViewContextMenu.end())
		{
			mUIViewContextMenu[type] = CreateUniquePtr<MenuItemNode>();
			mUIViewContextMenu[type]->Name = type.GetName();
			mUIViewContextMenu[type]->IsOpen = true;
		}
		RegisterMenuItem(mUIViewContextMenu[type].get(), menuPath, priority, action, enableAction);
	}
	void UIManager::BindShowContextMenuFunc(const std::function<bool(Type)>& func)
	{
		mShowContextMenuFunc = func;
	}
	bool UIManager::ShowContextMenu(const Type& type)
	{
		if (mShowContextMenuFunc != nullptr)
		{
			return mShowContextMenuFunc(type);
		}
		return false;
	}
	void UIManager::ForEach(const std::function<void(IUIView*)> action)
	{
		for (auto& _pair : mUIViewPool)
		{
			action(_pair.second.get());
		}
	}

	void UIManager::ForEach(
		const std::function<void(const MenuItemNode*)>& beginAction,
		const std::function<void(const MenuItemNode*)>& endAction)
	{
		ForEach(mMainMenuItemRootNode.get(), beginAction, endAction);
	}

	void UIManager::ForEach(const Type& type, const std::function<void(const MenuItemNode*)>& beginAction, const std::function<void(const MenuItemNode*)>& endAction)
	{
		auto iter = mUIViewContextMenu.find(type);
		if (iter == mUIViewContextMenu.end())
		{
			return;
		}
		ForEach(iter->second.get(), beginAction, endAction);
	}
	void UIManager::LoadIcons()
	{
		auto iconPath = Application::GetInstance().GetIconPath();
		for (auto& iter : fs::recursive_directory_iterator(iconPath))
		{
			auto pt = AssetDataBase::GetInstance().LoadOriginAsset(iter.path().string());
			if (pt == nullptr)
			{
				continue;
			}
			auto t = pt->As<ITexture>();
			if (t != nullptr)
			{
				mIcons.emplace(t->GetAssetName(), t);
				JG_CORE_INFO("Load Success Icon : {0} ", t->GetAssetName());
			}
		}
		mIsLoadIcons = true;
	}
	void UIManager::OnGUI()
	{
		for (auto& _pair : mUIViewPool)
		{
			if (_pair.second->IsOpen())
			{
				_pair.second->OnGUI();
			}
		}
		for (auto& _pair : mPopupUIViewPool)
		{
			if (_pair.second->GetPopupType() != EPopupType::Context && _pair.second->IsOpen())
			{
				if (_pair.second->OnGUI() == false)
				{
					_pair.second->Close();
				}
			}
		}
	}
	void UIManager::OnEvent(IEvent& e)
	{
		for (auto& _pair : mUIViewPool)
		{
			if (_pair.second->IsOpen() && e.Handled == false)
			{
				_pair.second->OnEvent(e);
			}
		}
	}
	void UIManager::ForEach(MenuItemNode* rootNode, const std::function<void(const MenuItemNode*)>& beginAction, const std::function<void(const MenuItemNode*)>& endAction)
	{
		if (rootNode == nullptr)
		{
			return;
		}

		using NodeIterator = SortedDictionary<u64, List<UniquePtr<MenuItemNode>>>::iterator;
		struct NodeHistroy
		{
			MenuItemNode* Node = nullptr;
			NodeIterator Iterater;
			u64 Index = 0;
		};


		Stack<NodeHistroy> nodeStack;
		NodeHistroy curr;
		curr.Node = rootNode;
		curr.Iterater = curr.Node->ChildNodes.begin();
		curr.Index = 0;

		while (true)
		{
			if (curr.Node->MenuItem == nullptr)
			{
				auto& iter = curr.Iterater;
				auto index = curr.Index;
				if (iter != curr.Node->ChildNodes.end())
				{
					auto nodeCnt = iter->second.size();
					if (index < nodeCnt)
					{
						auto pushHistory = curr;

						curr.Node = iter->second[index].get();
						curr.Iterater = curr.Node->ChildNodes.begin();
						curr.Index = 0;
						// Begin
						if (index != 0)
						{
							curr.Node->IsSperator = false;
						}
						beginAction(curr.Node);
						nodeStack.push(pushHistory);
						continue;
					}
					else
					{
						u64 prevPriority = curr.Iterater->first;
						curr.Iterater++;
						if (curr.Iterater != curr.Node->ChildNodes.end())
						{
							u64 nextPriority = curr.Iterater->first;
							u64 delta = nextPriority - prevPriority;
							if (delta > SPERATOR_PRIORITY_DELTA)
							{
								curr.Iterater->second[0]->IsSperator = true;
							}
						}
						curr.Index = 0;
						continue;
					}
				}
				else
				{
					if (curr.Node != rootNode)
					{
						endAction(curr.Node);
					}
					if (nodeStack.empty() == false)
					{
						curr = nodeStack.top(); nodeStack.pop();
						curr.Index += 1;
						continue;
					}
					else
					{
						break;
					}
				}

			}
			else
			{
				if (nodeStack.empty() == false)
				{
					curr = nodeStack.top(); nodeStack.pop();
					curr.Index += 1;
					continue;
				}
				else
				{
					break;
				}
			}


		}
	}
	void UIManager::RegisterMenuItem(MenuItemNode* rootNode, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction)
	{
		String path;
		String shortCut;

		ExtractPathAndShortcut(menuPath, &path, &shortCut);

		// Path
		u64  pos = path.find_first_of("/");
		MenuItemNode* currRootNode = nullptr;
		if (pos == String::npos)
		{
			if (rootNode == mMainMenuItemRootNode.get())
			{
				RegisterMenuNode(rootNode, menuPath, priority);
			}
			else
			{
				currRootNode = RegisterMenuNode(rootNode, path, priority);
				currRootNode->MenuItem = CreateUniquePtr<MenuItem>();
				currRootNode->MenuItem->Action = action;
				currRootNode->MenuItem->EnableAction = enableAction;
				currRootNode->MenuItem->ShortCut = shortCut;
			}
			return;
		}
		else
		{
			auto rootName = path.substr(0, pos);
			currRootNode = FindMenuItemNode(rootNode, rootName, priority);
		}

		path = path.substr(pos + 1, path.length() - pos);
		u64    start = 0;
		while (true)
		{
			u64  end = path.find_first_of("/");
			if (end == String::npos)
			{
				currRootNode = RegisterMenuNode(currRootNode, path, priority);
				currRootNode->MenuItem = CreateUniquePtr<MenuItem>();
				currRootNode->MenuItem->Action = action;
				currRootNode->MenuItem->EnableAction = enableAction;
				currRootNode->MenuItem->ShortCut = shortCut;
				break;
			}
			else
			{
				auto menu = path.substr(start, end - start);
				path = path.substr(end + 1, path.length() - end);
				currRootNode = FindMenuItemNode(currRootNode, menu, priority);
			}
		}
	}

	MenuItemNode* UIManager::FindMenuItemNode(MenuItemNode* parentNode, const String& menuName, u64 default_priority)
	{
		for (auto& _pair : parentNode->ChildNodes)
		{
			for (auto& item : _pair.second)
			{
				if (item->MenuItem == nullptr && item->Name == menuName)
				{
					return item.get();
				}
			}
		}
		return RegisterMenuNode(parentNode, menuName, default_priority);
	}
	MenuItemNode* UIManager::RegisterMenuNode(MenuItemNode* parentNode, const String& menuName, u64 priority)
	{
		auto menuItem = CreateUniquePtr<MenuItemNode>();
		auto result = menuItem.get();
		menuItem->Name     = menuName;
		menuItem->Parent   = parentNode;
		menuItem->Priority = priority;
		parentNode->ChildNodes[priority].push_back(std::move(menuItem));
		return result;
	}
	void UIManager::ExtractPathAndShortcut(const String& menuPath, String* out_path, String* out_shortCut)
	{
		bool isShortCut = menuPath.find(CTRL_SHORTCUT_TOKEN) != String::npos || menuPath.find(SHIFT_SHORTCUT_TOKEN) != String::npos || menuPath.find(ALT_SHORTCUT_TOKEN) != String::npos;

	
		if (isShortCut == false)
		{
			if (out_path)
			{
				*out_path = menuPath;
			}
		}
		else
		{
			u64 midPos = menuPath.find_last_of(" ");
			if (out_path)
			{
				*out_path = menuPath.substr(0, midPos);
			}
			if (out_shortCut)
			{
				auto short_cut = menuPath.substr(midPos + 1, menuPath.length() - midPos);
				short_cut = ReplaceAll(short_cut, " ", "");

				char splitToken = '_';
				u64 splitPos = short_cut.find_first_of(splitToken);
				if (splitPos != String::npos)
				{
					String token = short_cut.substr(0, splitPos);
					short_cut    = short_cut.substr(splitPos + 1);

					if (token.find(CTRL_SHORTCUT_TOKEN) != String::npos)
					{
						*out_shortCut += "Ctrl + ";
					}
					if (token.find(SHIFT_SHORTCUT_TOKEN) != String::npos)
					{
						*out_shortCut += "Shift + ";
					}
					if (token.find(ALT_SHORTCUT_TOKEN) != String::npos)
					{
						*out_shortCut += "Alt + ";
					}


					if (short_cut.length() != 0 && out_shortCut->length() != 0)
					{
						while (true)
						{
							u64 pos = short_cut.find_first_of(splitToken);
							if (pos == String::npos)
							{
								*out_shortCut += short_cut;
								break;
							}
							else
							{
								*out_shortCut += short_cut.substr(0, pos) + " + ";
								short_cut = short_cut.substr(pos + 1);
							}
						}
					}
					else
					{
						out_shortCut->clear();
					}
				}
				
			}
		}


	}

}

