#include<PCH.h>
#include"Buffer.h"

using namespace Dx12;
using namespace std;


Buffer::Buffer(const std::string& name) : 
	Resource(name)
{ }
Buffer::Buffer(
	const D3D12_RESOURCE_DESC& Desc,
	uint32_t numElements,
	uint32_t elementSize,
	const std::string& name) :
	Resource(Desc, nullptr,  name)
{  }
Buffer::~Buffer() {}
void Buffer::CreateViews(uint32_t numElements, uint32_t elementSize)
{
	DX12_LOG_ERROR("This is Buffer Class name : %s", m_Name.c_str());
	return;
}