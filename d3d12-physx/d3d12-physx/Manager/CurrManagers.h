#pragma once

#include "Common/d3dUtil.h"

#include "Common/Camera.h"
#include "Manager/GameObjectManager.h"
#include "Manager/MeshRenderInstanceManager.h"
#include "Manager/TextureManager.h"
#include "Manager/MaterialManager.h"
#include "Manager/MeshManager.h"
#include "Manager/InputManager.h"

extern std::shared_ptr<Camera> GetCurrMainCamera();
extern std::shared_ptr<GameObjectManager> GetCurrGameObjectManager();
extern std::shared_ptr<MeshRenderInstanceManager> GetCurrMeshRenderInstanceManager();
extern std::shared_ptr<TextureManager> GetCurrTextureManager();
extern std::shared_ptr<MaterialManager> GetCurrMaterialManager();
extern std::shared_ptr<MeshManager> GetCurrMeshManager();
extern std::shared_ptr<InputManager> GetCurrInputManager();