#pragma once
#include "GECore.h"
#include "Core.h"
#include "DirectXApi.h"
namespace DX12
{
	class DX12Screen : public GE::Screen
	{
	protected:
		virtual void Create(HWND hWnd, int width, int height, GE::_FORMAT format, GE::_ScreenFlags = GE::ScreenFlag_None) override;
	private:



	};

}