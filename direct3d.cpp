/*==============================================================================

   Direct3D�̏������֘A [direct3d.cpp]
														 Author : Harada Ren
														 Date   : 2025/05/12
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include "direct3d.h"
#include "debug_ostream.h"

#pragma comment(lib, "d3d11.lib") //���C�u�����������N���閽��
// #pragma comment(lib, "dxgi.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DirectXTex_Debug.lib") 
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif

/* �e��C���^�[�t�F�[�X */
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11BlendState* g_pBlendStateMultiply = nullptr;
static ID3D11BlendState* g_pBlendStateAdd = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilStateDepthDisable = nullptr;

/* �o�b�N�o�b�t�@�֘A */
static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
static ID3D11Texture2D* g_pDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC g_BackBufferDesc{};
static D3D11_VIEWPORT g_Viewport{}; //�r���[�{�[�g�ݒ�p


static bool configureBackBuffer(); // �o�b�N�o�b�t�@�̐ݒ�E����
static void releaseBackBuffer(); // �o�b�N�o�b�t�@�̉��


bool Direct3D_Initialize(HWND hWnd) {
	/* �f�o�C�X�A�X���b�v�`�F�[���A�R���e�L�X�g���� */
	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	//swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//�x���`�}�[�N�e�X�g�p

	swap_chain_desc.OutputWindow = hWnd;

	UINT device_flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		device_flags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&g_pSwapChain,
		&g_pDevice,
		&feature_level,
		&g_pDeviceContext);

	if (FAILED(hr)) {
		MessageBox(hWnd, "Direct3D�̏������Ɏ��s���܂���", "�G���[", MB_OK);
		return false;
	}

	if (!configureBackBuffer()) {
		MessageBox(hWnd, "�o�b�N�o�b�t�@�̐ݒ�Ɏ��s���܂���", "�G���[", MB_OK);
		return false;
	}

	// �u�����h�X�e�[�g�ݒ�
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;

	bd.RenderTarget[0].BlendEnable = TRUE; //���u�����h����/���Ȃ�

	//--------���߃u�����h�̐ݒ�---------
	//RGB
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 	//SrcRGB*SrcA
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //DestRGB * (1-SrcA)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; ////OP�c�I�y�����h(���Z�q) ADD�c+
	//SrcRGB * SrcA + DestRGB * (1-SrcA)


	//��
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; //1
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; //0
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; //OP�c�I�y�����h(���Z�q)
	//SrcA * 1 + DestA * 0

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDevice->CreateBlendState(&bd, &g_pBlendStateMultiply);


	//--------���Z�u�����h----------
	//RGB
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; //DestRGB * (1-SrcA)
	//SrcRGB * SrcA + DestRGB * (1-SrcA)

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDevice->CreateBlendState(&bd, &g_pBlendStateAdd); //�N���G�C�g�����烊���[�X����

	Direct3D_SetAlphaBlendTransparent();//�f�t�H���g�̃u�����h�X�e�[�g


	// �[�x�X�e���V���X�e�[�g�ݒ�
	D3D11_DEPTH_STENCIL_DESC dsd{};
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = FALSE;
	dsd.DepthEnable = FALSE; // �����ɂ���
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthDisable);

	g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthDisable, NULL);

	return true;
}

void Direct3D_Finalize() {
	SAFE_RELEASE(g_pDepthStencilStateDepthDisable);
	SAFE_RELEASE(g_pBlendStateMultiply);
	releaseBackBuffer();
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pDevice);
}

void Direct3D_Clear() {
	float clear_color[4] = { 0.0f, 0.f, 1.0f, 1.0f }; //0~1

	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�̐ݒ�
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}

void Direct3D_Present() {
	// �X���b�v�`�F�[���̕\��
	g_pSwapChain->Present(1, 0); //(0,0)�Ő�����������
}

unsigned int Direct3D_GetBackBufferWidth() {
	return g_BackBufferDesc.Width;
}

unsigned int Direct3D_GetBackBufferHeight() {
	return g_BackBufferDesc.Height;
}

ID3D11Device* Direct3D_GetDevice() {
	return g_pDevice;
}

ID3D11DeviceContext* Direct3D_GetContext() {
	return g_pDeviceContext;
}

void Direct3D_SetAlphaBlendTransparent() {
	float blend_factor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->OMSetBlendState(g_pBlendStateMultiply, blend_factor, 0xffffffff);

}

void Direct3D_SetAlphaBlendAdd() {
	float blend_factor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->OMSetBlendState(g_pBlendStateAdd, blend_factor, 0xffffffff);
}

bool configureBackBuffer() {
	HRESULT hr;

	ID3D11Texture2D* back_buffer_pointer = nullptr;

	// �o�b�N�o�b�t�@�̎擾
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_pointer);

	if (FAILED(hr)) {
		hal::dout << "�o�b�N�o�b�t�@�̎擾�Ɏ��s���܂���" << std::endl;
		return false;
	}

	// �o�b�N�o�b�t�@�̃����_�[�^�[�Q�b�g�r���[�̐���
	hr = g_pDevice->CreateRenderTargetView(back_buffer_pointer, nullptr, &g_pRenderTargetView);

	if (FAILED(hr)) {
		back_buffer_pointer->Release();
		hal::dout << "�o�b�N�o�b�t�@�̃����_�[�^�[�Q�b�g�r���[�̐����Ɏ��s���܂���" << std::endl;
		return false;
	}

	// �o�b�N�o�b�t�@�̏�ԁi���j���擾
	back_buffer_pointer->GetDesc(&g_BackBufferDesc);

	back_buffer_pointer->Release(); // �o�b�N�o�b�t�@�̃|�C���^�͕s�v�Ȃ̂ŉ��

	// �f�v�X�X�e���V���o�b�t�@�̐���
	D3D11_TEXTURE2D_DESC depth_stencil_desc{};
	depth_stencil_desc.Width = g_BackBufferDesc.Width;
	depth_stencil_desc.Height = g_BackBufferDesc.Height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	hr = g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pDepthStencilBuffer);

	if (FAILED(hr)) {
		hal::dout << "�f�v�X�X�e���V���o�b�t�@�̐����Ɏ��s���܂���" << std::endl;
		return false;
	}

	// �f�v�X�X�e���V���r���[�̐���
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, &depth_stencil_view_desc, &g_pDepthStencilView);

	if (FAILED(hr)) {
		hal::dout << "�f�v�X�X�e���V���r���[�̐����Ɏ��s���܂���" << std::endl;
		return false;
	}


	// �r���[�|�[�g�̐ݒ�
	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_Viewport.Width = static_cast<FLOAT>(g_BackBufferDesc.Width);
	g_Viewport.Height = static_cast<FLOAT>(g_BackBufferDesc.Height);
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;
	g_pDeviceContext->RSSetViewports(1, &g_Viewport); // �r���[�|�[�g�̐ݒ�

	return true;
}

void releaseBackBuffer() {
	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pDepthStencilBuffer);
	SAFE_RELEASE(g_pDepthStencilView);
}
