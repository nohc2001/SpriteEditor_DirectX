#pragma once
#define _CORRECT_ISO_CPP11_MATH_H_PROTO
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <unordered_map>
#include "Utill_FreeMemory.h"
#include "Utill_SpaceMath.h"

#define TTF_FONT_PARSER_IMPLEMENTATION
#include "ttfParser.h"

using namespace std;
using namespace freemem;
using namespace TTFFontParser;

#define uint unsigned int

ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
ID3D11Buffer* g_pConstantBuffer = NULL;

TTFFontParser::FontData font_data;

extern XMMATRIX                g_View;
extern XMMATRIX                g_Projection_2d;

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
        arr.NULLState();
        indexes.NULLState();
        islocal = local;
        arr.Init(8, false, true);
        indexes.Init(8, false, true);
        m_pVertexBuffer = nullptr;
        m_pIndexBuffer = nullptr;
    }

    inline void FreePolygonToTriangles() {
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

        int savesiz = 0;
        while (lt.size >= 3) {
            if (savesiz == lt.size) break;
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
                        //lti = inslti2;
                        //여기에 도달하기 전에 lt의 first의 nest가 nullptr에서 쓰레기 값으로 덮어진다. 원인을 찾자
                    }
                }

                lti = lti->next;
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

        FreePolygonToTriangles();

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

void font_parsed(void* args, void* _font_data, int error)
{
    if (error)
    {
        *(uint8_t*)args = error;
    }
    else
    {
        TTFFontParser::FontData* font_data = (TTFFontParser::FontData*)_font_data;

        {
            for (const auto& glyph_iterator : font_data->glyphs)
            {
                uint32_t num_curves = 0, num_lines = 0;
                for (const auto& path_list : glyph_iterator.second.path_list)
                {
                    for (const auto& geometry : path_list.geometry)
                    {
                        if (geometry.is_curve)
                            num_curves++;
                        else
                            num_lines++;
                    }
                }
            }
        }

        *(uint8_t*)args = 1;
    }
}

class CharBuffer
{
public:
    fmvecarr < rbuffer* >frag;
    shp::vec2f xrange;

    CharBuffer()
    {
    }
    ~CharBuffer()
    {
    }

    void ready(unsigned int unicode, ID3D11VertexShader* vshader, ID3D11PixelShader* fshader)
    {
        //func_in("CharBuffer::ready");
        Glyph g = font_data.glyphs.at(unicode);
        frag.NULLState();
        frag.Init(g.path_list.size(), false, true);
        // drt->begin(rendertype::color_nouv);
        
        for (int i = g.path_list.size() - 1; i >= 0; --i)
        {
            Curve c = g.path_list.at(0).geometry.at(0);
            xrange.x = c.p0.x;
            xrange.y = c.p0.x;

            rbuffer* rbuff = (rbuffer*)fm->_New(sizeof(rbuffer), true);
            rbuff->Init(false);
            rbuff->begin(vshader, fshader);
            for (int k = g.path_list.at(i).geometry.size() - 1; k >= 0; --k)
            {
                c = g.path_list.at(i).geometry.at(k);
                rbuff->av(SimpleVertex(c.p0.x, c.p0.y, 0, 255, 255, 255, 255));
                if (xrange.x > c.p0.x) xrange.x = c.p0.x;
                if (xrange.y < c.p0.y) xrange.y = c.p0.y;
            }
            rbuff->end();
            frag.push_back(rbuff);
        }
        //func_out;
    }

    void render(const ConstantBuffer& uniform, bool debuging = false)
    {
        for (int i = 0; i < frag.size(); ++i)
        {
            frag[i]->render(uniform);
        }
    }
};

typedef unordered_map < uint32_t, CharBuffer* >CharMap;
CharMap char_map;
void draw_string(wchar_t* wstr, size_t len, float fontsiz, shp::rect4f loc, 
    ID3D11VertexShader* vshader, ID3D11PixelShader* fshader)
{
    XMMATRIX StringWorld = XMMatrixIdentity();
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(StringWorld);
    cb.mView = XMMatrixTranspose(g_View);
    cb.mProjection = XMMatrixTranspose(g_Projection_2d);
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);

    //dbg << "insert start" << endl;
    float fsiz = (float)fontsiz/500.0f;
    shp::vec2f stackpos = shp::vec2f(loc.fx, loc.fy);
    for (int i = 0; i < len; ++i)
    {
        unsigned int c = (unsigned int)wstr[i];
        if (c == 0) return;
        if (char_map.find(c) == char_map.end())
        {
            //dbg << "insert" << endl;
            CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
            //dbg << "bpbpb" << endl;
            cbuf->ready(c, vshader, fshader);
            char_map.insert(CharMap::value_type(c, cbuf));
            // dbg << "insertend" << endl;
        }

        StringWorld = XMMatrixScaling(fsiz, fsiz, 1);
        StringWorld = XMMatrixMultiply(StringWorld, XMMatrixTranslation(stackpos.x, stackpos.y, 0.9f));
        cb.mWorld = XMMatrixTranspose(StringWorld);
        char_map.at(c)->render(cb);

        if (c < 255) {
            stackpos.x += 450.0f * fsiz;
        }
        else {
            stackpos.x += 900.0f * fsiz;
        }
    }
}

struct ShapeObject {
    rbuffer* origin_shape;
    ConstantBuffer uniform;
};

