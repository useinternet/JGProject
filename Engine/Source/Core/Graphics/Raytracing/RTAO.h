#pragma once
#include "JGCore.h"
#include "Graphics/RenderParam.h"
#include <random>
#include <numeric>

namespace JG
{
    class IStructuredBuffer;
    class IRootSignature;
    class Renderer;
    class IComputeContext;
    class IRayTracingPipeline;
    class IRayTracingShaderResourceTable;
    class ITopLevelAccelerationStructure;

	class RTAO
	{
    private:
    private:
        typedef JVector2 UnitSquareSample2D;  // unit square sample with a valid range of <0,1>
        typedef JVector3 HemisphereSample3D;  // hemisphere sample <0,1>

        enum Enum {
            Uniform,
            Cosine
        };
        class Sampler
        {
            static const UINT s_seed = 1729;
            std::function<UINT()> GetRandomJump;        // Generates a random uniform index within [0, m_numSamples - 1]
            std::function<UINT()> GetRandomSetJump;     // Generates a random uniform index within [0, m_numSampleSets - 1]
            std::function<float()> GetRandomFloat01;    // Generates a random uniform float within [0,1)
            std::function<float()> GetRandomFloat01inclusive; // Generates a random uniform float within [0,1]
        public:
            // Constructor, desctructor
            Sampler();
            ~Sampler() {}

            // Accessors
            u32 NumSamples() { return mNumSamples; }
            u32 NumSampleSets() { return mNumSampleSets; }

            // Member functions
            UnitSquareSample2D GetSample2D();
            HemisphereSample3D GetHemisphereSample3D();
            void Reset(u32 numSamples, u32 numSampleSets, Enum useConsineHemisphereDistribution);

        private:
            u32 GetSampleIndex();
            void InitializeHemisphereSamples(float cosDensityPower);

        protected:
            virtual void GenerateSamples2D() = 0; // Generate sample patterns in a unit square.
            UnitSquareSample2D RandomFloat01_2D();
            u32 GetRandomNumber(u32 min, u32 max);


            std::mt19937 mGeneratorURNG;  // Uniform random number generator
            u32 mNumSamples;      // number of samples in a set (pattern).
            u32 mNumSampleSets;   // number of sample sets.
            std::vector<UnitSquareSample2D> mSamples; // samples on a unit square.
            std::vector<HemisphereSample3D> mHemisphereSamples; // unit square samples on a hemisphere

            u32 mIndex;           // next sample index.
            List<u32> mShuffledIndices;
            u32 mJump;
            u32 mSetJump;
        };
        class MultiJittered : public Sampler
        {
        private:
            void GenerateSamples2D();
        };
        class Random : public Sampler
        {
        private:
            void GenerateSamples2D();
        };
    public:
        struct Input
        {
            JVector2 Resolution;
            SharedPtr<ITopLevelAccelerationStructure> SceneAS;
            SharedPtr<ITexture> NormalDepth;
            SharedPtr<ITexture> HitPosition;
            
        };
        struct Output
        {
            SharedPtr<ITexture> AO;
            SharedPtr<ITexture> AORayDistance;
            f32 MaxRayDistance;
        };
    private:
        enum EResource
        {
            AO,
            AoRayDistance,
            Count,
        };
        u32 mNumSampleSets = 83;
        MultiJittered mRandomSampler;
        List<SharedPtr<IStructuredBuffer>> mHemisphereSamples;



        RP_Global_Float mSPP;
        RP_Global_Int mAOSampleSetDistributedAcrossPixels;
        RP_Global_Float mMaxAoRayHitTime;
        RP_Global_Float mMinimumAmbientIllunmination;

        std::mt19937 mGeneratorURNG;
        Renderer* mRenderer = nullptr;
        JVector2 mResolution;



        List<SharedPtr<ITexture>> mResources[EResource::Count];
        RP_Global_Tex mTex[EResource::Count];
        SharedPtr<IRootSignature> mRootSignature;
        SharedPtr<IRayTracingShaderResourceTable> mSRT;
        SharedPtr<IRayTracingPipeline> mPipeline;




        SharedPtr<ScheduleHandle> mSamplerUpdateSH;
        SharedPtr<ScheduleHandle> mSamplerUpdateAyncSH;
    public:
        RTAO(Renderer* renderer);
        ~RTAO();
        Output Execute(SharedPtr<IComputeContext> context, const Input& input);
    private:
        void Init();
        void InitLibrary();
        void InitTexture();
        void UpdateSampler();
        SharedPtr<ITexture> GetResource(EResource resource);
	};


 
}