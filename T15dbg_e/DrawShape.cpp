
#include "pch.h"
#include "DrawShape.h"
extern IDirect3DDevice9* device;
extern float windowWidth;
extern float scale;

Vector2 TransFromStageToScreen(Vector2 pt)
{
	Vector2 ret((pt.x + 192 + 32) * scale, (pt.y + 16) * scale);
	return ret;
}

Vector2 TransFromStageToScreen(float x, float y)
{
	Vector2 ret((x + 192 + 32) * scale, (y + 16) * scale);
	return ret;
}

void TransFromStageToScreenS(Vector2& pt)
{
	pt.x = (pt.x + 192 + 32) * scale;
	pt.y = (pt.y + 16) * scale;
	return;
}

void GetPosition(DWORD center, float x, float y, float w, float h, float& left, float& top)
{
	if (center == DT_CENTER)
	{
		left = x - w * 0.5f; top = y - h * 0.5f; return;
	}
	left = x; top = y;
	if (center & DT_RIGHT)
		left = x - w;
	if (center & DT_BOTTOM)
		top = y - h;
	return;
}

void ShapeRectangle::Draw()
{
	float l, t;
	GetPosition(centerType, x, y, width, height, l, t);
	Vector2 plt;
	Vector2 prb;
	if (is_in_stage)
	{
		plt = TransFromStageToScreen(l, t);
		prb = TransFromStageToScreen(l + width, t + height);
	}
	else {
		plt = Vector2(toScreen(l), toScreen(t));
		prb = Vector2(toScreen(l + width), toScreen(t + height));
	}

	DWORD pre1;
	device->GetRenderState(D3DRS_CULLMODE, &pre1);
	if (this->fillType & fillContent)
	{
		Vertex vertices[] = {
			{plt.x,plt.y,0,1,this->cContent},
			{prb.x,plt.y,0,1,this->cContent},
			{plt.x,prb.y,0,1,this->cContent},
			{prb.x,prb.y,0,1,this->cContent},
		};
		Vertex* pVertices;
		LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
		device->CreateVertexBuffer(4 * sizeof(Vertex), 0, ShapeVertex, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL);
		g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0);
		memcpy(pVertices, vertices, sizeof(vertices));
		g_pVertexBuffer->Unlock();
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);//render all the plane
		device->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex));
		device->SetFVF(ShapeVertex);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		g_pVertexBuffer->Release();
	}
	if (this->fillType & fillBorder) {
		Vertex vertices[] = {
			{plt.x,plt.y,0,1,this->cBorder},
			{prb.x,plt.y,0,1,this->cBorder},
			{prb.x,prb.y,0,1,this->cBorder},
			{plt.x,prb.y,0,1,this->cBorder},
			{plt.x,plt.y,0,1,this->cBorder},
		};
		Vertex* pVertices;
		LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
		device->CreateVertexBuffer(4 * sizeof(Vertex), 0, ShapeVertex, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL);
		g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0);
		memcpy(pVertices, vertices, sizeof(vertices));
		g_pVertexBuffer->Unlock();
		device->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex));
		device->SetFVF(ShapeVertex);
		device->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
		g_pVertexBuffer->Release();
	}
	device->SetRenderState(D3DRS_CULLMODE, pre1);
}

Vector2 GetMousePosition()
{

	return Vector2(0, 0);
}