#pragma once
#include"Dx12Include.h"


namespace GR
{
	namespace Dx12
	{
		class Viewport
		{
		private:
			D3D12_VIEWPORT m_Viewport;

		public:
			Viewport() = default;
			Viewport(float width, float height) {
				Set(width, height);
			}
			void Set(float width, float height) {
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
			}
			void Set(float topLeftX, float topLeftY, float width, float height) {
				m_Viewport = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
			}
			const D3D12_VIEWPORT& Get() const {
				return m_Viewport;
			}
		};
	}
}
