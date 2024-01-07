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
TTFFontParser::FontData* sub_font_data[10];
unsigned int max_sub_font = 1;

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

bool VertexEqual(SimpleVertex& a, SimpleVertex& b) {
    __m128i* ka = reinterpret_cast<__m128i*>(&a);
    __m128i* kb = reinterpret_cast<__m128i*>(&b);
    __m128i* ka2 = reinterpret_cast<__m128i*>(((byte8*)&a) + 3);
    __m128i* kb2 = reinterpret_cast<__m128i*>(((byte8*)&b) + 3);
    __m128i vcmp = _mm_cmpeq_epi32(*ka, *kb);
    __m128i vcmp2 = _mm_cmpeq_epi32(*ka2, *kb2);
    uint16_t mask = _mm_movemask_epi8(vcmp);
    uint16_t mask2 = _mm_movemask_epi8(vcmp2);
    if (mask != 0xffff || mask2 != 0xffff) {
        return false;
    }
    return true;
}

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

    fmvecarr<SimpleVertex> arr[2] = {};
    fmvecarr<SimpleIndex> indexes[2] = {};
    
    ID3D11Buffer* m_pVertexBuffer[2] = {};
    ID3D11Buffer* m_pIndexBuffer[2] = {};
    unsigned int choice = 1; // double buffering required

    ID3D11VertexShader* m_pVertexShader = NULL;
    ID3D11PixelShader* m_pPixelShader = NULL;

    static XMFLOAT4 InputColor;

    static void dxColor4f(float r, float g, float b, float a) {
        *reinterpret_cast<__m128i*>(&InputColor) = *reinterpret_cast<__m128i*>(&r);
    }

    rbuffer() {

    }

    rbuffer(bool local) : islocal(local) {
        arr[0].Init(8, false, true);
        arr[1].Init(8, false, true);
        indexes[0].Init(8, false, true);
        indexes[1].Init(8, false, true);
    }

    ~rbuffer() {
        if (islocal) {
            arr[0].release();
            arr[1].release();
            indexes[0].release();
            indexes[1].release();
        }
    }

    void Init(bool local) {
        arr[0].NULLState();
        indexes[0].NULLState();
        arr[1].NULLState();
        indexes[1].NULLState();
        islocal = local;
        arr[0].Init(8, false, true);
        indexes[0].Init(8, false, true);
        arr[1].Init(8, false, true);
        indexes[1].Init(8, false, true);
        m_pVertexBuffer[0] = nullptr;
        m_pIndexBuffer[0] = nullptr;
        m_pVertexBuffer[1] = nullptr;
        m_pIndexBuffer[1] = nullptr;
        choice = 1;
    }

    inline void FreePolygonToTriangles() {
        int nextchoice = (choice + 1) % 2;
        fm->_tempPushLayer();
        fmvecarr<shp::vec3f> polygon;
        polygon.Init(arr[nextchoice].size(), true);
        polygon.up = arr[nextchoice].size();
        for (int i = 0; i < arr[nextchoice].size(); ++i) {
            polygon[i] = shp::vec3f(arr[nextchoice][i].Pos.x, arr[nextchoice][i].Pos.y, arr[nextchoice][i].Pos.z);
        }

        fmlist<uint> lt;
        lt.Init(0);
        fmlist_node<uint>* ltlast = lt.first;
        for (uint i = 1; i < arr[nextchoice].size(); ++i) {
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
					
					bdraw = bdraw && !shp::bPointInTriangleRange(shp::vec2f(arr[nextchoice][kv].Pos.x, arr[nextchoice][kv].Pos.y),
						shp::vec2f(arr[nextchoice][inslti0->value].Pos.x, arr[nextchoice][inslti0->value].Pos.y),
						shp::vec2f(arr[nextchoice][inslti1->value].Pos.x, arr[nextchoice][inslti1->value].Pos.y),
						shp::vec2f(arr[nextchoice][inslti2->value].Pos.x, arr[nextchoice][inslti2->value].Pos.y));

                    ltk = ltk->next;
				}

                if (bdraw == true) {
                    //fmlist_node<uint>* inslti = lti;
                    uint pi = inslti0->value;
                    uint pi1 = inslti1->value;
                    uint pi2 = inslti2->value;
                    
                    //SimpleVertex* gcenter = vec3f((pi.x + pi1.x + pi2.x) / 3, (pi.y + pi1.y + pi2.y) / 3, pi.z);
                    shp::triangle3v3f tri(shp::vec3f(arr[nextchoice][pi].Pos.x, arr[nextchoice][pi].Pos.y, arr[nextchoice][pi].Pos.z),
                        shp::vec3f(arr[nextchoice][pi1].Pos.x, arr[nextchoice][pi1].Pos.y, arr[nextchoice][pi1].Pos.z),
                        shp::vec3f(arr[nextchoice][pi2].Pos.x, arr[nextchoice][pi2].Pos.y, arr[nextchoice][pi2].Pos.z));
                    if (shp::bTriangleInPolygonRange(tri, polygon) || lt.size <= 4) {
                        indexes[nextchoice].push_back(aindex(pi, pi1, pi2));
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

        int nextchoice = (choice + 1) % 2;

        if (m_pVertexBuffer[nextchoice] != nullptr) {
            while (m_pVertexBuffer[nextchoice]->Release());
            m_pVertexBuffer[nextchoice] = nullptr;
        }

        if (m_pIndexBuffer[nextchoice] != nullptr) {
            while (m_pIndexBuffer[nextchoice]->Release());
            m_pIndexBuffer[nextchoice] = nullptr;
        }
    }

    inline void av(const SimpleVertex& vertex) {
        int nextchoice = (choice + 1) % 2;
        arr[nextchoice].push_back(vertex);
    }

    HRESULT end() {
        int nextchoice = (choice + 1) % 2;

        if (arr[nextchoice].size() != arr[choice].size()) goto RBUFFER_END_NEWPOLY;
        for (int i = 0; i < arr[choice].size(); ++i) {
            if (!VertexEqual(arr[nextchoice][i], arr[choice][i])) {
                goto RBUFFER_END_NEWPOLY;
            }
        }

        return 0;

        RBUFFER_END_NEWPOLY:

        indexes[nextchoice].up = 0;
        if (m_pVertexBuffer[nextchoice] != nullptr) {
            while (m_pVertexBuffer[nextchoice]->Release());
            m_pVertexBuffer[nextchoice] = nullptr;
        }

        if (m_pIndexBuffer[nextchoice] != nullptr) {
            while (m_pIndexBuffer[nextchoice]->Release());
            m_pIndexBuffer[nextchoice] = nullptr;
        }

        FreePolygonToTriangles();

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * arr[nextchoice].up;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = arr[nextchoice].Arr;
        HRESULT hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &(m_pVertexBuffer[nextchoice]));
        if (FAILED(hr))
            return hr;

        // Create index buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 3 * indexes[nextchoice].up;       // 36 vertices needed for 12 triangles in a triangle list
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = indexes[nextchoice].Arr;
        hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &(m_pIndexBuffer[nextchoice]));
        if (FAILED(hr))
            return hr;

        choice = nextchoice;
    }

    void render(const ConstantBuffer& uniform) {
        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        g_pImmediateContext->IASetVertexBuffers(0, 1, &(m_pVertexBuffer[choice]), &stride, &offset);

        // Set index buffer
        g_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer[choice], DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &uniform, 0, 0);

        // Render
        g_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
        g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
        g_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
        UINT IndexCount = 3 * indexes[choice].size();
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
    shp::rect4f range;

    CharBuffer()
    {
    }
    ~CharBuffer()
    {
    }

    void ready(unsigned int unicode, ID3D11VertexShader* vshader, ID3D11PixelShader* fshader)
    {
        //func_in("CharBuffer::ready");
        Glyph g;
        int fontindex = -1;
        unordered_map<uint32_t, TTFFontParser::Glyph>* glyphmap = &font_data.glyphs;
        while (glyphmap->find(unicode) == glyphmap->end()) {
            ++fontindex;
            glyphmap = &sub_font_data[fontindex]->glyphs;
            if (fontindex >= max_sub_font) {
                return;
            }
        }

        g = glyphmap->at(unicode);

        frag.NULLState();
        frag.Init(g.path_list.size(), false, true);

        // drt->begin(rendertype::color_nouv);
        Curve c;
        if (g.path_list.size() > 0 && g.path_list.at(0).geometry.size() > 0) {
            c = g.path_list.at(0).geometry.at(0);
        }
        else {
            range = shp::rect4f(0, 0, 500, 500);
            return;
        }
        
        range.fx = c.p0.x;
        range.lx = c.p0.x;
        range.fy = c.p0.y;
        range.ly = c.p0.y;

        for (int i = g.path_list.size() - 1; i >= 0; --i)
        {
            rbuffer* rbuff = (rbuffer*)fm->_New(sizeof(rbuffer), true);
            rbuff->Init(false);
            rbuff->begin(vshader, fshader);
            for (int k = g.path_list.at(i).geometry.size() - 1; k >= 0; --k)
            {
                c = g.path_list.at(i).geometry.at(k);
                rbuff->av(SimpleVertex(c.p0.x, c.p0.y, 0, 255, 255, 255, 255));
                if (range.fx > c.p0.x) range.fx = c.p0.x;
                if (range.lx < c.p0.x) range.lx = c.p0.x;
                if (range.fy > c.p0.y) range.fy = c.p0.y;
                if (range.ly < c.p0.y) range.ly = c.p0.y;
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

    unsigned int c = (unsigned int)wstr[0];
    if (char_map.find((unsigned int)c) == char_map.end())
    {
        CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
        cbuf->ready((unsigned int)c, vshader, fshader);
        char_map.insert(CharMap::value_type((unsigned int)c, cbuf));
    }

    unsigned int nextC = c;
    for (int i = 0; i < len; ++i)
    {
        c = nextC;
        if (len > i + 1) {
             nextC = (unsigned int)wstr[i + 1];
        }

        if (c == 0) return;
        if (nextC != 0 && char_map.find(nextC) == char_map.end())
        {
            CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
            cbuf->ready(nextC, vshader, fshader);
            char_map.insert(CharMap::value_type(nextC, cbuf));
        }

        StringWorld = XMMatrixScaling(fsiz, fsiz, 1);
        StringWorld = XMMatrixMultiply(StringWorld, XMMatrixTranslation(stackpos.x, stackpos.y, 0.9f));
        cb.mWorld = XMMatrixTranspose(StringWorld);
        char_map.at(c)->render(cb);

        if (nextC != 0) {
            stackpos.x += (fsiz) * (float)(char_map.at(c)->range.lx + char_map.at(nextC)->range.fx);
        }
        else {
            break;
        }
        
        //if (c < 255) {
        //    if (('A' <= c && c <= 'Z')) {
        //        stackpos.x += 1000.0f * fsiz;
        //    }
        //    else {
        //        stackpos.x += 500.0f * fsiz;
        //    }
        //}
        //else {
        //    stackpos.x += 1000.0f * fsiz;
        //}
    }
}

struct string_check {
    shp::rect4f loc;
    int index;
};

string_check check_string(wchar_t* wstr, size_t len, float fontsiz, shp::rect4f loc,
    ID3D11VertexShader* vshader, ID3D11PixelShader* fshader, shp::vec2f checkpos) {
    string_check sc;
    sc.index = 0;
    sc.loc = shp::rect4f(0, 0, 0, 0);
    //dbg << "insert start" << endl;
    float fsiz = (float)fontsiz / 500.0f;
    shp::rect4f stackrange = shp::rect4f(loc.fx, 0, loc.fy, 0);

    unsigned int c = (unsigned int)wstr[0];
    if (char_map.find((unsigned int)c) == char_map.end())
    {
        CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
        cbuf->ready((unsigned int)c, vshader, fshader);
        char_map.insert(CharMap::value_type((unsigned int)c, cbuf));
    }

    unsigned int nextC = c;
    for (int i = 0; i < len; ++i)
    {
        c = nextC;
        if (len > i + 1) {
            nextC = (unsigned int)wstr[i + 1];
        }

        if (c == 0) return sc;
        if (nextC != 0 && char_map.find(nextC) == char_map.end())
        {
            CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
            cbuf->ready(nextC, vshader, fshader);
            char_map.insert(CharMap::value_type(nextC, cbuf));
        }

        if (nextC != 0) {
            stackrange.fx += (fsiz) * (float)(char_map.at(c)->range.lx + char_map.at(nextC)->range.fx);
            if (shp::bPointInRectRange(checkpos, stackrange)) {
                sc.index = i;
                sc.loc = stackrange;
                return sc;
            }
        }
        else {
            sc.index = i;
            sc.loc = stackrange;
            return sc;
        }
    }
}

struct ShapeObject {
    rbuffer* origin_shape;
    ConstantBuffer uniform;
};

