#pragma once
#include "Object/ReObject.h"
#define NAME_SHADERLIB
#ifndef NAME_SHADERLIB 
#define NAME_SHADERLIB


#endif
namespace RE
{
	class STStruct;
	class ShaderLibManager;
	class ShaderLib : public ReObject
	{

	private:
		virtual bool Load(const std::string& path) = 0;
		virtual bool Save(const std::string& path) = 0;
		virtual std::string GetCode() const = 0;
		virtual std::string GetSuccessedCode() const = 0;
	};

	class ShaderTypeLib : public ShaderLib
	{
		friend ShaderLibManager;
	public:
		STStruct* AddStruct(const std::string& struct_name);
		STStruct* FindStruct(const std::string& struct_name);
		std::shared_ptr<STStruct>  CloneStruct(const std::string& struct_name);
		void SetSortingOrder(const std::string& struct_name, uint32_t sorting_order);
		void RemoveStruct(const std::string& struct_name);
	private:
		virtual bool Load(const std::string& path) override;
		virtual bool Save(const std::string& path) override;
		virtual std::string GetCode() const        override;
		virtual std::string GetSuccessedCode() const override;
	private:
		std::unordered_map<std::string, std::shared_ptr<STStruct>> m_ShaderStructPool;
		std::map<uint32_t,STStruct*>   m_SortingOrderMap;
		std::string m_SuccessCode;
		bool m_IsSuccess = true;
		uint32_t m_SortingIndex = 0;
	};

	class ShaderFunctionLib : public ShaderLib
	{
		friend ShaderLibManager;
	};

	class ShaderLibManager : public ReObject
	{
	public:
		ShaderLibManager();
		void Load(const std::string& path);
		void Save(const std::string& path);

		ShaderTypeLib*     GetTypeLib();
		//ShaderFunctionLib* GetFuncLib();
		std::string GetCode() const;
	private:
		std::shared_ptr<ShaderTypeLib>     m_ShaderTypeLib;
		//std::shared_ptr<ShaderFunctionLib> m_ShaderFuncLib;
	};
}