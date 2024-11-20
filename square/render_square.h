#pragma once
#include "../D3D/D3D_app.h"
#include <d3d11.h>
#include <imgui.h>

#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;

#include "Camera.h"
#include "FBXImport.h"




struct directionLight
{

	Vector3 Direction = { 0,0,-1 };
	float LightPower = 1;
	Vector4 LightDiffuseColor = { 1,1,1,1 };
	Vector4 LightAmbientColor = { 1,1,1,1 };
	float LightSpecularPower = 1;

	void SetDirection(float roll, float pitch);
	void ImGUI();
};

struct Cube {
	Cube();

	static const int nameSize = 8;

	Transform tf;
	float MatAmbientPower = 0.15f;
	Vector4 MatSpecularColor = { 1,1,1,1 };
	float MatSpecularPow = 500;
	
	void Render();
	void ImGUI(const char name[]);
};

class render_square : public D3D_app
{
public:

	render_square(HINSTANCE hInstance);
	~render_square();

	virtual bool Initialize(UINT Width, UINT Height);

private:
	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽� (  �X�� ���ٽ� �䵵 ������ ���� ������� �ʴ´�.)
	ID3D11Device* m_pDevice = nullptr;						// ����̽�	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthBufferView;

	// ������ ���������ο� �����ϴ�  ��ü�� ����
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// ���ؽ� ����.
	ID3D11Buffer* m_pIndexBuffer = nullptr;			// ���ؽ� ����.
	ID3D11VertexShader* m_pVertexShader = nullptr;	// ���� ���̴�.
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�.	
	ID3D11InputLayout* m_pInputLayout = nullptr;	// �Է� ���̾ƿ�.
	ID3D11Buffer* m_pConstantBuffer = nullptr;			// ��� ����.
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;	// �ؽ�ó ���ҽ� ��.
	ID3D11ShaderResourceView* m_pNormalMap = nullptr;
	ID3D11ShaderResourceView* m_pSpecularMap = nullptr;
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// ���÷� ����.

	UINT m_VertextBufferStride = 0;					// ���ؽ� �ϳ��� ũ��.
	UINT m_VertextBufferOffset = 0;					// ���ؽ� ������ ������.
	int m_nIndices = 0;								// �ε��� ����.

	Camera camera;
	directionLight light;
	Matrix mProjection;
	//Cube cube1;
	//Cube cube2;
	
	FBXImport Import;

	Vector4	m_vMeshColor = { 0.7f, 0.7f, 0.7f, 1.0f };

	virtual void Update(float deltaT) override;
	virtual void Render() override;
	void ShowImGUI();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// ���̴�,���ؽ�,�ε���
	void UninitScene();

	bool InitImGUI();
	void UninitImGUI();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//----------------------------------


};

