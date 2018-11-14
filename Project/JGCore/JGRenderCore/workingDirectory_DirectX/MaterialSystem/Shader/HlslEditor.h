#pragma once

#include"DirectX/JGRCObject.h"
namespace JGRC
{
	class InputLayout;
	class CBuffer;
	class Texture2D;
	class SamplerState;
	typedef std::vector<CBuffer*> CBufferArray;
	class CORE_EXPORT HlslEditor : public JGRCObject
	{
	private:
		std::string   m_Path;
		EShaderType   m_ShaderType;
		InputLayout*  m_InputLayout  = nullptr;
		CBufferArray  m_CBuffers;
		Texture2D*    m_Textures     = nullptr;
		SamplerState* m_SamplerState = nullptr;
	public:
		HlslEditor();
		HlslEditor(const HlslEditor& copy);
		HlslEditor(HlslEditor&& he);
		virtual ~HlslEditor();
	public:
		HlslEditor& operator=(const HlslEditor& copy);
		HlslEditor& operator=(HlslEditor&& he);
	public:
		void RegisterHlslPath(const EShaderType type, const std::string& path);
		InputLayout*  CreateInputLayout();
	    CBuffer*      CreateCBuffer();
		Texture2D*    CreateTexture2D();
		SamplerState* CreateSamplerState();
	public:
		InputLayout*  GetInputLayout()  { return m_InputLayout; }
		CBuffer*      GetCBuffer(const uint idx) { return m_CBuffers[idx]; }
		CBufferArray& GetCBuffers()     { return m_CBuffers; }
		Texture2D*    GetTextures()     { return m_Textures; }
		SamplerState* GetSamplerState() { return m_SamplerState; }
		void WriteData(std::ofstream& fout);
	};
	////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////



	class CORE_EXPORT InputLayout : public JGRCObject
	{
	private:
		std::vector<std::string> m_SementicName;
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_Descs;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_Layout;
	public:
		InputLayout() { m_SementicName.reserve(7); }
		virtual ~InputLayout() {};
		void AddDesc(const char* SemanticName, const UINT SemanticIndex, DXGI_FORMAT Format,
			UINT InputSlot, UINT Offset, D3D11_INPUT_CLASSIFICATION InputClass, UINT IDStepData);
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetArray() { return m_Descs; }
		D3D11_INPUT_ELEMENT_DESC* Get() { return &m_Descs[0]; }
		ID3D11InputLayout** GetLayoutAddress() { return m_Layout.GetAddressOf(); }
		ID3D11InputLayout*  GetLayout() { return m_Layout.Get(); }
		UINT Size() { return (UINT)m_Descs.size(); }
	};
	class CORE_EXPORT CBuffer : public JGRCObject
	{
	public:
		struct CORE_EXPORT Var
		{
			std::string  Name;
			unsigned int BtStart = 0;
			unsigned int BtEnd   = 0;
		};
	private:
		std::string m_Name;
		std::vector<Var>  m_Vars;
		std::vector<real> m_Data;
	public:
		bool  SetParam(const std::string& varName, void* data, const uint count);
		uint  GetParam(const std::string& varName, real* data);
		real* GetData() { return &m_Data[0]; }
		uint  GetSize() { return m_Data.capacity() * 4; }
		void  AssignMemory() { 
			m_Data.reserve(m_Vars.back().BtEnd + 1); 
			m_Data.resize(m_Vars.back().BtEnd + 1); }
		const std::vector<Var>& GetVarArray() { return m_Vars; }
		const std::string& GetName() { return m_Name; }
		void SetName(const std::string& Name) { m_Name = Name; }
		void AddVar(const std::string& Name,const unsigned int BtStart, const unsigned int BtEnd);
	};
	class CORE_EXPORT Texture2D : public JGRCObject
	{
	private:
		std::vector<std::string> m_TextureNames;
		std::vector<ID3D11ShaderResourceView*> m_SRVs;
	public:
		const std::vector<std::string>& GetArray() { return m_TextureNames; }
		void Add(const std::string& Name);
		void AddResource(ID3D11ShaderResourceView* srv)                 { m_SRVs.push_back(srv); }
		void SetResource(ID3D11ShaderResourceView* srv, const uint idx) { m_SRVs[idx] = srv; }
		ID3D11ShaderResourceView*  GetResource(const uint idx)           { return m_SRVs[idx]; }
		ID3D11ShaderResourceView** GetResourceAddress(const uint idx)    { return &m_SRVs[idx]; }
		ID3D11ShaderResourceView*  GetResource(const std::string& name);
		ID3D11ShaderResourceView** GetResourceAddress(const std::string& name);
		void SetResource(ID3D11ShaderResourceView* srv,const std::string& name);
		uint Size() { return m_TextureNames.size(); }
	};
	class CORE_EXPORT SamplerState : public JGRCObject
	{
	private:
		std::vector<D3D11_SAMPLER_DESC> Descs;
		std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_SamplerStates;
	public:
		bool CreateSamplerState();
		const std::vector<D3D11_SAMPLER_DESC>& GetArray() { return Descs; }
		void AddDefaultWrapMode();
		void AddDefaultClampMode();
		void AddCustomMode(const D3D11_SAMPLER_DESC& desc);
		ID3D11SamplerState** GetAddress(const uint idx);
		ID3D11SamplerState*  Get(const uint idx);
		uint Size() { return m_SamplerStates.size(); }
	};
}
