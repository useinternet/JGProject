#pragma once


#include "RE.h"
#include "Defines/ReEnumDefines.h"
#include "Object/DxObject/Resource.h"
namespace RE
{
	class Resource;
	class SCVar;
	class SCStruct;
	class ShaderModule;
	// Shader 코드 
	class ShaderCode 
	{

	public:
		ShaderCode() = default;
		ShaderCode(ShaderModule* owner_module, const std::string& name, ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
	public:
		virtual std::string GetShaderCode() const { return""; };
	public:
		const std::string& GetName() const { return m_Name; }
		ShaderEnum::EShaderVisible GetVisible() const { return m_Visible; }
		void SetOwner(ShaderModule* owner_module) { m_OwnerModule = owner_module; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetVisible(ShaderEnum::EShaderVisible visible) { m_Visible = visible; }
	protected:
		void CodeSet(const std::string& name, ShaderEnum::EShaderVisible visible) {
			m_Name = name;
			m_Visible = visible;
		}
	private:
		std::string m_Name;
		ShaderEnum::EShaderVisible m_Visible;
	protected:
		ShaderModule* m_OwnerModule;
	};
	// 구조체
	class SCStruct : public ShaderCode
	{
	public:
		SCStruct() : ShaderCode() {}
		SCStruct(ShaderModule* owner_module, const std::string& name, ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All)
			: ShaderCode(owner_module, name, visible) {}
		void Set(const std::string& struct_name, ShaderEnum::EShaderVisible visible); 
		void AddVar(const SCVar& data);
		void AddVar(
			ShaderEnum::EShaderDataType type,
			const std::string& name,
			uint32_t array_size = 1,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
		void AddVar(const SCStruct& _struct, const std::string& var_name);
		uint32_t GetSize() const;
		virtual std::string GetShaderCode() const override;
	private:
		std::vector<std::shared_ptr<ShaderCode>>  m_Datas;
		std::vector<std::string> m_DataNames;
		uint32_t m_Size = 0;
	};

	// 입력 구조체 
	class SCInputStruct : public ShaderCode
	{
	public:
		SCInputStruct() : ShaderCode() {}
		SCInputStruct(ShaderModule* owner_module, const std::string& name, ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All)
			: ShaderCode(owner_module, name, visible) {}
		void Set(const std::string& struct_name, ShaderEnum::EShaderVisible visible);
		void AddVar(const SCVar& var, const std::string& semantic_name);
		void AddVar(const ShaderEnum::EShaderDataType type, const std::string& var_name, const std::string& semantic_name);
		virtual std::string GetShaderCode() const override;
		std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElementDescArray() const;
	private:
		std::vector<std::shared_ptr<SCVar>>   m_Datas;
		std::vector<std::string> m_SemanticNames;
	};

	// 변수
	class SCVar : public ShaderCode
	{
	public:
		SCVar() = default;
		SCVar(
			ShaderModule* owner_module,
			ShaderEnum::EShaderDataType type,
			const std::string& name,
			uint32_t array_size = 1,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All) :
			ShaderCode(owner_module, name, visible),
			m_Type(type), m_ArraySize(array_size) { }

		void Set(
			ShaderEnum::EShaderDataType type,
			const std::string& name,
			uint32_t array_size = 1,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);

		ShaderEnum::EShaderDataType GetType() const;
		uint32_t GetArraySize() const;
		uint32_t GetSize() const;


		virtual std::string GetShaderCode() const override;
	
	private:
		ShaderEnum::EShaderDataType m_Type;
		uint32_t    m_ArraySize = 1;
	};

	//
	class ShaderData
	{
	public:
		const std::string& GetName() const { return m_Name; }
		ShaderEnum::EShaderVisible GetVisible() const { return m_Visible; }
		uint32_t GetRegisterNumber() const { return m_RegisterNumber; }
		uint32_t GetRegisterSpace() const { return m_RegisterSpace; }
		void SetOwner(ShaderModule* owner_module) { m_OwnerModule = owner_module; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetVisible(ShaderEnum::EShaderVisible visible) { m_Visible = visible;}
		virtual void SetRegisterIndex(uint32_t register_number, uint32_t register_space)
		{
			m_RegisterNumber = register_number; m_RegisterSpace = register_space;
		}
	public:
		ShaderData(ShaderModule* owner_module) { m_OwnerModule = owner_module; }
		virtual std::string GetShaderCode() const = 0;
	private:
		std::string m_Name;
		ShaderEnum::EShaderVisible m_Visible;
		uint32_t m_RegisterNumber = 0;
		uint32_t m_RegisterSpace = 0;
	protected:
		ShaderModule* m_OwnerModule;
	};

	class SDBuffer : public ShaderData
	{
	public:
		SDBuffer(ShaderModule* owner_module) : ShaderData(owner_module) {}
		virtual std::vector<byte> GetData() const = 0;
		virtual std::string GetShaderCode() const override = 0;
	};

	class SDVar : public ShaderData
	{

	};

	class SDResource : public ShaderData
	{
	public:
		SDResource(ShaderModule* owner_module) : ShaderData(owner_module) {}
		void Set(
			ShaderEnum::EShaderResourceType type,
			const std::string& name,
			uint32_t init_array_size,
			ShaderEnum::EShaderVisible visible);
		void BindResource(const Texture& resource);
		void BindResource(const std::vector<Texture>& resource);
		void SetResourceType(ShaderEnum::EShaderResourceType type);
		uint32_t GetBindedResourceSize() const;
		const std::vector<Texture> GetBindedResources() const;
		ShaderEnum::EShaderResourceType GetResourceType() const;
	public:
		virtual std::string GetShaderCode() const override;
	private:
		std::vector<Texture> m_BindedResources;
		ShaderEnum::EShaderResourceType m_ResourceType;
	};
	class SDSamplerState : public ShaderData
	{
	public:
		SDSamplerState(ShaderModule* owner_module) : ShaderData(owner_module) {}
		void Set(
			const D3D12_STATIC_SAMPLER_DESC& desc,
			const std::string& name,
			ShaderEnum::EShaderVisible visible);
		void SetDesc(const D3D12_STATIC_SAMPLER_DESC& desc);
		virtual void SetRegisterIndex(uint32_t register_number, uint32_t register_space) override;
		const D3D12_STATIC_SAMPLER_DESC& GetDesc() const;
	public:
		virtual std::string GetShaderCode() const override;
	private:
		std::shared_ptr<D3D12_STATIC_SAMPLER_DESC> m_Desc;
	};

	class SDConstantBuffer : public SDBuffer
	{
	public:
		SDConstantBuffer(ShaderModule* owner_module) : SDBuffer(owner_module) {}
		void Set(
			const std::string& name,
			ShaderEnum::EShaderVisible visible);
		void AddVar(const SCVar& var);
		void AddVar(ShaderEnum::EShaderDataType type,
			        const std::string& name,
			        uint32_t array_size = 1,
			        ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
		void AddVar(const SCStruct& var);

		template<typename T>
		void SetVar(const std::string& var_name, const T& data)
		{
			auto iter = m_DataSheet.find(var_name);
			if (iter == m_DataSheet.end())
			{
				RE_LOG_ERROR("{0} not exist in {1}", var_name, GetName());
				return;
			}
			if (sizeof(T) != m_DataSheet[var_name].size())
			{
				RE_LOG_ERROR("{0} not match datasize in {1}", var_name, GetName());
				return;
			}
			memcpy_s(&m_DataSheet[var_name][0], m_DataSheet[var_name].size(),
				&data, m_DataSheet[var_name].size());
		}
		template<typename T>
		void SetVar(const T& data)
		{

			if (sizeof(T) != GetDataSize())
			{
				RE_LOG_ERROR("not match datasize in {0}", GetName());
				return;
			}
			std::vector<byte> btData(sizeof(T));
			memcpy_s(&btData[0], btData.size(), &data, sizeof(T));
			int idx = 0;

			for (auto& var : m_SCVars)
			{
				auto iter = m_DataSheet.find(var->GetName());
				memcpy_s(&iter->second[0], iter->second.size(), &btData[idx], iter->second.size());
				idx += iter->second.size();
			}
		}
		template<typename T>
		void GetVar(const std::string& var_name, T* out_data)
		{
			auto iter = m_DataSheet.find(var_name);
			if (iter == m_DataSheet.end())
			{
				RE_LOG_ERROR("{0} not exist in {1}", var_name, GetName());
				return;
			}
			if (sizeof(T) != m_DataSheet[var_name])
			{
				RE_LOG_ERROR("{0} not match datasize in {1}", var_name, GetName());
				return;
			}
			memcpy_s(out_data, m_DataSheet[var_name].size(), m_DataSheet[var_name].data(),
				m_DataSheet[var_name].size());
		}
		template<typename T>
		void GetVar(T* out_data)
		{
			if (sizeof(T) != GetDataSize())
			{
				RE_LOG_ERROR("not match datasize in {0}", GetName());
				return;
			}
			auto data = GetData();
			memcpy_s(out_data, data.size(), data.data(), data.size());
		}
	public:
		uint32_t GetDataSize() const;
		virtual std::vector<byte> GetData() const override;
		virtual std::string GetShaderCode() const override;
	private:
		using DataSheet = std::unordered_map<std::string, std::vector<byte>>;

		std::vector<std::shared_ptr<ShaderCode>> m_SCVars;
		DataSheet m_DataSheet;
	};
	//
	class SDStructuredBuffer : public SDBuffer
	{
	public:
		SDStructuredBuffer(ShaderModule* owner_module) : SDBuffer(owner_module) {}

		void Set(
			const std::string& define_name,
			const SCStruct& _struct,
			ShaderEnum::EShaderVisible visible);

		template<typename T>
		void AddData(const T& data)
		{
			uint32_t size = sizeof(T);

			if (m_StructDataType->GetSize() != size)
			{
				RE_LOG_ERROR("not match type size in {0}", GetName());
				return;
			}

			
			// 구현중
			std::vector<byte> btArray(size);
			memcpy_s(&btArray[0], size, &data, size);

			m_Data.insert(m_Data.end(), btArray.begin(), btArray.end());
		}
		
		template<typename T>
		void SetData(int idx, const T& data)
		{
			uint32_t elementCount = m_Data.size() / m_StructDataType->GetSize();
			if (elementCount <= (uint32_t)idx)
			{
				assert(false && "SDStructuredBuffer::SetData() index exceed");
				return;
			}
			uint32_t pos = idx * m_StructDataType->GetSize();

			memcpy_s(&m_Data[pos], m_StructDataType->GetSize(), &data, m_StructDataType->GetSize());
		}

		template<typename T>
		T GetData(int idx)
		{
			uint32_t elementCount = m_Data.size() / m_StructDataType->GetSize();
			if (elementCount <= (uint32_t)idx)
			{
				assert(false && "SDStructuredBuffer::SetData() index exceed");
				return T();
			}
			T result;

			uint32_t pos = idx * m_StructDataType->GetSize();
			memcpy_s(&result, m_StructDataType->GetSize(), &m_Data[pos], m_StructDataType->GetSize());
			return result;
		}
		uint32_t GetElementSize() const {
			return m_StructDataType->GetSize();
		}
	public:
		virtual std::vector<byte> GetData() const override;
		virtual std::string GetShaderCode() const override;
	private:
		std::shared_ptr<SCStruct> m_StructDataType;
		std::vector<byte> m_Data;
	};






}