#pragma once
#include"Dx12Include.h"

namespace GR
{
	namespace Dx12
	{
		class ScissorRect
		{
			D3D12_RECT m_Rect;
		public:
			ScissorRect() = default;
			ScissorRect(int width, int height) {
				Set(width, height);
			}

			void Set(int width, int height) {
				m_Rect = CD3DX12_RECT(0, 0, width, height);
			}
			void Set(int left, int top, int right, int bottom) {
				m_Rect = CD3DX12_RECT(left, top, right, bottom);
			}
			const D3D12_RECT& Get() const {
				return m_Rect;
			}
		};
	}
}

