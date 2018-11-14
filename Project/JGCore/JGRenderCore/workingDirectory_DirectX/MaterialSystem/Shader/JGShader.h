#pragma once
#include"HlslEditor.h"
#include"BufferSystem/JGBuffer.h"
namespace JGRC
{
	class CORE_EXPORT JGShader : public JGRCObject
	{
	private:
		friend class ShaderReader;
	public:
		/*
		셰이더 컴파일 셋팅 변수 */
		static std::string FunctionName;
		static std::string VSVersion;
		static std::string PSVersion;
		static UINT Flags;
	private:
		EShaderType m_ShaderType;
		void* ShaderBuffer;
		/*
		hlsl파일 경로 와 상수 버퍼*/
		std::string m_Path = "None";
		std::vector<JGBuffer*> m_ConstantBuffer;
		/*
		셰이더를 구성하고 있는 데이터 */
		InputLayout* m_InputLayout   = nullptr;
		CBufferArray m_CBufferValue;
		Texture2D*   m_Texture2D     = nullptr;
		SamplerState* m_SamplerState = nullptr;
	public:
		JGShader() {}
		virtual ~JGShader();
		/*
		Exp : 현재 셰이더가 참조하고있는 hlsl 경로를 가져온다. */
		const std::string& GetPath() { return m_Path; }
		/*
		Exp : 변수를 가져온다. 변수의 크기를 반환한다. 
		@param VarName : 변수 이름 
		@param outData : 출력 데이터 */
		uint  GetParam(const std::string& VarName, real* outData);
		void  SetParam(const std::string& VarName, void* data, const uint Size);
		void  Render();
	private:
		void SetPath(const std::string& path) { m_Path = path; }
		void SetShaderType(const EShaderType type) { m_ShaderType = type; }
		InputLayout*  GetLayout();
		CBufferArray* GetCBufferArray();
		Texture2D*    GetTexture2D();
		SamplerState* GetSamplerState();
		bool RegisterBuffer();
		bool Compile(HWND hWnd);
	};
}
