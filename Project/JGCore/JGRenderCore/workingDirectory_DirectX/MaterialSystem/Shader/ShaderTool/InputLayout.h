#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
// 나중에 바꿀예정
namespace JGRC
{
	class InputLayout
	{
	private:
		static std::vector<D3D11_INPUT_ELEMENT_DESC> m_vLayoutDescs;
	private:
		InputLayout(const InputLayout& copy) = delete;
		InputLayout& operator=(const InputLayout& IL) = delete;
	public:
		static void AddInputLayout(const char* SemanticName, const UINT SemanticIndex, const DXGI_FORMAT format,
			const UINT InputSlot, const UINT Offset, const D3D11_INPUT_CLASSIFICATION InputSlotClass, const UINT InstanceDataStepRate);
	public:
		InputLayout();
		~InputLayout();
		static D3D11_INPUT_ELEMENT_DESC* Get();
		static UINT Size();
	};
}