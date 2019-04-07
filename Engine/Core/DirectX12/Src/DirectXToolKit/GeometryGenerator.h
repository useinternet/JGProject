#pragma once

#include"DirectXCommon/DirectXCommon.h"





namespace Dx12
{
	class CommandList;
	class JgVertex;
	class JgSkinnedVertex;
	class JgPosColorVertex;
	class Mesh;
	namespace GeometryGenerator
	{
		Mesh CreateBox(
			CommandList* commandList,
			float width, float height, float depth, 
			uint8_t numSubdivision);

		Mesh CreateSphere(
			CommandList* commandList,
			float radius, uint32_t sliceCount, uint32_t stackCount);


		Mesh CreateCylinder(
			CommandList* commandList,
			float bottomRadius, float topRadius, float height, 
			uint32_t sliceCount, uint32_t stackCount);

		Mesh CreateGrid(
			CommandList* commandList, 
			float width, float depth, uint32_t m, uint32_t n);

		Mesh CreateQuad(
			CommandList* commandList,
			float x, float y, float w, float h, float depth);
	}
}


