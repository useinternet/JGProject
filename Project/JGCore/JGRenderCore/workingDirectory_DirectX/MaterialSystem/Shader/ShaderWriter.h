#pragma once
#include"HlslEditor.h"

namespace JGRC
{
	class CORE_EXPORT ShaderWriter
	{
	private:
		std::vector<HlslEditor*> m_Editor;
	public:
		void AddEditor(HlslEditor* edit);
		bool Write(const std::string& Path);
	};
}
