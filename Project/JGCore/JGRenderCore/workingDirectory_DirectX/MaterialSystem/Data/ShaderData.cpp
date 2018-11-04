#include"ShaderData.h"
using namespace JGRC;
using namespace std;
D3D11_INPUT_ELEMENT_DESC* InputLayoutData::Get() 
{
	return &vDescs[0];
}
void* CBufferData::getData()
{
	Data.clear();
	for (auto& var : vVars)
	{
		for (auto& value : var.Value)
		{
			Data.push_back(value);
		}
	}
	return &Data[0];
}
real* CBufferData::getValue(const std::string& name)
{
	for (auto& var : vVars)
	{
		if (var.Name == name)
		{
			return &var.Value[0];
		}
	}
	return nullptr;
}
bool  CBufferData::setValue(const std::string& name, real* data)
{
	for (auto& var : vVars)
	{
		if (var.Name == name)
		{
			for (uint i = 0; i < var.Value.size(); ++i)
			{
				
				var.Value[i] = data[i];
			}
			return true;
		}
	}
	return false;
}
uint  CBufferData::getCount(const std::string& name)
{
	for (auto& var : vVars)
	{
		if (var.Name == name)
		{
			return var.Value.size();
		}
	}
	return 0;
}
D3D11_SAMPLER_DESC* SamplerStateData::GetDescs(const uint idx)
{
	return &vDescs[idx];
}
