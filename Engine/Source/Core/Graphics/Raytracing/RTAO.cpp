#include "pch.h"
#include "RTAO.h"
#include "Graphics/RootSignature.h"
#include "Graphics/Renderer.h"
#include "Graphics/Resource.h"
#include "Graphics/Shader.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "Graphics/RayTracing/RayTracingPipeline.h"
#include "Graphics/RayTracing/RayTracingShaderResourceTable.h"
#include "Application.h"
namespace JG
{
    RTAO::Sampler::Sampler() :
        mNumSamples(0),
        mNumSampleSets(0),
        mIndex(0)
    {
    }

    u32 RTAO::Sampler::GetSampleIndex()
    {
        // Initialize sample and set jumps.
        if (mIndex % mNumSamples == 0)
        {
            // Pick a random index jump within a set.
            mJump = GetRandomJump();

            // Pick a random set index jump.
            mSetJump = GetRandomSetJump() * mNumSamples;
        }
        return mSetJump + mShuffledIndices[(mIndex++ + mJump) % mNumSamples];
    }

    // Resets the sampler with newly randomly generated samples
    void RTAO::Sampler::Reset(u32 numSamples, u32 numSampleSets, Enum hemisphereDistribution)
    {
        mIndex = 0;
        mNumSamples = numSamples;
        mNumSampleSets = numSampleSets;
        mSamples.resize(mNumSamples * mNumSampleSets, UnitSquareSample2D(FLT_MAX, FLT_MAX));
        mShuffledIndices.resize(mNumSamples * mNumSampleSets);
        mHemisphereSamples.resize(mNumSamples * mNumSampleSets, HemisphereSample3D(FLT_MAX, FLT_MAX, FLT_MAX));

        {
            mGeneratorURNG.seed(s_seed);

            std::uniform_int_distribution<u32> jumpDistribution(0, mNumSamples - 1);
            std::uniform_int_distribution<u32> jumpSetDistribution(0, mNumSampleSets - 1);

            std::uniform_real_distribution<f32> unitSquareDistribution(0.f, 1.f);
            std::uniform_real_distribution<f32> unitSquareDistributionInclusive(0.f, nextafter(1.f, FLT_MAX));

            GetRandomJump = bind(jumpDistribution, ref(mGeneratorURNG));
            GetRandomSetJump = bind(jumpSetDistribution, ref(mGeneratorURNG));
            GetRandomFloat01 = bind(unitSquareDistribution, ref(mGeneratorURNG));
            GetRandomFloat01inclusive = bind(unitSquareDistributionInclusive, ref(mGeneratorURNG));
        }

        // Generate random samples.
        {
            GenerateSamples2D();

            switch (hemisphereDistribution)
            {
            case Uniform: InitializeHemisphereSamples(0.f); break;
            case Cosine: InitializeHemisphereSamples(1.f); break;
            }

            for (UINT i = 0; i < mNumSampleSets; i++)
            {
                auto first = begin(mShuffledIndices) + i * mNumSamples;
                auto last = first + mNumSamples;

                std::iota(first, last, 0u); // Fill with 0, 1, ..., m_numSamples - 1 
                shuffle(first, last, mGeneratorURNG);
            }
        }
    };

    RTAO::UnitSquareSample2D RTAO::Sampler::RandomFloat01_2D()
    {
        return JVector2(GetRandomFloat01(), GetRandomFloat01());
    }

    UINT RTAO::Sampler::GetRandomNumber(u32 min, u32 max)
    {
        std::uniform_int_distribution<u32> distribution(min, max);
        return distribution(mGeneratorURNG);
    }
    RTAO::UnitSquareSample2D RTAO::Sampler::GetSample2D()
    {
        return mSamples[GetSampleIndex()];
    }

    RTAO::HemisphereSample3D RTAO::Sampler::GetHemisphereSample3D()
    {
        return mHemisphereSamples[GetSampleIndex()];
    }
    void RTAO::Sampler::InitializeHemisphereSamples(float cosDensityPower)
    {
        for (UINT i = 0; i < mSamples.size(); i++)
        {
            // Compute azimuth (phi) and polar angle (theta)
            /*
            float phi = XM_2PI * m_samples[i].x;
            float theta = acos(powf((1.f - m_samples[i].y), 1.f / (cosDensityPower + 1)));

            // Convert the polar angles to a 3D point in local orthornomal
            // basis with orthogonal unit vectors along x, y, z.
            m_hemisphereSamples[i].x = sinf(theta) * cosf(phi);
            m_hemisphereSamples[i].y = sinf(theta) * sinf(phi);
            m_hemisphereSamples[i].z = cosf(theta);
            */
            // Optimized version using trigonometry equations.
            float cosTheta = powf((1.f - mSamples[i].y), 1.f / (cosDensityPower + 1));
            float sinTheta = sqrtf(1.f - cosTheta * cosTheta);
            mHemisphereSamples[i].x = sinTheta * cosf(JG_2PI * mSamples[i].x);
            mHemisphereSamples[i].y = sinTheta * sinf(JG_2PI * mSamples[i].x);
            mHemisphereSamples[i].z = cosTheta;

        }
    }
    void RTAO::MultiJittered::GenerateSamples2D()
    {
        for (UINT s = 0; s < NumSampleSets(); s++)
        {
            // Generate samples on 2 level grid, with one sample per each (x,y)
            UINT sampleSetStartID = s * NumSamples();

            const UINT T = NumSamples();
            const UINT N = static_cast<UINT>(sqrt(T));

#define SAMPLE(i) mSamples[sampleSetStartID + i]

            // Generate random samples
            for (UINT col = 0, i = 0; col < N; col++)
                for (UINT row = 0; row < N; row++, i++)
                {
                    JVector2 stratum(static_cast<float>(row), static_cast<float>(col));
                    JVector2 cell(static_cast<float>(col), static_cast<float>(row));
                    UnitSquareSample2D randomValue01 = RandomFloat01_2D();

                    SAMPLE(i).x = (randomValue01.x + cell.x) / T + stratum.x / N;
                    SAMPLE(i).y = (randomValue01.y + cell.y) / T + stratum.y / N;
                }

            // Shuffle sample axes such that there's a sample in each stratum 
            // and n-rooks is maintained.

            // Shuffle x coordinate across rows within a column
            for (UINT row = 0; row < N - 1; row++)
                for (UINT col = 0; col < N; col++)
                {
                    UINT k = GetRandomNumber(row + 1, N - 1);
                    std::swap(SAMPLE(row * N + col).x, SAMPLE(k * N + col).x);
                }

            // Shuffle y coordinate across columns within a row
            for (UINT row = 0; row < N; row++)
                for (UINT col = 0; col < N - 1; col++)
                {
                    UINT k = GetRandomNumber(col + 1, N - 1);
                    std::swap(SAMPLE(row * N + col).y, SAMPLE(row * N + k).y);
                }

        }
    }

    // Generate random sample patterns on unit square.
    void RTAO::Random::GenerateSamples2D()
    {
        for (auto& sample : mSamples)
        {
            sample = RandomFloat01_2D();
        }
    }






    RTAO::RTAO(Renderer* renderer)
    {
        mRenderer = renderer;
        Init();
    }

    RTAO::~RTAO()
    {
        if (mSamplerUpdateSH != nullptr)
        {
            mSamplerUpdateSH->Reset();
            mSamplerUpdateSH = nullptr;
        }
        if (mSamplerUpdateAyncSH != nullptr)
        {
            mSamplerUpdateAyncSH->Reset();
            mSamplerUpdateAyncSH = nullptr;
        }
    }


    RTAO::Output RTAO::Execute(SharedPtr<IComputeContext> context, const Input& input)
    {
        if (input.Resolution != mResolution)
        {
            mResolution = input.Resolution;
            InitTexture();
        }

        // Sample Update 가 끝날때까지 대기
        while (mSamplerUpdateAyncSH->GetState() != EScheduleState::Compelete) {}
        mSamplerUpdateAyncSH->Reset();
        mSamplerUpdateAyncSH = nullptr;

        struct CB
        {
            u32 Seed;
            u32 NumSamplesPerSet;
            u32 NumSampleSets;
            u32 NumPixelsPerDimPerSet;


            JVector2Uint Resolution;
            f32 SPP;  //1, 1, 1024, 1
            f32 MaxAORayHitTime; // 22, 0.0f, 100.0f, 0.2f
            f32 MinimumAmbientIllumination;   //0.07f, 0.0f, 1.0f, 0.01fju
        };
        std::uniform_int_distribution<u32> seedDistribution(0, UINT_MAX);
        CB CB;
        CB.Seed = seedDistribution(mGeneratorURNG);
        CB.NumSamplesPerSet = mRandomSampler.NumSamples();
        CB.NumSampleSets = mRandomSampler.NumSampleSets();
        CB.NumPixelsPerDimPerSet = mAOSampleSetDistributedAcrossPixels.GetValue();
        CB.Resolution = JVector2Uint((u32)mResolution.x, (u32)mResolution.y);
        CB.SPP = mSPP.GetValue();
        CB.MaxAORayHitTime = mMaxAoRayHitTime.GetValue();
        CB.MinimumAmbientIllumination = mMinimumAmbientIllunmination.GetValue();


        context->BindRootSignature(mRootSignature);
        context->BindConstantBuffer(0, CB);
        context->BindTextures(1,
            {
                input.NormalDepth,
                input.HitPosition
            });
        context->BindTextures(2,
            {
                GetResource(EResource::AO),
                GetResource(EResource::AoRayDistance)
            });

        context->BindSturcturedBuffer(3, mHemisphereSamples);
        context->BindAccelerationStructure(4, input.SceneAS);
        
        context->DispatchRay(input.Resolution.x, input.Resolution.y, 1.0F, mPipeline, mSRT);

        for (i32 i = 0; i < EResource::Count; ++i)
        {
            mTex[i].SetValue(GetResource((EResource)i));
        }


        RTAO::Output output;
        output.AO = GetResource(EResource::AO);
        output.AORayDistance = GetResource(EResource::AoRayDistance);
        output.MaxRayDistance = mMaxAoRayHitTime.GetValue();
        return output;

    }

    void RTAO::Init()
    {
        mSPP = RP_Global_Float::Create("Renderer/RTAO/SPP", 1, 1, 1024, mRenderer->GetRenderParamManager());
        mMaxAoRayHitTime = RP_Global_Float::Create("Renderer/RTAO/MaxAORayHitTime", 25, 0.0f, 100.0f, mRenderer->GetRenderParamManager());
        mMinimumAmbientIllunmination = RP_Global_Float::Create("Renderer/RTAO/MinimumAmbientIllumination", 0.07f, 0.0f, 1.0f, mRenderer->GetRenderParamManager());
        mAOSampleSetDistributedAcrossPixels = RP_Global_Int::Create("Renderer/RTAO/AOSampleSetDistributedAcrossPixels", 8,1,8, mRenderer->GetRenderParamManager());


        mTex[EResource::AO] = RP_Global_Tex::Create("Renderer/RTAO/AO", nullptr, mRenderer->GetRenderParamManager());
        mTex[EResource::AoRayDistance] = RP_Global_Tex::Create("Renderer/RTAO/AoRayDistance", nullptr, mRenderer->GetRenderParamManager());

        mHemisphereSamples = IStructuredBuffer::Create("HemisphereSamples", sizeof(JVector4), 1024 * 8 * 8 * mNumSampleSets, EBufferLoadMethod::CPULoad);



        mSamplerUpdateSH = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1,
            SchedulePriority::BeginSystem, [&]() -> EScheduleResult
        {
            if (mSamplerUpdateAyncSH == nullptr)
            {
                mSamplerUpdateAyncSH = Scheduler::GetInstance().ScheduleAsync([&]()
                {
                    UpdateSampler();
                });
            }

            return EScheduleResult::Continue;
        });
        InitLibrary();
    }

    void RTAO::InitLibrary()
    {
        // RootSig
        SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
        creater->AddCBV(0, 0, 0);
        creater->AddDescriptorTable(1, EDescriptorTableRangeType::SRV, 2, 0, 0);
        creater->AddDescriptorTable(2, EDescriptorTableRangeType::UAV, 2, 0, 0);
        creater->AddSRV(3, 2, 0);
        creater->AddSRV(4, 3, 0);
        mRootSignature = creater->Generate();


        // Lib
        String shaderPath = PathHelper::CombinePath(Application::GetEnginePath(), "Shader/Raytracing");
        mPipeline = IRayTracingPipeline::Create();
        mPipeline->AddLibrary(PathHelper::CombinePath(shaderPath, "RTAO.hlsli"),
            { "RayGeneration", "ClosestHit", "Miss" }, false);
        mPipeline->AddHitGroup("AOHit", "ClosestHit", "", "");
        mPipeline->SetGlobalRootSignature(mRootSignature);
        mPipeline->SetMaxPayloadSize(sizeof(float[2]));
        mPipeline->SetMaxRecursionDepth(1);
        mPipeline->Generate();

        mSRT = IRayTracingShaderResourceTable::Create();
        mSRT->AddRayGeneration("RayGeneration");
        mSRT->AddMiss("Miss");
        mSRT->AddHitGroupAndBindLocalRootArgument("AOHit");
       

    }

    void RTAO::InitTexture()
    {
        TextureInfo texInfo;
        texInfo.Width = std::max<u32>(1, mResolution.x);
        texInfo.Height = std::max<u32>(1, mResolution.y);
        texInfo.ArraySize = 1;
        texInfo.Format = ETextureFormat::R16_Float;
        texInfo.Flags = ETextureFlags::Allow_UnorderedAccessView | ETextureFlags::Allow_RenderTarget;
        texInfo.MipLevel = 1;
        texInfo.ClearColor = Color();
        mResources[EResource::AO] = ITexture::Create("AOOutput", texInfo);
        mResources[EResource::AoRayDistance] =  ITexture::Create("AORayDistanceOutput", texInfo);

    }

    void RTAO::UpdateSampler()
    {
        SharedPtr<IStructuredBuffer> sb = mHemisphereSamples;
        u32 pixelsInSampleSet1D = mAOSampleSetDistributedAcrossPixels.GetValue();
        u32 samplesPerSet = mSPP.GetValue() * pixelsInSampleSet1D * pixelsInSampleSet1D;
        mRandomSampler.Reset(samplesPerSet, mNumSampleSets, Cosine);

        u32 numSamples = mRandomSampler.NumSamples() * mRandomSampler.NumSampleSets();
        for (u32 i = 0; i < numSamples; i++)
        {
            JVector3 p = mRandomSampler.GetHemisphereSample3D();
    
            sb->SetDataByIndex(i, &p);
        }
    }

    SharedPtr<ITexture> RTAO::GetResource(EResource resource)
    {
        return mResources[resource];
    }



    


}