#include<PCH.h>
#include"Data_IO.h"
using namespace Common;
using namespace std;

Data_IO::Data_IO(EIOType type) : m_IOType(type) {}
IO_Result Data_IO::DataPush(const void* data, uint32_t dataSize)
{
	if (m_IOType == EIOType::Read) {
		return IO_Result_TypeError;
	}

	// 데이터 헤더 정보 저장
	Data_Header dh = { m_Curr_Off, dataSize };
	m_HeaderDatas.push_back(dh);
	m_Curr_Off += dataSize;

	// byte 정보로 변환 및 저장
	vector<unsigned char> btArr(dataSize);
	memcpy_s(&btArr[0], (rsize_t)dataSize, data, (rsize_t)dataSize);
	m_ByteDatas.insert(m_ByteDatas.end(), btArr.begin(), btArr.end());


	m_DataSize += dataSize;
	m_FileSize += (sizeof(Data_Header) + dataSize);

	return IO_Result_Success;
}
IO_Result Data_IO::Write(const string& path)
{
	uint32_t headerCount = (uint32_t)m_HeaderDatas.size();


	if (m_IOType == EIOType::Read) {
		return IO_Result_TypeError;
	}
	else if (headerCount == 0){
		return IO_Result_HeaderCountError;
	}

	m_fout.open(path, ios::binary);
	if (!m_fout.is_open()) {
		return IO_Result_PathError;
	}


	m_fout.write(
		reinterpret_cast<const char*>(&headerCount),
		sizeof(uint32_t));
	
	m_fout.write(
		reinterpret_cast<const char*>(m_HeaderDatas.data()),
		sizeof(Data_Header) * headerCount);

	m_fout.write(
		reinterpret_cast<const char*>(m_ByteDatas.data()), 
		sizeof(unsigned char) * m_ByteDatas.size());

	m_fout.close();
	return IO_Result_Success;
}
IO_Result Data_IO::Read(const string& path)
{
	if (m_IOType == EIOType::Write) {
		return IO_Result_TypeError;
	}

	m_fin.open(path, ios::binary);
	if (!m_fin.is_open()){
		return IO_Result_PathError;
	}

	uint32_t headerCount = 0;
	m_fin.read(
		reinterpret_cast<char*>(&headerCount),
		sizeof(uint32_t));

	m_FileSize += sizeof(Data_Header) * headerCount;
	m_HeaderDatas.resize(headerCount);


	m_fin.read(
		reinterpret_cast<char*>(m_HeaderDatas.data()),
		sizeof(Data_Header) * headerCount);

	for (auto& h : m_HeaderDatas) {
		m_DataSize += h.size;
	}

	m_FileSize += m_DataSize;


	m_ByteDatas.resize(m_DataSize);
	m_fin.read(
		reinterpret_cast<char*>(m_ByteDatas.data()), 
		m_DataSize);

	m_fin.close();
	return IO_Result_Success;

}
IO_Result Data_IO::GetData(void* Data, uint32_t dataSize)
{
	if (m_Curr_ByteData_Off + dataSize > m_ByteDatas.size())
		return IO_Result_DataSizeOver;

	memcpy_s(
		Data,
		(rsize_t)dataSize,
		&m_ByteDatas[m_Curr_ByteData_Off],
		(rsize_t)dataSize);
	m_Curr_ByteData_Off += dataSize;


	return IO_Result_Success;
}
string Data_IO::ErrorString(IO_Result error)
{
	switch (error)
	{
	case IO_Result_TypeError:
		return "The IO type is incorrect.";
	case IO_Result_PathError:
		return "Path not found.";
	case IO_Result_HeaderCountError:
		return "Header information is not exist";
	case IO_Result_Success:
		return "Success";
	case IO_Result_DataSizeOver:
		return "Data size exceeded";
	default:
		return "???";
	}
}

Common::Data_IO& operator<<(Common::Data_IO& io, const std::string& str)
{
	uint32_t strSize = str.length();
	io.DataPush(&strSize, sizeof(uint32_t));
	io.DataPush(str.data(), sizeof(char) * strSize);
	return io;
}
Common::Data_IO& operator>>(Common::Data_IO& io, std::string& str)
{
	uint32_t len = 0;
	io.GetData(&len, sizeof(uint32_t));
	str.resize(len);
	io.GetData(&str[0], sizeof(char) * len);
	return io;
}


Common::Data_IO& operator<<(Common::Data_IO& io, const bool& num)
{
	io.DataPush(&num, sizeof(bool));
	return io;
}
Common::Data_IO& operator>>(Common::Data_IO& io, bool& num)
{
	io.GetData(&num, sizeof(bool));
	return io;
}


Common::Data_IO& operator<<(Common::Data_IO& io,const uint32_t& num)
{
	io.DataPush(&num, sizeof(uint32_t));
	return io;
}
Common::Data_IO& operator>>(Common::Data_IO& io, uint32_t& num)
{
	io.GetData(&num, sizeof(uint32_t));
	return io;
}

Common::Data_IO& operator<<(Common::Data_IO& io, const float& num)
{
	io.DataPush(&num, sizeof(float));
	return io;
}
Common::Data_IO& operator>>(Common::Data_IO& io, float& num)
{
	io.GetData(&num, sizeof(float));
	return io;
}

Common::Data_IO& operator<<(Common::Data_IO& io, const int& num)
{
	io.DataPush(&num, sizeof(int));
	return io;
}
Common::Data_IO& operator>>(Common::Data_IO& io, int& num)
{
	io.GetData(&num, sizeof(int));
	return io;
}

Common::Data_IO& operator<<(Common::Data_IO& io, const DirectX::XMFLOAT4X4& mat)
{
	io.DataPush(&mat, sizeof(DirectX::XMFLOAT4X4));
	return io;
}
Common::Data_IO& operator>>(Common::Data_IO& io, DirectX::XMFLOAT4X4& mat)
{
	io.GetData(&mat, sizeof(DirectX::XMFLOAT4X4));
	return io;
}