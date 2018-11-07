#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"


namespace JGRC
{
	class CORE_EXPORT SRSScene
	{
	private:
		std::unique_ptr<class Mesh> m_Mesh;
		std::unique_ptr<class SRSMaterial> m_Material;
		std::unique_ptr<jgMatrix4x4> m_viewMatrix;
		std::unique_ptr<jgMatrix4x4> m_orthoMatrix;
		std::unique_ptr<jgMatrix4x4> m_worldMatrix;
		std::unique_ptr<jgMatrix4x4> m_wvpMatrix;
		bool m_bMatrixChange = false;
	public:
		SRSScene();
		~SRSScene();
		void CreateScene(const int width, const int height);
	
		void Render(class SRSRenderTarget* SRST);

		void SetOrthoMatrix(const jgMatrix4x4& m);
		void SetViewMatrix(const jgMatrix4x4& m);
	};
}