#pragma once

#include "Manager/Scene.h"

class MainScene : public Scene
{
public:

	MainScene();
	~MainScene();

	virtual void Initialize()override;

	virtual void Update()override;

private:

	virtual void BuildManagers()override;
	virtual void BuildTextures()override;
	virtual void BuildMaterials()override;
	virtual void BuildMeshes()override;
	virtual void BuildGameObjects()override;

	virtual void MoveCamera()override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

private:
	POINT mLastMousePos;
};