#pragma once
#include "Core.h"
#include "Misc/DevelopUnit.h"

#ifdef _DUT_GRAPHICS
#define DUT_GRAPHICS_API __declspec(dllexport)
#define DUT_GRAPHICS_C_API extern "C" __declspec(dllexport)
#else
#define DUT_GRAPHICS_API __declspec(dllimport)
#define DUT_GRAPHICS_C_API extern "C" __declspec(dllimport)
#endif

JGCLASS()
class DUT_GRAPHICS_API JGDUT_Graphics : public JGDevelopUnit
{
	JG_GENERATED_CLASS_BODY
public:
	virtual ~JGDUT_Graphics() = default;

	// 어떤 씬을 내보낼 것인지
	// DetailView 데이터 선언

public:
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Shutdown() override;
};