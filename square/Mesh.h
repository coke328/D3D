#pragma once
#include <vector>
#include <d3d11.h>
#include <string>
#include <directxtk/SimpleMath.h>
#include "../D3D/Helper.h"
#include <assimp/scene.h>


using namespace DirectX;
using namespace DirectX::SimpleMath;

struct ConstantBuffer;

struct Vertex
{
	Vector3 position;		// 정점 위치 정보.
	Vector3 normal;
	Vector3 tangent;
	Vector2 tex;
};

struct Texture {
	aiTextureType aiType;
	std::string path;
	ID3D11ShaderResourceView* texture;

	inline void Release() {
		SAFE_RELEASE(texture);
	}
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> Textures;
	bool diffuse = 0;
	bool normal = 0;
	bool specular = 0;
	bool emissive = 0;
	bool opacity = 0;

	ID3D11Device* dev;
	
	Mesh(ID3D11Device* dev_, const std::vector<Vertex>& vertices_, const std::vector<UINT>& indices_, const std::vector<Texture>& textures_);

	void Draw(ID3D11DeviceContext* devcon, ID3D11Buffer* pCb, ConstantBuffer *cb);

	inline void Close() {
		SAFE_RELEASE(VertexBuffer);
		SAFE_RELEASE(IndexBuffer);
	}
private:
	ID3D11Buffer* VertexBuffer,* IndexBuffer;

	void SetupMesh(ID3D11Device* dev_);

};

