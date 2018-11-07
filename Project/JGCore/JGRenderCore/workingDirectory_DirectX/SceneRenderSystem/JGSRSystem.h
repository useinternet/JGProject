#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
// 
/* 대략적인 순서
1. 일단 SRSRenderTarget 준비
2. ShaderAnalyzer로 분석후 머터리얼 추출
3. Material 생성
4. 그린다.
5. DirectX Render 부분은 윈도우텍스쳐를 가져와서 그린다.
*/
/*

그릴때 셰이더가 틀리다.. 고로 머터리얼 내에서 메쉬 Draw와 셰이더 Draw를 분리..
머터리얼->SetMesh() 함수를 통해 언제든지 탈부착 가능하게 셋팅
Mesh 클래스를 따로 만들자 걍( 메쉬 부터 차근히 만들자 걍 고럼 이건 미뤄두기로하고 메쉬부터 ㄱㄱ) <- 수정 mrt 렌더링 먼저 구현하고 fbx sdk 이용하여 메쉬
//**** 리소스 매니저 프로젝트를 하나 추가..
텍스쳐에는 ( 리소스뷰와 샘플러를 같이 묶어두자..)
// fbx sdk 알아보자
*/
namespace JGRC
{
	class JGSRSystem
	{
	private:
		class DirectX* m_Dx;
		std::function<void()> m_RenderFunc;
		std::unique_ptr<class SRSRenderTarget> m_SRSRT;
		std::vector<real> m_ClearColor;
	public:
		JGSRSystem();
		~JGSRSystem();
		void BindingRenderFunc(const std::function<void()>& func);
		bool InitSRSystem(const int width, const int height);
		void Render();
		void SetClearColor(const real r, const real g, const real b, const real a);
		SRSRenderTarget* GetRenderTarget() const;
	};
}
