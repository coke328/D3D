#include "FBXImport.h"
#include "ConstantBuffer.h"

FBXImport::FBXImport() :
	dev_(nullptr),
	devcon_(nullptr),
	meshes_(),
	directory_(),
	textures_loaded_(),
	hwnd_(nullptr) 
{
}

FBXImport::~FBXImport()
{
}

bool FBXImport::Load(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filename)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;
	this->hwnd_ = hwnd;

	processNode(pScene->mRootNode, pScene);

	return true;
}

void FBXImport::Draw(ID3D11DeviceContext* devcon, ID3D11Buffer* pCb, ConstantBuffer&& cb_)
{
	ConstantBuffer cb = std::move(cb_);

	for (size_t i = 0; i < meshes_.size(); ++i) {
		meshes_[i].Draw(devcon, pCb, &cb);
	}
}

void FBXImport::Close()
{
	for (auto& t : textures_loaded_)
		t.Release();

	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i].Close();
	}
}

void FBXImport::processNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(this->processMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh FBXImport::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.tex.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.tex.y = (float)mesh->mTextureCoords[0][i].y;
		}
	
		if (mesh->HasNormals()) {
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}
		else {
			vertex.normal = { 0,0,0 };
		}

		if (mesh->HasTangentsAndBitangents()) {
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;
		}
		else {
			vertex.tangent = {0,0,0};
		}
		

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuse = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, scene);
		std::vector<Texture> normal = this->loadMaterialTextures(material, aiTextureType_NORMALS, scene);
		std::vector<Texture> specular = this->loadMaterialTextures(material, aiTextureType_SPECULAR, scene);
		std::vector<Texture> emissive = this->loadMaterialTextures(material, aiTextureType_EMISSIVE, scene);
		std::vector<Texture> opacity = this->loadMaterialTextures(material, aiTextureType_OPACITY, scene);
		
		textures.insert(textures.end(), diffuse.begin(), diffuse.end());
		textures.insert(textures.end(), normal.begin(), normal.end());
		textures.insert(textures.end(), specular.begin(), specular.end());
		textures.insert(textures.end(), emissive.begin(), emissive.end());
		textures.insert(textures.end(), opacity.begin(), opacity.end());
	}

	return Mesh(dev_, vertices, indices, textures);
}

std::vector<Texture> FBXImport::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene)
{
	std::vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < textures_loaded_.size(); j++) {
			if (std::strcmp(textures_loaded_[j].path.c_str(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded_[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // If texture hasn't been loaded already, load it
			HRESULT hr;
			Texture texture;

			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr) {
				texture.texture = loadEmbeddedTexture(embeddedTexture);
			}
			else {
				std::string filename = std::string(str.C_Str());
				filename = directory_ + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.texture);
				if (FAILED(hr))
					MessageBox(hwnd_, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);
			}
			texture.aiType = type;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->textures_loaded_.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

ID3D11ShaderResourceView* FBXImport::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	HRESULT hr;
	ID3D11ShaderResourceView* texture = nullptr;

	if (embeddedTexture->mHeight != 0) {
		// Load an uncompressed ARGB8888 embedded texture
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;
		desc.Height = embeddedTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

		ID3D11Texture2D* texture2D = nullptr;
		hr = dev_->CreateTexture2D(&desc, &subresourceData, &texture2D);
		if (FAILED(hr))
			MessageBox(hwnd_, L"CreateTexture2D failed!", L"Error!", MB_ICONERROR | MB_OK);

		hr = dev_->CreateShaderResourceView(texture2D, nullptr, &texture);
		if (FAILED(hr))
			MessageBox(hwnd_, L"CreateShaderResourceView failed!", L"Error!", MB_ICONERROR | MB_OK);

		return texture;
	}

	// mHeight is 0, so try to load a compressed texture of mWidth bytes
	const size_t size = embeddedTexture->mWidth;

	hr = CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture);
	if (FAILED(hr))
		MessageBox(hwnd_, L"Texture couldn't be created from memory!", L"Error!", MB_ICONERROR | MB_OK);

	return texture;
}
