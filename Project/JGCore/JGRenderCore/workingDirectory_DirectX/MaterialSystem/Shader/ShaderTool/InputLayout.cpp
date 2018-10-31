#include"InputLayout.h"
#include"DirectX/DirectX.h"
#include"DirectX/JGDevice.h"
using namespace JGRC;
using namespace Microsoft::WRL;
InputLayout::InputLayout()
{

}
InputLayout::~InputLayout()
{

}
void InputLayout::AddInputLayout(const char* SemanticName, const UINT SemanticIndex, const DXGI_FORMAT format,
	const UINT InputSlot, const UINT Offset, const D3D11_INPUT_CLASSIFICATION InputSlotClass, const UINT InstanceDataStepRate)
{
	D3D11_INPUT_ELEMENT_DESC desc;
	desc.SemanticName = SemanticName;
	desc.SemanticIndex = SemanticIndex;
	desc.Format = format;
	desc.InputSlot = InputSlot;
	desc.AlignedByteOffset = Offset;
	desc.InputSlotClass = InputSlotClass;
	desc.InstanceDataStepRate = InstanceDataStepRate;
	m_vLayoutDescs.push_back(desc);
}
D3D11_INPUT_ELEMENT_DESC* InputLayout::Get()
{
	return &m_vLayoutDescs[0];
}
UINT InputLayout::Size()
{
	return (UINT)m_vLayoutDescs.size();
}