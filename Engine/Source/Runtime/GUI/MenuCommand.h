#include "Core.h"
#include "GUIDefine.h"


JG_DECLARE_DELEGATE(HMenuAction)
JG_DECLARE_DELEGATE_RETVAL(bool, HIsVisibleMenu);
JG_DECLARE_DELEGATE_RETVAL(bool, HIsEnableMenu);

struct HMenuShortcut
{
	HSet<EKeyCode> ShortcutKeys;
	bool bAlt;
	bool bCtrl;
	bool bShift;

	HMenuShortcut()
	{
		bAlt   = false;
		bCtrl  = false;
		bShift = false;
	}
	HMenuShortcut(const HSet<EKeyCode>& InShortcutKeys, bool bInAlt, bool bInCtrl, bool bInShift)
	{
		ShortcutKeys = InShortcutKeys;
		bAlt    = bInAlt;
		bInCtrl = bInCtrl;
		bShift  = bInShift;

		UpdateShortcutString();
	}

	void UpdateShortcutString()
	{
		cacheString.Reset();

		if (bAlt)
		{
			if (cacheString.Length() > 0) cacheString += " + ";
			cacheString += "Alt";
		}

		if (bCtrl)
		{
			if (cacheString.Length() > 0) cacheString += " + ";
			cacheString += "Ctrl";
		}

		if (bShift)
		{
			if (cacheString.Length() > 0) cacheString += " + ";
			cacheString += "Shift";
		}

		if (cacheString.Length() > 0) cacheString += "+";

		int32 tempIndex = 0;
		PSharedPtr<JGEnum> Enum = StaticEnum<EKeyCode>();
		int32 numShortcut = (int32)ShortcutKeys.size();

		for (EKeyCode keyCode : ShortcutKeys)
		{
			++tempIndex;

			cacheString += Enum->GetEnumNameByValue((int32)keyCode).ToString();
			if (tempIndex < numShortcut)
			{
				cacheString += "+";
			}
		}
	}

	const PString& ToString() const
	{
		return cacheString;
	}

private:
	PString cacheString;
};

class HMenuCommand
{
public:
	HGuid	MenuGuid;
	PString MenuName;
	HMenuShortcut Shortcut;

	HMenuCommand() {}

	HMenuCommand(const PString& inMenuName, const HMenuShortcut& inShortcut)
	{
		MenuGuid = HGuid::New();
		MenuName = inMenuName;
		Shortcut = inShortcut;
	}
};

template<class T>
class HMenuCommands
{
	static T Instance;

	static T& Get()
	{
		return Instance;
	}
};

template<class T>
T HMenuCommands<T>::Instance;