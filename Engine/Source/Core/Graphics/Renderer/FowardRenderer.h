#pragma once
#include "Graphics/Renderer.h"



namespace JG
{
	class PreRenderProcess_ComputeCluster;
	class FowardRenderer : public Renderer
	{
		PreRenderProcess_ComputeCluster* mComputeCluster = nullptr;
	public:
		FowardRenderer();
		virtual ~FowardRenderer() = default;
		//
		// 
		/* 동시에 할 일
		* Light 기준으로 깊이버퍼 생성
		* SSAO를 위해 그리기
		* Cluster 준비
		*/
		/* Cluster
		1. Cluster 를 생성
		2. 라이트가 포함되어있는 Cluster에 LightIndex 계산
		3. Rendering 과정에서 현재 위치의 ClusterIndex를 알아내고, ClusterIndex에 해당하는 Light 정보들을 불러옴
		4. Light 계산
		*/
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	};
}