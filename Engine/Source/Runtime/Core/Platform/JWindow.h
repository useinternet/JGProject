#pragma once
#include "Memory/Memory.h"
#include "String/String.h"
#include "Math/Vector.h"
#include "PlatformDefines.h"


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