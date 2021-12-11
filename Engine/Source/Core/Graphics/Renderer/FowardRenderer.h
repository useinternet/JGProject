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
		/* ���ÿ� �� ��
		* Light �������� ���̹��� ����
		* SSAO�� ���� �׸���
		* Cluster �غ�
		*/
		/* Cluster
		1. Cluster �� ����
		2. ����Ʈ�� ���ԵǾ��ִ� Cluster�� LightIndex ���
		3. Rendering �������� ���� ��ġ�� ClusterIndex�� �˾Ƴ���, ClusterIndex�� �ش��ϴ� Light �������� �ҷ���
		4. Light ���
		*/
	public:
		virtual ERendererPath GetRendererPath() const override { return ERendererPath::Foward; }
		virtual void ReadyImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual void RenderImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual void CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual int ArrangeObject(const ObjectInfo& info) override;
	};
}