#pragma once

#include "GameObject.h"

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Initialize();

	bool HasGameObject(std::string name);
	std::shared_ptr<GameObject> GetGameObject(std::string name);
	std::string NewGameObjectName();
	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void DeleteGameObject(std::string name);

	void Update(const GameTimer& gt);

private:
	//

public:
	//

private:
	std::unordered_map<std::string, std::shared_ptr<GameObject>> mGameObjects;
	std::unordered_set<std::string> mToBeDeleted;
};