#pragma once

#include <map>
#include <string>
#include <vector>
#include "Defines/ReEnumDefines.h"


namespace RE
{
	class SDConstantBuffer;
	class SDSamplerState;
	class SDResource;
	class SDStructuredBuffer;

	class ShaderRegisterMap
	{
		enum
		{
			_s,
			_t,
			_b,
			_num
		};
		struct RegisterInfo
		{
			uint32_t start = 0;
			uint32_t num_space = 0;
			bool HasSpace(uint32_t space)
			{
				return num_space <= space;
			}
		};
	public:
		ShaderRegisterMap();
		void sRegister(SDSamplerState* sampler);
		void t2DRegister(SDResource* resource);
		void tCubeRegister(SDResource* resource);
		void tStructuredRegister(SDStructuredBuffer* sbuffer);
		void bRegister(SDConstantBuffer* cbuffer);

		void sDeRegister(const std::string& name);
		void t2DDeRegister(const std::string& name);
		void tCubeDeRegister(const std::string& name);
		void tStructuredDeRegister(const std::string& name);
		void bDeRegister(const std::string& name);
	private:
		// constantbuffer
		std::map<std::string, uint32_t> m_bRegisterMap;
		std::queue<uint32_t>            m_bPendingRegisterNumber;
		uint32_t                        m_bIndex = 0;
		// samplerstate
		std::map<std::string, uint32_t> m_sRegisterMap;
		std::queue<uint32_t>            m_sPendingRegisterNumber;
		uint32_t                        m_sIndex = 0;

		// texture 2d
		std::map<std::string, RegisterInfo> m_tRegisterMap;
		std::vector<RegisterInfo>           m_tPendingRegisterNumber;
		uint32_t                            m_tIndex = 0;

		
		// cube texture
		std::map<std::string, RegisterInfo> m_ctRegisterMap;
		std::vector<RegisterInfo>           m_ctPendingRegisterNumber;
		uint32_t                            m_ctIndex = 0;


		// structuredbuffer
		std::map<std::string, uint32_t> m_sbRegisterMap;
		std::queue<uint32_t>            m_sbPendingRegisterNumber;
		uint32_t                        m_sbIndex = 2;
	};
}