#include "pch.h"
#include "DirectX12RayTracingPipeline.h"
#include "Platform/Graphics/DirectX12/Utill/RootSignature.h"
namespace JG
{

    DirectX12RayTracingPipeline::DirectX12RayTracingPipeline()
    {
        mRaytracingRootSig = CreateSharedPtr<RootSignature>();
    }

    void DirectX12RayTracingPipeline::AddLibrary(const String& shaderPath, const List<String>& symbolExports)
    {
        if (mShaderDic.find(shaderPath) != mShaderDic.end())
        {
            return;
        }
        ComPtr<IDxcBlob> blob = CompileShaderLibrary(shaderPath);
        if (blob == nullptr)
        {
            return;
        }
        mShaderDic[shaderPath] = blob;
        mPipelineGen.AddLibrary(blob.Get(), symbolExports);
    }
    void DirectX12RayTracingPipeline::AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol)
    {
        mPipelineGen.AddHitGroup(hitGroupName, closestHitSymbol, anyHitSymbol, intersectionSymbol);
    }
    void DirectX12RayTracingPipeline::AddRayGenerationProgram(const String& entryPoint)
    {
        mSBTGen.AddRayGenerationProgram(StringHelper::s2ws(entryPoint), {});
    }
    void DirectX12RayTracingPipeline::AddHitProgram(const String& entryPoint)
    {
        mSBTGen.AddHitGroup(StringHelper::s2ws(entryPoint), {});
    }
    void DirectX12RayTracingPipeline::AddMissProgram(const String& entryPoint)
    {
        mSBTGen.AddMissProgram(StringHelper::s2ws(entryPoint), {});
    }


    void DirectX12RayTracingPipeline::SetMaxPayloadSize(u32 byteSize)
    {
        mPipelineGen.SetMaxPayloadSize(byteSize);
    }
    void DirectX12RayTracingPipeline::SetMaxAttributeSize(u32 byteSize)
    {
        mPipelineGen.SetMaxAttributeSize(byteSize);
    }
    void DirectX12RayTracingPipeline::SetMaxRecursionDepth(u32 maxDepth)
    {
        mPipelineGen.SetMaxRecursionDepth(maxDepth);
    }


    bool DirectX12RayTracingPipeline::Finalize()
    {
        //mSBTGen.AddRayGenerationProgram();
        //mSBTGen.AddHitGroup()
        return false;
    }
    bool DirectX12RayTracingPipeline::IsValid()
    {
        //D3D12_SHADER_VARIABLE_DESC desc;
        //desc.
        return false;
    }

    IDxcBlob* DirectX12RayTracingPipeline::CompileShaderLibrary(const String& filePath)
	{
        static IDxcCompiler*       pCompiler = nullptr;
        static IDxcLibrary*        pLibrary = nullptr;
        static IDxcIncludeHandler* dxcIncludeHandler;

        HRESULT hr;

        // Initialize the DXC compiler and compiler helper
        if (!pCompiler)
        {
            (DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), (void**)&pCompiler));
            (DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary), (void**)&pLibrary));
            (pLibrary->CreateIncludeHandler(&dxcIncludeHandler));
        }
        auto w_filepath = StringHelper::s2ws(filePath);
        // Open and read the file
        std::ifstream shaderFile(w_filepath);
        if (shaderFile.good() == false)
        {
            throw std::logic_error("Cannot find shader file");
        }
        std::stringstream strStream;
        strStream << shaderFile.rdbuf();
        std::string sShader = strStream.str();

        // Create blob from the string
        IDxcBlobEncoding* pTextBlob;
        (pLibrary->CreateBlobWithEncodingFromPinned(
            (LPBYTE)sShader.c_str(), (uint32_t)sShader.size(), 0, &pTextBlob));

        // Compile
        IDxcOperationResult* pResult;
        (pCompiler->Compile(pTextBlob, w_filepath.c_str(), L"", L"lib_6_3", nullptr, 0, nullptr, 0,
            dxcIncludeHandler, &pResult));

        // Verify the result
        HRESULT resultCode;
        (pResult->GetStatus(&resultCode));
        if (FAILED(resultCode))
        {
            IDxcBlobEncoding* pError;
            hr = pResult->GetErrorBuffer(&pError);
            if (FAILED(hr))
            {
                throw std::logic_error("Failed to get shader compiler error");
            }

            // Convert error blob to a string
            std::vector<char> infoLog(pError->GetBufferSize() + 1);
            memcpy(infoLog.data(), pError->GetBufferPointer(), pError->GetBufferSize());
            infoLog[pError->GetBufferSize()] = 0;

            std::string errorMsg = "Shader Compiler Error:\n";
            errorMsg.append(infoLog.data());

            MessageBoxA(nullptr, errorMsg.c_str(), "Error!", MB_OK);
            throw std::logic_error("Failed compile shader");
        }

        IDxcBlob* pBlob;
        (pResult->GetResult(&pBlob));
        return pBlob;
	}
}