#pragma once
#include "pch.h"
#define toScreen(T) ((T)*scale)
Vector2 TransFromStageToScreen(float x, float y);
Vector2 TransFromStageToScreen(Vector2 pt);


struct Vertex
{
	float x, y, z, rhw;
	DWORD color;
};
const DWORD ShapeVertex = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);


class ShapeRectangle
{
public:
	float x, y;//normally the center of the shape
	float width, height;
	DWORD centerType;
	Color cBorder;
	Color cContent;
	DWORD fillType;
	static const DWORD fillBorder = 0x1;
	static const DWORD fillContent = 0x2;
	bool is_in_stage;
	ShapeRectangle(float ix, float iy, float w, float h, DWORD cen = DT_CENTER) :x(ix), y(iy), width(w), height(h), centerType(cen)
	{
		cBorder = cContent = D3DCOLOR_RGBA(255, 0, 0, 255); fillType = fillBorder; is_in_stage = true;
	};
	void Draw();
};