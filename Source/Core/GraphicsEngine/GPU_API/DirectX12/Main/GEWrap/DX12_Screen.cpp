#include "pch.h"
#include "DX12_Screen.h"
#include "DX12_Scene.h"
#include "Texture.h"
#include "DirectXApi.h"

namespace DX12
{
	void DX12_Screen::RequestResize(const JVector2Int& size)
	{
		if (m_Size == size) return;
		m_Size.x = std::max<int>(10, size.x);
		m_Size.y = std::max<int>(10, size.y);

		DX12::DXDevice::RequestResizeScreen(m_hWnd, m_Size.x, m_Size.y);
	}

	void DX12_Screen::BindScene(GE::Scene* scene)
	{
		DX12::Texture* t = (DX12::Texture*)scene->GetSceneTexture()->GetUserTextureData();
		DX12::DXDevice::RequestSubmitTextureToScreen(m_hWnd, *t);
	}

	void DX12_Screen::UnBindScene()
	{
		DX12::Texture t;
		DX12::DXDevice::RequestSubmitTextureToScreen(m_hWnd, t);
	}

	const JVector2Int& DX12_Screen::GetSize() const
	{
		return m_Size;
	}

	HWND DX12_Screen::GetHWND() const
	{
		return m_hWnd;
	}

	DX12_Screen::~DX12_Screen()
	{
		DX12::DXDevice::UnRegisterScreen(m_hWnd);
		m_hWnd = 0;
	}
	void DX12_Screen::Init(HWND hWnd, int width, int height)
	{
		width  = std::max<int>(10, width);
		height = std::max<int>(10, height);

		m_hWnd = hWnd;
		m_Size = { width, height };
		DX12::DXDevice::RegisterScreen(hWnd, width, height);
	}
}

