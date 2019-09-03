#pragma once
#include "ReEnumDefines.h"
#include "d3dx12.h"


namespace RE
{
	class ShaderDataSheet;
	class ShaderData
	{
	private:
		std::vector<byte> m_Data;
		ShaderEnum::EShaderDataType m_Type;
		std::string m_Name;
	public:
		ShaderData() {}
		ShaderData(const std::string& name) : m_Name(name) {}
		ShaderData(const std::string& name, ShaderEnum::EShaderDataType type) :
			m_Type(type), m_Name(name) {}

		void Set(const std::string& name)
		{
			m_Name = name;
		}

		void Set(const std::string& name, ShaderEnum::EShaderDataType type)
		{
			m_Name = name;
			m_Type = type;
			switch (type)
			{
			case ShaderEnum::_bool:
				m_Data.resize(sizeof(bool));
				break;
			case ShaderEnum::_float:
				m_Data.resize(sizeof(float));
				break;
			case ShaderEnum::_float2:
				m_Data.resize(sizeof(float) * 2);
				break;
			case ShaderEnum::_float3:
				m_Data.resize(sizeof(float) * 3);
				break;
			case ShaderEnum::_float4:
				m_Data.resize(sizeof(float) * 4);
				break;
			case ShaderEnum::_int:
				m_Data.resize(sizeof(int));
				break;
			case ShaderEnum::_int2:
				m_Data.resize(sizeof(int) * 2);
				break;
			case ShaderEnum::_int3:
				m_Data.resize(sizeof(int) * 3);
				break;
			case ShaderEnum::_int4:
				m_Data.resize(sizeof(int) * 4);
				break;
			case ShaderEnum::_uint:
				m_Data.resize(sizeof(unsigned int));
				break;
			case ShaderEnum::_uint2:
				m_Data.resize(sizeof(unsigned int) * 2);
				break;
			case ShaderEnum::_uint3:
				m_Data.resize(sizeof(unsigned int) * 3);
				break;
			case ShaderEnum::_uint4:
				m_Data.resize(sizeof(unsigned int) * 4);
				break;
			case ShaderEnum::_matrix3x3:
				m_Data.resize(sizeof(float) * 9);
				break;
			case ShaderEnum::_matrix4x4:
				m_Data.resize(sizeof(float) * 16);
				break;
			default:
				RE_LOG_ERROR("This ShaderDataType not supported");
				break;
			}
		}

		template<typename DataType>
		void Set(const std::string& name, ShaderEnum::EShaderDataType type, const DataType& value)
		{
			Set(name, type);
			Set(value);
		}

		template<typename DataType>
		void Set(const DataType& data)
		{
			if (sizeof(DataType) != m_Data.size())
			{
				RE_LOG_ERROR("not match Between {0}'s DataSize and input DataSize", m_Name);
				return;
			}
			memcpy_s(&m_Data[0], m_Data.size(), &data, m_Data.size());
		}

		const std::vector<byte> Get() const
		{
			return m_Data;
		}

		template<typename DataType>
		void Get(DataType* out_data) {
			memcpy_s(out_data, m_Data.size(), m_Data.data(), m_Data.size());
		}

		const std::string& GetName() const {
			return m_Name;
		}
		ShaderEnum::EShaderDataType GetType() const {
			return m_Type;
		}
		std::string GetTypeString() const {
			return ShaderEnum::ShaderDataTypeToString(m_Type);
		}
		std::string GetShaderCode() const {
			return GetTypeString() + " " + m_Name + ";\n";
		}
		
	};

	class ShaderInputData
	{
		std::string m_Name;
		std::vector<ShaderData> m_Sheet;
		std::vector<std::string> m_SemanticNames;
	public:
		ShaderInputData() {}
		ShaderInputData(const std::string& name) : m_Name(name) {}
		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const    { return m_Name; }
		void AddData(ShaderEnum::EShaderDataType type, const std::string& name, const std::string& SementicName)
		{
			ShaderData data;
			data.Set(name, type);
			m_Sheet.push_back(data);
			m_SemanticNames.push_back(SementicName);
		}
		std::vector<D3D12_INPUT_ELEMENT_DESC> GetDesc() const {
			//std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayouts =
			//{
			//	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			//	 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			//	 { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			//	 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			//};
			std::vector<D3D12_INPUT_ELEMENT_DESC> descs(m_Sheet.size());
			
			uint32_t sheet_size = (uint32_t)m_Sheet.size();
			// 구현중 ...///

			assert(false && "ShaderInputData");


		}
		std::string GetShaderCode() const
		{
			std::string code = "struct " + m_Name + "\n{\n";
			for (uint32_t i = 0; i < (uint32_t)m_Sheet.size(); ++i)
			{
				code += "   " + m_Sheet[i].GetTypeString() + " " + m_Sheet[i].GetName();
				code += " : " + m_SemanticNames[i] + ";\n";
			}
			code += "};\n";
			return code;
		}
	};



	class ShaderDataSheet
	{
		using DataSheet = std::vector<ShaderData>;
		using StructSheet = std::vector<std::pair<std::string, ShaderDataSheet>>;
	private:
		std::string m_Name;
		DataSheet   m_Sheet;
		StructSheet m_StructSheet;
	private:
		ShaderData* FindData(const std::string& name)
		{
			for (auto& data : m_Sheet)
			{
				if (data.GetName() == name)
				{
					return &data;
				}
			}
			return nullptr;
		}
		ShaderDataSheet* FindSheet(const std::string& name)
		{
			for (auto& sheet : m_StructSheet)
			{
				if (sheet.first == name)
					return &sheet.second;
			}
			return nullptr;
		}
	public:
		ShaderDataSheet() {}
		ShaderDataSheet(const std::string& name) : m_Name(name) {}

		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }
		void AddVar(ShaderEnum::EShaderDataType data_type, const std::string& name)
		{
			auto iter = FindData(name);
			if (iter != nullptr)
			{
				RE_LOG_ERROR("{0} is a duplicate in {1}'s ShaderDataSheet", name, m_Name);
				return;
			}
			ShaderData data;
			data.Set(name, data_type);
			m_Sheet.push_back(data);
		}
		void AddSheet(const ShaderDataSheet& sheet, const std::string& name)
		{
			auto iter = FindSheet(name);
			if (iter != nullptr)
			{
				RE_LOG_ERROR("{0} is a duplicate in {1}'s ShaderDataSheet", name, m_Name);
				return;
			}
			m_StructSheet.push_back(std::pair<std::string, ShaderDataSheet>(name, sheet));
		}
		template<typename DataType>
		void SetVar(const std::string& name, const DataType& value)
		{
			auto iter = FindData(name);
			if (iter == nullptr)
			{
				RE_LOG_ERROR("{0} not exist in {1}'s ShaderDataSheet", name, m_Name);
				return;
			}
			iter->Set(value);
		}
		template<typename DataType>
		void SetVarInSheet(const std::string& name, const std::string& var_name_inSheet, const DataType& value)
		{
			auto iter = FindSheet(name);
			if (iter == nullptr)
			{
				RE_LOG_ERROR("{0} not exist in {1}'s ShaderDataSheet", name, m_Name);
				return;
			}
			iter->SetVar(var_name_inSheet, value);
		}
		std::vector<byte> GetData() const {
			std::vector<byte> result;

			// 구현 중
			for (auto& sheet : m_StructSheet)
			{
				auto byte_array = sheet.second.GetData();
				result.insert(result.end(), byte_array.begin(), byte_array.end());
			}
			for (auto& var : m_Sheet)
			{
				auto byte_array = var.Get();
				result.insert(result.end(), byte_array.begin(), byte_array.end());
			}
			if (result.size() % 16 != 0)
				RE_LOG_ERROR("{0} not match data per 16byte unit", m_Name);
			return result;
		}
		template<typename DataType>
		void GetData(DataType* out_data)
		{
			auto bytedata = GetData();
			memcpy_s(out_data, bytedata.size(), bytedata.data(), bytedata.size());
		}

		std::string GetShaderCode() const {
			// 구현 중
			std::string code = "struct " + m_Name + "\n{ \n";
			for (auto& sheet : m_StructSheet)
			{
				code += "   " + sheet.second.GetName() + " " + sheet.first + "\n";
			}
			for (auto& var : m_Sheet)
			{
				code += "   " + var.GetShaderCode();
			}
			code += "}; \n";
			return code;
		}
	};





	class ShaderCompiler
	{
	public:
		std::string EntryPoint;
		std::string CompileVersion;
		uint32_t Flags1;
		uint32_t Flags2;

		//
		std::string SourceName;
		std::vector<D3D_SHADER_MACRO> Macros;
		ID3DInclude* pIncludes;
		uint32_t    SecondaryDataFlags;
		const void* SecondaryData;
		uint32_t    SecondaryDataSize;
	public:
		ShaderCompiler() :
			pIncludes(nullptr),
			SecondaryDataFlags(0),
			SecondaryData(nullptr),
			SecondaryDataSize(0),
			Flags1(0),
			Flags2(0){}
		ShaderCompiler(ShaderType type) :
			ShaderCompiler()
		{
			switch (type)
			{
			case ShaderType::Vertex:
				EntryPoint = "VS";
				CompileVersion = "vs_5_1";
				break;
			case ShaderType::Hull:
				EntryPoint = "HS";
				CompileVersion = "hs_5_1";
				break;
			case ShaderType::Domain:
				EntryPoint = "DS";
				CompileVersion = "ds_5_1";
				break;
			case ShaderType::Geometry:
				EntryPoint = "GS";
				CompileVersion = "gs_5_1";
				break;
			case ShaderType::Pixel:
				EntryPoint = "PS";
				CompileVersion = "ps_5_1";
				break;
			case ShaderType::Compute:
				EntryPoint = "CS";
				CompileVersion = "cs_5_1";
				break;
			}
		}
	};
	class ScissorRect
	{
		D3D12_RECT m_Rect;
	public:
		ScissorRect() = default;
		ScissorRect(int width, int height) {
			Set(width, height);
		}

		void Set(int width, int height) {
			m_Rect = CD3DX12_RECT(0, 0, width, height);
		}
		void Set(int left, int top, int right, int bottom) {
			m_Rect = CD3DX12_RECT(left, top, right, bottom);
		}
		const D3D12_RECT& Get() const {
			return m_Rect;
		}
	};

	class Viewport
	{
	private:
		D3D12_VIEWPORT m_Viewport;

	public:
		Viewport() = default;
		Viewport(float width, float height) {
			Set(width, height);
		}
		void Set(float width, float height) {
			m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
		}
		void Set(float topLeftX, float topLeftY, float width, float height) {
			m_Viewport = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
		}
		const D3D12_VIEWPORT& Get() const {
			return m_Viewport;
		}
	};
}
