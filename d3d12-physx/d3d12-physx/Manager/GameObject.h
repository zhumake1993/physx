#pragma once

#include "Common/d3dUtil.h"
#include "Common/GameTimer.h"

#include "Manager/MaterialManager.h"
#include "Manager/MeshRender.h"
#include "Manager/RigidDynamic.h"
#include "Manager/RigidStatic.h"

class GameObject
{
	friend class GameObjectManager;
public:
	GameObject(const std::string& name, const Transform& transform);
	virtual ~GameObject();

	virtual void Update(const GameTimer& gt);

	virtual void GetPicked(float dst, DirectX::XMFLOAT3 hitPoint);


	void Release();

protected:
	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

	bool HasGameObject(std::string name);
	std::shared_ptr<GameObject> GetGameObject(std::string name);
	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void DeleteGameObject(std::string name);
	std::shared_ptr<MaterialData> GetMaterial();
	void AddMaterial(const std::string& name, std::shared_ptr<MaterialData> materialData);
	void SwitchScene(std::string name);

public:
	std::string mName;
	Transform mTransform;

	std::unique_ptr<MeshRender> mMeshRender = nullptr;
	std::unique_ptr<RigidDynamic> mRigidDynamic = nullptr;
	std::unique_ptr<RigidStatic> mRigidStatic = nullptr;

private:

	bool mToBeDeleted = false;
};