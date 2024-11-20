#include "render_square.h"
#include "../D3D/Helper.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "Mesh.h"
#include "ConstantBuffer.h"

#pragma comment (lib, "d3d11.lib")

#define PI float(3.141592)

namespace global {
	ID3D11DeviceContext* pDevice;
	ID3D11Buffer* pConstantBufffer;
	int idxes;
	Camera* pCam;
	Matrix* pProjectionMatrix;
	directionLight* pLight;

	void Init(ID3D11DeviceContext* pDevice, ID3D11Buffer* pConstantBufffer, int idxes_, Camera* cam, Matrix* pProjectionMatrix, directionLight* light) {
		global::pDevice = pDevice;
		global::pConstantBufffer = pConstantBufffer;
		global::idxes = idxes_;
		global::pCam = cam;
		global::pProjectionMatrix = pProjectionMatrix;
		global::pLight = light;
	}
}



render_square::render_square(HINSTANCE hInstance) : D3D_app(hInstance)
{
	
}

render_square::~render_square()
{
	UninitScene();
	UninitD3D();
}

bool render_square::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	if (!InitD3D()) return false;

	if (!InitScene()) return false;

	if (!InitImGUI()) return false;

	return true;
}



bool render_square::InitD3D()
{
	// 결과값.
	HRESULT hr = 0;

	// 스왑체인 속성 설정 구조체 생성.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// 화면 주사율 설정.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// 샘플링 관련 설정.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. 장치 생성.   2.스왑체인 생성. 3.장치 컨텍스트 생성.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	// 4. 렌더타겟뷰 생성.  (백버퍼를 이용하는 렌더타겟뷰)	
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // 텍스처는 내부 참조 증가
	SAFE_RELEASE(pBackBufferTexture);	//외부 참조 카운트를 감소시킨다.
	// 렌더 타겟을 최종 출력 파이프라인에 바인딩합니다.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	// 뷰포트 설정.	
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)m_ClientWidth;
	viewport.Height = (float)m_ClientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_ClientWidth;
	descDepth.Height = m_ClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));


	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pDepthBufferView));
	SAFE_RELEASE(textureDepthStencil);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthBufferView);

	return true;
}

void render_square::UninitD3D()
{
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
}

bool render_square::InitScene()
{
	HRESULT hr = 0; // 결과값.
	// 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼및 버퍼 정보 준비

	//Vertex vertices[] = // Local or Object or Model Space    position color
	//{//   pos                           normal                      uvCoord(texCoord)
	//	{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0.0f, 1.0f, 0.0f),  Vector3(-1.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f) },// Normal Y +	 
	//	{ Vector3(1.0f, 1.0f, -1.0f),	Vector3(0.0f, 1.0f, 0.0f),  Vector3(-1.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f) },
	//	{ Vector3(1.0f, 1.0f, 1.0f),	Vector3(0.0f, 1.0f, 0.0f),  Vector3(-1.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f) },
	//	{ Vector3(-1.0f, 1.0f, 1.0f),	Vector3(0.0f, 1.0f, 0.0f),  Vector3(-1.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f) },
	//															    
	//	{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) },// Normal Y -		
	//	{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f) },
	//	{ Vector3(1.0f, -1.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f) },
	//	{ Vector3(-1.0f, -1.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f) },
	//															    
	//	{ Vector3(-1.0f, -1.0f, 1.0f),	Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },//	Normal X -
	//	{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
	//	{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
	//	{ Vector3(-1.0f, 1.0f, 1.0f),	Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
	//															    
	//	{ Vector3(1.0f, -1.0f, 1.0f),	Vector3(1.0f, 0.0f, 0.0f),  Vector3(0.0f, 0.0f, -1.0f),  Vector2(1.0f, 1.0f) },// Normal X +
	//	{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(1.0f, 0.0f, 0.0f),  Vector3(0.0f, 0.0f, -1.0f),  Vector2(0.0f, 1.0f) },
	//	{ Vector3(1.0f, 1.0f, -1.0f),	Vector3(1.0f, 0.0f, 0.0f),  Vector3(0.0f, 0.0f, -1.0f),  Vector2(0.0f, 0.0f) },
	//	{ Vector3(1.0f, 1.0f, 1.0f),	Vector3(1.0f, 0.0f, 0.0f),  Vector3(0.0f, 0.0f, -1.0f),  Vector2(1.0f, 0.0f) },
	//															    
	//	{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f) }, // Normal Z -
	//	{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f) },
	//	{ Vector3(1.0f, 1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f) },
	//	{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) },
	//															    
	//	{ Vector3(-1.0f, -1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),  Vector3(1.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f) },// Normal Z +
	//	{ Vector3(1.0f, -1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),  Vector3(1.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f) },
	//	{ Vector3(1.0f, 1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),  Vector3(1.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f) },
	//	{ Vector3(-1.0f, 1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),  Vector3(1.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f) },
	//};

	//D3D11_BUFFER_DESC bd = {};
	//bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA vbData = {};
	//vbData.pSysMem = vertices;	// 배열 데이터 할당.
	//HR_T(m_pDevice->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	//m_VertextBufferStride = sizeof(Vertex);		// 버텍스 버퍼 정보
	//m_VertextBufferOffset = 0;

	// 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성 	
	ID3D10Blob* vertexShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"VertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));

	D3D11_INPUT_ELEMENT_DESC layout[] = // 입력 레이아웃.
	{   // SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate	
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	// 3. Render() 에서 파이프라인에 바인딩할  버텍스 셰이더 생성
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);	// 버퍼 해제.

	//// 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성
	//WORD indices[] =
	//{
	//	3,1,0, 2,1,3,
	//	6,4,5, 7,4,6,
	//	11,9,8, 10,9,11,
	//	14,12,13, 15,12,14,
	//	19,17,16, 18,17,19,
	//	22,20,21, 23,20,22
	//};

	//// 인덱스 개수 저장.
	//m_nIndices = ARRAYSIZE(indices);

	//bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	//bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.CPUAccessFlags = 0;

	//D3D11_SUBRESOURCE_DATA ibData = {};
	//ibData.pSysMem = indices;
	//HR_T(m_pDevice->CreateBuffer(&bd, &ibData, &m_pIndexBuffer));

	// 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
	ID3D10Blob* pixelShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"PixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
	HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);	// 픽셀 셰이더 버퍼 더이상 필요없음.


	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer));

	// Load the Texture
	//HR_T(CreateDDSTextureFromFile(m_pDevice, L"seafloor.dds", nullptr, &m_pTextureRV));
	/*HR_T(CreateTextureFromFile(m_pDevice, L"Bricks059_1K-JPG_Color.jpg", &m_pTextureRV));
	HR_T(CreateTextureFromFile(m_pDevice, L"Bricks059_1K-JPG_NormalDX.jpg", &m_pNormalMap));
	HR_T(CreateTextureFromFile(m_pDevice, L"Bricks059_Specular.png", &m_pSpecularMap));*/

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));


	mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 1000.f);

	global::Init(m_pDeviceContext, m_pConstantBuffer, m_nIndices, &camera, &mProjection, &light);

	camera.transform.AddPosition({ 0,0,50 });

	Import.Load(m_hWnd, m_pDevice, m_pDeviceContext, "Resource/Character.fbx");

	return true;
}

void render_square::UninitScene()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
}



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK render_square::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}

void render_square::Update(float deltaT)
{
	//float deltaRot = 0.5f * deltaT;
	//cube1.tf.AddRotation(deltaRot, { 0,0,1 });

	//cube2.tf.AddRotation(deltaRot, { 0,1,1 });

}

void render_square::Render()
{
	float color[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	// 화면 칠하기.
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Draw계열 함수를 호출하기전에 렌더링 파이프라인에 필수 스테이지 설정을 해야한다.	
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정점을 이어서 그릴 방식 설정.
	//m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertextBufferStride, &m_VertextBufferOffset);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	//m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

	ConstantBuffer cb;
	cb.mWorld = Matrix::Identity;
	cb.mView = XMMatrixTranspose(global::pCam->GetViewMatrix());
	cb.mProjection = XMMatrixTranspose(*global::pProjectionMatrix);
	cb.lightDirection = global::pLight->Direction;
	cb.lightColor = global::pLight->LightDiffuseColor;
	cb.AmbientLightColor = global::pLight->LightAmbientColor;
	cb.MatAmbientPower = 0.15f;
	cb.MatSpecularColor = { 1,1,1,1 };
	cb.MatSpecularPower = 500;
	cb.CamPosition = global::pCam->transform.GetPosition();
	cb.LightPower = global::pLight->LightPower;
	cb.LightSpecularPower = global::pLight->LightSpecularPower;

	Import.Draw(m_pDeviceContext, global::pConstantBufffer, std::move(cb));

	ShowImGUI();

	// 스왑체인 교체.
	m_pSwapChain->Present(0, 0);
}


void Cube::Render()
{

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(tf.WorldMatrix());
	cb.mView = XMMatrixTranspose(global::pCam->GetViewMatrix());
	cb.mProjection = XMMatrixTranspose(*global::pProjectionMatrix);
	cb.lightDirection = global::pLight->Direction;
	cb.lightColor = global::pLight->LightDiffuseColor;
	cb.AmbientLightColor = global::pLight->LightAmbientColor;
	cb.MatAmbientPower = MatAmbientPower;
	cb.MatSpecularColor = MatSpecularColor;
	cb.MatSpecularPower = MatSpecularPow;
	cb.CamPosition = global::pCam->transform.GetPosition();
	cb.LightPower = global::pLight->LightPower;
	cb.LightSpecularPower = global::pLight->LightSpecularPower;

	global::pDevice->UpdateSubresource(global::pConstantBufffer, 0, nullptr, &cb, 0, 0);

	global::pDevice->DrawIndexed(global::idxes, 0, 0);
}

Cube::Cube()
{

}

void render_square::ShowImGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	/*if (m_show_demo_window)
		ImGui::ShowDemoWindow(&m_show_demo_window);*/

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{

		ImGui::Begin("HelloWorld");                          // Create a window called "Hello, world!" and append into it.

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Demo Window", &m_show_demo_window);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &m_show_another_window);

		//ImGui::SliderFloat("float", &m_f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::DragFloat3("mesh1 Position", (float*)&mesh1, 0.01f);

		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	m_counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", m_counter);
		ImGui::Text("wasd to move, shift to down space to up\nRight click and move mouse to turn Camera\n");

		//camera.ImGUI();

		light.ImGUI();

		//cube1.ImGUI("Cube1");

		//ImGui::Text("cube2");
		//cube2.ImGUI("Cube2");


		ImGui::Text("average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		//ImGui::ColorEdit3("clear color", (float*)&m_ClearColor); // Edit 3 floats representing a color	
		ImGui::End();
	}

	// 3. Show another simple window.
	//if (m_show_another_window)
	//{
	//	ImGui::Begin("Another Window", &m_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		m_show_another_window = false;
	//	ImGui::End();
	//}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void Cube::ImGUI(const char name[])
{

	ImGui::Text(name);

	const int strSize = 11;
	const char blank[10] = "         ";
	char str[strSize];
	strcpy_s(str,name);
	int nSize = strlen(name);
	memcpy_s((str + nSize), strSize - nSize, blank, strSize - nSize);

	Vector3 Pos = tf.GetPosition();
	Vector3 Rot = tf.GetRotation() * 180 / PI;
	//transform 자체는 쿼터니언 여기서 오일러로 바꾸기 때문에 짐벌락 발생

	memcpy_s((str + nameSize-1), 4, "Pos", 4);
	ImGui::DragFloat3(str, (float*)&Pos, 0.1f);

	memcpy_s((str + nameSize-1), 4, "Rot", 4);
	ImGui::DragFloat3(str, (float*)&Rot, 0.5f);

	//Vector3 scale;
	//ImGui::DragFloat3("Scale", (float*)&scale, 0.1f);

	tf.SetPosition(Pos);
	tf.SetRotation(Rot / 180 * PI);

	ImGui::SliderFloat("AmbientPower", &MatAmbientPower,0.0f,1.0f);
	ImGui::ColorEdit4("SpecularColor", (float*)&MatSpecularColor);
	ImGui::SliderFloat("SpecularPower", &MatSpecularPow, 10, 4000);

}

void directionLight::SetDirection(float yaw, float pitch)
{
	float y = yaw / 180 * PI;
	float x = pitch / 180 * PI;
	Direction.x = sinf(y) * cosf(x);
	Direction.z = cosf(y) * cosf(x);
	Direction.y = sinf(x);

	Direction.Normalize();
}

void directionLight::ImGUI()
{

	ImGui::Text("light");

	ImGui::SliderFloat("light Power", &LightPower, 0, 10);

	static float v[2] = { 180,0 };
	ImGui::DragFloat2("light Direction", v, 0.5f);
	SetDirection(v[0], v[1]);

	//ImGui::SliderFloat3("light Dir", (float*)&Direction, -1, 1);
	

	ImGui::ColorEdit4("light color", (float*)&LightDiffuseColor);
	ImGui::ColorEdit4("Ambient light", (float*)&LightAmbientColor);

	ImGui::SliderFloat("light SpecularPower", &LightSpecularPower, 0, 3);
}


bool render_square::InitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	return true;
}

void render_square::UninitImGUI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}