#include"PickingSystem.h"
#include"Camera.h"
#include"Data/JGMesh.h"
using namespace std;
using namespace JGRC;
using namespace DirectX;
bool PickingSystem::CheckPicking(int x, int y, RenderItem* Item,Camera* Cam)
{
	Cam->UpdateViewMatrix();
	XMFLOAT4X4 P = Cam->GetProj4x4f();

	float vx = (+2.0f*x / m_Width - 1.0f) / P(0, 0);
	float vy = (-2.0f*y / m_Height + 1.0f) / P(1, 1);

	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = Cam->GetView();
	XMMATRIX invV = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	auto geo = Item->Geo;
	XMMATRIX W = XMLoadFloat4x4(&Item->World);
	XMMATRIX invW = XMMatrixInverse(&XMMatrixDeterminant(W), W);
	XMMATRIX toLocal = XMMatrixMultiply(invV, invW);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	rayDir = XMVector3Normalize(rayDir);

	float tmin = 0.0f;
	if (Item->Bounds.Intersects(rayOrigin, rayDir, tmin))
	{
		// 메시 판정
		auto vertexData = (Vertex*)geo->VertexBufferCPU->GetBufferPointer();
		auto indexData = (uint16_t*)geo->IndexBufferCPU->GetBufferPointer();

		UINT triCount = Item->IndexCount / 3;

		tmin = MathHelper::Infinity;
		for (UINT i = 0; i < triCount; ++i)
		{
			UINT i0 = indexData[i * 3 + 0];
			UINT i1 = indexData[i * 3 + 1];
			UINT i2 = indexData[i * 3 + 2];

			XMVECTOR v0 = XMLoadFloat3(&vertexData[i0].Position);
			XMVECTOR v1 = XMLoadFloat3(&vertexData[i1].Position);
			XMVECTOR v2 = XMLoadFloat3(&vertexData[i2].Position);

			float t = 0.0f;
			if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t))
			{
				if (t < tmin)
				{
					tmin = t;
					return true;
				}
			}
		}
	}
	return false;
}