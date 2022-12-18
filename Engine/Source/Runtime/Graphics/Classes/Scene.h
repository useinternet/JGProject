#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"

class ITexture;
class IRenderer;

class HSceneArguments
{
	PSharedPtr<IRenderer> Renderer;
};

class GRAPHICS_API IScene
{
//@NOTE
// Scene에 필요한 것
// 1. Renderer
// 2. Camera 정보
// 3. SceneObject 리스트 ( 어떻게 그릴것인가 )
// 4. 
protected:
	virtual void Initialize(const HSceneArguments& args) = 0;

public:
	virtual void Render() = 0;
	virtual void WaitForFinish() = 0;

	// Interface
	// AddSceneObject(const SceneObjectArguments& args);
	// 
};