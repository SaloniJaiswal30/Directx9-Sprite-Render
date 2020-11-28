#pragma once
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#ifndef _DUPLICATIONMANAGER_H_
#define _DUPLICATIONMANAGER_H_
#include<windows.h>
#include <d3d11.h>
#include<DirectXMath.h>
using namespace DirectX;

#include <sal.h>
#include <new>
#include <stdio.h>
#include <dxgi1_2.h>




////------------------------------------------------------------------------------
//// 3D Vector; 32 bit floating point components
//struct XMFLOAT3
//{
//	float x;
//	float y;
//	float z;
//
//	XMFLOAT3() = default;
//
//	XMFLOAT3(const XMFLOAT3&) = default;
//	XMFLOAT3& operator=(const XMFLOAT3&) = default;
//
//	XMFLOAT3(XMFLOAT3&&) = default;
//	XMFLOAT3& operator=(XMFLOAT3&&) = default;
//
//	XM_CONSTEXPR XMFLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
//	explicit XMFLOAT3(_In_reads_(3) const float* pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]) {}
//};
//
////------------------------------------------------------------------------------
//// 2D Vector; 32 bit floating point components
//struct XMFLOAT2
//{
//	float x;
//	float y;
//
//	XMFLOAT2() = default;
//
//	XMFLOAT2(const XMFLOAT2&) = default;
//	XMFLOAT2& operator=(const XMFLOAT2&) = default;
//
//	XMFLOAT2(XMFLOAT2&&) = default;
//	XMFLOAT2& operator=(XMFLOAT2&&) = default;
//
//	XM_CONSTEXPR XMFLOAT2(float _x, float _y) : x(_x), y(_y) {}
//	explicit XMFLOAT2(_In_reads_(2) const float* pArray) : x(pArray[0]), y(pArray[1]) {}
//};

extern HRESULT SystemTransitionsExpectedErrors[];
extern HRESULT CreateDuplicationExpectedErrors[];
extern HRESULT FrameInfoExpectedErrors[];
extern HRESULT AcquireFrameExpectedError[];
extern HRESULT EnumOutputsExpectedErrors[];

typedef _Return_type_success_(return == DUPL_RETURN_SUCCESS) enum
{
	DUPL_RETURN_SUCCESS = 0,
	DUPL_RETURN_ERROR_EXPECTED = 1,
	DUPL_RETURN_ERROR_UNEXPECTED = 2
}DUPL_RETURN;


//
// Structure that holds D3D resources not directly tied to any one thread
//
typedef struct _DX_RESOURCES
{
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;
	ID3D11SamplerState* SamplerLinear;
	IDXGISwapChain1* swapchain;
} DX_RESOURCES;



//
// Handles the task of duplicating an output.
//
class DUPLICATIONMANAGER
{
public:

	//methods
	//DUPLICATIONMANAGER();
	~DUPLICATIONMANAGER();
	_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)
		DUPL_RETURN GetFrame(HWND hwnd);
	HRESULT render();
	DUPL_RETURN InitShaders();
	DUPL_RETURN InitDupl(_In_ FILE* log_file, UINT Output, HWND hwnd);
	int GetImageHeight();
	int GetImageWidth();
	int GetImagePitch();
	//vars

private:

	// vars
	IDXGIOutputDuplication* m_DeskDupl;
	ID3D11Texture2D* m_AcquiredDesktopImage;
	ID3D11Texture2D* m_DestImage;
	UINT m_OutputNumber;
	DXGI_OUTPUT_DESC m_OutputDesc;
	DX_RESOURCES* m_DxRes;
	FILE* m_log_file;
	int m_ImagePitch;

	//methods
	DUPL_RETURN InitializeDx(HWND hwnd);
	//_Post_satisfies_(return != DUPL_RETURN_SUCCESS)
	DUPL_RETURN ProcessFailure(_In_opt_ ID3D11Device* Device, _In_ LPCWSTR Str, HRESULT hr, _In_opt_z_ HRESULT* ExpectedErrors = nullptr);
	void DisplayMsg(_In_ LPCWSTR Str, HRESULT hr);
	void CopyImage(BYTE* ImageData);
	void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr);
	DUPL_RETURN DoneWithFrame();

};
typedef struct _VERTEX
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
} VERTEX;
#endif
