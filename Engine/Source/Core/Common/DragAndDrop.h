#pragma once
#include "Define.h"
#include "Type.h"
#include "Object.h"





namespace JG
{
	class IDragAndDropData : public IJGObject
	{
		JGCLASS
	public:
		virtual ~IDragAndDropData() = default;
		virtual void GetData(List<jbyte>& byteData) = 0;
		virtual void SetData(void* data) = 0;
	};

	class DDDContentsFile : public IDragAndDropData
	{
		JGCLASS
	public:
		String FilePath;
	public:
		virtual ~DDDContentsFile() = default;
		virtual void GetData(List<jbyte>& byteData) override
		{
			u64 offset = 0;
			u64 len = FilePath.length();
			u64 btSize = len + sizeof(u64);
		
			byteData.resize(btSize);

			memcpy(&byteData[offset], &len, sizeof(u64)); offset += sizeof(u64);
			memcpy(&byteData[offset], FilePath.c_str(), len);

		}
		virtual void SetData(void* data) override
		{
			u64 offset = 0;
			u64 len = 0;


			memcpy(&len, (void*)((u64)data + offset), sizeof(u64)); offset += sizeof(u64);
			FilePath.resize(len);
			memcpy(&FilePath[0], (void*)((u64)data + offset), len);
		}

	};

	class GameNode;
	class DDDGameNode : public IDragAndDropData
	{
		JGCLASS
	public:

		List<GameNode*> GameNodeList;
	public:
		virtual ~DDDGameNode() = default;
		virtual void GetData(List<jbyte>& byteData) override
		{
			u64 offset = 0;
			u64 len = GameNodeList.size();
			u64 btSize = len * sizeof(GameNode*) + sizeof(u64);

			byteData.resize(btSize);
			memcpy(&byteData[offset], &len, sizeof(u64)); offset += sizeof(u64);
			memcpy(&byteData[offset], GameNodeList.data(), len * sizeof(GameNode*));
		}
		virtual void SetData(void* data) override
		{

			u64 offset = 0;
			u64 len = 0;


			memcpy(&len, (void*)((u64)data + offset), sizeof(u64)); offset += sizeof(u64);
			GameNodeList.resize(len);
			memcpy(&GameNodeList[0], (void*)((u64)data + offset), len * sizeof(GameNode*));
		}
	};

}
