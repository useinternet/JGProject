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
        m_numSamples(0),
        m_numSampleSets(0),
        m_index(0)
    {
    }

    // Get a valid index from <0, m_numSampleSets * m_numSamples>.
    // The index increases by 1 on each call, but on a first 
    // access of a next sample set, the:
    // - sample set is randomly picked
    // - sample set is indexed from a random starting index within a set.
    // In addition the order of indices is retrieved from shuffledIndices.
    UINT RTAO::Sampler::GetSampleIndex()
    {
        // Initialize sample and set jumps.
        if (m_index % m_numSamples == 0)
        {
            // Pick a random index jump within a set.
            m_jump = GetRandomJump();

            // Pick a random set index jump.
            m_setJump = GetRandomSetJump() * m_numSamples;
        }
        return m_setJump + m_shuffledIndices[(m_index++ + m_jump) % m_numSamples];
    }

    // Resets the sampler with newly randomly generated samples
    void RTAO::Sampler::Reset(UINT numSamples, UINT numSampleSets, Enum hemisphereDistribution)
    {
        m_index = 0;
        m_numSamples = numSamples;
        m_numSampleSets = numSampleSets;
        m_samples.resize(m_numSamples * m_numSampleSets, UnitSquareSample2D(FLT_MAX, FLT_MAX));
        m_shuffledIndices.resize(m_numSamples * m_numSampleSets);
        m_hemisphereSamples.resize(m_numSamples * m_numSampleSets, HemisphereSample3D(FLT_MAX, FLT_MAX, FLT_MAX));

        // Reset generator and initialize distributions.
        {
            // Initialize to the same seed for determinism.
            m_generatorURNG.seed(s_seed);

            std::uniform_int_distribution<UINT> jumpDistribution(0, m_numSamples - 1);
            std::uniform_int_distribution<UINT> jumpSetDistribution(0, m_numSampleSets - 1);

            std::uniform_real_distribution<float> unitSquareDistribution(0.f, 1.f);

            // Specify the next representable value for the end range, since
            // uniform_real_distribution constructs excluding the end value [being, end).
            std::uniform_real_distribution<float> unitSquareDistributionInclusive(0.f, nextafter(1.f, FLT_MAX));

            GetRandomJump = bind(jumpDistribution, ref(m_generatorURNG));
            GetRandomSetJump = bind(jumpSetDistribution, ref(m_generatorURNG));
            GetRandomFloat01 = bind(unitSquareDistribution, ref(m_generatorURNG));
            GetRandomFloat01inclusive = bind(unitSquareDistributionInclusive, ref(m_generatorURNG));
        }

        // Generate random samples.
        {
            GenerateSamples2D();

            switch (hemisphereDistribution)
            {
            case Uniform: InitializeHemisphereSamples(0.f); break;
            case Cosine: InitializeHemisphereSamples(1.f); break;
            }

            for (UINT i = 0; i < m_numSampleSets; i++)
            {
                auto first = begin(m_shuffledIndices) + i * m_numSamples;
                auto last = first + m_numSamples;

                std::iota(first, last, 0u); // Fill with 0, 1, ..., m_numSamples - 1 
                shuffle(first, last, m_generatorURNG);
            }
        }
    };

    RTAO::UnitSquareSample2D RTAO::Sampler::RandomFloat01_2D()
    {
        return JVector2(GetRandomFloat01(), GetRandomFloat01());
    }

    UINT RTAO::Sampler::GetRandomNumber(UINT min, UINT max)
    {
        std::uniform_int_distribution<UINT> distribution(min, max);
        return distribution(m_generatorURNG);
    }
    RTAO::UnitSquareSample2D RTAO::Sampler::GetSample2D()
    {
        return m_samples[GetSampleIndex()];
    }

    RTAO::HemisphereSample3D RTAO::Sampler::GetHemisphereSample3D()
    {
        return m_hemisphereSamples[GetSampleIndex()];
    }

    // Initialize samples on a 3D hemisphere from 2D unit square samples
    // cosDensityPower - cosine density power {0, 1, ...}. 0:uniform, 1:cosine,...
    void RTAO::Sampler::InitializeHemisphereSamples(float cosDensityPower)
    {
        for (UINT i = 0; i < m_samples.size(); i++)
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
            float cosTheta = powf((1.f - m_samples[i].y), 1.f / (cosDensityPower + 1));
            float sinTheta = sqrtf(1.f - cosTheta * cosTheta);
            m_hemisphereSamples[i].x = sinTheta * cosf(JG_2PI * m_samples[i].x);
            m_hemisphereSamples[i].y = sinTheta * sinf(JG_2PI * m_samples[i].x);
            m_hemisphereSamples[i].z = cosTheta;

        }
    }

    // Generate multi-jittered sample patterns on a unit square [0,1].
    // Ref: Section 5.3.4 in Ray Tracing from the Ground Up.
    // The distribution has good random sampling distributions
    // with somewhat uniform distributions in both:
    // - 2D
    // - 1D projections of each axes.
    // Multi-jittered is a combination of two sample distributions:
    // - Jittered: samples are distributed on a NxN grid, 
    //             with each sample being random within its cell.
    // - N-rooks/Linear hypercube sampling: samples have uniform
    //             distribution in 1D projections of each axes.
    void RTAO::MultiJittered::GenerateSamples2D()
    {
        for (UINT s = 0; s < NumSampleSets(); s++)
        {
            // Generate samples on 2 level grid, with one sample per each (x,y)
            UINT sampleSetStartID = s * NumSamples();

            const UINT T = NumSamples();
            const UINT N = static_cast<UINT>(sqrt(T));

#define SAMPLE(i) m_samples[sampleSetStartID + i]

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
        for (auto& sample : m_samples)
        {
            sample = RandomFloat01_2D();
        }
    }






    RTAO::RTAO(Renderer* renderer)
    {
        mRenderer = renderer;
        Init();

    }

    RTAO::Output RTAO::Execute(SharedPtr<IComputeContext> context, const Input& input)
    {
        if (input.Resolution != mResolution)
        {
            mResolution = input.Resolution;
            InitTexture();
        }
        UpdateSampler();
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

        u32 bufferIndex = JGGraphics::GetInstance().GetBufferIndex();
        context->BindSturcturedBuffer(3, mHemisphereSamples[bufferIndex]);
        context->BindAccelerationStructure(4, input.SceneAS);
        

        context->DispatchRay(input.Resolution.x, input.Resolution.y, 1.0F, mPipeline, mSRT);

        for (i32 i = 0; i < EResource::Count; ++i)
        {
            mTex[i].SetValue(GetResource((EResource)i));
        }


        RTAO::Output output;
        output.AO = GetResource(EResource::AO);
        output.AORayDistance = GetResource(EResource::AoRayDistance);
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
        GraphicsHelper::InitStrucutredBuffer("HemisphereSamples", 1024 * 8 * 8 * mNumSampleSets, sizeof(JVector4), &mHemisphereSamples, EBufferLoadMethod::CPULoad);

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

        GraphicsHelper::InitRenderTextures(texInfo, "AOOutput", &mResources[EResource::AO]);
        GraphicsHelper::InitRenderTextures(texInfo, "AORayDistanceOutput", &mResources[EResource::AoRayDistance]);
    }

    void RTAO::UpdateSampler()
    {
        u32 bufferIndex = JGGraphics::GetInstance().GetBufferIndex();
        SharedPtr<IStructuredBuffer> sb = mHemisphereSamples[bufferIndex];
        UINT pixelsInSampleSet1D = mAOSampleSetDistributedAcrossPixels.GetValue();
        UINT samplesPerSet = mSPP.GetValue() * pixelsInSampleSet1D * pixelsInSampleSet1D;
        mRandomSampler.Reset(samplesPerSet, mNumSampleSets, Cosine);

        UINT numSamples = mRandomSampler.NumSamples() * mRandomSampler.NumSampleSets();
        for (UINT i = 0; i < numSamples; i++)
        {
            JVector3 p = mRandomSampler.GetHemisphereSample3D();
    
            sb->SetDataByIndex(i, &p);
        }
    }

    SharedPtr<ITexture> RTAO::GetResource(EResource resource)
    {
        u32 bufferIndex = JGGraphics::GetInstance().GetBufferIndex();
        return mResources[resource][bufferIndex];
    }



    


}