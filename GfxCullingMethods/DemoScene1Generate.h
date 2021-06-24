#pragma once
#include "GraphicsDevice.h"
#include "GraphicsTextureCollection.h"
#include "Scene.h"
#include "randomutils.h"

void DemoScene1Generate(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, Scene& scene, SuperMesh*& mesh, std::vector<SuperMesh*>& subMeshes, SuperMesh*& indoorMesh1);
void DemoScene1GenerateFor(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, std::vector<SuperMeshInstance>& meshInsts, std::vector<SuperMeshInstance>& occluders, SuperMesh*& mesh, std::vector<SuperMesh*>& subMeshes, SuperMesh*& indoorMesh1);