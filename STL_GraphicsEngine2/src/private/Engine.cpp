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
    // â �����.
    // ��ġ ����.
    if (DXApp::Initialize() == false)
    {
        return false;
    }

    // ��� �ʱ�ȭ.
    if (InitializeScene() == false)
    {
        return false;
    }

    return true;
}

// 1. �޽��� ó�� ����.
// 2. ���� ����.
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
            // ESC ����.
            if (InputProcessor::IsKeyDown(Keyboard::Keys::Escape) == true)
            {
                if (MessageBox(nullptr, L"�����Ͻðڽ��ϱ�?", L"����", MB_YESNO) == IDYES)
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
    // ī�޶� �̵� ó��.
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

    // ī�޶� ȸ�� ó��.
    Mouse::State state = InputProcessor::MouseState();
    static float rotationSpeed = 0.2f;
    if (state.leftButton == true)
    {
        camera.Yaw((float)state.x * rotationSpeed);
        camera.Pitch((float)state.y * rotationSpeed);
    }

    // �� �̵� ó��
    modelUV.SetRotation(modelUV.Rotation().x, modelUV.Rotation().y, modelUV.Rotation().z - 1);
    // �� ȸ��
    satelliteModel.SetPosition(50 + 100 * (cos(orbitAngle) - 0.5f ), 50 + 100 * (sin(orbitAngle) - 0.5f), satelliteModel.Position().z);
    orbitAngle += 0.050f;
    

    camera.UpdateCamera();
    modelUV.UpdateBuffers(deviceContext.Get());
    satelliteModel.UpdateBuffers(deviceContext.Get());
}

void Engine::DrawScene()
{
    // ���� ����.
    float backgroundColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };
    
    // ����� (Clear) - �����δ� ������� ��ĥ�ϱ�.
    // Begin Draw(Render) - DX9.
    deviceContext.Get()->ClearRenderTargetView(renderTargetView.Get(), backgroundColor);

    // ī�޶� ���ε�.
    camera.BindBuffer(deviceContext.Get());

    // �׸��� �غ�. (���̴� �ٲٱ�.)
    BasicShader::Bind(deviceContext.Get());
    satelliteModel.RenderBuffers(deviceContext.Get());
    textureShader.Bind(deviceContext.Get());
    modelUV.RenderBuffers(deviceContext.Get());

    // ������ �ٲٱ�. FrontBuffer <-> BackBuffer.
    swapChain.Get()->Present(1, 0);
}

bool Engine::InitializeScene()
{
    // ī�޶� ����.
    camera = Camera(
        70.0f * MathUtil::Deg2Rad,
        (float)Window::Width(),
        (float)Window::Height(),
        0.1f,
        10000.0f
    );
    // ī�޶� ��ġ ����.
    camera.SetPosition(0.0f, 0.0f, -200.0f);    

    // ī�޶� ���� ����.
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
