#pragma once
#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "TextureLoader.h"

class FBXImport
{
public:

	FBXImport();
	~FBXImport();

	bool Load(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filename);
	void Draw(ID3D11DeviceContext* devcon, ID3D11Buffer* pCb, ConstantBuffer&& cb);

	void Close();

private:

	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::vector<Mesh> meshes_;
	std::string directory_;
	std::vector<Texture> textures_loaded_;
	HWND hwnd_;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

};

