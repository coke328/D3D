#include "pch.h"
#include "Transform.h"

using namespace DirectX;

const Matrix& Transform::LocalMatrix()
{
	if (!UpdateMatrix) return localMatrix;

	localMatrix = Matrix::CreateScale(Scale) *
		Matrix::CreateFromQuaternion(Rotation) *
		Matrix::CreateTranslation(Position);

	UpdateMatrix = false;

	return localMatrix;
}

Transform::Transform() : Position(0, 0, 0),
						Rotation(1,0,0,0),
						Scale(1, 1, 1),
						localMatrix(XMMatrixIdentity()),
						UpdateMatrix(true),
						parent(nullptr)
{
	
}

Vector3 Transform::Forward()
{
	//const Matrix& m = WorldMatrix();
	return -WorldMatrix().Forward();//{ -m._13, -m._23, -m._33 }
}

Vector3 Transform::Up()
{
	//const Matrix& m = WorldMatrix();
	return WorldMatrix().Up();
}

Vector3 Transform::Right()
{
	//const Matrix& m = WorldMatrix();
	return WorldMatrix().Right();
}

const Matrix& Transform::WorldMatrix()
{
	worldMatrix = LocalMatrix();
	if (parent != nullptr)
		worldMatrix = worldMatrix * parent->WorldMatrix();
	return worldMatrix;
}

Vector3 Transform::GetPosition()
{
	return Position;
}

Vector3 Transform::GetRotation()
{
	return Rotation.ToEuler();
}

void Transform::SetParent(Transform* p)
{
	parent = p;
}

void Transform::SetPosition(Vector3 pos)
{
	Position = pos;
	UpdateMatrix = true;
}

void Transform::SetGPosition(Vector3 gPos)
{
	Matrix M = Matrix::CreateTranslation(gPos);
	M = WorldMatrix().Invert() * M;
	Position += M.Translation();

	UpdateMatrix = true;
}

void Transform::SetRotation(Vector3 rot)
{
	Rotation = DirectX::XMQuaternionRotationRollPitchYawFromVector(rot);
	UpdateMatrix = true;
}

void Transform::SetRotation(float theta, Vector3 axis)
{
	Rotation = DirectX::XMQuaternionRotationAxis(axis,theta);
	UpdateMatrix = true;
}

void Transform::AddPosition(Vector3 dpos)
{
	Position += dpos;
	UpdateMatrix = true;
}

void Transform::AddRotation(Vector3 drot)
{
	Rotation = XMQuaternionMultiply(XMQuaternionRotationRollPitchYawFromVector(drot), Rotation);
	UpdateMatrix = true;
}

void Transform::AddRotation(float theta, Vector3 axis)
{
	Rotation = XMQuaternionMultiply(XMQuaternionRotationAxis(axis, theta),Rotation);
	UpdateMatrix = true;
}

void Transform::SetScale(Vector3 scale)
{
	Scale = scale;
}
