#pragma once
#include"JGShader.h"


namespace JGRC
{
	enum class CORE_EXPORT EShaderRenderType
	{
		Draw,
		DrawIndexed
	};
	class CORE_EXPORT JGShaderArray : public JGRCObject
	{
	private:
		typedef std::map<EShaderType, JGShader*> mShaderArray;
		typedef std::pair<EShaderType, JGShader*> ShaderArrayPair;
		mShaderArray m_mShaders;
	private:
		JGShaderArray(const JGShaderArray& copy) = delete;
		JGShaderArray(JGShaderArray&& shaderarr) = delete;
		JGShaderArray& operator=(const JGShaderArray& shaderarr) = delete;
		JGShaderArray& operator=(JGShaderArray&& shaderarr)      = delete;
	public:
		JGShaderArray();
		~JGShaderArray();
		JGShader* Get(const EShaderType type);
		void AddShader(JGShader* shader);
		void Render(const uint indexCount, const EShaderRenderType type = EShaderRenderType::DrawIndexed);
		void Move(JGShaderArray* mover);
	};
}