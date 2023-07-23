#pragma once
#include "Memory/Memory.h"
#include "String/String.h"
#include "Math/Vector.h"
#include "PlatformDefines.h"
#include "Misc/Delegate.h"

struct HWindowCallBacks
{
#ifdef _PLATFORM_WINDOWS
	JG_DECLARE_MULTICAST_DELEGATE(HWndProc, HWND, uint32, WPARAM, LPARAM);
	HWndProc WndProc;
#endif

	JG_DECLARE_MULTICAST_DELEGATE(HOnResize, uint32, uint32);
	HOnResize OnResize;
};

struct HJWindowArguments
{
	PString     Title;
	HVector2Int Size;
	int32       IconID;

	HJWindowArguments() : IconID(INDEX_NONE) {}
};

class PJWindow : public IMemoryObject
{
	friend class HJWindowCreater;
private:
	HJWindowArguments _args;

public:
	virtual ~PJWindow() = default;

	virtual HJWHandle GetHandle() const { return 0; }
	virtual HVector2Int GetClientSize() const { return HVector2Int(); }
public:

	bool IsValid() const;
	const HJWindowArguments& GetArguments() const;


protected:
	virtual bool ConstructWindow() { return false; }
};

class HJWindowCreater
{
	bool _bIsSuccess;
public:
	HJWindowCreater(PJWindow* pWindow, const HJWindowArguments& args);

	bool IsSuccess() const;
};