#include"JGLogicCore.h"
#include"JGRenderCore.h"
#include"MaterialSystem/Material.h"
#include"MaterialSystem/Mesh/Mesh.h"
using namespace JGLC;
using namespace JGRC;
using namespace std;
JGLogicCore::~JGLogicCore()
{
	if (groundMesh)
	{
		delete groundMesh;
		groundMesh = nullptr;
	}
	if (rabbitMesh)
	{
		delete rabbitMesh;
		rabbitMesh = nullptr;
	}
}
void JGLogicCore::TestInit(JGRenderCore* Rc)
{
	m_RenderCore = Rc;
	string path[2] = { Game::path / "HLSL/SRS_vs.hlsl" ,Game::path / "HLSL/SRS_ps.hlsl" };
	EShaderType type[2] = { EShaderType::Vertex, EShaderType::Pixel };
	string mtPath = Game::path / "Ground";
	m_RenderCore->OutputMaterialData(path, type, 2, mtPath);
	ground_M = m_RenderCore->CreateMaterial(Game::path / "Ground.material");
	groundMesh = new Mesh;
	groundMesh->LoadModel(Game::path / "cube.txt");
	ground_M->AddTexturePath(Game::path / "Texture/T_Water_M.png");
	ground_M->AddTexturePath(Game::path / "Texture/T_Water_N.png");
	ground_M->SetMesh(groundMesh);


	string R_path[2] = { Game::path / "HLSL/CommonShader_vs.hlsl" ,Game::path / "HLSL/CommonShader_ps.hlsl" };
	string R_mtPath = Game::path / "Rabbit";
	m_RenderCore->OutputMaterialData(R_path, type, 2, R_mtPath);
	rabbit_M = m_RenderCore->CreateMaterial(Game::path / "Rabbit.material");
	rabbitMesh = new Mesh;
	rabbitMesh->LoadModel(Game::path / "bunny.txt",false);
	rabbit_M->SetMesh(rabbitMesh);
}
void JGLogicCore::TestTick()
{
	static real yaw = 0.0f;
	yaw += 0.001f;

	GroundSetting(yaw);
	RabbitSetting(yaw);
}

void JGLogicCore::GroundSetting(float yaw)
{
	jgVec3 CameraPos(0.0f, 5.0f, -10.0f);
	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	jgMatrix4x4 transMatrix;
	transMatrix.translation(jgVec3(0.0, -1.0f, 0.0f));
	jgMatrix4x4 scaleMatrix;
	scaleMatrix.scaling(5.0f, 0.2f, 5.0);
	jgMatrix4x4 rotationMatrix;
	rotationMatrix.rotationY(yaw);

	worldMatrix = worldMatrix * scaleMatrix * rotationMatrix * transMatrix;
	worldMatrix.transpose();
	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(CameraPos, jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	viewMatrix.transpose();


	jgMatrix4x4 projectionMatrix = m_RenderCore->GetProjectionMatrix();
	projectionMatrix.transpose();




	ground_M->SetParam("worldMatrix", &worldMatrix);
	worldMatrix = projectionMatrix * viewMatrix * worldMatrix;
	ground_M->SetParam("wvpMatrix", &worldMatrix);
	m_RenderCore->Push(ground_M);
}
void JGLogicCore::RabbitSetting(float yaw)
{
	jgVec3 CameraPos(0.0f, 5.0f, -10.0f);

	// 월드 매트릭스
	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix.rotationY(yaw);
	worldMatrix.transpose();

	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(CameraPos, jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	viewMatrix.transpose();


	jgMatrix4x4 projectionMatrix = m_RenderCore->GetProjectionMatrix();
	projectionMatrix.transpose();


	real color[4] = { 1.0f,1.0f,1.0f,1.0f };

	rabbit_M->SetParam("worldMatrix", &worldMatrix);

	worldMatrix = projectionMatrix * viewMatrix * worldMatrix;
	rabbit_M->SetParam("wvpMatrix", &worldMatrix);
	rabbit_M->SetParam("AmbientColor", color);
	m_RenderCore->Push(rabbit_M);
}