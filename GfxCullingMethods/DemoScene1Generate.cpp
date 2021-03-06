#include "DemoScene1Generate.h"
#include "Log.h"
#include "MicrosecondsTimer.h"

void DemoScene1Generate(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, Scene& scene, SuperMesh*& mesh, std::vector<SuperMesh*>& subMeshes, SuperMesh*& indoorMesh1)
{
    /*std::vector<PortalSystem::Room*> rooms;

    LoPoApproxGeom portalGeom;
    LoPoApproxGeom::GenXYAlignedQuad(portalGeom, glm::vec3(.0f, 200.0f, 20.0f), 100.0f, 100.0f);

    PortalSystem::Room* room1 = new PortalSystem::Room();
    PortalSystem::Room* room2 = new PortalSystem::Room();

    PortalSystem::Portal* portal = new PortalSystem::Portal(portalGeom, room1, room2);
    room1->AddPortal(portal, glm::vec3(0.0, 0.0, 1.0));
    room2->AddPortal(portal, glm::vec3(0.0, 0.0, -1.0));

    AABB portalAABB = portal->GetAABB();
    glm::vec3 portalLeftTop = portalAABB.GetPoint(AABB::LeftTopFront);
    glm::vec3 portalRightTop = portalAABB.GetPoint(AABB::RightTopFront);
    glm::vec3 portalLeftBottom = portalAABB.GetPoint(AABB::LeftBottomFront);
    glm::vec3 portalRightBottom = portalAABB.GetPoint(AABB::RightBottomFront);

    AABB room1Box = AABB(glm::vec3(-750.0f, -800.0f, -750.0f), glm::vec3(200.0f, 800.0f, 20.0f));
    Mesh* room1Mesh = new Mesh();

    std::vector<VertexPropertyPrototype> vertexPropertyProtos = { VertexFormatHelper::PurePosPropProto(0, 0) };
    vertexPropertyProtos.push_back(VertexFormatHelper::PureNormalPropProto(0, 0));
    vertexPropertyProtos.push_back(VertexFormatHelper::PureTexCoordPropProto(0, 0));

    VertexFormat vertexFormat(vertexPropertyProtos);
    VertexData vertexData(vertexFormat, 6 * 8);
    VertexProperty posDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PurePos);
    VertexProperty tcDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureTexCoord);
    VertexProperty normsDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureNormal);

    glm::vec3 dummyNormal = glm::vec3();
    glm::vec2 dummyTc = glm::vec2();

    //floor trs
    int vx = 0;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //left wall trs
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftTopBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //right wall trs
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //back wall trs
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftTopBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomBack), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //windowed wall trs, bottom part
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    glm::vec3 rightTopFront = room1Box.GetPoint(AABB::RightTopFront);
    rightTopFront.y = portalLeftBottom.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    glm::vec3 leftTopFront = room1Box.GetPoint(AABB::LeftTopFront);
    leftTopFront.y = portalLeftBottom.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftBottomFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //windowed wall trs, top part
    glm::vec3 leftBottomFront = room1Box.GetPoint(AABB::LeftBottomFront);
    leftBottomFront.y = portalLeftTop.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    glm::vec3 rightBottomFront = room1Box.GetPoint(AABB::RightBottomFront);
    rightBottomFront.y = portalLeftTop.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::RightTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(room1Box.GetPoint(AABB::LeftTopFront), 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //windowed wall trs, left part
    leftBottomFront = room1Box.GetPoint(AABB::LeftBottomFront);
    leftBottomFront.y = portalLeftBottom.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    rightBottomFront = portalLeftBottom;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    rightTopFront = portalLeftTop;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    leftTopFront = room1Box.GetPoint(AABB::LeftTopFront);
    leftTopFront.y = portalLeftTop.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    leftBottomFront = room1Box.GetPoint(AABB::LeftBottomFront);
    leftBottomFront.y = portalLeftBottom.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    //windowed wall trs, right part
    leftBottomFront = portalRightBottom;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    rightBottomFront = room1Box.GetPoint(AABB::RightBottomFront);
    rightBottomFront.y = portalLeftBottom.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    rightTopFront = room1Box.GetPoint(AABB::RightTopFront);
    rightTopFront.y = portalRightTop.y;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    leftTopFront = portalRightTop;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftTopFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    leftBottomFront = portalRightBottom;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottomFront, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    Mesh::FromVertexData(device, textureCollection, vertexData, Mesh::GetVertexShader(device), Mesh::GetPixelShader(device), nullptr, *room1Mesh);
    SuperMesh* room1SuperMesh = new SuperMesh({ room1Mesh });
    SuperMeshInstance* roomMesh = new SuperMeshInstance(new SuperMesh({ room1Mesh }), glm::identity<glm::mat4x4>());

    mesh = SuperMesh::FromFile(device, textureCollection, "Data/SchoolGirlOBJ/D0208059.obj");
    subMeshes.resize(mesh->GetSubMeshesCount());
    for (size_t i = 0; i < mesh->GetSubMeshesCount(); i++)
        subMeshes[i] = new SuperMesh({ mesh->GetSubMesh(i) });

    for (size_t i = 0; i < 50; i++)
    {
        glm::vec3 pos = RandomFromTo3(-650.0f, -50.0f);
        glm::mat4x4 transform = glm::translate(glm::identity<glm::mat4x4>(), pos);

        for (size_t j = 0; j < subMeshes.size(); j++)
        {
            SuperMeshInstance* inst = new SuperMeshInstance(subMeshes[j], transform);

            room1->AddMesh(inst);
        }
    }
    room1->AddMesh(roomMesh, false, true);

    for (size_t i = 0; i < 50; i++)
    {
        glm::vec3 pos = RandomFromTo3(40.0f, 1000.0f);
        glm::mat4x4 transform = glm::translate(glm::identity<glm::mat4x4>(), pos);

        for (size_t j = 0; j < subMeshes.size(); j++)
        {
            SuperMeshInstance* inst = new SuperMeshInstance(subMeshes[j], transform);

            room2->AddMesh(inst);
        }
    }
    SuperMeshInstance* dummyInst = new SuperMeshInstance(subMeshes[0], glm::identity<glm::mat4x4>());
    room2->AddMesh(dummyInst, true, true);

    indoorMesh1 = room1SuperMesh;

    scene.ConsumeRooms({ room1, room2 });*/

        FileRuntimeLogStream fileStream;
        fileStream.open("dat.csv", std::wofstream::trunc | std::wofstream::out);

        std::vector<PortalSystem::Room*> rooms;

        LoPoApproxGeom portalGeom;
        LoPoApproxGeom::GenXYAlignedQuad(portalGeom, glm::vec3(.0f, 200.0f, 20.0f), 100.0f, 100000000000000000.0f);

        PortalSystem::Room* room1 = new PortalSystem::Room();
        PortalSystem::Room* room2 = nullptr;

        room1->SetAABB(AABB(glm::vec3(-650.0f, -650.0F, -650.0F), glm::vec3(650.0f, 650.0F, 650.0F)));

        PortalSystem::Portal* portal = new PortalSystem::Portal(portalGeom, room1, room2);
        room1->AddPortal(portal, glm::vec3(0.0, 0.0, 1.0));

        mesh = SuperMesh::FromFile(device, textureCollection, "Data/SchoolGirlOBJ/D0208059.obj");
        subMeshes.resize(mesh->GetSubMeshesCount());
        for (size_t i = 0; i < mesh->GetSubMeshesCount(); i++)
            subMeshes[i] = new SuperMesh({ mesh->GetSubMesh(i) });

        //for (int n = 1; n < 50; n++)
        int n = 48;
        {
            MicrosecondsTimer timer;
            timer.Begin();
            int objsM = 20;
            for (size_t i = 0; i < n* objsM; i++) //x38
            {
                glm::vec3 pos = RandomFromTo3(-650.0f, 650.0f);
                glm::mat4x4 transform = glm::translate(glm::identity<glm::mat4x4>(), pos);

                for (size_t j = 0; j < subMeshes.size(); j++)
                {
                    SuperMeshInstance* inst = new SuperMeshInstance(subMeshes[j], transform);

                    room1->AddMesh(inst);
                }
            }

            __int64 score = timer.End();
            timer.Reset();
            fileStream.Write(std::to_wstring(38 * n * objsM) + L", " + std::to_wstring(score) + L"\n");
            std::cout << score << std::endl;


            SuperMeshInstance* dummyInst = new SuperMeshInstance(subMeshes[0], glm::identity<glm::mat4x4>());
            room1->AddMesh(dummyInst, true, true);
            
            //room1->Clear();
        }
        //std::cout << score << std::endl;

        indoorMesh1 = nullptr;

        scene.ConsumeRooms({ room1 });
}

void DemoScene1GenerateFor(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, std::vector<SuperMeshInstance>& meshInsts, std::vector<SuperMeshInstance>& occluders, SuperMesh*& mesh, std::vector<SuperMesh*>& subMeshes, SuperMesh*& occMesh)
{
    mesh = SuperMesh::FromFile(device, textureCollection, "Data/SchoolGirlOBJ/D0208059.obj");
    subMeshes.resize(mesh->GetSubMeshesCount());
    for (size_t i = 0; i < mesh->GetSubMeshesCount(); i++)
        subMeshes[i] = new SuperMesh({ mesh->GetSubMesh(i) });

    //for (int n = 1; n < 50; n++)
    int n = 4;
    {

        int objsM = 40;
        for (size_t i = 0; i < n * objsM; i++) //x38
        {
            glm::vec3 pos = RandomFromTo3(-650.0f, 650.0f);
            glm::mat4x4 transform = glm::translate(glm::identity<glm::mat4x4>(), pos);

            for (size_t j = 0; j < subMeshes.size(); j++)
            {
                SuperMeshInstance inst = SuperMeshInstance(subMeshes[j], transform);

                meshInsts.push_back(inst);
            }
        }
    }

    std::vector<VertexPropertyPrototype> vertexPropertyProtos = { VertexFormatHelper::PurePosPropProto(0, 0) };
    vertexPropertyProtos.push_back(VertexFormatHelper::PureNormalPropProto(0, 0));
    vertexPropertyProtos.push_back(VertexFormatHelper::PureTexCoordPropProto(0, 0));

    VertexFormat vertexFormat(vertexPropertyProtos);
    VertexData vertexData(vertexFormat, 6 * 8);
    VertexProperty posDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PurePos);
    VertexProperty tcDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureTexCoord);
    VertexProperty normsDesc = vertexData.GetVertexPropertyByName(VertexDataSemantics_PureNormal);

    glm::vec3 dummyNormal = glm::vec3();
    glm::vec2 dummyTc = glm::vec2();

    int vx = 0;
    float occWidth = 100.0f;
    float occHeight = 100.0f;
    float z = -650.0f;
    glm::vec3 leftBottom = glm::vec3(-1.0f * occWidth, -1.0f * occHeight, z);
    glm::vec3 rightBottom = glm::vec3(1.0f * occWidth, -1.0f * occHeight, z);
    glm::vec3 leftTop = glm::vec3(-1.0f * occWidth, 1.0f * occHeight, z);
    glm::vec3 rightTop = glm::vec3(1.0f * occWidth, 1.0f * occHeight, z);

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottom, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightBottom, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTop, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;

    PullVecToVertexData<glm::vec4>(vx, glm::vec4(rightTop, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftTop, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    PullVecToVertexData<glm::vec4>(vx, glm::vec4(leftBottom, 1.0f), vertexData, posDesc);
    PullVecToVertexData<glm::vec3>(vx, dummyNormal, vertexData, normsDesc);
    PullVecToVertexData<glm::vec2>(vx, dummyTc, vertexData, tcDesc); vx++;
    
    Mesh* occMeshTemp = new Mesh();
    Mesh::FromVertexData(device, textureCollection, vertexData, Mesh::GetVertexShader(device), Mesh::GetPixelShader(device), nullptr, *occMeshTemp);

    occMesh = new SuperMesh({ occMeshTemp });
    SuperMeshInstance occMeshInst = SuperMeshInstance(occMesh, glm::identity<glm::mat4x4>());

    occluders.push_back(occMeshInst);
}