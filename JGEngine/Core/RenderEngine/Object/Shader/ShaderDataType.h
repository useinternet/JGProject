#pragma once
#include "Object/ReObject.h"
#include "ShaderDefined.h"
#include <vector>
namespace RE
{

	template<typename T, JGShader::EShaderData data_type>
	class STDefault;
	class STStruct;
	class ShaderTypeLib;


	class ShaderDataType : public ReObject
	{
		friend STStruct;
	public:
		ShaderDataType() : ReObject("NoneShaderType"){

		}
		ShaderDataType(const std::string& name) :
			ReObject(name), m_Type(JGShader::_struct){

		}
		ShaderDataType(JGShader::EShaderData type) :
			ReObject(JGShader::ToString(type)), m_Type(type){
			m_Size = JGShader::GetDataSize(type);
		}

	public:
		JGShader::EShaderData GetType() const {
			return m_Type;
		}

		uint32_t GetSize() const {
			return m_Size;
		}
		virtual std::vector<byte> GetByteData() const = 0;
		virtual void Save(std::ofstream& fout);
		virtual void Load(std::ifstream& fin);


	protected:
		JGShader::EShaderData m_Type;
		uint32_t              m_Size = 0;
		uint32_t              m_SortingOrder = -1;
	};


	class STStruct : public ShaderDataType
	{
		friend ShaderTypeLib;
	public:
		STStruct() : ShaderDataType("SDStruct") {
			m_Type = JGShader::_struct;
		}
		STStruct(const std::string& name) : ShaderDataType(name) {
			m_Type = JGShader::_struct;
		}
		STStruct(const STStruct& copy);
		STStruct& operator=(const STStruct& copy);
	public:
		ShaderDataType* Add(JGShader::EShaderData data_type, const std::string& name);
		ShaderDataType* Add(const STStruct& _struct, const std::string& name);
		void Remove(const std::string& var_name);

		ShaderDataType* GetElement(const std::string& var_name);
		ShaderDataType* GetElement(uint32_t sorting_order);
		std::string GetElementName(uint32_t sorting_order);
		bool            IsElementStruct(const std::string& var_name);
		STStruct*       GetElementStruct(const std::string& var_name);
		std::string     GetShaderDefineCode() const;
		uint32_t GetElementCount() const {
			return (uint32_t)m_StructElements.size();
		}

		void SetElementName(const std::string& var_name, const std::string& set_name);
		void SetElementSortingOrder(const std::string& var_name, uint32_t sorting_order);

		virtual std::vector<byte> GetByteData() const override;
		virtual void Save(std::ofstream& fout) override;
		virtual void Load(std::ifstream& fin) override;
	private:
		std::map<std::string, std::shared_ptr<ShaderDataType>>      m_StructElements;
		std::map<uint32_t, std::pair<ShaderDataType*, std::string>> m_SortingOrderMap;
		uint32_t m_SortingIndex = 0;
	};

	template<typename T, JGShader::EShaderData data_type>
	class STDefault : public ShaderDataType
	{
		T m_Value;
	public:
		STDefault() : ShaderDataType(data_type) {
			memset(&m_Value, 0, sizeof(T));
		}
		void Set(T value)
		{
			m_Value = value;
		}
		T Get() const {
			return m_Value;
		}
		virtual std::vector<byte> GetByteData() const override {
			std::vector<byte> result(GetSize());
			memcpy_s(&result[0], GetSize(), &m_Value, GetSize());
			return result;
		}
	};
	using STInt  = STDefault<int, JGShader::_int>;
	using STInt2 = STDefault<JVector2Int, JGShader::_int2>;
	using STInt3 = STDefault<JVector3Int, JGShader::_int3>;
	using STInt4 = STDefault<JVector4Int, JGShader::_int4>;
	using STUint =  STDefault<uint32_t, JGShader::_uint>;
	using STUint2 =  STDefault<JVector2Uint, JGShader::_uint2>;
	using STUint3 =  STDefault<JVector3Uint, JGShader::_uint3>;
	using STUint4 =  STDefault<JVector4Uint, JGShader::_uint4>;
	using STFloat = STDefault<float, JGShader::_float>;
	using STFloat2 = STDefault<JVector2, JGShader::_float2>;
	using STFloat3 = STDefault<JVector3, JGShader::_float3>;
	using STFloat4 = STDefault<JVector4, JGShader::_float4>;
	using STMatrix = STDefault<JMatrix, JGShader::_matrix4x4>;


	template<typename T>
	inline T* CastShaderDataType(ShaderDataType* sdt)
	{
		return nullptr;
	}
	template<>
	inline STInt* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_int)
			return (STInt*)sdt;
		return nullptr;
	}
	template<>
	inline STInt2* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_int2)
			return (STInt2*)sdt;
		return nullptr;
	}

	template<>
	inline STInt3* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_int3)
			return (STInt3*)sdt;
		return nullptr;
	}
	template<>
	inline STInt4* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_int4)
			return (STInt4*)sdt;
		return nullptr;
	}

	template<>
	inline STUint* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_uint)
			return (STUint*)sdt;
		return nullptr;
	}
	template<>
	inline STUint2* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_uint2)
			return (STUint2*)sdt;
		return nullptr;
	}
	template<>
	inline STUint3* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_uint3)
			return (STUint3*)sdt;
		return nullptr;
	}
	template<>
	inline STUint4* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_uint4)
			return (STUint4*)sdt;
		return nullptr;
	}

	template<>
	inline STFloat* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_float)
			return (STFloat*)sdt;
		return nullptr;
	}
	template<>
	inline STFloat2* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_float2)
			return (STFloat2*)sdt;
		return nullptr;
	}
	template<>
	inline STFloat3* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_float3)
			return (STFloat3*)sdt;
		return nullptr;
	}
	template<>
	inline STFloat4* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_float4)
			return (STFloat4*)sdt;
		return nullptr;
	}
	template<>
	inline STMatrix* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_matrix4x4)
			return (STMatrix*)sdt;
		return nullptr;
	}
	template<>
	inline STStruct* CastShaderDataType(ShaderDataType* sdt)
	{
		if (sdt->GetType() == JGShader::_struct)
			return (STStruct*)sdt;
		return nullptr;
	}
	inline std::shared_ptr<ShaderDataType> GetShaderDataTypeByType(JGShader::EShaderData type)
	{
		std::shared_ptr<ShaderDataType> result;

		switch (type)
		{
		case JGShader::_int:
			result = std::make_shared<STInt>();
			break;
		case JGShader::_int2:
			result = std::make_shared<STInt2>();
			break;
		case JGShader::_int3:
			result = std::make_shared<STInt3>();
			break;
		case JGShader::_int4:
			result = std::make_shared<STInt4>();
			break;
		case JGShader::_uint:
			result = std::make_shared<STUint>();
			break;
		case JGShader::_uint2:
			result = std::make_shared<STUint2>();
			break;
		case JGShader::_uint3:
			result = std::make_shared<STUint3>();
			break;
		case JGShader::_uint4:
			result = std::make_shared<STUint4>();
			break;
		case JGShader::_float:
			result = std::make_shared<STFloat>();
			break;
		case JGShader::_float2:
			result = std::make_shared<STFloat2>();
			break;
		case JGShader::_float3:
			result = std::make_shared<STFloat3>();
			break;
		case JGShader::_float4:
			result = std::make_shared<STFloat4>();
			break;
		case JGShader::_matrix4x4:
			result = std::make_shared<STMatrix>();
			break;
		case JGShader::_struct:
			result = std::make_shared<STStruct>();
			break;
		}


		return result;
	}


	inline std::shared_ptr<ShaderDataType> GetShaderDataTypeByType(JGShader::EShaderData type, std::shared_ptr<ShaderDataType> init_data)
	{
		std::shared_ptr<ShaderDataType> result;

		switch (type)
		{
		case JGShader::_int:
			result = std::make_shared<STInt>(*(CastShaderDataType<STInt>(init_data.get())));
			break;
		case JGShader::_int2:
			result = std::make_shared<STInt2>(*(CastShaderDataType<STInt2>(init_data.get())));
			break;
		case JGShader::_int3:
			result = std::make_shared<STInt3>(*(CastShaderDataType<STInt3>(init_data.get())));
			break;
		case JGShader::_int4:
			result = std::make_shared<STInt4>(*(CastShaderDataType<STInt4>(init_data.get())));
			break;
		case JGShader::_uint:
			result = std::make_shared<STUint>(*(CastShaderDataType<STUint>(init_data.get())));
			break;
		case JGShader::_uint2:
			result = std::make_shared<STUint2>(*(CastShaderDataType<STUint2>(init_data.get())));
			break;
		case JGShader::_uint3:
			result = std::make_shared<STUint3>(*(CastShaderDataType<STUint3>(init_data.get())));
			break;
		case JGShader::_uint4:
			result = std::make_shared<STUint4>(*(CastShaderDataType<STUint4>(init_data.get())));
			break;
		case JGShader::_float:
			result = std::make_shared<STFloat>(*(CastShaderDataType<STFloat>(init_data.get())));
			break;
		case JGShader::_float2:
			result = std::make_shared<STFloat2>(*(CastShaderDataType<STFloat2>(init_data.get())));
			break;
		case JGShader::_float3:
			result = std::make_shared<STFloat3>(*(CastShaderDataType<STFloat3>(init_data.get())));
			break;
		case JGShader::_float4:
			result = std::make_shared<STFloat4>(*(CastShaderDataType<STFloat4>(init_data.get())));
			break;
		case JGShader::_matrix4x4:
			result = std::make_shared<STMatrix>(*(CastShaderDataType<STMatrix>(init_data.get())));
			break;
		case JGShader::_struct:
			result = std::make_shared<STStruct>(*(CastShaderDataType<STStruct>(init_data.get())));
			break;
		}


		return result;
	}
}