#pragma once
#include "DxObject.h"


namespace RE
{
	class Shader : public DxObject
	{
		friend class RenderDevice;

		
	public:
		Shader();
		Shader(const std::string& name);


		void Set(const std::string& shader_contents);


		const std::string& GetErrorCode() const;
		Microsoft::WRL::ComPtr<ID3DBlob> GetShaderCode() const;
		std::string ToString() const;
		bool IsVaild() const;
	public:
		virtual ShaderType  GetType()  const = 0;
	private:
		std::string m_ShaderCode;
		std::string m_ErrorCode;
		Microsoft::WRL::ComPtr<ID3DBlob> m_CompiledShaderCode;

	};

	class VertexShader : public Shader
	{
	public:
		VertexShader() : Shader("VertexShader") {}
		VertexShader(const std::string& name) : Shader(name) {}

		virtual ShaderType  GetType() const override {
			return ShaderType::Vertex;
		}
	};
	class HullShader : public Shader
	{
	public:
		HullShader() : Shader("HullShader") {}
		HullShader(const std::string& name) : Shader(name) {}

		virtual ShaderType GetType() const override {
			return ShaderType::Hull;
		}
	};
	class DomainShader : public Shader
	{
	public:
		DomainShader() : Shader("DomainShader") {}
		DomainShader(const std::string& name) : Shader(name) {}

		virtual ShaderType GetType() const override {
			return ShaderType::Domain;
		}
	};
	class GeometryShader : public Shader
	{
	public:
		GeometryShader() : Shader("GeometryShader") {}
		GeometryShader(const std::string& name) : Shader(name) {}

		virtual ShaderType GetType() const override {
			return ShaderType::Geometry;
		}
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader() : Shader("PixelShader") {}
		PixelShader(const std::string& name) : Shader(name) {}

		virtual ShaderType GetType() const override {
			return ShaderType::Pixel;
		}
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader() : Shader("ComputeShader") {}
		ComputeShader(const std::string& name) : Shader(name) {}

		virtual ShaderType GetType() const override {
			return ShaderType::Compute;
		}
	};


	


}