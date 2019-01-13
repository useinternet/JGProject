#pragma once
#include"Common.h"

namespace JGLibrary
{
	typedef unsigned long long jg_offset;
	typedef unsigned long long jg_size;
	typedef unsigned long long jg_datapos;
	typedef unsigned int jg_ID;

	/*
	Exp : 데이터 입출력 모드 */
	enum class EData_IO_Mode
	{
		Write, // 쓰기
		Read   // 읽기
	};
	/*
	Exp : 데이터 헤더 정보 */
	typedef struct Data_Header
	{
		jg_offset offset = 0; // 위치
		jg_size   size   = 0; // 사이즈
	}Data_Header;

	class JGData_IO
	{
	private:
		EData_IO_Mode  m_ioMode;  // I/O  모드
		std::ofstream  m_fout;    
		std::ifstream  m_fin;
	private:
		//std::vector<jg_ID>       m_ID;
		//std::unordered_map<jg_ID, Data_Header> m_umHeaders;
		std::vector<Data_Header> m_headerData; // 데이터 헤더 정보 배열
		std::vector<byte> m_data;              // 실질적인 데이터
		jg_datapos        m_curr_off    = 0;               // 현재 오프셋
		jg_size           m_fileSize    = sizeof(jg_size); // 파일 사이즈
		jg_size           m_headerCount = 0;              // 헤더 갯수
		jg_size           m_dataSize    = 0;               // 데이터 사이즈
	public:
		JGData_IO(const EData_IO_Mode IOmode);
		JGData_IO(const JGData_IO& io);
		JGData_IO(JGData_IO&& io);
	public:
		JGData_IO& operator=(const JGData_IO& io);
		JGData_IO& operator=(JGData_IO&& io);
	public:
		/*
		@param data     : 푸쉬할 데이터 
		@param dataSize : 푸쉬할 데이터 사이즈 */
		bool DataPush(void* data, const jg_size dataSize);
		bool Write(const std::string& path);
		bool Read(const std::string& path);
	public:
		const jg_size GetFileSize() const;
		const jg_size GetDataSize() const;
		const byte*   GetData() const;
	public:
		void DataInfo_printcpp() const;
	};
}
