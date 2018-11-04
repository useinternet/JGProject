#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

namespace JGRC
{
	class TestModel
	{
	private:
		struct ModelType
		{
			float x, y, z;
			float tu, tv;
			float nx, ny, nz;
		};
	public:
		TestModel();
		~TestModel();

		bool Initialize(ID3D11Device*, const char*, bool bump);
		void Shutdown();
		void Render(ID3D11DeviceContext*);

		int GetIndexCount();
		bool LoadModel(const char*);
	private:
		bool InitializeBuffers(ID3D11Device*);
	private:
		bool bBump = false;
		int m_vertexCount, m_indexCount;
		std::vector<real> m_Sample;
		ModelType* m_model;
		class JGBuffer* VertexBuffer;
		class JGBuffer* PosBuffer;
		class JGBuffer* TexBuffer;
		class JGBuffer* NormalBuffer;
		class JGBuffer* IndexBuffer;
	private:
		void MakeBump();
	};
}
