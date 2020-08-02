#include "pch.h"
#include "DX12_RenderingPass.h"

using namespace std;


namespace DX12
{
	bool DX12_RenderingPass::SetUp(void* userData)
	{
		auto& setUp = *(PassSetUpData*)userData;
		
		m_Name = setUp.name;
		m_ShaderPath = setUp.shaderPath;
		if (setUp.ownerRenderer == nullptr)
		{
			GELOG_ERROR("%s 의 OwnerRenderer가 nullptr", s2ws(m_Name).c_str());
		}
		m_Flag = setUp.flag;
		m_OwnerRenderer = setUp.ownerRenderer;
		m_RunFunc = setUp.runFunc;
		return setUp.setUpFunc(m_RenderTarget, m_PSO);
	}
	bool DX12_RenderingPass::Run(void* userData)
	{
		PassRunData& data = *(PassRunData*)userData;

		if (m_RunFunc) m_RunFunc(this, data);
		else
		{
			GELOG_ERROR("%s 의 RunFunc가 바인딩 되지않음.", s2ws(m_Name).c_str());
			return false;
		}
		return true;
	}
	
	void DX12_RenderingPass::SetName(const std::string& name)
	{
		m_Name = name;
	}
	const std::string& DX12_RenderingPass::GetName() const
	{
		return m_Name;
	}

}