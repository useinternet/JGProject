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
private:
	HJWindowArguments _args;

public:
	PJWindow(const HJWindowArguments& args);
	virtual ~PJWindow() = default;

	virtual HJWHandle GetHandle() const { return 0; }

public:

	bool IsValid() const;
	const HJWindowArguments& GetArguments() const;


protected:
	virtual void ConstructWindow(const HJWindowArguments& args) {}
};