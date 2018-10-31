#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"

namespace JGRC
{
	class ShaderAnalyzer;
	class TestShader
	{
	private:
		struct MatrixBufferType
		{
			jgMatrix4x4 world;
			jgMatrix4x4 view;
			jgMatrix4x4 projection;
		};
	public:
		TestShader();
		~TestShader();

		bool Initialize(ID3D11Device*, HWND);
		bool Render(ID3D11DeviceContext*, int, jgMatrix4x4, jgMatrix4x4, jgMatrix4x4, ID3D11ShaderResourceView*);
	private:
		bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
		bool SetShaderParameters(ID3D11DeviceContext*, jgMatrix4x4, jgMatrix4x4, jgMatrix4x4, ID3D11ShaderResourceView*);
		void RenderShader(ID3D11DeviceContext*, int);
	private:
		std::vector<real> LightArray;
		std::vector<real> MatrixArray;

		class JGBuffer* MatrixBuffer;
		class JGBuffer* LightBuffer;
		struct ShaderObject* VertexShader;
		struct ShaderObject* PixelShader;
		std::unique_ptr<ShaderAnalyzer> VSAyzer;
		std::unique_ptr<ShaderAnalyzer> PSAyzer;
	};
}
