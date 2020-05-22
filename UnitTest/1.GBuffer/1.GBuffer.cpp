// 1.GBuffer.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "resource.h"

#include "DirectXApi.h"
#include "DirectXSDDefines.h"
// #pragma comment(lib, "DirectX12.lib")
using namespace Concurrency;

using namespace std;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND g_hWnd;
using namespace DX12;
using namespace std::chrono;
class App;




enum class TimeStepType
{
    Hour,
    Minute,
    Second,
    MiliSecond
};
class EngineTimer
{
public:
    void Start();
    void Tick();
    void Stop();

    float GetTotalTime(TimeStepType step_type = TimeStepType::Second) const;
    float GetTick(TimeStepType step_type = TimeStepType::Second) const;
    uint32_t GetFPS() const;
private:
    float m_TotalTime = 0.0f;
    float m_DeltaTime = 0.0f;
    uint32_t m_FPS = 0;
    bool m_IsStart = false;
    bool m_IsFirst = true;
    uint32_t init_count = 0;
};





void EngineTimer::Start()
{
    m_IsStart = true;
}
void EngineTimer::Tick()
{
    static system_clock::time_point start_point;
    static uint32_t frame_count = 0;
    static float    time_elapsed = 0.0f;
    if (init_count < 5) init_count++;
    if (!m_IsStart)
    {
        m_DeltaTime = 0.0f;
        return;
    }




    duration<float> sec = system_clock::now() - start_point;
    m_DeltaTime = sec.count();






    if (m_DeltaTime >= 0.0f && !m_IsFirst)
    {
        m_TotalTime += m_DeltaTime;

        // fps 구하기
        ++frame_count;
        time_elapsed += m_DeltaTime;
        if (time_elapsed >= 1.0f)
        {
            m_FPS = frame_count;
            frame_count = 0;
            time_elapsed = 0.0f;
        }
    }
    start_point = system_clock::now();
    m_IsFirst = false;
}
void EngineTimer::Stop()
{
    m_IsStart = false;
}

float EngineTimer::GetTotalTime(TimeStepType step_type) const
{
    switch (step_type)
    {
    case TimeStepType::MiliSecond:
        return m_TotalTime * 1000;
    case TimeStepType::Second:
        return m_TotalTime;
    case TimeStepType::Minute:
        return m_TotalTime / 60.0f;
    case TimeStepType::Hour:
        return m_TotalTime / 60.0f / 60.0f;
    }
    return 0.0f;
}
float EngineTimer::GetTick(TimeStepType step_type) const
{
    if (init_count < 5)
        return 0.0f;
    switch (step_type)
    {
    case TimeStepType::MiliSecond:
        return m_DeltaTime * 1000;
    case TimeStepType::Second:
        return m_DeltaTime;
    case TimeStepType::Minute:
        return m_DeltaTime / 60.0f;
    case TimeStepType::Hour:
        return m_DeltaTime / 60.0f / 60.0f;
    }
    return 0.0f;
}
uint32_t EngineTimer::GetFPS() const
{
    return m_FPS;
}




//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY1GBUFFER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY1GBUFFER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   g_hWnd = hWnd;
   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//std::vector<Vertex> v = {
//        Vertex(DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f)),
//       Vertex(DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f)),
//       Vertex(DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f)),
//       Vertex(DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f)),
//       Vertex(DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f)),
//       Vertex(DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f)),
//       Vertex(DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f)),
//       Vertex(DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f))
//};
        
void LoadMesh(vector<StaticVertex>& v, vector<uint32_t>& i, string path);

      //std::vector<std::uint32_t> i =
      //  	{
      //  		// front face
      //  		0, 1, 2,
      //  		0, 2, 3,

      //  		// back face
      //  		4, 6, 5,
      //  		4, 7, 6,

      //  		// left face
      //  		4, 5, 1,
      //  		4, 1, 0,

      //  		// right face
      //  		3, 2, 6,
      //  		3, 6, 7,

      //  		// top face
      //  		1, 5, 6,
      //  		1, 6, 2,

      //  		// bottom face
      //  		4, 0, 3,
      //  		4, 3, 7
      //  	};
class App
{
public:
    void Init();
    void Update();
private:

    EngineTimer timer;

    StaticMesh<StaticVertex, DXSD::SDStaticObject> mesh;
    DX12::Texture texture1;
    RenderTarget renderTarget;
    RootSignature rootSig;


    PixelShader  pShader;
    VertexShader vShader;

    GraphicsPipelineState gPSO;
};

struct GameObject
{
    float4x4 world;
};

struct CameraCB
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
};



void App::Init()
{
    timer.Start();
    DX12::DXDevice::Create();
    DX12::DXDevice::RegisterScreen(g_hWnd, 1920, 1080);
    
    



    // 루트 서명
    {


        rootSig.InitAsSRV(0, 1);
      //  rootSig.InitAsSRV(0, 0);
        rootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
        rootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(0));
        rootSig.Finalize();

    }
    //
    {
        vShader = DXDevice::CreateVertexShaderFromFile("../../JGEngine/Resource/Shader/Core/template_vs.hlsl", {});
        pShader = DXDevice::CreatePixelShaderFromFile("../../JGEngine/Resource/Shader/Core/template_ps.hlsl", {});

        if (vShader.GetErrorCode().length() != 0)
        {
            string error = vShader.GetErrorCode();
            int blanck = 0;
        }
        if (pShader.GetErrorCode().length() != 0)
        {
            string error = pShader.GetErrorCode();
            int blanck = 0;
        }
    }
    // Geo
    {
        //

        auto cmdKey = DXDevice::RequestGraphicsCommandKey();

   
        vector<StaticVertex> v;
        vector<uint32_t> i;

        LoadMesh(v, i, "../Resource/xbot.mesh");
       
        uint32_t vbtSize = sizeof(StaticVertex) * (uint32_t)v.size();
        uint32_t ibtSize = sizeof(uint32_t) * (uint32_t)i.size();
        mesh.indexCount = i.size();
        mesh.vBuffer.Create("VertexBuffer", vbtSize);
        mesh.iBuffer.Create("IndexBuffer", ibtSize);
        DXCommand::CopyVertexBuffer(cmdKey, mesh.vBuffer, v.data(), sizeof(StaticVertex), v.size());
        DXCommand::CopyIndexBuffer(cmdKey, mesh.iBuffer, i.data(), i.size());


        bool is_result = DXCommand::LoadTextureFromFile(cmdKey, "../../JGEngine/Resource/Textures/Test/bricks3.dds", false);

    }


    // 렌더 타겟 설정
    {
        renderTarget[0].CreateRenderTargetTexture("GBuffer", DXGI_FORMAT_R8G8B8A8_UNORM, 1920, 1080, 1);
        renderTarget.GetDepthStencilTexture().CreateDepthStencilTexture("GBufferDepth",
            DXGI_FORMAT_D24_UNORM_S8_UINT, 1920, 1080, 1);
        DXDevice::RequestSubmitTextureToScreen(g_hWnd, renderTarget[0]);
    }

    {
        InputLayout input;
		input.Add("POSITION", _float3_, 0);
		input.Add("NORMAL", _float3_, 0);
		input.Add("TANGENT", _float3_, 0);
		input.Add("TEXCOORD", _float2_, 0);
        gPSO.BindInputLayout(input);
        gPSO.BindRootSignature(rootSig);
        gPSO.BindShader(ShaderType_Vertex, vShader);
        gPSO.BindShader(ShaderType_Pixel, pShader);
        gPSO.BindRenderTarget(renderTarget);
        gPSO.Finalize();

    }
    DX12::DXDevice::EndFrame();


    {
        string workDir = "../../JGEngine/Resource/New_Shader/";
        string headerFile = "DirectXSDDefines.h";
        DXModuleSystem::Init(workDir, headerFile);


        RendererModuleInitDesc desc;
        desc.SignConstantBuffer(0, 0, TO_STRUCT_STRING(CBCamera));
        desc.SignStructuredBuffer("gStaticInstances", 0, 0, TO_STRUCT_STRING(SDStaticObject));
        desc.SignStructuredBuffer("gSekeltalInstances", 0, 1, TO_STRUCT_STRING(SDSkinnedObject));
        desc.SignStructuredBuffer("gPaperInstances", 0, 2, TO_STRUCT_STRING(SDPaperObject));
        desc.SignTexture2D("gInputTexture", 8, 0, 3);
        desc.SetEntryObject(TO_STRUCT_STRING(SDStaticObject), Input_Vertex_Type_Static, TO_STRUCT_STRING(SDStaticObject));
        desc.SetEntryObject(TO_STRUCT_STRING(SDSkinnedObject), Input_Vertex_Type_Skinned, TO_STRUCT_STRING(SDSkinnedObject));
        desc.SetEntryObject(TO_STRUCT_STRING(SDPaperObject), Input_Vertex_Type_Paper, TO_STRUCT_STRING(SDPaperObject));
        desc.AddRenderTarget("normal", DXGI_FORMAT_R8G8B8A8_UNORM);
        desc.AddRenderTarget("test1", DXGI_FORMAT_R8G8B8A8_UNORM);
        desc.AddRenderTarget("test2", DXGI_FORMAT_R8G8B8A8_UNORM);






        RendererModule module;
        module.SubmitDescAndCreateRootSigFile(desc);


    }
}
int scount = 0;
void App::Update()
{
    DX12::DXDevice::BeginFrame();
    if (!texture1.IsVaild())
    {
        texture1 = DXDevice::LoadRegisteredTexture("../../JGEngine/Resource/Textures/Test/bricks3.dds");

    }
 



    timer.Tick();

    float fps = timer.GetFPS();
    std::string str = "fps : " + std::to_string(fps);
    SetWindowTextA(g_hWnd, str.c_str());

    
    Viewport viewport;
    ScissorRect scisrect;
    viewport.Set(1920, 1080);
    scisrect.Set(1920, 1080);


    auto cmdKey = DXDevice::RequestGraphicsCommandKey(-1);
    DXCommand::SetViewport(cmdKey, viewport);
    DXCommand::SetScissorRect(cmdKey, scisrect);

    DXCommand::ClearRenderTarget(cmdKey, renderTarget);
    DXCommand::SetRenderTarget(cmdKey, renderTarget);
    DXCommand::BindRootSignature(cmdKey, rootSig);
    DXCommand::BindPipelineState(cmdKey, gPSO);
    static float ss = 0.0f;
    auto pos = DirectX::XMVectorSet(0, 400, -700, 1);
    auto target = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0, 0.0F, 1.0F, 1));
    auto up = DirectX::XMVectorSet(0, 1, 0, 1);
    float3 rorate = { 0.0f, ss, 0.0f };
    auto world = DirectX::XMMatrixRotationY(rorate.y);
    auto view = DirectX::XMMatrixLookAtLH(pos, target, up);

    auto proj = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, (float)1920 / (float)1080, 0.1f, 1000.0f);

    auto worldViewProj = DirectX::XMMatrixMultiply(world, view);
    worldViewProj = DirectX::XMMatrixMultiply(worldViewProj, proj);
    worldViewProj = DirectX::XMMatrixTranspose(worldViewProj);

    ss += 0.0001f;
    float4x4 result;
    DirectX::XMStoreFloat4x4(&result, worldViewProj);
    DXCommand::BindDynamicStructuredBuffer(cmdKey, 0, &result, sizeof(result), 1);
    if (texture1.IsVaild())
    {
        DXCommand::BindTexture(cmdKey, 1, texture1);
    }


    DXCommand::SetPrimitiveTopology(cmdKey, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    DXCommand::BindVertexBuffer(cmdKey, mesh.vBuffer);
    DXCommand::BindIndexBuffer(cmdKey, mesh.iBuffer);
    DXCommand::DrawIndexed(cmdKey, mesh.indexCount);
    DXCommand::DrawIndexed(cmdKey, mesh.indexCount);

    DX12::DXDevice::EndFrame();
}

void LoadMesh(vector<StaticVertex>& _v, vector<uint32_t>& _i, string path)
{

    fs::path file_path(path);
    auto file_format = file_path.extension();

    if (file_format != ".mesh")
        return;


    ifstream fin;
    fin.open(path, ios::binary);

    // 메시 데이터 갯수
    uint32_t meshCount = 0;
    fin.read(
        reinterpret_cast<char*>(&meshCount),
        sizeof(uint32_t));


    std::vector<std::string> MeshNames(meshCount);

    // 스켈레톤 메시 인가 아닌가.
    bool is_skeletal = false;
    fin.read(
        reinterpret_cast<char*>(&is_skeletal),
        sizeof(bool));

    for (uint32_t i = 0; i < meshCount; ++i)
    {
        // 메시 이름 길이
        uint32_t name_len = 0;
        fin.read(
            reinterpret_cast<char*>(&name_len),
            sizeof(uint32_t));

        // 메시 이름
        char* mesh_name_Cstr = new char[name_len];
        fin.read(
            reinterpret_cast<char*>(mesh_name_Cstr),
            sizeof(char) * name_len);
        std::string meshname = mesh_name_Cstr;
        MeshNames[i] = meshname;
        delete mesh_name_Cstr;


        // Vertex & Index 갯수
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        fin.read(
            reinterpret_cast<char*>(&vertexCount),
            sizeof(uint32_t));
        fin.read(
            reinterpret_cast<char*>(&indexCount),
            sizeof(uint32_t));

        if (is_skeletal)
        {
            assert(false && "current is not supported skeletalmesh");
        }
        else // 스태틱 메시 일경우
        {
            std::vector<StaticVertex> vertices;
            std::vector<uint32_t> indices;

            vertices.resize(vertexCount);
            indices.resize(indexCount);

            fin.read(
                reinterpret_cast<char*>(vertices.data()),
                sizeof(StaticVertex) * vertexCount);
            fin.read(
                reinterpret_cast<char*>(indices.data()),
                sizeof(uint32_t) * indexCount);


            _v.insert(_v.end(), vertices.begin(), vertices.end());
            _i.insert(_i.end(), indices.begin(), indices.end());
        }
    }
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY1GBUFFER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1GBUFFER));

    MSG msg = { 0 };

    App* app = new App();

    app->Init();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        app->Update();
    }



    delete app;
    DX12::DXDevice::Destroy();
    return (int)msg.wParam;
}
