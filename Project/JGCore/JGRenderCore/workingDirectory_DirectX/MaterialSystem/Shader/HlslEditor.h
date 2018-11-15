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
	public:
		/*
		Exp : 입력 레아아웃 목록 추가 (해당 변수는(D3D11_INPUT_ELEMENT_DESC) msdn 참조 **)*/
		void AddDesc(const char* SemanticName, const UINT SemanticIndex, DXGI_FORMAT Format,
			UINT InputSlot, UINT Offset, D3D11_INPUT_CLASSIFICATION InputClass, UINT IDStepData);
		/*
		Exp : 입력 레아아웃 목록 배열을 가져온다. */
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetArray() { return m_Descs; }
		/*
		Exp : 입력 레아아웃 목록 포인터를 가져온다. */
		D3D11_INPUT_ELEMENT_DESC* Get() { return &m_Descs[0]; }
		/*
		Exp : 입력 레아아웃 포인터 주소를 가져온다. */
		ID3D11InputLayout** GetLayoutAddress() { return m_Layout.GetAddressOf(); }
		/*
		Exp : 입력 레아아웃 포인터를 가져온다. */
		ID3D11InputLayout*  GetLayout() { return m_Layout.Get(); }
		/*
		Exp : 입력 레아아웃목록 갯수*/
		UINT Size() { return (UINT)m_Descs.size(); }
	};
	class CORE_EXPORT CBuffer : public JGRCObject
	{
	public:
		struct CORE_EXPORT Var
		{ 
			std::string  Name;         // 변수 이름
			unsigned int BtStart = 0;  // 데이터 시작 위치
			unsigned int BtEnd   = 0;  // 데이터 끝 위치
		};
	private:
		std::string m_Name;         // 상수 버퍼 이름
		std::vector<Var>  m_Vars;   // 변수 정보 배열
		std::vector<real> m_Data;   // 실질적인 버퍼 데이터
	public:
		/*
		Exp : 변수의 값을 변경 */
		bool  SetParam(const std::string& varName, void* data, const uint count);
		/*
		Exp : 변수의 값을 가져온다 */
		uint  GetParam(const std::string& varName, real* data);
		/*
		Exp : 실질적인 버퍼 데이터를 가져온다. */
		real* GetData() { return &m_Data[0]; }
		/*
		Exp : 실질적인 버퍼 데이터 크기를 가져온다. */
		uint  GetSize() { return m_Data.capacity() * 4; }
		/*
		Exp : 설정이 끝난 CBuffer 클래스에게 메모리 할당. */
		void  AssignMemory() { 
			m_Data.reserve(m_Vars.back().BtEnd + 1); 
			m_Data.resize(m_Vars.back().BtEnd + 1); }
		/*
		Exp : 변수 정보 배열을 가져온다. */
		const std::vector<Var>& GetVarArray() { return m_Vars; }
		/*
		Exp : 해당 버퍼의 이름을 가져온다. */
		const std::string& GetName() { return m_Name; }
		/*
		Exp : 해당 버퍼의 이름 설정 */
		void SetName(const std::string& Name) { m_Name = Name; }
		/*
		Exp : 변수 정보 추가
		@param Name : 변수 이름 
		@param BtStart : 변수 데이터 시작 위치 
		@param BtEnd : 변수 데이터 끝 위치*/
		void AddVar(const std::string& Name,const unsigned int BtStart, const unsigned int BtEnd);
	};
	class CORE_EXPORT Texture2D : public JGRCObject
	{
	private:
		std::vector<std::string> m_TextureNames;       // 텍스쳐 이름 ( 에디터용 )
		std::vector<ID3D11ShaderResourceView*> m_SRVs; // 리소스 배열
	public:
		/*
		Exp : 텍스쳐 이름 배열을 가져온다. */
		const std::vector<std::string>& GetNameArray()              { return m_TextureNames; }
		/*
		Exp : 텍스쳐 리소스 뷰 배열 을 가져온다. */
		const std::vector<ID3D11ShaderResourceView*>& GetSRVArray() { return m_SRVs; }
		/*
		Exp : 텍스쳐 이름을 추가한다.*/
		void Add(const std::string& Name);
		/*
		Exp : 텍스쳐 리소스 뷰를 추가한다.*/
		void AddResource(ID3D11ShaderResourceView* srv)             { m_SRVs.push_back(srv); }
		/*
		Epx: 텍스쳐 리소스 뷰를 수정한다. 
		@param srv : 리소스 뷰 
		@param idx : 인덱스 */
		void SetResource(ID3D11ShaderResourceView* srv, const uint idx) { m_SRVs[idx] = srv; }
		/*
		Epx: 텍스쳐 리소스 뷰를 수정한다.
		@param srv : 리소스 뷰
		@param name : 텍스쳐 이름 */
		void SetResource(ID3D11ShaderResourceView* srv, const std::string& name);
		ID3D11ShaderResourceView*  GetResource(const uint idx)           { return m_SRVs[idx]; }
		ID3D11ShaderResourceView** GetResourceAddress(const uint idx)    { return &m_SRVs[idx]; }
		ID3D11ShaderResourceView*  GetResource(const std::string& name);
		ID3D11ShaderResourceView** GetResourceAddress(const std::string& name);
	
		uint NameSize() { return m_TextureNames.size(); }
		uint SRVSize()  { return m_SRVs.size(); }
	};
	class CORE_EXPORT SamplerState : public JGRCObject
	{
	private:
		std::vector<D3D11_SAMPLER_DESC> Descs;                 // 샘플러 목록
		std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_SamplerStates; // 샘플러 스테이트
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
