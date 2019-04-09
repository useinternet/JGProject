#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"ShaderCommonDefines.h"




namespace Dx12
{
	class GraphicsShader;
	class Material
	{
		static std::vector<MaterialCB>  ms_MaterialCBArray;
		static std::map<uint32_t, Material*> ms_MaterialMemorys;
		static uint32_t ms_MaterialID_Offset;
		static std::mutex ms_MaterialMutex;
	public:
		static const std::vector<MaterialCB>& GetMaterialCBArray();
	private:
		uint32_t   m_ID;
	public:
		Material();
		~Material();
	public:
		uint32_t GetID();
		MaterialCB& Get();
		const MaterialCB& Get_c() const;

		void RegisterShaderMacro(GraphicsShader& shader);
	};

}