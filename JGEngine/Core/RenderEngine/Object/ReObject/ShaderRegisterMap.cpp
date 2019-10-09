#include "pch.h"
#include "ShaderRegisterMap.h"
#include "ShaderCode.h"
namespace RE
{
	ShaderRegisterMap::ShaderRegisterMap()
	{

	}
	void ShaderRegisterMap::sRegister(SDSamplerState* sampler)
	{
		if (m_sPendingRegisterNumber.empty())
		{
			m_sRegisterMap[sampler->GetName()] = m_sIndex;
			sampler->SetRegisterIndex(m_sIndex, 0);
			m_sIndex++;
		}
		else
		{
			uint32_t idx = m_sPendingRegisterNumber.front();
			m_sPendingRegisterNumber.pop();
			sampler->SetRegisterIndex(idx, 0);
			m_sRegisterMap[sampler->GetName()] = idx;
		}

	}
	void ShaderRegisterMap::t2DRegister(SDResource* resource)
	{
		if (m_tPendingRegisterNumber.empty())
		{
			RegisterInfo info;
			info.start = m_tIndex;
			info.num_space = resource->GetBindedResourceSize();
			resource->SetRegisterIndex(info.start, 0);
			m_tRegisterMap[resource->GetName()] = info;

			m_tIndex += info.num_space;
		}
		else
		{
			uint32_t idx = 0;
			for (auto& info : m_tPendingRegisterNumber)
			{
				if (info.HasSpace(resource->GetBindedResourceSize()))
				{
					break;
				}
				++idx;
			}

			if (m_tPendingRegisterNumber.size() == idx)
			{
				RegisterInfo info = m_tPendingRegisterNumber[idx];
				auto iter = m_tPendingRegisterNumber.begin();
				iter = iter + idx;
				m_tPendingRegisterNumber.erase(iter);



				uint32_t deltaSpace = info.num_space - resource->GetBindedResourceSize();
				if (deltaSpace == 0)
				{
					resource->SetRegisterIndex(info.start, 0);
					m_tRegisterMap[resource->GetName()] = info;
				}
				else
				{
					RegisterInfo remindInfo;
					remindInfo.start = info.start;
					remindInfo.num_space = deltaSpace;

					resource->SetRegisterIndex(remindInfo.start, 0);
					m_tRegisterMap[resource->GetName()] = remindInfo;


					remindInfo.start = info.start + deltaSpace;
					remindInfo.num_space = info.num_space - deltaSpace;
					m_tPendingRegisterNumber.push_back(remindInfo);
				}
			}
			else
			{
				RegisterInfo info;
				info.start = m_tIndex;
				info.num_space = resource->GetBindedResourceSize();
				resource->SetRegisterIndex(info.start, 0);
				m_tRegisterMap[resource->GetName()] = info;

				m_tIndex += info.num_space;

			}
		}
	}
	void ShaderRegisterMap::tCubeRegister(SDResource* resource)
	{

		if (m_ctPendingRegisterNumber.empty())
		{
			RegisterInfo info;
			info.start = m_tIndex;
			info.num_space = resource->GetBindedResourceSize();
			resource->SetRegisterIndex(info.start, 1);
			m_ctRegisterMap[resource->GetName()] = info;

			m_ctIndex += info.num_space;
		}
		else
		{
			uint32_t idx = 0;
			for (auto& info : m_ctPendingRegisterNumber)
			{
				if (info.HasSpace(resource->GetBindedResourceSize()))
				{
					break;
				}
				++idx;
			}

			if (m_ctPendingRegisterNumber.size() == idx)
			{
				RegisterInfo info = m_ctPendingRegisterNumber[idx];
				auto iter = m_ctPendingRegisterNumber.begin();
				iter = iter + idx;
				m_ctPendingRegisterNumber.erase(iter);



				uint32_t deltaSpace = info.num_space - resource->GetBindedResourceSize();
				if (deltaSpace == 0)
				{
					resource->SetRegisterIndex(info.start, 1);
					m_ctRegisterMap[resource->GetName()] = info;
				}
				else
				{
					RegisterInfo remindInfo;
					remindInfo.start = info.start;
					remindInfo.num_space = deltaSpace;

					resource->SetRegisterIndex(remindInfo.start, 1);
					m_ctRegisterMap[resource->GetName()] = remindInfo;


					remindInfo.start = info.start + deltaSpace;
					remindInfo.num_space = info.num_space - deltaSpace;
					m_ctPendingRegisterNumber.push_back(remindInfo);
				}
			}
			else
			{
				RegisterInfo info;
				info.start = m_tIndex;
				info.num_space = resource->GetBindedResourceSize();
				resource->SetRegisterIndex(info.start, 1);
				m_ctRegisterMap[resource->GetName()] = info;

				m_ctIndex += info.num_space;

			}
		}


	}
	void ShaderRegisterMap::tStructuredRegister(SDStructuredBuffer* sbuffer)
	{
		if (m_sbPendingRegisterNumber.empty())
		{
			m_sbRegisterMap[sbuffer->GetName()] = m_sbIndex;
			sbuffer->SetRegisterIndex(0, m_sbIndex);
			m_sbIndex++;
		}
		else
		{
			uint32_t idx = m_sbPendingRegisterNumber.front();
			m_sbPendingRegisterNumber.pop();
			sbuffer->SetRegisterIndex(0, idx);
			m_sbRegisterMap[sbuffer->GetName()] = idx;
		}
	}
	void ShaderRegisterMap::bRegister(SDConstantBuffer* cbuffer)
	{
		if (m_bPendingRegisterNumber.empty())
		{
			m_bRegisterMap[cbuffer->GetName()] = m_bIndex;
			cbuffer->SetRegisterIndex(m_bIndex, 0);
			m_bIndex++;
		}
		else
		{
			uint32_t idx = m_bPendingRegisterNumber.front();
			m_bPendingRegisterNumber.pop();
			cbuffer->SetRegisterIndex(idx, 0);
			m_bRegisterMap[cbuffer->GetName()] = idx;
		}
	}

	void ShaderRegisterMap::sDeRegister(const std::string& name)
	{
		m_sPendingRegisterNumber.push(m_sRegisterMap[name]);
		m_sRegisterMap.erase(name);
	}
	void ShaderRegisterMap::t2DDeRegister(const std::string& name)
	{
		m_tPendingRegisterNumber.push_back(m_tRegisterMap[name]);
		m_tRegisterMap.erase(name);
	}
	void ShaderRegisterMap::tCubeDeRegister(const std::string& name)
	{
		m_ctPendingRegisterNumber.push_back(m_ctRegisterMap[name]);
		m_ctRegisterMap.erase(name);
	}
	void ShaderRegisterMap::tStructuredDeRegister(const std::string& name)
	{
		m_sbPendingRegisterNumber.push(m_sbRegisterMap[name]);
		m_sbRegisterMap.erase(name);
	}
	void ShaderRegisterMap::bDeRegister(const std::string& name)
	{
		m_bPendingRegisterNumber.push(m_bRegisterMap[name]);
		m_bRegisterMap.erase(name);
	}

}