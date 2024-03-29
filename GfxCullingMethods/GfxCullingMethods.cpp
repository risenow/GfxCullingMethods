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
#include "Portal.h"
#include "BasicVertexShaderStorage.h"
#include "BasicPixelShaderStorage.h"
#include "DepthResolveCopyShaderStorage.h"
#include "GPUDrivenRenderer.h"
#include "GraphicsMarker.h"
#include "DemoScene1Generate.h"
#include "gfxutils.h"
#include "randomutils.h"
#include "basicvsconstants.h"

const bool EnableCullResultsDemoViewport = true;
const int ViewportSizeDivider = EnableCullResultsDemoViewport ? 2 : 1;

Camera CreateInitialCamera(float aspect)
{
    const glm::vec3 position = glm::vec3(0.0f, .0f, 0.0f);
    const glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Camera camera = Camera(position, rotation);
    camera.SetProjection(75.5f, aspect, 0.1f, 100000.0f);//(90.5f, aspect, 0.1f, 100000.0f);

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
    glm::mat4x4 pawnTransform = glm::rotate(glm::scale(glm::identity<glm::mat4x4>(), glm::vec3(100.0f, 100.0f, 100.0f)), cameraRot.y, glm::vec3(0.0, 1.0, 0.0));
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

    //more optimal way is to use push every object of target visibility result
    //to std::set and then just to check if the ref object is in set if not then just return 1
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

    GraphicsDevice device(D3D11DeviceCreationFlags(true, true), FEATURE_LEVEL_ONLY_D3D11, nullptr);

    const MultisampleType multisampleType = MultisampleType::MULTISAMPLE_TYPE_4X;

    static const std::string WindowTitle = "Culling techniques";
    Window window(WindowTitle, 1, 1, 1024, 768);
    GraphicsSwapChain swapchain(device, window, MULTISAMPLE_TYPE_NONE);
    GraphicsTextureCollection textureCollection(device);

    BasicVertexShaderStorage::GetInstance().Load(device);
    BasicPixelShaderStorage::GetInstance().Load(device);
    DepthResolveCopyShaderStorage::GetInstance().Load(device);

    ImmediateRenderer immediateRenderer(device);
    GPUDrivenRenderer renderer(device, false, 1.0f);

    D3D11_RASTERIZER_DESC rastState;
    rastState.AntialiasedLineEnable = FALSE;
    rastState.CullMode = D3D11_CULL_NONE;
    rastState.FillMode = D3D11_FILL_SOLID;
    rastState.FrontCounterClockwise = TRUE;
    rastState.DepthBias = 0;
    rastState.SlopeScaledDepthBias = 0.0f;
    rastState.DepthBiasClamp = 0.0f;
    rastState.DepthClipEnable = FALSE;
    rastState.ScissorEnable = FALSE;
    rastState.MultisampleEnable = multisampleType != MultisampleType::MULTISAMPLE_TYPE_NONE;

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

    ColorSurface backbufferTarget = swapchain.GetBackBufferSurface();

    DXGI_SAMPLE_DESC colorSampleDesc = GetSampleDesc(device, backbufferTarget.GetTexture()->GetFormat(), multisampleType);
    ColorSurface colorTarget(
        device, backbufferTarget.GetWidth() / 2, backbufferTarget.GetHeight(),
        1, 1,
        backbufferTarget.GetTexture()->GetFormat(), colorSampleDesc,
        D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET, 0, 0);
    DepthSurface depthTarget(device, 
        colorTarget.GetWidth(), colorTarget.GetHeight(), 
        1, 1, DXGI_FORMAT_R24G8_TYPELESS, GetSampleDesc(device, DXGI_FORMAT_D24_UNORM_S8_UINT, multisampleType),
        D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 0, 0);

    ColorSurface colorTargetDemo(
        device, backbufferTarget.GetWidth() / 2, backbufferTarget.GetHeight(),
        1, 1,
        colorTarget.GetTexture()->GetFormat(), colorSampleDesc,
        D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET, 0, 0);
    DepthSurface depthTargetDemo(device, backbufferTarget.GetWidth()/2, backbufferTarget.GetHeight(),
        1, 1, DXGI_FORMAT_R24G8_TYPELESS, 
        GetSampleDesc(device, DXGI_FORMAT_D24_UNORM_S8_UINT, multisampleType),
        D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 0, 0);;

    Texture2D tempTexture(device,
        backbufferTarget.GetWidth() / 2, backbufferTarget.GetHeight(), 1, 1,
        backbufferTarget.GetTexture()->GetFormat(), GetSampleDesc(device, backbufferTarget.GetTexture()->GetFormat(), MULTISAMPLE_TYPE_NONE),
        D3D11_USAGE_DEFAULT, 0, 0, 0);

    Camera camera1 = CreateInitialCamera((float)window.GetWidth() / (float)window.GetHeight());
    Camera camera2 = CreateInitialCamera2((float)window.GetWidth() / (float)window.GetHeight());
    camera1.m_UseAngles = true;
    camera2.m_UseAngles = true;

    LinearFrustumVisibility referenceVisibility({});

    Scene scene;

    SuperMesh* roomMesh;
    SuperMesh* mesh;
    std::vector<SuperMesh*> subMeshes;
    std::vector<SuperMeshInstance> meshInsts;
    std::vector<SuperMeshInstance> occluderMeshInsts;
    DemoScene1GenerateFor(device, textureCollection, meshInsts, occluderMeshInsts, mesh, subMeshes, roomMesh);

    renderer.Consume(device, meshInsts, occluderMeshInsts);
   /* SuperMesh* roomMesh;
    SuperMesh* mesh;
    std::vector<SuperMesh*> subMeshes;
    Scene scene;
    DemoScene1Generate(device, textureCollection, scene, mesh, subMeshes, roomMesh);*/

    GraphicsViewport viewport1(BoundRect(Point2D(0, 0), window.GetWidth() / 2, window.GetHeight()));
    //GraphicsViewport viewport1(BoundRect(Point2D(0, 0), window.GetWidth(), window.GetHeight()));
    SuperViewport superViewport1(viewport1, camera1, &scene);

    //!!!! change!!! fixme
    GraphicsViewport viewport2(BoundRect(Point2D(0, 0), window.GetWidth() / 2, window.GetHeight()));
    SuperViewport superViewport2(viewport2, camera2, nullptr);

    SuperMeshInstance* pawnInstance = new SuperMeshInstance(GetPawnPlaceholderMesh(device, textureCollection), CreatePawnTranform(camera1));
    superViewport2.AddMeshDbg(pawnInstance);

    MouseKeyboardCameraController cameraController(camera1);

    while (!window.IsClosed())
    {
        renderer.OnFrameBegin(device);
        glm::vec3 cam1Pos = camera1.GetPosition();
        camera2.SetPosition(glm::vec3(cam1Pos.x, Camera2FixedY, cam1Pos.z));

        pawnInstance->SetTransform(CreatePawnTranform(camera1));

        MARKER_BEGIN(device, L"GPU driven rendering")
        viewport1.Bind(device);
        renderer.Render(device, camera1, colorTarget, depthTarget);
        MARKER_END()
        
        MARKER_BEGIN(device, L"Demo of culling results")
        viewport2.Bind(device);
        renderer.Render(device, camera2, colorTargetDemo, depthTargetDemo, true);
        MARKER_END()

            /*device.GetD3D11DeviceContext()->CopySubresourceRegion(
                colorTarget.GetTexture()->GetD3D11Texture2D(), 0,
                window.GetWidth() / 2, 0,
                0, colorTargetDemo.GetTexture()->GetD3D11Texture2D(), 0, nullptr);*/
        device.GetD3D11DeviceContext()->ResolveSubresource(
            tempTexture.GetD3D11Texture2D(), 0,
            colorTarget.GetTexture()->GetD3D11Texture2D(), 0,
            tempTexture.GetFormat()
        );
        device.GetD3D11DeviceContext()->CopySubresourceRegion(
            backbufferTarget.GetTexture()->GetD3D11Texture2D(), 0,
            0, 0, 0,
            tempTexture.GetD3D11Texture2D(), 0, nullptr
        );

        device.GetD3D11DeviceContext()->ResolveSubresource(
            tempTexture.GetD3D11Texture2D(), 0,
            colorTargetDemo.GetTexture()->GetD3D11Texture2D(), 0,
            tempTexture.GetFormat()
        );
        device.GetD3D11DeviceContext()->CopySubresourceRegion(
            backbufferTarget.GetTexture()->GetD3D11Texture2D(), 0,
            colorTarget.GetWidth(), 0, 0,
            tempTexture.GetD3D11Texture2D(), 0, nullptr
        );
        //viewport1.Bind(device);



        swapchain.Present();
        device.OnPresent();
        cameraController.Update(window);
        window.ExplicitUpdate(); // if you want to get rid of it, just uncomment MT_WINDOW define in Window.h
    }

    for (size_t i = 0; i < mesh->GetSubMeshesCount(); i++)
    {
        mesh->GetSubMesh(i)->ReleaseGPUData();
        delete mesh->GetSubMesh(i);
    }
    delete mesh;
    for (SuperMesh* subMesh : subMeshes)
        delete subMesh;
    //roomMesh->GetSubMesh(0)->ReleaseGPUData();
    //delete roomMesh->GetSubMesh(0);
    //delete roomMesh;

    Mesh::ReleaseShadersGPUData(device);
    device.ReleaseGPUData();
    swapchain.ReleaseGPUData();
    superViewport1.ReleaseGPUData();
    superViewport2.ReleaseGPUData();
    GetPawnPlaceholderMesh(device, textureCollection)->GetSubMesh(0)->ReleaseGPUData();

    
}
