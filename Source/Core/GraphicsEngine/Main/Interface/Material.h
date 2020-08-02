#pragma once
#include "GEObject.h"


namespace GE
{
	class Material;
	class Texture;
	//class Rasterizer

	class GRAPHICSENGINE_API MaterialInstance : public GEObject
	{
	public:
		virtual bool SetVarAsFloat(const std::string& name, float f) = 0;
		virtual bool SetVarAsFloat2(const std::string& name, const JVector2& v) = 0;
		virtual bool SetVarAsFloat3(const std::string& name, const JVector3& v) = 0;
		virtual bool SetVarAsFloat4(const std::string& name, const JVector4& v) = 0;
		virtual bool SetVarAsInt(const std::string& name, int i) = 0;
		virtual bool SetVarAsInt2(const std::string& name, const JVector2Int& i) = 0;
		virtual bool SetVarAsInt3(const std::string& name, const JVector3Int& i) = 0;
		virtual bool SetVarAsInt4(const std::string& name, const JVector4Int& i) = 0;
		virtual bool SetVarAsUint(const std::string& name, uint32_t i) = 0;
		virtual bool SetVarAsUint2(const std::string& name, const JVector2Uint& i) = 0;
		virtual bool SetVarAsUint3(const std::string& name, const JVector3Uint& i) = 0;
		virtual bool SetVarAsUint4(const std::string& name, const JVector4Uint& i) = 0;
		virtual bool SetVarAsMatrix4x4(const std::string& name, const JMatrix& m) = 0;



		virtual bool BindTexture(const std::string& name, GE::Texture* t) = 0;



		virtual bool GetVarAsFloat(const std::string& name, float& dataRef) const = 0;
		virtual bool GetVarAsFloat2(const std::string& name, JVector2& dataRef) const = 0;
		virtual bool GetVarAsFloat3(const std::string& name, JVector3& dataRef) const = 0;
		virtual bool GetVarAsFloat4(const std::string& name, JVector4& dataRef) const = 0;
		virtual bool GetVarAsInt(const std::string& name, int& dataRef) const = 0;
		virtual bool GetVarAsInt2(const std::string& name, JVector2Int& dataRef) const = 0;
		virtual bool GetVarAsInt3(const std::string& name, JVector3Int& dataRef) const = 0;
		virtual bool GetVarAsInt4(const std::string& name, JVector4Int& dataRef) const = 0;
		virtual bool GetVarAsUint(const std::string& name, uint32_t& dataRef) const = 0;
		virtual bool GetVarAsUint2(const std::string& name, JVector2Uint& dataRef) const = 0;
		virtual bool GetVarAsUint3(const std::string& name, JVector3Uint& dataRef) const = 0;
		virtual bool GetVarAsUint4(const std::string& name, JVector4Uint& dataRef) const = 0;
		virtual bool GetVarAsMatrix4x4(const std::string& name, JMatrix& dataRef) const = 0;


		virtual const char*  BtData()  const = 0;
		virtual size_t       BtSize() const  = 0;
		virtual Material*    GetOwnerMaterial() const    = 0;
		virtual const std::string& GetErrorCode() const  = 0;
		virtual void GetTextureData(void* dataPtr) const = 0;

		virtual ~MaterialInstance() {}
	};

	class GRAPHICSENGINE_API Material : public GEObject
	{


	public:
		virtual void SetMaterialProperty(const MaterialProperty& ppt) = 0;
		virtual const MaterialProperty& GetMaterialProperty() const = 0;


		// 머터리얼 변수 선언
		virtual void SetCode(const std::string& code) = 0;
		virtual bool DefineVarAsFloat(const std::string& name, float f) = 0;
		virtual bool DefineVarAsFloat2(const std::string& name, const JVector2& v) = 0;
		virtual bool DefineVarAsFloat3(const std::string& name, const JVector3& v) = 0;
		virtual bool DefineVarAsFloat4(const std::string& name, const JVector4& v) = 0;
		virtual bool DefineVarAsInt(const std::string& name, int i) = 0;
		virtual bool DefineVarAsInt2(const std::string& name, const JVector2Int& i) = 0;
		virtual bool DefineVarAsInt3(const std::string& name, const JVector3Int& i) = 0;
		virtual bool DefineVarAsInt4(const std::string& name, const JVector4Int& i) = 0;
		virtual bool DefineVarAsUint(const std::string& name, uint32_t i) = 0;
		virtual bool DefineVarAsUint2(const std::string& name, const JVector2Uint& i) = 0;
		virtual bool DefineVarAsUint3(const std::string& name, const JVector3Uint& i) = 0;
		virtual bool DefineVarAsUint4(const std::string& name, const JVector4Uint& i) = 0;
		virtual bool DefineVarAsMatrix4x4(const std::string& name, const JMatrix& m) = 0;


		virtual void UnDefineVar(const std::string& name) = 0;

		//
		virtual bool DefineTexture(const std::string& name, GE::Texture* init_t) = 0;
		virtual void UnDefineTexture(const std::string& name) = 0;




		// 추후 텍스쳐 선언하는것도 만들자
		virtual MaterialInstance* CreateMaterialInstance() = 0;
		virtual void DeleteMaterialInstance(MaterialInstance* instance) = 0;
		virtual uint32_t GetMaterialInstanceCount() const = 0;


		virtual bool IsDirty() const = 0;
		virtual void ClearDirty() = 0;
		virtual void SendDirty() = 0;



		virtual std::vector<std::string> GetMaterialDataNameList() const = 0;
		virtual std::vector<std::string> GetMaterialTextureNameList() const = 0;
		virtual const std::string& GetCode() const = 0;
		virtual const std::string& GetMaterialCBCode() const = 0;
		virtual const std::string& GetErrorCode() const = 0;

		virtual ~Material() {}
	};
}