#include "pch.h"
#include "FileIO.h"



namespace JG
{
	//void FileStream::Header::Write(std::ofstream& fout)
	//{
	//	u64 offset = CalcSize();
	//	fout.write(reinterpret_cast<const char*>(&offset), sizeof(u64));


	//	u64 dataCnt = DataMap.size(); //u64
	//	fout.write(reinterpret_cast<const char*>(&dataCnt), sizeof(u64));
	//	for (auto& _pair : DataMap)
	//	{
	//		u64 keyLen = _pair.first.length();
	//		const String& key = _pair.first;
	//		u64 dataPos = _pair.second;
	//		fout.write(reinterpret_cast<const char*>(&keyLen), sizeof(u64));
	//		fout.write(reinterpret_cast<const char*>(key.data()), sizeof(wchar_t) * keyLen);
	//		fout.write(reinterpret_cast<const char*>(&dataPos), sizeof(u64));
	//	} // 44
	//	// 28
	//}

	//void FileStream::Header::Read(std::ifstream& fin)
	//{
	//	fin.read(reinterpret_cast<char*>(&Offset), sizeof(u64));

	//	u64 dataCnt = 0;
	//	fin.read(reinterpret_cast<char*>(&dataCnt), sizeof(u64));

	//	for (u64 i = 0; i < dataCnt; ++i)
	//	{
	//		u64 keyLen = 0;
	//		String key;
	//		u64 dataPos = 0;

	//		fin.read(reinterpret_cast<char*>(&keyLen), sizeof(u64));
	//		key.resize(keyLen);
	//		fin.read(reinterpret_cast<char*>(key.data()), sizeof(wchar_t) * keyLen);
	//		fin.read(reinterpret_cast<char*>(&dataPos), sizeof(u64));

	//		if (keyLen > 0)
	//		{
	//			DataMap.emplace(key, dataPos);
	//		}
	//	}
	//}

	//u64 FileStream::Header::GetOffset() const
	//{
	//	return Offset;
	//}

	//u64 FileStream::Header::CalcSize() const
	//{
	//	u64 size = (sizeof(u64) * 2);
	//	for (auto& _pair : DataMap)
	//	{
	//		size += (sizeof(u64) * 2);
	//		size += (sizeof(wchar_t) * _pair.first.length());
	//	}
	//	return size;
	//}




	//FileStreamWriter::FileStreamWriter()
	//{

	//}
	//FileStreamWriter::~FileStreamWriter()
	//{
	//	mFout.close();
	//}


	//bool FileStreamWriter::Open(const String& path, bool isAppend)
	//{
	//	if (isAppend)
	//	{
	//		mFout.open(path, std::ios::out | std::ios::binary | std::ios::app);
	//	}
	//	else
	//	{
	//		mFout.open(path, std::ios::out | std::ios_base::binary);
	//	}
	//	
	//	
	//	mOffset = 0;
	//	return mFout.is_open();
	//}
	//void FileStreamWriter::Close()
	//{
	//	if (mFout.is_open())
	//	{
	//		mHeader.Write(mFout);
	//		for (auto& data : mDatas)
	//		{
	//			mFout.write(reinterpret_cast<const char*>(data.ByteData.data()), data.ByteData.size());
	//		}
	//		mFout.close();
	//	}
	//}
	//void FileStreamWriter::Write(const void* data, u64 size)
	//{
	//	Data fileData;
	//	fileData.ByteData.resize(size);

	//	memcpy(fileData.ByteData.data(), data, size);
	//	mDatas.push_back(fileData);
	//	mOffset += size;
	//}
	//bool FileStreamReader::Open(const String& path)
	//{
	//	mFin.open(path, std::ios::in | std::ios::binary);
	//	bool is_open = mFin.is_open();

	//	if (is_open == true)
	//	{
	//		mHeader.Read(mFin);
	//	}

	//	return mFin.is_open();
	//}
	//void FileStreamReader::Close()
	//{
	//}
	//void FileStreamReader::Read(void* data, u64 size)
	//{
	//	mFin.read(reinterpret_cast<char*>(data), size);
	//}

}