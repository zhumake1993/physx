#pragma once

#include "GameObject.h"

using namespace DirectX;

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Initialize();

	void AddGameObject(std::unique_ptr<GameObject> gameObject);

	std::unique_ptr<GameObject>& GetGameObject(std::string name);

	void Update();

private:
	//

public:
	//

private:
	std::unordered_map<std::string, std::unique_ptr<GameObject>> mGameObjects;
};

extern std::unique_ptr<GameObjectManager> gGameObjectManager;