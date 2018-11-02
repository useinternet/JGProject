#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"Shader/ShaderTool/ShaderObject.h"
namespace JGRC
{
	class CORE_EXPORT MaterialReader
	{
	private:
		std::unique_ptr<std::fstream> m_Stream;

	public:
		MaterialReader();
		~MaterialReader();

		ShaderObject* Read(const std::string& materialPath);
	};
}
