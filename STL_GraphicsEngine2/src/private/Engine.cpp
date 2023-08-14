#include "Engine.h"
#include <d3dcompiler.h>

#include "Vertex.h"

#include "BasicShader.h"
#include "TextureMappingShader.h"

#include "InputProcessor.h"

#include "MathUtil.h"

static float orbitAngle = 0.0f;

Engine::Engine(HINSTANCE hInstance, int width, int height, std::wstring title)
    : DXApp(hInstance, width, height, title)
{

}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
    // 창 만들기.
    // 장치 생성.
    if (DXApp::Initialize() == false)
    {
        return false;
    }

    // 장면 초기화.
    if (InitializeScene() == false)
    {
        return false;
    }

    return true;
}

// 1. 메시지 처리 루프.
// 2. 엔진 루프.
int Engine::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // ESC 종료.
            if (InputProcessor::IsKeyDown(Keyboard::Keys::Escape) == true)
            {
                if (MessageBox(nullptr, L"종료하시겠습니까?", L"종료", MB_YESNO) == IDYES)
                {
                    DestroyWindow(Window::WindowHandle());
                    return 0;
                }
            }
           
            Update();
            DrawScene();
        }
    }

    return 0;
}

void Engine::Update()
{
    // 카메라 이동 처리.
    static float moveSpeed = 2.0f;
    if (InputProcessor::IsKeyDown(Keyboard::Keys::W) == true)
    {
        camera.MoveForward(moveSpeed);
    }
    if (InputProcessor::IsKeyDown(Keyboard::Keys::S) == true)
    {
        camera.MoveForward(-moveSpeed);
    }
    if (InputProcessor::IsKeyDown(Keyboard::Keys::A) == true)
    {
        camera.MoveRight(-moveSpeed);
    }
    if (InputProcessor::IsKeyDown(Keyboard::Keys::D) == true)
    {
        camera.MoveRight(moveSpeed);
    }
    if (InputProcessor::IsKeyDown(Keyboard::Keys::Q) == true)
    {
        camera.MoveUp(moveSpeed);
    }
    if (InputProcessor::IsKeyDown(Keyboard::Keys::E) == true)
    {
        camera.MoveUp(-moveSpeed);
    }

    // 카메라 회전 처리.
    Mouse::State state = InputProcessor::MouseState();
    static float rotationSpeed = 0.2f;
    if (state.leftButton == true)
    {
        camera.Yaw((float)state.x * rotationSpeed);
        camera.Pitch((float)state.y * rotationSpeed);
    }

    // 모델 이동 처리
    modelUV.SetRotation(modelUV.Rotation().x, modelUV.Rotation().y, modelUV.Rotation().z - 1);
    // 모델 회전
    satelliteModel.SetPosition(50 + 100 * (cos(orbitAngle) - 0.5f ), 50 + 100 * (sin(orbitAngle) - 0.5f), satelliteModel.Position().z);
    orbitAngle += 0.050f;
    

    camera.UpdateCamera();
    modelUV.UpdateBuffers(deviceContext.Get());
    satelliteModel.UpdateBuffers(deviceContext.Get());
}

void Engine::DrawScene()
{
    // 색상 고르기.
    float backgroundColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };
    
    // 지우기 (Clear) - 실제로는 덮어씌워서 색칠하기.
    // Begin Draw(Render) - DX9.
    deviceContext.Get()->ClearRenderTargetView(renderTargetView.Get(), backgroundColor);

    // 카메라 바인딩.
    camera.BindBuffer(deviceContext.Get());

    // 그리기 준비. (쉐이더 바꾸기.)
    BasicShader::Bind(deviceContext.Get());
    satelliteModel.RenderBuffers(deviceContext.Get());
    textureShader.Bind(deviceContext.Get());
    modelUV.RenderBuffers(deviceContext.Get());

    // 프레임 바꾸기. FrontBuffer <-> BackBuffer.
    swapChain.Get()->Present(1, 0);
}

bool Engine::InitializeScene()
{
    // 카메라 생성.
    camera = Camera(
        70.0f * MathUtil::Deg2Rad,
        (float)Window::Width(),
        (float)Window::Height(),
        0.1f,
        10000.0f
    );
    // 카메라 위치 설정.
    camera.SetPosition(0.0f, 0.0f, -200.0f);    

    // 카메라 버퍼 생성.
    if (camera.CreateBuffer(device.Get()) == false)
    {
        return false;
    }

    if (BasicShader::Compile(device.Get()) == false)
    {
        return false;
    }
    if (BasicShader::Create(device.Get()) == false)
    {
        return false;
    }

    if (textureShader.Initialize(device.Get(), L"dog.jpg") == false)
    {
        return false;
    }

    if (modelUV.InitializeBuffers(device.Get(), textureShader.ShaderBuffer(), "sphere.fbx") == false)
    {
        return false;
    }
    modelUV.SetPosition(0.0f, 0.0f, 0.0f);
    modelUV.SetRotation(0.0f, 0.0f, 0.0f);
    //modelUV.SetScale(50.0f, 50.0f, 50.0f);
    modelUV.SetScale(1.0f, 1.0f, 1.0f);

    if (satelliteModel.InitializeBuffers(device.Get(), BasicShader::ShaderBuffer(), "sphere.fbx") == false)
    {
        return false;
    }
    satelliteModel.SetPosition(0.0f, 0.0f, 0.0f);
    satelliteModel.SetScale(0.6f, 0.6f, 0.6f);

    return true;
}
