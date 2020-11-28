// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//#include "DuplicationManager.h"

#include "DuplicationManager.h"
#include <stdio.h> //for the sprinf_s function used by DrawString
#include <d3d9.h> //include a directx header
//#include "DirectX/d3d9.h" //include a directx header
#include "DirectX/d3dx9.h" //include a directx header
#include <string>
#include <dxgi1_2.h>
//#pragma comment(lib, "d3d9.lib") //link the library
//#pragma comment(lib, "d3dx9.lib") //link the library
//#include "VertexShader.h"
//#include "PixelShader.h"

// Below are lists of errors expect from Dxgi API calls when a transition event like mode change, PnpStop, PnpStart
// desktop switch, TDR or session disconnect/reconnect. In all these cases we want the application to clean up the threads that process
// the desktop updates and attempt to recreate them.
// If we get an error that is not on the appropriate list then we exit the application

// These are the errors we expect from general Dxgi API due to a transition
HRESULT SystemTransitionsExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	DXGI_ERROR_ACCESS_LOST,
	static_cast<HRESULT>(WAIT_ABANDONED),
	S_OK                                    // Terminate list with zero valued HRESULT
};


// These are the errors we expect from IDXGIOutput1::DuplicateOutput due to a transition
HRESULT CreateDuplicationExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	static_cast<HRESULT>(E_ACCESSDENIED),
	DXGI_ERROR_UNSUPPORTED,
	DXGI_ERROR_SESSION_DISCONNECTED,
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIOutputDuplication methods due to a transition
HRESULT FrameInfoExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	DXGI_ERROR_ACCESS_LOST,
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIAdapter::EnumOutputs methods due to outputs becoming stale during a transition
HRESULT EnumOutputsExpectedErrors[] = {
	DXGI_ERROR_NOT_FOUND,
	S_OK                                    // Terminate list with zero valued HRESULT
};

int WINDOW_WIDTH = 1024; //window width
int WINDOW_HEIGHT = 768; //window height
bool windowed = true; //window mode, change to false for full screen
IDirect3D9* pD3D; //global variable that will be used by a couple of our function
IDirect3DDevice9* pDevice; //a device that will be used for msot of our function created inside *pD3D
D3D_FEATURE_LEVEL FeatureLevel;
D3DPRESENT_PARAMETERS d3dpp; //the presentation parameters that will be used when we will create the device
LPDIRECT3DTEXTURE9 imagetex;
LPD3DXSPRITE sprite;
D3DXVECTOR3 imagepos;
LPDIRECT3DTEXTURE9  m_pTexture;
IDirect3DSurface9* m_pTextureSurface;
//
// Constructor sets up references / variables
//
ID3D11RenderTargetView* backbuffer;    // global declaration

HWND m_WindowHandle;
IDXGIFactory2* m_Factory;
ID3D11RenderTargetView* m_RTV;
ID3D11DeviceContext* m_DeviceContext;
#define NUMVERTICES 6

struct CUSTOMVERTEX {
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
	FLOAT tu, tv;   // The texture coordinates
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
LPDIRECT3DVERTEXBUFFER9 g_pVB;
LPDIRECT3DINDEXBUFFER9 g_IB;
DUPLICATIONMANAGER::~DUPLICATIONMANAGER()
{
	if (pD3D) //check if pD3D ism't released
		pD3D->Release(); //release it
	if (pDevice) //check if pDevice ism't released
		pDevice->Release(); //release it
	if (sprite) //check if sprite ism't released
		sprite->Release(); //release it
	if (imagetex) //check if imagetex ism't released
		imagetex->Release(); //release it
	if (m_DeskDupl)
	{
		m_DeskDupl->Release();
		m_DeskDupl = nullptr;
	}
	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}
	if (m_Factory)
	{
		m_Factory->Release();
		m_Factory = nullptr;
	}
	if (m_DestImage)
	{
		m_DestImage->Release();
		m_DestImage = nullptr;
	}
	if (m_RTV)
	{
		m_RTV->Release();
		m_RTV = nullptr;
	}

	if (m_DxRes->Device)
	{
		m_DxRes->Device->Release();
		m_DxRes->Device = nullptr;
	}
	if (m_DxRes->swapchain)
	{
		m_DxRes->swapchain->Release();
		m_DxRes->swapchain = nullptr;
	}
	if (m_DxRes->Context)
	{
		m_DxRes->Context->Release();
		m_DxRes->Context = nullptr;
	}
}

//
// Initialize duplication interfaces
//

DUPL_RETURN DUPLICATIONMANAGER::InitDupl(_In_ FILE* log_file, UINT Output, HWND hwnd)
{
	m_WindowHandle = hwnd;
	m_log_file = log_file;
	m_DxRes = new (std::nothrow) DX_RESOURCES;
	RtlZeroMemory(m_DxRes, sizeof(DX_RESOURCES));
	DUPL_RETURN Ret = InitializeDx(hwnd);
	
	if (Ret != DUPL_RETURN_SUCCESS)
	{
		fprintf_s(log_file, "DX_RESOURCES couldn't be initialized.");
		return Ret;
	}
	m_OutputNumber = Output;
	
	return DUPL_RETURN_SUCCESS;
}
_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)

IDXGIResource* DesktopResource = nullptr;
DXGI_OUTDUPL_FRAME_INFO FrameInfo;

DUPL_RETURN DUPLICATIONMANAGER::GetFrame(HWND hwnd)
{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, L"C:\\img\\img1.bmp", &imagetex)))//first parameter is our device,second is the path to our image, third is a texture variable to load the image into
	{
		// Emsg("Failed to laod the image"); //error pop-up for debug purpose
		return DUPL_RETURN_ERROR_EXPECTED; //return it failed
	}
	

	if (FAILED(D3DXCreateSprite(pDevice, &sprite)))//first parameter is our device, second is a empty sprite variable
	{
		// Emsg("Failed to create the sprite"); //error pop-up for debug purpose
		return DUPL_RETURN_ERROR_EXPECTED; //return it failed
	}
	imagepos.x = 8.0f; //coord x of our sprite
	imagepos.y = 18.0f; //coord y of out sprite
	imagepos.z = 0.0f; //coord z of out sprite
	
	return DUPL_RETURN_SUCCESS;
}
HRESULT DUPLICATIONMANAGER::render() //function where all Drawing/Render stuff go
{
	
	if (SUCCEEDED(pDevice->BeginScene())) //Call the BeginScene function with our Device and see if it succeeded, all drawing/render code go behind BeginScene and EndScene
	{
		sprite->Begin(D3DXSPRITE_ALPHABLEND); //begin our sprite with alphablend so it support alpha, like png use alpha
		sprite->Draw(imagetex, NULL, NULL, &imagepos, 0xFFFFFFFF); //Draw the sprite, first parameter is the image texture, 4th parameter is the position, last parameter is the color change leave to 0xFFFFFFFF for no change
		sprite->End(); //end the sprite
		pDevice->EndScene(); //Call the EndScene function with our Device

	}
	pDevice->Present(NULL, NULL, NULL, NULL);
	return E_FAIL; //return it failed
}

DUPL_RETURN DUPLICATIONMANAGER::InitializeDx(HWND hwnd)
{
	
	ZeroMemory(&d3dpp, sizeof(d3dpp)); //to be sure d3dpp is empty
	d3dpp.Windowed = windowed; //use our global windowed variable to tell if the program is windowed or not
	d3dpp.hDeviceWindow = hwnd; //give the window handle of the window we created above
	d3dpp.BackBufferCount = 1; //set it to only use 1 backbuffer
	d3dpp.BackBufferWidth = WINDOW_WIDTH; //set the buffer to our window width
	d3dpp.BackBufferHeight = WINDOW_HEIGHT; //set the buffer to out window height
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; //the backbuffer format
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //SwapEffect
	// Create device
	pD3D = Direct3DCreate9(D3D_SDK_VERSION); //Create the presentation parameters
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice)))
	{
		return DUPL_RETURN_ERROR_EXPECTED;
	}
	return DUPL_RETURN_SUCCESS;
}