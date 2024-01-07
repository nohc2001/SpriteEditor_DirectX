//--------------------------------------------------------------------------------------
// File: Tutorial05.cpp
//
// This application demonstrates animation using matrix transformations
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
//#include <xnamath.h>
#include "resource.h"
#include "ShapeObject.h"
#include "DX11_UI.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*        g_pDepthStencil = NULL;
ID3D11DepthStencilView* g_pDepthStencilView = NULL;
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
XMMATRIX                g_World1;
XMMATRIX                g_World2;
XMMATRIX                g_CursorWorld;
XMMATRIX                g_DBGWorld;
XMMATRIX                g_View;
XMMATRIX                g_Projection_3d;
XMMATRIX                g_Projection_2d;
ConstantBuffer polygon_cb;

//control
bool ctrl_3d = true;
shp::vec2f DX_UI::mousePos = shp::vec2f(0, 0);
int DX_UI::FocusID = -1;
XMFLOAT4 rbuffer::InputColor;
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();

FM_System0* fm;

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    fm = new freemem::FM_System0();
    fm->SetHeapData(4096, 4096, 4096, 4096);
    fm->_tempPushLayer();

    const char* font_path = "Cafe24Ssurround-v2.0.ttf";
    uint8_t condition_variable = 0;
    int8_t error = TTFFontParser::parse_file(font_path, &font_data, &font_parsed, &condition_variable);

    sub_font_data[0] = new TTFFontParser::FontData();
    const char* subfont_path = "ARLRDBD.TTF";
    error = TTFFontParser::parse_file(subfont_path, sub_font_data[0], &font_parsed, &condition_variable);

    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    fm->_tempPopLayer();
    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_ICON2 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_ICON2);
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 1600, 900 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"SpriteEditor_DirectX", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
UINT width;
UINT height;
rbuffer cursor_obj;
rbuffer polygon_obj;
rbuffer dbgpos_obj;

HRESULT InitDevice()
{

    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = false;
    ID3D11RasterizerState* rasterizerState;
    g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

    // Set the rasterizer state
    g_pImmediateContext->RSSetState(rasterizerState);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"SpriteEditor_DirectX.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
        return hr;
	}

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
	pVSBlob->Release();
	if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"SpriteEditor_DirectX.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }


	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
	pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        SimpleVertex( -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 255, 255 ),
        SimpleVertex( 1.0f, 1.0f, -1.0f, 0.0f, 255, 0.0f, 255 ),
        SimpleVertex( 1.0f, 1.0f, 1.0f, 0.0f, 255, 255, 255 ),
        SimpleVertex( -1.0f, 1.0f, 1.0f, 255, 0.0f, 0.0f, 255 ),
        SimpleVertex( -1.0f, -1.0f, -1.0f, 255, 0.0f, 255, 255 ),
        SimpleVertex( 1.0f, -1.0f, -1.0f, 255, 255, 0.0f, 255 ),
        SimpleVertex( 1.0f, -1.0f, 1.0f, 255, 255, 255, 255 ),
        SimpleVertex( -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 255 ),
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pConstantBuffer );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrix
	g_World1 = XMMatrixIdentity();
	g_World2 = XMMatrixIdentity();
    g_CursorWorld = XMMatrixIdentity();

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 0.0f, 0.0f, -5.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	g_View = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
    float farZ = 100.0f;
    float nearZ = 0.01f;

    //원근투영
	g_Projection_3d = XMMatrixPerspectiveFovLH( XM_PIDIV2, width / (FLOAT)height, nearZ, farZ);
    //직교투영
    g_Projection_2d = XMMATRIX(
        2.0f/(float)width, 0, 0, 0,
        0, 2.0f/(float)height, 0, 0,
        0, 0, 1.0f / (farZ - nearZ), 0,
        0, 0, 1.0f / (nearZ - farZ), 1);

    cursor_obj.Init(false);
    cursor_obj.begin(g_pVertexShader, g_pPixelShader);
    cursor_obj.av(SimpleVertex(0.0f, -30.0f, 0.0f, 255.0f, 255.0f, 255.0f, 0.0f));
    cursor_obj.av(SimpleVertex(30.0f, -30.0f, 0.0f, 255.0f, 255.0f, 255.0f, 0.0f));
    cursor_obj.av(SimpleVertex(0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f));
    cursor_obj.end();

    polygon_obj.Init(false);
    polygon_obj.begin(g_pVertexShader, g_pPixelShader);
    polygon_obj.av(SimpleVertex(100.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f));
    polygon_obj.av(SimpleVertex(0.0f, 100.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f));
    polygon_obj.av(SimpleVertex(0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f));
    polygon_obj.end();

    dbgpos_obj.Init(false);
    dbgpos_obj.begin(g_pVertexShader, g_pPixelShader);
    dbgpos_obj.av(SimpleVertex(10.0f, -10.0f, 0.0f, 255.0f, 255.0f, 0.0f, 200.0f));
    dbgpos_obj.av(SimpleVertex(10.0f, 10.0f, 0.0f, 255.0f, 255.0f, 0.0f, 200.0f));
    dbgpos_obj.av(SimpleVertex(-10.0f, 10.0f, 0.0f, 255.0f, 255.0f, 0.0f, 200.0f));
    dbgpos_obj.av(SimpleVertex(-10.0f, -10.0f, 0.0f, 255.0f, 255.0f, 0.0f, 200.0f));
    dbgpos_obj.end();

    polygon_cb.mWorld = XMMatrixTranspose(g_World1);
    polygon_cb.mView = XMMatrixTranspose(g_View);
    polygon_cb.mProjection = XMMatrixTranspose(g_Projection_2d);

    CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
    cbuf->ready(L'안', g_pVertexShader, g_pPixelShader);
    char_map.insert(CharMap::value_type(L'안', cbuf));

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}

void MainPage_Init(Page* page) {
    page->pfm.SetHeapData(new byte8[4096], 4096);
    DXBtn* PolyAddBtn = (DXBtn*)page->pfm._New(sizeof(DXBtn));
    DXBtn* PolyClearBtn = (DXBtn*)page->pfm._New(sizeof(DXBtn));
}

void MainPage_Render(Page* page) {

}

void MainPage_Update(Page* page, float delta) {

}

void MainPage_Event(Page* page, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    float mx, my;
    switch (message)
    {
    case WM_LBUTTONDOWN:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        polygon_obj.av(SimpleVertex((float)mx - (float)width / 2.0f, -1.0f * (float)my + (float)height / 2.0f, 0.0f, 255.0f, 0.0f, 255.0f, 255.0f));
        polygon_obj.end();
        break;
    case WM_RBUTTONDOWN:
        polygon_obj.arr[polygon_obj.choice].up = 0;
        polygon_obj.indexes[polygon_obj.choice].up = 0;
        polygon_obj.end();
        break;
    }
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
int mx, my;
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    //pageEvent
    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;
        case WM_KEYDOWN:
            PostQuitMessage(0);
            break;
        case WM_MOUSEMOVE:
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            DX_UI::mousePos = shp::vec2f((float)mx, (float)my);
            g_CursorWorld = XMMatrixTranslation((float)mx - (float)width/2.0f, -1.0f * (float)my + (float)height/2.0f, 0.9f);
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    fm->_tempPushLayer();
    // Update our time
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();
        if( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;
        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

    // 1st Cube: Rotate around the origin
	//g_World1 = XMMatrixRotationY( t );

    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red, green, blue, alpha
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update variables for the first cube
    //
    ConstantBuffer cursor_cb;
    cursor_cb.mWorld = XMMatrixTranspose(g_CursorWorld);
    cursor_cb.mView = XMMatrixTranspose(g_View);
    cursor_cb.mProjection = XMMatrixTranspose(g_Projection_2d);
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cursor_cb, 0, 0);
    cursor_obj.render(cursor_cb);
    
    //char_map.at(L'안')->render(cursor_cb);
    
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &polygon_cb, 0, 0);
    //polygon_obj.render(polygon_cb);

    ConstantBuffer dbgpos_cb;
    dbgpos_cb.mView = XMMatrixTranspose(g_View);
    dbgpos_cb.mProjection = XMMatrixTranspose(g_Projection_2d);
    for (int i = 0; i < polygon_obj.arr[polygon_obj.choice].size(); ++i) {
        g_DBGWorld = XMMatrixTranslation((float)polygon_obj.arr[polygon_obj.choice][i].Pos.x, (float)polygon_obj.arr[polygon_obj.choice][i].Pos.y, 0.9f);
        dbgpos_cb.mWorld = XMMatrixTranspose(g_DBGWorld);
        g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &dbgpos_cb, 0, 0);
        dbgpos_obj.render(dbgpos_cb);
    }

    draw_string(L"안녕하세용!! World!!", 16, 30, shp::rect4f(0, 0, 100, 100), g_pVertexShader, g_pPixelShader);
    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
    fm->_tempPopLayer();
}
