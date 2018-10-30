#pragma once
#include"ShaderInformation.h"
namespace JGRC
{
	enum class EShaderType;
	// 상수 버퍼 정보 
	// 셰이더 타입
	// 상수 버퍼 총 갯수
	// map 형태로 저장
	// key는 상수 버퍼 이름
	// value는 상수 버퍼

	// 상수 버퍼 이름
	// 상수 버퍼 변수 이름
	// 상수 버퍼 변수 크기
	// 상수 버퍼 변수 실질적인 값

	// 사용 stl map
	// key는 변수 이름으로 
	class CORE_EXPORT CBufferVar
	{
	public:
		std::string       VarName;
		uint              VarSize;
		std::vector<float> vValue;
	};
	class CORE_EXPORT CBuffer
	{
	public:
		std::string CBufferName;
		std::map<uint, CBufferVar> mVars;
	public:
		uint size() const;
	};
	class CORE_EXPORT CBufferInformation : public ShaderInformation
	{
	private:
		std::map<uint, CBuffer> m_mCBuffers;
		//
		uint    m_varNum    = 0;
		uint    m_bufferNum = 0;
		CBuffer m_buffer;
	public:
		CBufferInformation();
		virtual ~CBufferInformation();

		virtual void AnalyzeSentence(std::string& sentence) override;
		const CBuffer& GetCBuffer(const uint idx);
	private:
		bool BlankCheck(const std::string& sentence);
		void CBufferClear();
	};

}
