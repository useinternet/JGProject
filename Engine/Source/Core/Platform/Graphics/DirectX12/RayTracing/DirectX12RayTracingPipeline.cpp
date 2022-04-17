#include "pch.h"
#include "DirectX12RayTracingPipeline.h"
#include "Graphics/Shader.h"
#include "Graphics/Raytracing/Raytracer.h"
#include "Platform/Graphics/DirectX12/Utill/RootSignature.h"
#include "Platform/Graphics/DirectX12/DirectX12RootSignature.h"
#include "Platform/Graphics/DirectX12/DirectX12Shader.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
namespace JG
{
    DirectX12RayTracingPipeline::~DirectX12RayTracingPipeline()
    {
        Reset();
    }


    void DirectX12RayTracingPipeline::AddLibrary(const String& shaderPath, const List<String>& symbolExports, bool isIncludeGlobalLib)
    {
        if (mShaderDic.find(shaderPath) != mShaderDic.end())
        {
            return;
        }
        auto w_filepath = StringHelper::s2ws(shaderPath);
        // Open and read the file
        std::ifstream shaderFile(w_filepath);
        if (shaderFile.good() == false)
        {
            throw std::logic_error("Cannot find shader file");
        }
        std::stringstream strStream;
        strStream << shaderFile.rdbuf();
        std::string sShader = strStream.str();


        String libCode = ShaderLibrary::GetInstance().GetGlobalShaderLibCode();

        if (isIncludeGlobalLib)
        {
            libCode += ShaderLibrary::GetInstance().GetGlobalRayTracingLibCode();
        }
        sShader = libCode + sShader;


        ComPtr<IDxcBlob> blob = CompileShaderLibrary(shaderPath, sShader);
        if (blob == nullptr)
        {
            return;
        }



        mShaderDic[shaderPath] = blob;
        mPipelineGen.AddLibrary(blob.Get(), symbolExports);
        mIsDirty = true;
    }
    void DirectX12RayTracingPipeline::AddLibrary(const SharedPtr<IShader> shader)
    {
        if (shader == nullptr || shader->IsSuccessed() == false)
        {
            return;
        }
        switch (shader->GetShaderType())
        {
        case EShaderType::ClosestHit:
        {
            DirectX12ClosestHitShader* closestHitShader = static_cast<DirectX12ClosestHitShader*>(shader.get());


            ComPtr<IDxcBlob> blob = closestHitShader->GetData();
			if (blob == nullptr)
			{
				return;
			}
			mShaderDic[closestHitShader->GetName()] = blob;
			mPipelineGen.AddLibrary(blob.Get(), { closestHitShader->GetEntryPoint() });
			AddHitGroup(closestHitShader->GetHitGroupName(), closestHitShader->GetEntryPoint(), "", "");
			AddLocalRootSignature(RayTracer::CreateLocalRootSignature(), { closestHitShader->GetEntryPoint() });
        }
            break;
        default:
            return;
        }
    }
    void DirectX12RayTracingPipeline::AddLibraryAsSourceCode(const String& name, const String& sourceCode, const List<String>& symbolExports)
    {
        if (mShaderDic.find(name) != mShaderDic.end())
        {
            return;
        }
 
        String sshader = ShaderLibrary::GetInstance().GetGlobalShaderLibCode() + ShaderLibrary::GetInstance().GetGlobalRayTracingLibCode();;
        sshader = sshader + sourceCode;


        ComPtr<IDxcBlob> blob = CompileShaderLibrary(name, sshader);
        if (blob == nullptr)
        {
            return;
        }



        mShaderDic[name] = blob;
        mPipelineGen.AddLibrary(blob.Get(), symbolExports);
        mIsDirty = true;
    }

    void DirectX12RayTracingPipeline::AddHitGroup(const String& hitGroupName, const String& closestHitSymbol, const String& anyHitSymbol, const String& intersectionSymbol)
    {
        mPipelineGen.AddHitGroup(hitGroupName, closestHitSymbol, anyHitSymbol, intersectionSymbol);
        mIsDirty = true;
    }
    void DirectX12RayTracingPipeline::SetMaxPayloadSize(u32 byteSize)
    {
        mPipelineGen.SetMaxPayloadSize(byteSize);
        mIsDirty = true;
    }
    void DirectX12RayTracingPipeline::SetMaxAttributeSize(u32 byteSize)
    {
        mPipelineGen.SetMaxAttributeSize(byteSize);
        mIsDirty = true;
    }
    void DirectX12RayTracingPipeline::SetMaxRecursionDepth(u32 maxDepth)
    {
        mPipelineGen.SetMaxRecursionDepth(maxDepth);
        mIsDirty = true;
    }

    void DirectX12RayTracingPipeline::SetGlobalRootSignature(SharedPtr<IRootSignature> rootSig)
    {
        if (rootSig == nullptr)
        {
            return;
        }
        if (mRaytracingRootSig != rootSig)
        {
            mIsDirty = true;
        }
        mRaytracingRootSig = rootSig;
    }

    void DirectX12RayTracingPipeline::AddLocalRootSignature(SharedPtr<IRootSignature> rootSig, const List<String>& symbols)
    {
        auto dx12RootSig = static_cast<DirectX12RootSignature*>(rootSig.get())->Get();
        mPipelineGen.AddLocalRootSignature(dx12RootSig->Get(), symbols);
        mIsDirty = true;
    }
    bool DirectX12RayTracingPipeline::IsValid()
    {
        return mRaytracingPipelineState != nullptr;
    }
    bool DirectX12RayTracingPipeline::Generate()
    {
        if (mRaytracingPipelineState == nullptr || mIsDirty ==  true)
        {
            mIsDirty = false;
            auto dx12RootSig = static_cast<DirectX12RootSignature*>(mRaytracingRootSig.get())->Get();
            mPipelineGen.SetRootSignature(dx12RootSig->Get());
            mRaytracingPipelineState = mPipelineGen.Generate();
            mRaytracingPipelineState->QueryInterface(mRaytracingPipelineStateProperties.GetAddressOf());
        }
        return IsValid();
    }
    void DirectX12RayTracingPipeline::Reset()
    {
        mShaderDic.clear();
        mRaytracingRootSig = nullptr;
        mRaytracingPipelineState = nullptr;
        mRaytracingPipelineStateProperties = nullptr;
        mPipelineGen = RayTracingPipelineGenerator();
    }
    SharedPtr<IRootSignature> DirectX12RayTracingPipeline::GetGlobalRootSignature() const
    {
        return mRaytracingRootSig;
    }
    IDxcBlob* DirectX12RayTracingPipeline::CompileShaderLibrary(const String& filePath, const String& sourceCode)
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


        // Create blob from the string
        IDxcBlobEncoding* pTextBlob;
        (pLibrary->CreateBlobWithEncodingFromPinned(
            (LPBYTE)sourceCode.c_str(), (uint32_t)sourceCode.size(), 0, &pTextBlob));

        // Compile
        IDxcOperationResult* pResult;
        (pCompiler->Compile(pTextBlob, StringHelper::s2ws(filePath).c_str(), L"", L"lib_6_3", nullptr, 0, nullptr, 0,
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