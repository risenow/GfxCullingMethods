#include <iostream>
#include <random>
#include "Mesh.h"
#include "SuperMeshInstance.h"
#include "GraphicsDevice.h"
#include "Window.h"
#include "GraphicsSwapchain.h"
#include "GraphicsShader.h"
#include "dxlogicsafety.h"
#include "GraphicsBuffer.h"
#include "GraphicsConstantsBuffer.h"
#include "GraphicsInputLayout.h"
#include "GraphicsViewport.h"
#include "GraphicsTextureCollection.h"
#include "MouseKeyboardCameraController.h"
#include "Camera.h"
#include "SuperViewport.h"
#include "DisplayAdaptersList.h"
#include "ImmediateRenderer.h"
#include "MicrosecondsTimer.h"
#include "PagedObjPool.h"
#include "LightList.h"
#include "MicrosecondsTimer.h"
#include "basicvsconstants.h"

float Random01()
{
    static const size_t res = 10000;
    return float(rand()) / float(RAND_MAX);
}
float RandomFromTo(float from, float to)
{
    float t1 = Random01();
    float t2 = (to - from);
    float t3 = t1 * t2;
    float t4 = t3 + from;

    return Random01() * (to - from) + from;
}
glm::vec3 RandomFromTo3(float from, float to)
{
    return glm::vec3(RandomFromTo(from, to), RandomFromTo(from, to), RandomFromTo(from, to));
}

Camera CreateInitialCamera(float aspect)
{
    const glm::vec3 position = glm::vec3(0.0f, .0f, 0.0f);
    const glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Camera camera = Camera(position, rotation);
    camera.SetProjection(45.0f, aspect, 0.1f, 100000.0f);

    return camera;
}

static const float Camera2FixedY = 4000.0f;

Camera CreateInitialCamera2(float aspect)
{
    const glm::vec3 position = glm::vec3(0.0f, Camera2FixedY, -550.0f);
    const glm::vec3 rotation = glm::vec3(-3.14f/2.0f, 0.0f, 0.0f);
    Camera camera = Camera(position, rotation);
    camera.SetProjection(45.0f, aspect, 0.1f, 100000.0f);

    return camera;
}

glm::mat4x4 CreatePawnTranform(const Camera& camera)
{
    glm::vec3 cameraRot = camera.GetRotation();
    glm::mat4x4 pawnTransform = glm::rotate(glm::scale(glm::mat4x4(), glm::vec3(100.0f, 100.0f, 100.0f)), cameraRot.y, glm::vec3(0.0, 1.0, 0.0));
    pawnTransform[3] = glm::vec4(camera.GetPosition(), 1.0f);

    return pawnTransform;
}

int TestVisibilityAgainstReference(GraphicsDevice& device, Camera& cam, Visibility* vis, Visibility* refVis)
{
    MicrosecondsTimer timer;

    int res = 0;

    std::vector<SuperMeshInstance*> visMeshes;
    std::vector<SuperMeshInstance*> refMeshes;

    timer.Begin();
    refVis->Gather(device, cam, refMeshes);
    __int64 time1 = timer.End();
    
    timer.Begin();
    vis->Gather(device, cam, visMeshes);
    __int64 time2 = timer.End();

    double ratio = (double)time1 / (double)time2;
    std::cout << ratio << std::endl;

    for (SuperMeshInstance* refMesh : refMeshes)
    {
        res++;
        for (SuperMeshInstance* mesh : visMeshes)
        {
            if (mesh == refMesh)
            {
                res--;
                break;
            }
        }
    }

    return res;
}

int main()
{
    srand(time(NULL));

    DisplayAdaptersList displayAdapters;

    GraphicsDevice device(D3D11DeviceCreationFlags(false, true), FEATURE_LEVEL_ONLY_D3D11, nullptr);

    static const std::string WindowTitle = "Culling techniques";
    Window window(WindowTitle, 1, 1, 1024, 712);
    GraphicsSwapChain swapchain(device, window);
    GraphicsTextureCollection textureCollection(device);
    ImmediateRenderer immediateRenderer(device);

    D3D11_RASTERIZER_DESC rastState;
    rastState.AntialiasedLineEnable = false;
    rastState.CullMode = D3D11_CULL_NONE;
    rastState.FillMode = D3D11_FILL_SOLID;
    rastState.FrontCounterClockwise = TRUE;
    rastState.DepthBias = 0;
    rastState.SlopeScaledDepthBias = 0.0f;
    rastState.DepthBiasClamp = 0.0f;
    rastState.DepthClipEnable = FALSE;
    rastState.ScissorEnable = FALSE;
    rastState.MultisampleEnable = FALSE;

    ID3D11RasterizerState* d3dRastState;

    D3D_HR_OP(device.GetD3D11Device()->CreateRasterizerState(&rastState, &d3dRastState));
    device.GetD3D11DeviceContext()->RSSetState(d3dRastState);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* noDepthStencilState;
    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilState(&depthStencilDesc, &noDepthStencilState));
    device.GetD3D11DeviceContext()->OMSetDepthStencilState(noDepthStencilState, 0);

    ColorSurface colorTarget = swapchain.GetBackBufferSurface();
    DepthSurface depthTarget(device, colorTarget.GetWidth(), colorTarget.GetHeight(), 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT, GetSampleDesc(device, DXGI_FORMAT_D24_UNORM_S8_UINT, MULTISAMPLE_TYPE_NONE), D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL, 0, 0);

    Camera camera1 = CreateInitialCamera((float)window.GetWidth() / (float)window.GetHeight());

    Camera camera2 = CreateInitialCamera2((float)window.GetWidth() / (float)window.GetHeight());
    camera1.m_UseAngles = true;
    camera2.m_UseAngles = true;

    SuperMesh* mesh = SuperMesh::FromFile(device, textureCollection, "SchoolGirlOBJ/D0208059.obj");
    std::vector<SuperMesh*> subMeshes;
    subMeshes.resize(mesh->GetSubMeshesCount());
    for (size_t i = 0; i < mesh->GetSubMeshesCount(); i++)
        subMeshes[i] = new SuperMesh({ mesh->GetSubMesh(i) });

    LinearFrustumVisibility referenceVisibility({});

    GraphicsViewport viewport1(BoundRect(Point2D(0, 0), window.GetWidth() / 2, window.GetHeight()));
    SuperViewport superViewport1(viewport1, camera1, ViewportVisibility_BVHFrustum);

    AABB sceneAABB;
    std::vector<SuperMeshInstance*> tempMeshes;

    for (size_t i = 0; i < 1600; i++) 
    {   
        glm::vec3 pos = RandomFromTo3(-2000.0f, 2000.0f);
        glm::mat4x4 transform = glm::translate(glm::mat4x4(), pos);

        for (size_t j = 0; j < subMeshes.size(); j++)
        {
            SuperMeshInstance* inst = new SuperMeshInstance(subMeshes[j], transform);

            sceneAABB.Extend(inst->GetAABB());

            tempMeshes.push_back(inst);
            referenceVisibility.AddMesh(inst);
        }
    }
    superViewport1.SetVisHelpInfo(sceneAABB);

    MicrosecondsTimer estTimer;
    estTimer.Begin();
    for (SuperMeshInstance* mesh : tempMeshes)
        superViewport1.AddMesh(mesh);
    superViewport1.AddMesh(new SuperMeshInstance(mesh, glm::translate(glm::scale(glm::mat4x4(), RandomFromTo3(0.2f, 1.1f)), RandomFromTo3(-1000.0f, 1000.0f))), true, true);
    std::cout << estTimer.End() << std::endl;

    GraphicsViewport viewport2(BoundRect(Point2D(window.GetWidth() / 2, 0), window.GetWidth() / 2, window.GetHeight()));
    SuperViewport superViewport2(viewport2, camera2, ViewportVisibility_None);

    SuperMeshInstance* pawnInstance = new SuperMeshInstance(GetPawnPlaceholderMesh(device, textureCollection), CreatePawnTranform(camera1));
    superViewport2.AddMesh(pawnInstance);

    superViewport1.ShareVisibility({ &superViewport2 });

    MouseKeyboardCameraController cameraController(camera1);

    RenderStatistics stats;

    short visTypeIndex = (short)ViewportVisibility_LinearFrustum;
    long long visAlterLastFrame = 0;

    while (!window.IsClosed())
    {
        immediateRenderer.OnFrameBegin(device);

        glm::vec3 cam1Pos = camera1.GetPosition();
        camera2.SetPosition(glm::vec3(cam1Pos.x, Camera2FixedY, cam1Pos.z));

        pawnInstance->SetTransform(CreatePawnTranform(camera1));

        stats = superViewport1.Render(device, colorTarget, depthTarget);
        immediateRenderer.OnFrameEnd(device, camera1, colorTarget, depthTarget);

        superViewport2.Render(device, colorTarget, depthTarget, false);

        window.SetTitle(WindowTitle + " " + std::to_string(stats.primCount) + " " + std::to_string(stats.drawCallsCount));

        //do sm kinda shit here
        swapchain.Present();
        device.OnPresent();
        cameraController.Update(window);
        window.ExplicitUpdate(); // if you want to get rid of it, just uncomment MT_WINDOW define in Window.h
    }

    mesh->ReleaseGPUData();

    Mesh::ReleaseShadersGPUData(device);
    device.ReleaseGPUData();
    swapchain.ReleaseGPUData();
    superViewport1.ReleaseGPUData();
    superViewport2.ReleaseGPUData();
    GetPawnPlaceholderMesh(device, textureCollection)->ReleaseGPUData();
}
