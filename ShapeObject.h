#pragma once
#define _CORRECT_ISO_CPP11_MATH_H_PROTO
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "Utill_FreeMemory.h"
#include "Utill_SpaceMath.h"

using namespace freemem;

#define uint unsigned int

ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
ID3D11Buffer* g_pConstantBuffer = NULL;

struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;

    SimpleVertex(float x, float y, float z, float r, float g, float b, float a) {
        Pos = XMFLOAT3(x, y, z);
        Color = XMFLOAT4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
};

struct SimpleIndex
{
    WORD pos0;
    WORD pos1;
    WORD pos2;
};

SimpleIndex aindex(unsigned int a, unsigned int b, unsigned int c) {
    SimpleIndex index = { a, b, c };
    return index;
}

struct ConstantBuffer
{
    XMMATRIX mWorld;
    XMMATRIX mView;
    XMMATRIX mProjection;
};

class rbuffer {
public:
    bool islocal = true;

    fmvecarr<SimpleVertex> arr;
    fmvecarr<SimpleIndex> indexes;
    
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    ID3D11VertexShader* m_pVertexShader = NULL;
    ID3D11PixelShader* m_pPixelShader = NULL;
    rbuffer() {

    }

    rbuffer(bool local) : islocal(local) {
        arr.Init(8, false, true);
        indexes.Init(8, false, true);
    }

    ~rbuffer() {
        if (islocal) {
            arr.release();
            indexes.release();
        }
    }

    void Init(bool local) {
        islocal = local;
        arr.Init(8, false, true);
        indexes.Init(8, false, true);
    }

    inline void FreePolygonToTriangles1() {
        fm->_tempPushLayer();
        fmvecarr<shp::vec3f> polygon;
        polygon.Init(arr.size(), true);
        polygon.up = arr.size();
        for (int i = 0; i < arr.size(); ++i) {
            polygon[i] = shp::vec3f(arr[i].Pos.x, arr[i].Pos.y, arr[i].Pos.z);
        }

        fmlist<uint> lt;
        lt.Init(0);
        fmlist_node<uint>* ltlast = lt.first;
        for (uint i = 1; i < arr.size(); ++i) {
            lt.push_front(i);
        }

        int savesiz = lt.size;
        while (lt.size >= 3 || savesiz != lt.size) {
            //ltlast->next = nullptr;
            savesiz = lt.size;
            fmlist_node<uint>* lti = lt.first;
            for (int index = 0; index < lt.size - 2; ++index) {
                //ltlast->next = nullptr;
                fmlist_node<uint>* inslti0 = lti;
                fmlist_node<uint>* inslti1 = lti->next;
                fmlist_node<uint>* inslti2 = inslti1->next;
                if (lti == nullptr) continue;
                bool bdraw = true;
				fmlist_node<uint>* ltk = lt.first;
				for (int kndex = 0; kndex < lt.size; ++kndex) {
                    //ltlast->next = nullptr;
                    bool b = false;
                    uint kv = ltk->value;
                    b = b || inslti0->value == kv;
                    b = b || inslti1->value == kv;
                    b = b || inslti2->value == kv;
					if (b) {
                        ltk = ltk->next;
						continue;
					}
					
					bdraw = bdraw && !shp::bPointInTriangleRange(shp::vec2f(arr[kv].Pos.x, arr[kv].Pos.y),
						shp::vec2f(arr[inslti0->value].Pos.x, arr[inslti0->value].Pos.y),
						shp::vec2f(arr[inslti1->value].Pos.x, arr[inslti1->value].Pos.y),
						shp::vec2f(arr[inslti2->value].Pos.x, arr[inslti2->value].Pos.y));

                    ltk = ltk->next;
				}

                if (bdraw == true) {
                    //fmlist_node<uint>* inslti = lti;
                    uint pi = inslti0->value;
                    uint pi1 = inslti1->value;
                    uint pi2 = inslti2->value;
                    
                    //SimpleVertex* gcenter = vec3f((pi.x + pi1.x + pi2.x) / 3, (pi.y + pi1.y + pi2.y) / 3, pi.z);
                    shp::triangle3v3f tri(shp::vec3f(arr[pi].Pos.x, arr[pi].Pos.y, arr[pi].Pos.z),
                        shp::vec3f(arr[pi1].Pos.x, arr[pi1].Pos.y, arr[pi1].Pos.z),
                        shp::vec3f(arr[pi2].Pos.x, arr[pi2].Pos.y, arr[pi2].Pos.z));
                    if (shp::bTriangleInPolygonRange(tri, polygon) || lt.size <= 4) {
                        indexes.push_back(aindex(pi, pi1, pi2));
                        lt.erase(inslti1);
                        lti = inslti2;
                        //여기에 도달하기 전에 lt의 first의 nest가 nullptr에서 쓰레기 값으로 덮어진다. 원인을 찾자
                    }
                }

                lti = lti->next;
            }
        }

        fm->_tempPopLayer();
    }

    inline void FreePolygonToTriangles2() {
        fm->_tempPushLayer();
        fmvecarr<shp::vec2f> polygon;
        fmvecarr<uint> polygon_index;
        fmvecarr<shp::vec3f> save;
        polygon.Init(arr.size(), true);
        polygon.up = arr.size();
        polygon_index.Init(arr.size(), true);
        polygon_index.up = arr.size();
        save.Init(arr.size(), true);
        save.up = arr.size();
        for (int i = 0; i < arr.size(); ++i) {
            polygon[i] = shp::vec2f(arr[i].Pos.x, arr[i].Pos.y);
            polygon_index[i] = i;
            save[i] = shp::vec3f(arr[i].Pos.x, arr[i].Pos.y, -1);
        }
        int siz = polygon.size();
        while (polygon.size() >= 3) {
            for (int i = 0; i < polygon.size() - 2; i++) {
                bool bdraw = true;
                for (int k = 0; k < polygon.size(); k++) {
                    if (i <= k && k <= i + 2) continue;

                    if (bPointInTriangleRange(polygon.at(k), polygon.at(i), polygon.at(i + 1), polygon.at(i + 2))) {
                        bdraw = false;
                        break;
                    }
                }

                if (bdraw == true) {
                    shp::vec2f gcenter = shp::vec2f((polygon.at(i).x + polygon.at(i + 1).x + polygon.at(i + 2).x) / 3, (polygon.at(i).y + polygon.at(i + 1).y + polygon.at(i + 2).y) / 3);
                    shp::triangle3v3f tri(shp::vec3f(polygon[i].x, polygon[i].y, 0),
                        shp::vec3f(polygon[i+1].x, polygon[i+1].y, 0),
                        shp::vec3f(polygon[i+2].x, polygon[i+2].y, 0));
                    if (shp::bPolyTriangleInPolygonRange(tri, save)) {
                        indexes.push_back(aindex(polygon_index[i], polygon_index[i+1], polygon_index[i+2]));
                        polygon.erase(i + 1);
                        polygon_index.erase(i + 1);
                    }
                }
            }

            if (siz == polygon.size()) {
                break;
            }
            else {
                siz = polygon.size();
            }
        }
        fm->_tempPopLayer();
    }

    void begin(ID3D11VertexShader* vshader, ID3D11PixelShader* fshader) {
        m_pVertexShader = vshader;
        m_pPixelShader = fshader;
    }

    inline void av(const SimpleVertex& vertex) {
        arr.push_back(vertex);
    }

    HRESULT end() {
        indexes.up = 0;
        if (m_pVertexBuffer != nullptr) {
            while (m_pVertexBuffer->Release());
            m_pVertexBuffer = nullptr;
        }

        if (m_pIndexBuffer != nullptr) {
            while (m_pIndexBuffer->Release());
            m_pIndexBuffer = nullptr;
        }

        FreePolygonToTriangles1();

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * arr.up;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = arr.Arr;
        HRESULT hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
        if (FAILED(hr))
            return hr;

        // Create index buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 3 * indexes.up;       // 36 vertices needed for 12 triangles in a triangle list
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = indexes.Arr;
        hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
        if (FAILED(hr))
            return hr;
    }

    void render(const ConstantBuffer& uniform) {
        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        g_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

        // Set index buffer
        g_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &uniform, 0, 0);

        // Render
        g_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
        g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
        g_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
        UINT IndexCount = 3 * indexes.size();
        g_pImmediateContext->DrawIndexed(IndexCount, 0, 0);
    }
};

struct ShapeObject {
    rbuffer* origin_shape;
    ConstantBuffer uniform;
};

