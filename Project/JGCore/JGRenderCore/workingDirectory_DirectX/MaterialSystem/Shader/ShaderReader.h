#pragma once
#include"JGShader.h"

namespace JGRC
{
	class CORE_EXPORT ShaderReader
	{
	private:
		static class DirectX* m_Dx;
		HWND m_hWnd;

	public:
		ShaderReader(HWND hWnd);
		std::vector<JGShader*> ReadShader(const std::string& path);

	private:
		void Read(std::fstream& fin, JGShader* Shader);
	};
}


