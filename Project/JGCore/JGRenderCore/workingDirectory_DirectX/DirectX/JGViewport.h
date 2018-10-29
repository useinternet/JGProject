#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"

namespace JGRC
{
	class CORE_EXPORT JGViewport
	{
	private:
		std::unique_ptr<D3D11_VIEWPORT> m_Viewport;
		std::unique_ptr<jgMatrix4x4> m_projectionMatrix;
		std::unique_ptr<jgMatrix4x4> m_orthoMatrix;
		real m_FOV = 0;
		real m_FarZ = 0;
		real m_NearZ = 0;
		real m_Width = 0;
		real m_Height = 0;
	public:
		JGViewport();
		~JGViewport();
		/*
		Exp : 뷰포트를 초기화 한다.
		@param const int Width   : 가로 길이
		@param const int Height  : 세로 길이
		@param const float FOV   : 시야각
		@param const float FarZ  : 렌더링 최대 거리
		@param const float NearZ : 렌더링 최소 거리  */
		bool InitViewport(const int Width, const int Height, const real FOV, const real FarZ, const real NearZ);

		/*
		Exp : 뷰포트를 가져온다. */
		D3D11_VIEWPORT* Get();
		///*
		//Exp : 투영 행렬을 가져온다. */
		jgMatrix4x4& GetProjectionMatrix();
		///*
		//Exp : 정사영 행렬을 가져온다. */
		jgMatrix4x4& GetOrthoMatrix();

		/*
		Exp : 시야각을 가져온다. */
		const real GetFOV();
		/*
		Exp : 가로길이를 가져온다. */
		const real GetWidth();
		/*
		Exp : 세로길이를 가져온다. */
		const real GetHeight();

		/*
		Exp : 시야각을 셋팅한다. */
		void SetFOV(const real FOV);
	private:
		void CreateViewport(const real MinDepth = 0.0f, const real MaxDepth = 1.0f,
			const real TopLeftX = 0.0f, const real TopLeftY = 0.0f);
		void CreateProjectionMatrix();
		void CreateOrthoMatrix();
	};


}

