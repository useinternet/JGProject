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
// Scene�� �ʿ��� ��
// 1. Renderer
// 2. Camera ����
// 3. SceneObject ����Ʈ ( ��� �׸����ΰ� )
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