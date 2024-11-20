#pragma once

class Transform
{
	Transform* parent;

	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scale;
	
	Matrix localMatrix;
	Matrix worldMatrix;
	bool UpdateMatrix;

	const Matrix& LocalMatrix();
public:
	Transform();

	Vector3 Forward();
	Vector3 Up();
	Vector3 Right();
	const Matrix& WorldMatrix();
	Vector3 GetPosition();
	Vector3 GetRotation();
	void SetParent(Transform* p);
	void SetPosition(Vector3 pos);
	void SetGPosition(Vector3 gPos);
	void SetRotation(Vector3 rot);
	void SetRotation(float theta, Vector3 axis);
	void AddPosition(Vector3 dpos);
	void AddRotation(Vector3 drot);
	void AddRotation(float theta, Vector3 axis);
	void SetScale(Vector3 scale);
};

