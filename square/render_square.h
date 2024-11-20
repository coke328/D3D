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
	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스 (  뎊스 스텐실 뷰도 있지만 아직 사용하지 않는다.)
	ID3D11Device* m_pDevice = nullptr;						// 디바이스	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthBufferView;

	// 렌더링 파이프라인에 적용하는  객체와 정보
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// 버텍스 버퍼.
	ID3D11Buffer* m_pIndexBuffer = nullptr;			// 버텍스 버퍼.
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 셰이더.
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 셰이더.	
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃.
	ID3D11Buffer* m_pConstantBuffer = nullptr;			// 상수 버퍼.
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;	// 텍스처 리소스 뷰.
	ID3D11ShaderResourceView* m_pNormalMap = nullptr;
	ID3D11ShaderResourceView* m_pSpecularMap = nullptr;
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태.

	UINT m_VertextBufferStride = 0;					// 버텍스 하나의 크기.
	UINT m_VertextBufferOffset = 0;					// 버텍스 버퍼의 오프셋.
	int m_nIndices = 0;								// 인덱스 개수.

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

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();

	bool InitImGUI();
	void UninitImGUI();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//----------------------------------


};

