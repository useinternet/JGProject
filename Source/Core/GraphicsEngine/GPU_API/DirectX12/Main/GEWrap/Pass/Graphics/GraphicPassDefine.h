#pragma once

#include "Pass.h"
#include "SceneObject.h"
#include "Scene.h"
#include "DirectXApi.h"




namespace DX12
{
	class DX12_GraphicPass;
	class GraphicPassMap;
	class DX12_GraphicsRenderer;
	class GPRunData
	{
		friend GraphicPassMap;
		friend DX12_GraphicsRenderer;
	public:
		virtual ~GPRunData() {}
	public:
		template<typename PassType>
		DX12_GraphicPass* GetPass()
		{
			if (PassMap) return PassMap->GetPass<PassType>();
			else return nullptr;
		}

		bool GetPSO(GE::SceneObject* obj, GraphicsPipelineState& pso);
		size_t GetFinalHashCode() const {
			return Final;
		}

		template<typename PassType>
		void SetFinalPass() {
			Final = typeid(PassType).hash_code();
		}
	public:
		DX12_GraphicPass*     Pass    = nullptr;
		GraphicsCommandKeyPtr CmdKey;
		GE::Scene*            Scene   = nullptr;
		std::vector<GE::SceneObject*> DrawingObjects;
	private:
		DX12_GraphicsRenderer* OwnerRenderer = nullptr;
		GraphicPassMap*        PassMap = nullptr;
		size_t Final = 0;
	};



	class DX12_GraphicPass : public GE::GraphicPass
	{
		DX12_GraphicPass(const DX12_GraphicPass& rhs) = delete;
		DX12_GraphicPass& operator=(const DX12_GraphicPass& rhs) = delete;

	public:
		DX12_GraphicPass() {}
		virtual ~DX12_GraphicPass() {}

		std::unique_ptr<DX12_GraphicPass> Copy(std::wstring& errorCode) const;
	public:
		RenderTarget          RT;
		GraphicsPipelineState PSO;



		// [] (RenderTarget& rt, GraphicsPipelineState& pso, std::wstring& errCode) {}
	
		std::function<void(RenderTarget&, GraphicsPipelineState&, std::wstring&)> SetUp;

		// [](GPRunData& data, std::wstring& error) {}

		std::function<void(GPRunData&, std::wstring&)> Run;
	};




	class GraphicPassMap
	{
	public:
		static void SetRootSig(const RootSignature& rootSig)
		{
			if (RootSig == nullptr) RootSig = std::make_unique<RootSignature>();
			*RootSig = rootSig;
		}
		static RootSignature& GetRootSig() {
			return *RootSig.get();
		}
		static void ResetRootSig()
		{
			RootSig.reset();
			RootSig = nullptr;
		}
	private:
		static std::unique_ptr<RootSignature> RootSig;
		std::unordered_map<size_t, std::unique_ptr<DX12_GraphicPass>> PassMap;
		std::wstring  ErrorCode;
		size_t        Entry = 0;
	public:
		template<typename PassType>
		void SetEntryPass() {
			size_t hash = typeid(PassType).hash_code();
			auto iter = PassMap.find(hash);

			if (iter == PassMap.end()) return;
			Entry = hash;
		}

		template<typename PassType>
		DX12_GraphicPass* GetPass()
		{
			return GetPass(typeid(PassType).hash_code());
		}

		DX12_GraphicPass* GetPass(size_t hash) {
			auto iter = PassMap.find(hash);
			if (iter == PassMap.end()) return nullptr;
			return iter->second.get();
		}




		template<typename PassType>
		void AddPass()
		{
			if (RootSig == nullptr) return;

			size_t hash = typeid(PassType).hash_code();
			auto iter = PassMap.find(hash);
			if (iter == PassMap.end())
			{
				PassMap[hash] = std::make_unique<PassType>();
				PassMap[hash]->SetUp(PassMap[hash]->RT, PassMap[hash]->PSO, ErrorCode);
			}
		}

		std::unique_ptr<GraphicPassMap> Copy()
		{
			auto result = std::make_unique<GraphicPassMap>();
			result->Entry = Entry;
			for (auto& passPair : PassMap)
			{
				result->PassMap[passPair.first] = std::move(passPair.second->Copy(result->ErrorCode));
			}
			return std::move(result);
		}
	public:
		void Run(GPRunData& data)
		{
			if(!ErrorCode.empty()) ErrorCode.clear();
			
			if (Entry == 0) assert("GraphicPassMap Entry Point is 0");
			data.PassMap = this;
			data.Pass = PassMap[Entry].get();
			PassMap[Entry]->Run(data, ErrorCode);
		}
	};

	namespace RootParam
	{
		enum
		{
			// InitCBV
			Camera, // ConstantBuffer
			Material, // ConstantBuffer
			AnimData,
			DirectionalLight,
			PassData,

			// InitSRV
			Object,
			PointLight,
			SpotLight,

			// InitDescriptorTable
			InputTexture,
			BindTexture,
			SLShadowMap,
			PLShadowMap,
			DLShadowMap,
			//
			RootParam_Count
		};
	}
}


