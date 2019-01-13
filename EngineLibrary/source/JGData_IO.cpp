#include"JGData_IO.h"


using namespace std;
using namespace JGLibrary;


JGData_IO::JGData_IO(const EData_IO_Mode IOmode)
{
	m_ioMode  = IOmode;
}
JGData_IO::JGData_IO(const JGData_IO& io)
{
	m_ioMode      = io.m_ioMode;
	m_headerData  = io.m_headerData;
	m_data        = io.m_data;
	m_curr_off    = io.m_curr_off;
	m_fileSize    = io.m_fileSize;
	m_headerCount = io.m_headerCount;
	m_dataSize    = io.m_dataSize;

}
JGData_IO::JGData_IO(JGData_IO&& io)
{
	m_ioMode  = io.m_ioMode;
	m_headerData = move(io.m_headerData);
	m_data       = move(io.m_data);
	m_curr_off    = io.m_curr_off;
	m_fileSize    = io.m_fileSize;
	m_headerCount = io.m_headerCount;
	m_dataSize    = io.m_dataSize;

	//

	io.m_ioMode = EData_IO_Mode::Read; 
	io.m_headerData.clear();            io.m_data.clear();
	io.m_curr_off = 0;                  io.m_fileSize = sizeof(jg_size);
	io.m_headerCount = 0;               io.m_dataSize = 0;
}

JGData_IO& JGData_IO::operator=(const JGData_IO& io)
{
	m_ioMode     = io.m_ioMode;
	m_headerData = io.m_headerData;
	m_data       = io.m_data;
	m_curr_off   = io.m_curr_off;
	m_fileSize   = io.m_fileSize;
	m_headerCount = io.m_headerCount;
	m_dataSize   = io.m_dataSize;
	return *this;
}
JGData_IO& JGData_IO::operator=(JGData_IO&& io)
{
	m_ioMode     = io.m_ioMode;
	m_headerData = move(io.m_headerData);
	m_data       = move(io.m_data);
	m_curr_off   = io.m_curr_off;
	m_fileSize   = io.m_fileSize;
	m_headerCount = io.m_headerCount;
	m_dataSize   = io.m_dataSize;

	//
	io.m_ioMode = EData_IO_Mode::Read;  
	io.m_headerData.clear();            io.m_data.clear();
	io.m_curr_off = 0;                  io.m_fileSize = sizeof(jg_size);
	io.m_headerCount = 0;               io.m_dataSize = 0;
	return *this;
}
bool JGData_IO::DataPush(void* data, const jg_size dataSize)
{
	if (m_ioMode == EData_IO_Mode::Read)
	{
		return false;
	}
	// 데이터 헤더 정보 저장
	Data_Header dh = { m_curr_off, dataSize };
	m_headerData.push_back(dh);
	m_curr_off += dataSize;


	// byte 정보로 변환
	vector<byte> btArr(dataSize);
	memcpy_s(&btArr[0],(rsize_t)dataSize, data, (rsize_t)dataSize);

	// 저장
	for (auto& iter : btArr)
	{
		m_data.push_back(iter);
	}

	// 각종 정보 저장
	m_headerCount = m_headerData.size();
	m_dataSize += dataSize;
	m_fileSize += (sizeof(Data_Header) + dataSize);
	return true;
}
bool JGData_IO::Write(const std::string& path)
{
	if (m_ioMode == EData_IO_Mode::Read)
	{
		return false;
	}
	m_fout.open(path, ios::binary);
	if (!m_fout.is_open())
	{
		return false;
	}
	m_fout.write(reinterpret_cast<const char*>(&m_headerCount), sizeof(jg_size));
	if (m_headerCount == 0)
	{
		m_fout.close();
		return true;
	}
	m_fout.write(reinterpret_cast<const char*>(&m_headerData[0]), sizeof(Data_Header) * m_headerData.size());
	m_fout.write(reinterpret_cast<const char*>(&m_data[0]), sizeof(byte) * m_data.size());
	m_fout.close();
	return true;
}
bool JGData_IO::Read(const std::string& path)
{
	if (m_ioMode == EData_IO_Mode::Write)
	{
		return false;
	}

	m_fin.open(path, ios::binary);
	if (!m_fin.is_open())
	{
		return false;
	}
	// 헤더데이터 정보 사이즈  read
	m_fin.read(reinterpret_cast<char*>(&m_headerCount), sizeof(jg_size));
	if (m_headerCount == 0)
	{
		m_fin.close();
		return true;
	}
	m_fileSize += sizeof(Data_Header) * m_headerCount;
	// 사이즈 만큼 할당 후 데이터 헤더 정보 read
	m_headerData.reserve(m_headerCount);
	m_headerData.resize(m_headerCount);
	m_fin.read(reinterpret_cast<char*>(&m_headerData[0]), sizeof(Data_Header) * m_headerCount);
	for (auto& h : m_headerData)
	{
		m_dataSize += h.size;
	}
	m_fileSize += m_dataSize;
	// 파일 사이즈 만큼 할당후 byte단위로 읽기
	m_data.reserve(m_dataSize);
	m_data.resize(m_dataSize);
	m_fin.read(reinterpret_cast<char*>(&m_data[0]), m_dataSize);


	m_fin.close();
	return true;
}
const jg_size JGData_IO::GetFileSize() const
{
	return m_fileSize;
}
const jg_size JGData_IO::GetDataSize() const
{
	return m_dataSize;
}
const byte* JGData_IO::GetData() const
{
	return &m_data[0];
}
void JGData_IO::DataInfo_printcpp() const
{
	cout << "FileSize : " << m_fileSize << endl;
	cout << "HeaderInfo Count : " << m_headerCount << endl;
	cout << "HeaderInfo DataSize : " << sizeof(Data_Header) * m_headerCount <<"bt"<< endl;
	cout << "Data Size : " << m_dataSize << "bt" << endl;
}