#pragma once
#include "Screen.h"

namespace DX12
{
	REGISTER_GRAPHICS_INTERFACE_SCREEN(DX12_Screen)
	class DX12_Screen : public GE::Screen
	{
	public:

		virtual void  RequestResize(const JVector2Int& size) override;

		virtual void  BindScene(GE::Scene* scene) override;
		virtual void  UnBindScene() override;
		virtual const JVector2Int& GetSize() const override;


		virtual HWND  GetHWND() const override;

		virtual ~DX12_Screen() override;

	private:
		virtual void Init(HWND hWnd, int width, int height) override;
	private:
		HWND m_hWnd;
		JVector2Int m_Size;

	};
}