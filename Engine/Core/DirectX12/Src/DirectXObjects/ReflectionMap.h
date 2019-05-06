#pragma once
#include"CubeMap.h"


namespace Dx12
{
	class GBuffer;
	class Scene;
	class CommandList;
	class Dx12Object;
	class ReflectionMap : public CubeMap
	{
	private:
		std::unique_ptr<GBuffer> m_GBuffer;
		std::unique_ptr<Scene>   m_Scene[6];
		bool m_EnableMipMaps;
	public:
		/* 큐브맵 생성자
		@param width,height : 큐브맵 크기 
		@param farZ, NearZ  : 최소거리, 최대 거리,
		@param format : 큐브맵 Format */
		ReflectionMap(
			uint32_t width, uint32_t height,
			float farZ, float NearZ , bool enableMipmap = false,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);


		// 큐브 맵 그리기
		// @param commandList : commandList 클래스
		// @param objArray : 그릴 오브젝트 배열(vector 타입)
		void Draw(CommandList* commandList, const std::vector<Dx12Object*>& objArray);

		// 크기 재설정
		// @param width,height : 변경할 큐브맵 크기
		void Resize(uint32_t width, uint32_t height);

		// CubeMap 텍스쳐 가져오기
		virtual const Texture& GetTexture() const override;
	protected:
		virtual void Build() override;
		void ResourceBuild();
	};
}