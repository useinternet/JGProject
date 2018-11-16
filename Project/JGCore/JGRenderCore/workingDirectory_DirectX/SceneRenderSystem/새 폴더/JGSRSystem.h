#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{
	class CORE_EXPORT JGSRSystem
	{
	private:
		class DirectX* m_Dx;
		std::unique_ptr<class SRSScene> m_Scene;
		std::unique_ptr<class SRSRenderTarget> m_SRSRTT;
		std::queue<class Material*> m_matQue;
	public:
		JGSRSystem();
		~JGSRSystem();
		bool Init(const int width, const int height);
		void push_Material(class Material* mt);
		void Draw();
		void SceneDraw();
	};
}
