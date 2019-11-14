#pragma once
#include "ReEnumDefines.h"
#include "d3dx12.h"
#pragma warning(disable : 4996)

namespace RE
{

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
		const void* SecondaryData = nullptr;
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
		void Save(std::ofstream& fout)
		{
			DataIO::write(fout, EntryPoint);
			DataIO::write(fout, CompileVersion);
			DataIO::write(fout, Flags1);
			DataIO::write(fout, Flags2);
			DataIO::write(fout, Macros.size());
			for (auto& m : Macros)
			{
				DataIO::write(fout, std::string(m.Name));
				DataIO::write(fout, std::string(m.Definition));
			}
			//DataIO::write(fout, SecondaryDataFlags);
			//DataIO::write(fout, SecondaryDataSize);
			//DataIO::write(fout, reinterpret_cast<const char*>(SecondaryData));
		}
		void Load(std::ifstream& fin)
		{
			DataIO::read(fin, EntryPoint);
			DataIO::read(fin, CompileVersion);

			DataIO::read(fin, Flags1);
			DataIO::read(fin, Flags2);
			size_t size = 0;
			DataIO::read(fin, size);

			for (size_t i = 0; i < size; ++i)
			{
				D3D_SHADER_MACRO macro = {};
				std::string name;
				std::string definition;
				DataIO::read(fin, name);
				DataIO::read(fin, definition);
				strcpy(const_cast<char*>(macro.Name), name.data());
				strcpy(const_cast<char*>(macro.Definition), definition.data());
				Macros.push_back(macro);
			}

		/*	DataIO::read(fin, SecondaryDataFlags);
			DataIO::read(fin, SecondaryDataSize);
			fin.read(const_cast<char*>(reinterpret_cast<const char*>(SecondaryData)), SecondaryDataSize);*/
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
