#include"JGLogicCore.h"
#include"JGRenderCore.h"
#include"MaterialSystem/Mesh/Mesh.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"DirectX/DxWindow.h"
#include"JGRenderCore.h"
#include"DirectX/DirectX.h"
using namespace JGLC;
using namespace JGRC;
using namespace std;
JGLogicCore::~JGLogicCore()
{
	delete Cube;
}
void JGLogicCore::TestInit(JGRenderCore* Rc, HWND hWnd)
{
	this->hWnd = hWnd;
	this->Rc = Rc;
	HlslInit();
	ShaderReader reader(hWnd);
	arr = reader.ReadShader(Game::path / "Engine/Shader/Shader/Sample.shader");

	arr->Get(EShaderType::Pixel)->AddTexture("Default_Texture",Game::path / "Engine/Texture/stone01.dds");
	arr->Get(EShaderType::Pixel)->AddTexture("Normal_Texture", Game::path / "Engine/Texture/bump01.dds");
	// 설정
	Mesh* Cube = new Mesh;
	Cube->LoadModel(Game::path / "cube.txt",true);
	Cube->CreateBuffer(arr->Get(EShaderType::Vertex)->GetLayout());
	DxWindow* wn = Rc->GetDxWindow(hWnd);
	wn->AddRenderPassEvent([Cube,this]()
	{
		Cube->Render();
		arr->Render(Cube->getIndexCount());
	});
	ParamInit();
}
void JGLogicCore::TestTick()
{
	static real yaw = 0;
	yaw += 0.0001f;
	jgMatrix4x4 rotation;
	rotation.rotationY(yaw);

	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix = worldMatrix * rotation;
	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(jgVec3(0.0f, 0.0f, -5.0f), jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	DxWindow* wn = Rc->GetDxWindow(hWnd);
	jgMatrix4x4 projectionMatrix;

	projectionMatrix = wn->GetProjectionMatrix();

	jgMatrix4x4 mvpMatrix = worldMatrix * viewMatrix * projectionMatrix;


	worldMatrix.transpose();
	arr->Get(EShaderType::Vertex)->SetParam("worldMatrix", &worldMatrix, 16);
	mvpMatrix.transpose();
	arr->Get(EShaderType::Vertex)->SetParam("wvpMatrix", &mvpMatrix, 16);
}
void JGLogicCore::HlslInit()
{
	////////// Vertex ///////////////
	HlslEditor* VertexHlsl = Rc->GetHlslEditor(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/StandardShader_vs.hlsl");


	// 입력 레이아웃
	InputLayout* Vertex_Il = VertexHlsl->CreateInputLayout();
	Vertex_Il->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0);
	// 상수 버퍼
	CBuffer* Cbf = VertexHlsl->CreateCBuffer();
	Cbf->SetName("MatrixBuffer");
	Cbf->AddVar("worldMatrix", 0, 15);
	Cbf->AddVar("wvpMatrix", 16, 31);

	////////// Pixel ///////////////
	HlslEditor* PixelHlsl = Rc->GetHlslEditor(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/StandardShader_ps.hlsl");

	CBuffer* psCbf = PixelHlsl->CreateCBuffer();
	psCbf->SetName("Material");
	psCbf->AddVar("Ambient", 0, 2);
	psCbf->AddVar("ReflectIntensity",3, 3);
	psCbf->AddVar("SpecularColor", 4, 6);
	psCbf->AddVar("SpecularPower", 7, 7);
	psCbf->AddVar("Emissive", 8, 10);
	psCbf->AddVar("CustomVar", 11, 11);


	// 텍스쳐
	Texture2D* texture = PixelHlsl->CreateTexture2D();
	texture->Add("Default_Texture");
	texture->Add("Normal_Texture");
	SamplerState* Sampler = PixelHlsl->CreateSamplerState();
	Sampler->AddDefaultWrapMode();

	ShaderWriter writer;
	writer.AddEditor(VertexHlsl);
	writer.AddEditor(PixelHlsl);
	writer.Write(Game::path / "Engine/Shader/Shader/Sample");
}
void JGLogicCore::ParamInit()
{
	real Ambient[3] = { 1.0f,1.0f,1.0f };
	real ReflectIntensity = 0.5f;
	real SpecularColor[3] = { 1.0f,1.0f,1.0f };
	real SpecularPower = 1.0f;
	real Emissive[3] = { 1.0f,1.0f,1.0f };
	real CustomVar = 0.5f;



	arr->Get(EShaderType::Pixel)->SetParam("Ambient", Ambient, 3);
	arr->Get(EShaderType::Pixel)->SetParam("ReflectIntensity", &ReflectIntensity, 1);
	arr->Get(EShaderType::Pixel)->SetParam("SpecularColor", SpecularColor, 3);
	arr->Get(EShaderType::Pixel)->SetParam("SpecularPower", &SpecularPower, 1);
	arr->Get(EShaderType::Pixel)->SetParam("Emissive", Emissive, 3);
	arr->Get(EShaderType::Pixel)->SetParam("CustomVar", &CustomVar, 1);
}