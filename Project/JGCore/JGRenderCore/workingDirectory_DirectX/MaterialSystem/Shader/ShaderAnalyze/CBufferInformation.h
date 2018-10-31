#pragma once
#include"ShaderInformation.h"
namespace JGRC
{
	enum class EShaderType;


	/*
	Exp : CBuffer안에 들어있는 변수 */
	class CORE_EXPORT CBufferVar
	{
	public:
		std::string       VarName;
		uint              VarSize;
		std::vector<float> vValue;
	};
	/*
	Exp : 상수 버퍼 */
	class CORE_EXPORT CBuffer
	{
	public:
		std::string CBufferName;
		std::map<uint, CBufferVar> mVars;
	public:
		uint  size() const;
		void  getData(std::vector<float>* dataArray);
	};
	class CORE_EXPORT CBufferInformation : public ShaderInformation
	{
	private:
		std::map<uint, CBuffer> m_mCBuffers;    // 상수 버퍼 배열
		// 
		uint    m_varNum    = 0;                // 변수 번호
		uint    m_bufferNum = 0;                // 버퍼 번호
		CBuffer m_buffer;                       // 저장할 상수 버퍼 변수
	public:
		CBufferInformation();
		virtual ~CBufferInformation();

		virtual void AnalyzeSentence(std::string& sentence) override;
		virtual bool Decryptable(const std::string& sentence) override;
		/*
		Exp : 상수 버퍼 정보를 얻어 온다. 
		@param idx : 버퍼 인덱스 */
		CBuffer* GetCBuffer(const uint idx);


		float*   GetParam(const std::string& paramName);
		void     SetParam(const std::string& paramName, void* value);
		uint     GetParamSize(const std::string& paramName);
		uint     GetCBufferCount();
	private:
		bool BlankCheck(const std::string& sentence);
		void CBufferClear();
		CBufferVar* FindVar(const std::string& name);
	};

}
