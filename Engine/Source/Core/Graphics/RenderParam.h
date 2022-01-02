#pragma once
#include "JGCore.h"

namespace JG
{
	class Renderer;
	class ITexture;
	class RenderParam
	{
	protected:
		Renderer* mRenderer = nullptr;
		bool      mIsVaild = false;
		String    mName;

	protected:
		RenderParam() = default;
	public:
		virtual ~RenderParam() = default;
		
	public:
		bool IsVaild() const {
			return mIsVaild && mRenderer != nullptr;
		}
		const String& GetName() const {
			return mName;
		}
	};

	template<class T>
	class RP_GlobalVar : public RenderParam
	{
		T Value;
	public:
		virtual ~RP_GlobalVar() = default;
	public:
		bool SetValue(const T& value)
		{
			if (IsVaild() == false)
			{
				return false;
			}
			Value = value;
			mRenderer->SetGlobalRenderParam(GetName(), Value);
			return true;
		}
		const T& GetValue() const
		{
			if (IsVaild() == false)
			{
				return Value;
			}
			mRenderer->GetGlobalRenderParam(GetName(), &Value);
			return Value;
		}
	public:
		static RP_GlobalVar<T> Create(const String& name, const T& initValue, Renderer* renderer)
		{
			RP_GlobalVar<T> val;
			val.mRenderer = renderer;
			val.mName = name;
			val.Value = initValue;

			if (renderer->RegisterGlobalRenderParam(name, sizeof(T)) == false)
			{
				return RP_GlobalVar<T>();
			}
			if (renderer->SetGlobalRenderParam(name, initValue) == false)
			{
				return RP_GlobalVar<T>();
			}
			val.mIsVaild  = true;
			return val;
		}
		static RP_GlobalVar<T> Load(const String& name, Renderer* renderer)
		{
			T value;
			if (renderer->GetGlobalRenderParam(name, &value) == false)
			{
				return RP_GlobalVar<T>();
			}
			RP_GlobalVar<T> val;
			val.mRenderer = renderer;
			val.mName     = name;
			val.Value     = value;
			val.mIsVaild  = true;
			return val;
		}
	};

	class RP_Global_Tex : public RenderParam
	{
	public:
		virtual ~RP_Global_Tex() = default;
	public:
		bool SetValue(SharedPtr<ITexture> value);
		SharedPtr<ITexture> GetValue() const;
	public:
		static RP_Global_Tex Create(const String& name, SharedPtr<ITexture> initTex, Renderer* renderer, bool isPrivate = false);
		static RP_Global_Tex Load(const String& name, Renderer* renderer);
	};



	template<class T>
	class RP_LocalVar : public RenderParam
	{
		T    Value;
		Type Type_Key;
	public:
		virtual ~RP_LocalVar() = default;
	public:
		bool SetValue(const T& value)
		{
			if (IsVaild() == false)
			{
				return false;
			}
			Value = value;
			mRenderer->SetLocalRenderParam(Type_Key, GetName(), value);
			return true;
		}
		const T& GetValue() const
		{
			if (IsVaild() == false)
			{
				return Value;
			}
			mRenderer->GetLocalRenderParam(Type_Key, GetName(), &Value);
			return Value;
		}
	public:

		static RP_LocalVar<T> Create(const String& name, const T& initValue, const Type& type, Renderer* renderer)
		{
			RP_LocalVar<T> val;
			val.mRenderer = renderer;
			val.mName = name;
			val.Value = initValue;
			val.Type_Key = type;

			if (renderer->RegisterLocalRenderParam(type, name, sizeof(T)) == false)
			{
				return RP_LocalVar<T>();
			}
			if (renderer->SetLocalRenderParam(type, name, initValue) == false)
			{
				return RP_LocalVar<T>();
			}
			val.mIsVaild = true;
			return val;
		}

		static RP_LocalVar<T> Load(const Type& type, const String& name, Renderer* renderer)
		{
			T value;
			if (renderer->GetLocalRenderParam(type, name, &value) == false)
			{
				return RP_LocalVar<T>();
			}

			RP_LocalVar<T> val;
			val.mRenderer = renderer;
			val.mName = name;
			val.Value = value;
			val.Type_Key = type;
			val.mIsVaild = true;

			return val;
		}

	};

	class RP_Local_Tex : public RenderParam
	{
		Type Type_Key;
	public:
		virtual ~RP_Local_Tex() = default;
	public:
		bool SetValue(SharedPtr<ITexture> value);
		SharedPtr<ITexture> GetValue() const;
	public:
		static RP_Local_Tex Create(const String& name, SharedPtr<ITexture> initTex, const Type& type, Renderer* renderer, bool isPrivate = false);
		static RP_Local_Tex Load(const Type& type, const String& name, Renderer* renderer);
	};




	



	using RP_Global_Bool   = RP_GlobalVar<bool>;
	using RP_Global_Float  = RP_GlobalVar<float>;
	using RP_Global_Float2 = RP_GlobalVar<JVector2>;
	using RP_Global_Float3 = RP_GlobalVar<JVector3>;
	using RP_Global_Float4 = RP_GlobalVar<JVector4>;
	using RP_Global_Uint   = RP_GlobalVar<u32>;
	using RP_Global_Uint2  = RP_GlobalVar<JVector2Uint>;
	using RP_Global_Uint3  = RP_GlobalVar<JVector3Uint>;
	using RP_Global_Uint4  = RP_GlobalVar<JVector4Uint>;
	using RP_Global_Int    = RP_GlobalVar<u32>;
	using RP_Global_Int2   = RP_GlobalVar<JVector2Int>;
	using RP_Global_Int3   = RP_GlobalVar<JVector3Int>;
	using RP_Global_Int4   = RP_GlobalVar<JVector4Int>;



	using RP_Local_Bool   = RP_LocalVar<bool>;
	using RP_Local_Float  = RP_LocalVar<float>;
	using RP_Local_Float2 = RP_LocalVar<JVector2>;
	using RP_Local_Float3 = RP_LocalVar<JVector3>;
	using RP_Local_Float4 = RP_LocalVar<JVector4>;
	using RP_Local_Uint   = RP_LocalVar<u32>;
	using RP_Local_Uint2  = RP_LocalVar<JVector2Uint>;
	using RP_Local_Uint3  = RP_LocalVar<JVector3Uint>;
	using RP_Local_Uint4  = RP_LocalVar<JVector4Uint>;
	using RP_Local_Int    = RP_LocalVar<u32>;
	using RP_Local_Int2   = RP_LocalVar<JVector2Int>;
	using RP_Local_Int3   = RP_LocalVar<JVector3Int>;
	using RP_Local_Int4   = RP_LocalVar<JVector4Int>;








}