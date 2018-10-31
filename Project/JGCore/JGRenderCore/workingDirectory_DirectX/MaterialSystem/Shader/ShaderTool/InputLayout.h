#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
// 나중에 바꿀예정
namespace JGRC
{
	class InputLayout
	{
	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_vLayoutDescs;
	public:
		void AddInputLayout(const char* SemanticName, const UINT SemanticIndex, const DXGI_FORMAT format,
			const UINT InputSlot, const UINT Offset, const D3D11_INPUT_CLASSIFICATION InputSlotClass, const UINT InstanceDataStepRate);
	public:
		InputLayout();
		~InputLayout();
		D3D11_INPUT_ELEMENT_DESC* Get();
		UINT Size();
	};
}