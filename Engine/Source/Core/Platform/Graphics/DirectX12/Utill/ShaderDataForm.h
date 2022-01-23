//#pragma once
//#include "JGCore.h"
//#include "DirectX12Helper.h"
//#include "UploadAllocator.h"
//#include "Graphics/GraphicsDefine.h"
//
//namespace JG
//{
//	class ITexture;
//	class ShaderDataForm
//	{
//	private:
//		static const u64 MAX_CB_COUNT = 5;
//		static const u64 MAX_SB_COUNT   = 10;
//		static const u64 MAX_RWSB_COUNT = 10;
//	public:
//		class CBufferData;
//
//		class Data
//		{
//		public:
//			EShaderDataType Type;
//			u64 DataSize = 0;
//			u64 DataPos = 0;
//			CBufferData* Owner = nullptr;
//		};
//		class StructData
//		{
//		public:
//			String Name;
//			List<String> DataTypeList;
//			List<String> DataNameList;
//			u64 DataSize = 0;
//		};
//
//
//		class ShaderElement
//		{
//		public:
//			String Name;
//			u32 RootParm = 0;
//			u32 RegisterNum = 0;
//			u32 RegisterSpace = 0;
//		public:
//			virtual ~ShaderElement() = default;
//		};
//
//
//
//		class ByteAddressData : public ShaderElement {};
//		class TextureData : public ShaderElement
//		{
//		public:
//			u64 TextureCount = 0;
//		public:
//			virtual ~TextureData() = default;
//		};
//
//
//		class CBufferData : public ShaderElement
//		{
//		public:
//			Dictionary<String, UniquePtr<Data>> DataMap;
//			u64 DataSize = 0;
//		public:
//			virtual ~CBufferData() = default;
//		};
//
//
//		class StructuredBufferData : public ShaderElement
//		{
//		public:
//			String Type;
//			u64 ElementDataSize = 0;
//		};
//
//	public:
//		Dictionary<String, UniquePtr<CBufferData>>		CBufferDataMap;
//		Dictionary<String, UniquePtr<StructuredBufferData>> StructuredBufferDataMap;
//		Dictionary<String, UniquePtr<StructuredBufferData>> RWStructuredBufferDataMap;
//
//
//		Dictionary<String, UniquePtr<ByteAddressData>> ByteAddrDataMap;
//		SortedDictionary<u64, ByteAddressData*>        SortedByteAddrDataMap;
//		Dictionary<String, UniquePtr<ByteAddressData>> RWByteAddrDataMap;
//		SortedDictionary<u64, ByteAddressData*>        SortedRWByteAddrDataMap;
//
//
//		Dictionary<String, UniquePtr<TextureData>>		TextureDataMap;
//		SortedDictionary<u64, TextureData*>             SortedTextureDataMap;
//
//		Dictionary<String, UniquePtr<TextureData>>		RWTextureDataMap;
//		SortedDictionary<u64, TextureData*>             SortedRWTextureDataMap;
//
//		Dictionary<String, Data*>		                CBufferVarMap;
//		Dictionary<String, UniquePtr<StructData>>       StructDataMap;
//	private:
//		u64 CBOffset = 0;
//		u64 SBOffset = 0;
//		u64 RWSBOffset = 0;
//
//
//		u64 ByteAddrRegisterNumOffset   = 0;
//		u64 RWByteAddrRegisterNumOffset = 0;
//		u64 TexRegisterNumOffset   = 0;
//		u64 RWTexRegisterNumOffset = 0;;
//	public:
//		bool Set(String& code);
//		void Reset();
//	private:
//		u64 AnalysisStruct(const String& code, u64 startPos, bool* result);
//		u64 AnalysisCBuffer(String& code, u64 startPos, bool* result);
//		u64 AnalysisByteAddressBuffer(String& code, u64 startPos, bool* result);
//		u64 AnalysisRWByteAddressBuffer(String& code, u64 startPos, bool* result);
//		u64 AnalysisStructuredBuffer(String& code, u64 startPos, bool* result);
//		u64 AnalysisTexture2D(String& code, u64 startPos, bool* result);
//		u64 AnalysisRWStructuredBuffer(String& code, u64 startPos, bool* result);
//		u64 AnalysisRWTexture2D(String& code, u64 startPos, bool* result);
//	public:
//		bool FindTypeInfo(const String& typeCode, String* out_type, u64* out_typeSize);
//	private:
//		void ExtractStructName(const String& code, u64 pos, String* out_value);
//		void ExtractCBufferName(const String& code, u64 pos, String* out_value);
//		u64 ExtractVarCode(const String& code, u64 pos, String* out_value);
//		u64 ExtractSamplerStateValue(const String& samplerStateDataCode, u64 startPos, String* out_key, String* out_value);
//	private:
//		bool RegisterStruct(const String& name);
//		bool RegisterStructVar(StructData* structData, const String& varCode);
//		bool RegisterByteAddrBuffer(const String& name);
//		bool RegisterRWByteAddrBuffer(const String& name);
//		bool RegisterStructuredBuffer(const String& name);
//		bool RegisterRWStructuredBuffer(const String& name);
//		bool RegisterCBuffer(const String& name);
//		bool RegisterTextureData(const String& name);
//		bool RegisterRWTextureData(const String& name);
//		bool RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize);
//	};
//
//	class IShader;
//	class ITexture;
//	class IStructuredBuffer;
//	class IByteAddressBuffer;
//	class RootSignature;
//	class ShaderData
//	{
//	public:
//		const static u64 MaxDataSize = _256KB;
//	private:
//		UniquePtr<UploadAllocator>      mUploadAllocator;
//		Dictionary<String, List<jbyte>> mReadDatas;
//		Dictionary<String, SharedPtr<IStructuredBuffer>> mSBDatas;
//		Dictionary<String, SharedPtr<IStructuredBuffer>> mRWSBDatas;
//		Dictionary<String, SharedPtr<IByteAddressBuffer>> mByteAddrDatas;
//		Dictionary<String, SharedPtr<IByteAddressBuffer>> mRWByteAddrDatas;
//		Dictionary<String, List<SharedPtr<ITexture>>>   mTextureDatas;
//		Dictionary<String, List<SharedPtr<ITexture>>>   mRWTextureDatas;
//		SharedPtr<ShaderDataForm> mShaderDataForm = nullptr;
//		SharedPtr<RootSignature>  mRootSignature;
//		std::shared_mutex mMutex;
//	public:
//		ShaderData(SharedPtr<ShaderDataForm> shaderDataForm);
//	public:
//		void ForEach_CB(const std::function<void(const ShaderDataForm::CBufferData*, const List<jbyte>&)>& action);
//		void ForEach_SB(const std::function<void(const ShaderDataForm::StructuredBufferData*, SharedPtr<IStructuredBuffer>)>& action);
//		void ForEach_RWSB(const std::function<void(const ShaderDataForm::StructuredBufferData*, SharedPtr<IStructuredBuffer>)>& action);
//		void ForEach_Tex(const std::function<void(const ShaderDataForm::TextureData*, const List<SharedPtr<ITexture>>&)>& action);
//		void ForEach_RWTex(const std::function<void(const ShaderDataForm::TextureData*, const List<SharedPtr<ITexture>>&)>& action);
//		void ForEach_BAB(const std::function<void(const ShaderDataForm::ByteAddressData*, SharedPtr<IByteAddressBuffer>)>& action);
//		void ForEach_RWBAB(const std::function<void(const ShaderDataForm::ByteAddressData*, SharedPtr<IByteAddressBuffer>)>& action);
//		void Reset();
//	public:
//		bool SetFloat(const String& name, float value);
//		bool SetFloat2(const String& name, const JVector2& value);
//		bool SetFloat3(const String& name, const JVector3& value);
//		bool SetFloat4(const String& name, const JVector4& value);
//		bool SetInt(const String& name, i32 value);
//		bool SetInt2(const String& name, const JVector2Int& value);
//		bool SetInt3(const String& name, const JVector3Int& value);
//		bool SetInt4(const String& name, const JVector4Int& value);
//		bool SetUint(const String& name, u32 value);
//		bool SetUint2(const String& name, const JVector2Uint& value);
//		bool SetUint3(const String& name, const JVector3Uint& value);
//		bool SetUint4(const String& name, const JVector4Uint& value);
//		bool SetFloat4x4(const String& name, const JMatrix& value);
//		bool SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture);
//		bool SetByteAddressBuffer(const String& name, SharedPtr<IByteAddressBuffer> bab);
//		bool SetStructuredBuffer(const String& name, SharedPtr<IStructuredBuffer> sb);
//
//		bool GetFloat(const String& name, float* out_value);
//		bool GetFloat2(const String& name, JVector2* out_value);
//		bool GetFloat3(const String& name, JVector3* out_value);
//		bool GetFloat4(const String& name, JVector4* out_value);
//		bool GetInt(const String& name, i32* out_value);
//		bool GetInt2(const String& name, JVector2Int* out_value);
//		bool GetInt3(const String& name, JVector3Int* out_value);
//		bool GetInt4(const String& name, JVector4Int* out_value);
//		bool GetUint(const String& name, u32* out_value);
//		bool GetUint2(const String& name, JVector2Uint* out_value);
//		bool GetUint3(const String& name, JVector3Uint* out_value);
//		bool GetUint4(const String& name, JVector4Uint* out_value);
//		bool GetFloat4x4(const String& name, JMatrix* outValue);
//		bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value);
//
//		SharedPtr<IByteAddressBuffer> GetByteAddressBuffer(const String& name);
//		SharedPtr<IStructuredBuffer> GetStructuredBuffer(const String& name);
//	public:
//		template<class T, EShaderDataType type>
//		bool SetData(const String& name, const T* value)
//		{
//			std::shared_lock<std::shared_mutex> lock(mMutex);
//			auto data = GetAndCheckData(name, type);
//			if (data == nullptr)
//			{
//				return false;
//			}
//
//			u64 dataSize   = sizeof(T);
//			u64 dataPos    = data->DataPos;
//			String& cbName = data->Owner->Name;
//
//			auto& btList = mReadDatas[cbName];
//			void* dest = (void*)((ptraddr)btList.data() + dataPos);
//			memcpy(dest, value, dataSize);
//			return true;
//		}
//		template<class T, EShaderDataType type>
//		bool GetData(const String& name, T* value)
//		{
//			std::shared_lock<std::shared_mutex> lock(mMutex);
//			if (value == nullptr)
//			{
//				return false;
//			}
//			auto data = GetAndCheckData(name, type);
//			if (data == nullptr)
//			{
//				return false;
//			}
//			u64 dataSize   = sizeof(T);
//			u64 dataPos    = data->DataPos;
//			String& cbName = data->Owner->Name;
//
//			void* src = (void*)((ptraddr)(mReadDatas[cbName].data()) + dataPos);
//			memcpy(value, src , dataSize);
//			return true;
//		}
//		ShaderDataForm::Data* GetAndCheckData(const String& name, EShaderDataType checkType);
//	};
//}