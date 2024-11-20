#include "Mesh.h"
#include "ConstantBuffer.h"
#include <stdexcept>

Mesh::Mesh(ID3D11Device* dev_, const std::vector<Vertex>& vertices_, const std::vector<UINT>& indices_, 
	const std::vector<Texture>& textures_)
	:	vertices(vertices_), indices(indices_), Textures(textures_), dev(dev_), VertexBuffer(nullptr), IndexBuffer(nullptr)
{
	this->SetupMesh(this->dev);

	for (int i = 0; i < Textures.size(); i++) {
		switch (Textures[i].aiType)
		{
		case aiTextureType_DIFFUSE:
			diffuse = true;
			break;
		case aiTextureType_NORMALS:
			normal = true;
			break;
		case aiTextureType_SPECULAR:
			specular = true;
			break;
		case aiTextureType_EMISSIVE:
			emissive = true;
			break;
		case aiTextureType_OPACITY:
			opacity = true;
			break;
		default:
			break;
		}
	}
}

void Mesh::Draw(ID3D11DeviceContext* devcon, ID3D11Buffer* pCb, ConstantBuffer *cb)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	devcon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	for (int i = 0; i < Textures.size(); i++) {
		devcon->PSSetShaderResources(i, 1, &Textures[i].texture);
	}

	cb->isDiffuse = diffuse;
	cb->isNormal = normal;
	cb->isSpecular = specular;
	cb->isEmissive = emissive;
	cb->isOpacity = opacity;
	
	devcon->UpdateSubresource(pCb, 0, nullptr, cb, 0, 0);

	devcon->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}

void Mesh::SetupMesh(ID3D11Device* dev_)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initDate;
	initDate.pSysMem = &vertices[0];
	
	hr = dev_->CreateBuffer(&vbd, &initDate, &VertexBuffer);
	if (FAILED(hr)) {
		Close();
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initDate.pSysMem = &indices[0];

	hr = dev_->CreateBuffer(&ibd, &initDate, &IndexBuffer);
	if (FAILED(hr)) {
		Close();
		throw std::runtime_error("Failed to create index buffer.");
	}
}
