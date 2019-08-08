#pragma once

#include "Manager/GameObject.h"

class Logic :
	public GameObject
{
public:
	Logic(const Transform& transform = Transform(), const std::string & name = "");
	~Logic();

	void PickCube(Int3 int3);

private:
	virtual void Update(const GameTimer& gt)override;

	void Move(const GameTimer& gt);
	
	// Ðý×ª
	void Pitch(float angle);
	void RotateY(float angle);

	// Æ½ÒÆ
	void Strafe(float d);
	void Walk(float d);
	void Fly(float d);

	void CreateCubes();
	void CreateSegments(const Int3& end);
	std::vector<DirectX::XMFLOAT3> CalRandColor();
	void Shuffle();
	bool IsLinkPair(Int3 a, Int3 b);
	bool DeepFirstSearch(Int3 v, Int3 t);
	bool IsTurning(const Int3& curr, const Int3& next);
	void CreateFragment(Int3 int3);

public:
	//

private:
	
	int mMapX;
	int mMapY;
	int mMapZ;
	int mNumOfLeft;
	Int3 mLastPick;
	std::unordered_map<Int3, int, Int3_Hash, Int3_Cmp> mCubeMap;
	std::unordered_map<Int3, std::shared_ptr<GameObject>, Int3_Hash, Int3_Cmp> mCubeMapping;

	std::unordered_map<Int3, bool, Int3_Hash, Int3_Cmp> mMarked;
	std::unordered_map<Int3, Int3, Int3_Hash, Int3_Cmp> mEdgeTo;
	std::unordered_map<Int3, int, Int3_Hash, Int3_Cmp> mTurns;

	POINT mLastMousePos;
};