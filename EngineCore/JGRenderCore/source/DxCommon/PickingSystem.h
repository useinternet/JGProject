#pragma once
#include"DxCommon.h"
#include"DxCore/RenderItem.h"
class Camera;
namespace JGRC
{
	class RCORE_EXPORT PickingSystem
	{
		RenderItem* m_PickingItem = nullptr;

		UINT m_Width;
		UINT m_Height;
	private:
		PickingSystem(const PickingSystem& rhs) = delete;
		PickingSystem& operator=(const PickingSystem& rhs) = delete;
	public:
		PickingSystem(UINT width, UINT height) : m_Width(width), m_Height(height) {}
		bool CheckPicking(int x, int y, RenderItem* Item, Camera* Cam);
		void OnReSize(UINT width, UINT height) { m_Width = width; m_Height = height; }
	};
}