#pragma once
#include"Common.h"


namespace Common
{
	typedef unsigned int IO_Result;
	enum class EIOType
	{
		Write  = 0x001,
		Read   = 0x002,
	};
	class Data_IO
	{
		struct Data_Header
		{
			std::uint32_t offset = 0;
			std::uint32_t size   = 0;
		};
	public:
		enum
		{
			IO_Result_TypeError = 0,
			IO_Result_PathError = 1,
			IO_Result_HeaderCountError = 2,
			IO_Result_Success = 3,
			IO_Result_DataSizeOver = 4
		};
	private:
		EIOType m_IOType;
		std::ofstream m_fout;
		std::ifstream m_fin;
	private:
		std::vector<Data_Header>   m_HeaderDatas;
		std::vector<unsigned char> m_ByteDatas;
		std::uint32_t m_Curr_Off = 0;
		std::uint32_t m_FileSize = 0;
		std::uint32_t m_DataSize = 0;
		std::uint32_t m_Curr_ByteData_Off = 0;
	public:
		Data_IO(EIOType type);
		Data_IO(const Data_IO& rhs) = default;
		Data_IO(Data_IO&& rhs)      = default;
		Data_IO& operator=(const Data_IO& rhs) = default;
		Data_IO& operator=(Data_IO&& rhs)      = default;
	public:
		IO_Result DataPush(const void* data, std::uint32_t dataSize);
		IO_Result Write(const std::string& path);
		IO_Result Read(const std::string& path);
	public:
		std::uint32_t GetFileSize() const {
			return m_FileSize;
		}
		std::uint32_t GetDataSize() const {
			return m_DataSize;
		}
		IO_Result GetData(void* Data, std::uint32_t dataSize);
	public:
		static std::string ErrorString(IO_Result error);
	};
}
// 문자열
Common::Data_IO& operator<<(Common::Data_IO& io, const std::string& str);
Common::Data_IO& operator>>(Common::Data_IO& io, std::string& str);



// 정수
Common::Data_IO& operator<<(Common::Data_IO& io, const bool& num);
Common::Data_IO& operator>>(Common::Data_IO& io, bool& num);

Common::Data_IO& operator<<(Common::Data_IO& io,const uint32_t& num);
Common::Data_IO& operator>>(Common::Data_IO& io, uint32_t& num);

Common::Data_IO& operator<<(Common::Data_IO& io,const float& num);
Common::Data_IO& operator>>(Common::Data_IO& io, float& num);

Common::Data_IO& operator<<(Common::Data_IO& io, const int& num);
Common::Data_IO& operator>>(Common::Data_IO& io, int& num);

Common::Data_IO& operator<<(Common::Data_IO& io, const DirectX::XMFLOAT4X4& mat);
Common::Data_IO& operator>>(Common::Data_IO& io, DirectX::XMFLOAT4X4& mat);