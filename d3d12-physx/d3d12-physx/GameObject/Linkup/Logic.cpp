#include "LinkupGameObjects.h"

#include <algorithm>

using namespace DirectX;

extern XMVECTOR QuaterionLookAtLH(FXMVECTOR EyePosition, FXMVECTOR FocusPosition, FXMVECTOR UpDirection);

std::vector<XMFLOAT3> rainbowColors = {
	XMFLOAT3(1.00000000f,0.00000000f,0.00000000f),// 赤
	XMFLOAT3(1.00000000f,0.64705882f,0.00000000f),// 橙
	XMFLOAT3(1.00000000f,1.00000000f,0.00000000f),// 黄
	XMFLOAT3(0.00000000f,1.00000000f,0.00000000f),// 绿
	XMFLOAT3(0.00000000f,0.49803921f,1.00000000f),// 青
	XMFLOAT3(0.00000000f,0.00000000f,1.00000000f),// 蓝
	XMFLOAT3(0.54509804f,0.00000000f,1.00000000f),// 紫
};

//auto a = DirectX::Colors::LightSteelBlue;

enum class Face : int
{
	up,
	down,
	left,
	right,
	forward,
	back,
};

XMFLOAT3 Int3ToXMFLOAT3(const Int3& int3) {
	return XMFLOAT3(static_cast<float>(int3.x), static_cast<float>(int3.y), static_cast<float>(int3.z));
}

Logic::Logic(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = true;

	// 至少有一个必须是偶数
	mMapX = 8;
	mMapY = 8;
	mMapZ = 8;
	mNumOfLeft = mMapX * mMapY * mMapZ;
	mLastPick = Int3(0, 0, 0);

	CreateCubes();
}

Logic::~Logic()
{
}

void Logic::PickCube(Int3 int3)
{
	if (Int3_Cmp()(mLastPick, Int3(0, 0, 0))) {

		mLastPick = int3;
		auto cube = std::dynamic_pointer_cast<Cube>(mCubeMapping[int3]);
		cube->TurnOnBlink();
	}
	else if (Int3_Cmp()(mLastPick, int3)) {

		mLastPick = Int3(0, 0, 0);
		auto cube = std::dynamic_pointer_cast<Cube>(mCubeMapping[int3]);
		cube->TurnOffBlink();
	}
	else {
		if (IsLinkPair(mLastPick, int3)) {

			auto cube1 = std::dynamic_pointer_cast<Cube>(mCubeMapping[mLastPick]);
			auto cube2 = std::dynamic_pointer_cast<Cube>(mCubeMapping[int3]);
			DeleteGameObject(cube1->mName);
			DeleteGameObject(cube2->mName);

			CreateSegments(int3);

			CreateFragment(mLastPick);
			CreateFragment(int3);

			mCubeMap[mLastPick] = 0;
			mCubeMap[int3] = 0;

			mCubeMapping.erase(mLastPick);
			mCubeMapping.erase(int3);

			mLastPick = Int3(0, 0, 0);
			mNumOfLeft -= 2;
		}
		else {

			auto cube1 = std::dynamic_pointer_cast<Cube>(mCubeMapping[mLastPick]);
			auto cube2 = std::dynamic_pointer_cast<Cube>(mCubeMapping[int3]);
			cube1->TurnOffBlink();
			cube2->TurnOffBlink();

			mLastPick = Int3(0, 0, 0);
		}
	}
}

void Logic::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	if (GetKeyDown('C')) {
		Shuffle();
	}
}

void Logic::CreateCubes()
{
	// -1表示边界
	for (int x = -1; x <= mMapX + 2; x++) {
		for (int y = -1; y <= mMapY + 2; y++) {
			for (int z = -1; z <= mMapZ + 2; z++) {
				mCubeMap[Int3(x, y, z)] = -1;
			}
		}
	}

	// 0表示通路
	for (int x = 0; x <= mMapX + 1; x++) {
		for (int y = 0; y <= mMapY + 1; y++) {
			for (int z = 0; z <= mMapZ + 1; z++) {
				mCubeMap[Int3(x, y, z)] = 0;
			}
		}
	}

	// 1表示Cube
	for (int x = 1; x <= mMapX; x++) {
		for (int y = 1; y <= mMapY; y++) {
			for (int z = 1; z <= mMapZ; z++) {

				mCubeMap[Int3(x, y, z)] = 1;

				std::string n = "Cube_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
				Transform t = Transform(XMFLOAT3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)),
					XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.95f, 0.95f, 0.95f));
				auto cube = std::make_shared<Cube>(t, n);
				cube->mMapIndex = Int3(x, y, z);
				AddGameObject(cube);

				mCubeMapping[Int3(x, y, z)] = cube;
			}
		}
	}

	Shuffle();
}

void Logic::CreateSegments(const Int3& end)
{
	std::vector<DirectX::XMFLOAT3> linePoints;
	Int3 last = end;

	// 线段模拟
	linePoints.push_back(Int3ToXMFLOAT3(last));
	while (mEdgeTo.find(last) != mEdgeTo.end()) {
		last = mEdgeTo[last];
		linePoints.push_back(Int3ToXMFLOAT3(last));
	}
	float radius = 0.1f;

	// 绘制拐点
	for (int i = 1; i < linePoints.size() - 1; i++) {
		Transform a;
		a.Translation = linePoints[i];
		a.Scale = XMFLOAT3(radius, radius, radius);

		auto inflection = std::make_shared<Inflection>(a);
		AddGameObject(inflection);
	}

	// 绘制圆柱
	for (int i = 0; i < linePoints.size() - 1; i++) {
		XMVECTOR start = XMLoadFloat3(&linePoints[i]);
		XMVECTOR end = XMLoadFloat3(&linePoints[i + 1]);

		Transform a;
		XMStoreFloat3(&a.Translation, (start + end) / 2);
		auto q1 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PIDIV2);
		auto q2 = QuaterionLookAtLH(start, end, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		XMStoreFloat4(&a.Quaternion, XMQuaternionMultiply(q1, q2));
		a.Scale = XMFLOAT3(radius, XMVectorGetX(XMVector3Length(end - start)), radius);

		auto s1 = std::make_shared<Segment>(a);
		AddGameObject(s1);
	}
}

std::vector<XMFLOAT3> Logic::CalRandColor()
{
	std::vector<XMFLOAT3> result;

	int pair = mNumOfLeft / 2;

	for (int i = 0; i < pair; i++) {
		auto color = MathHelper::Rand(0, static_cast<int>(rainbowColors.size()) - 1);
		result.push_back(rainbowColors[color]);
		result.push_back(rainbowColors[color]);
	}

	std::random_shuffle(result.begin(), result.end());

	return result;
}

void Logic::Shuffle()
{
	auto randColors = CalRandColor();
	int index = 0;

	for (int x = 1; x <= mMapX; x++) {
		for (int y = 1; y <= mMapY; y++) {
			for (int z = 1; z <= mMapZ; z++) {

				// 已经被消除的方块
				if (mCubeMap[Int3(x, y, z)] == 0) {
					continue;
				}

				auto cube = std::dynamic_pointer_cast<Cube>(mCubeMapping[Int3(x, y, z)]);
				cube->SetColor(randColors[index++]);
			}
		}
	}
}

bool Logic::IsLinkPair(Int3 a, Int3 b)
{
	mMarked.clear();
	mEdgeTo.clear();
	mTurns.clear();

	for (int x = 0; x <= mMapX + 1; x++) {
		for (int y = 0; y <= mMapY + 1; y++) {
			for (int z = 0; z <= mMapZ + 1; z++) {
				mMarked[Int3(x, y, z)] = false;
			}
		}
	}

	mTurns[a] = 0;

	if (DeepFirstSearch(a, b)) {
		auto cube1 = std::dynamic_pointer_cast<Cube>(mCubeMapping[a]);
		auto cube2 = std::dynamic_pointer_cast<Cube>(mCubeMapping[b]);
		auto color1 = cube1->GetColor();
		auto color2 = cube2->GetColor();
		return color1.x == color2.x && color1.y == color2.y && color1.z == color2.z;
	}

	return false;
}

bool Logic::DeepFirstSearch(Int3 v, Int3 t)
{
	if (Int3_Cmp()(v, t)) {
		return true;
	}

	mMarked[v] = true;

	std::unordered_map<int, Int3> faces = {
		{(int)Face::up,Int3(v.x,v.y + 1,v.z)},
		{(int)Face::down,Int3(v.x,v.y - 1,v.z)},
		{(int)Face::left,Int3(v.x - 1,v.y,v.z)},
		{(int)Face::right,Int3(v.x + 1,v.y,v.z)},
		{(int)Face::forward,Int3(v.x,v.y,v.z + 1)},
		{(int)Face::back,Int3(v.x,v.y,v.z - 1)}
	};

	for (auto p : faces) {
		if (mCubeMap[p.second] == -1) {
			continue;
		}

		if (mCubeMap[p.second] == 1) {
			if (!mMarked[p.second]) {

				if (IsTurning(v, p.second)) {
					if (mTurns[v] == 2) {
						continue;
					}
					else {
						if (Int3_Cmp()(p.second, t)) {
							mEdgeTo[p.second] = v;
							return true;
						}
					}
				}
				else {
					if (Int3_Cmp()(p.second, t)) {
						mEdgeTo[p.second] = v;
						return true;
					}
				}
			}
		}

		if (mCubeMap[p.second] == 0) {
			if (!mMarked[p.second]) {

				if (IsTurning(v, p.second)) {
					if (mTurns[v] == 2) {
						continue;
					}
					else {
						mEdgeTo[p.second] = v;
						mTurns[p.second] = mTurns[v] + 1;

						if (DeepFirstSearch(p.second, t)) {
							return true;
						}
					}
				}
				else {
					mEdgeTo[p.second] = v;
					mTurns[p.second] = mTurns[v];
					
					if (DeepFirstSearch(p.second, t)) {
						return true;
					}
				}
			}
			else {

				if (IsTurning(v, p.second)) {

					if (mTurns[p.second] >= mTurns[v] + 1) {
						mEdgeTo[p.second] = v;
						mTurns[p.second] = mTurns[v] + 1;
						
						if (DeepFirstSearch(p.second, t)) {
							return true;
						}
					}
				}
				else {

					if (mTurns[p.second] >= mTurns[v]) {
						mEdgeTo[p.second] = v;
						mTurns[p.second] = mTurns[v];
						
						if (DeepFirstSearch(p.second, t)) {
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool Logic::IsTurning(const Int3& curr, const Int3& next)
{
	if (mEdgeTo.find(curr) == mEdgeTo.end()) {
		return false;
	}

	auto pre = mEdgeTo[curr];

	Int3 a = Int3(curr.x - pre.x, curr.y - pre.y, curr.z - pre.z);
	Int3 b = Int3(next.x - curr.x, next.y - curr.y, next.z - curr.z);

	return !Int3_Cmp()(a, b);
}

void Logic::CreateFragment(Int3 int3)
{
	for (float x = int3.x - 0.375f; x <= int3.x + 0.375f; x += 0.25f) {
		for (float y = int3.y - 0.375f; y <= int3.y + 0.375f; y += 0.25f) {
			for (float z = int3.z - 0.375f; z <= int3.z + 0.375f; z += 0.25f) {

				if (MathHelper::RandF() > 0.2f)continue;

				Transform t = Transform(XMFLOAT3(x, y, z), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.2f, 0.2f, 0.2f));
				auto fragment = std::make_shared<Fragment>(t);

				fragment->SetColor(rainbowColors[MathHelper::Rand(0, static_cast<int>(rainbowColors.size()) - 1)]);

				fragment->mRigidDynamicCPT->SetAngularDamping(0.5f);

				auto v = XMFLOAT3(MathHelper::RandF(-10.0f, 10.0f), MathHelper::RandF(-10.0f, 10.0f), MathHelper::RandF(-10.0f, 10.0f));
				fragment->mRigidDynamicCPT->SetLinearVelocity(v);

				AddGameObject(fragment);
			}
		}
	}
}