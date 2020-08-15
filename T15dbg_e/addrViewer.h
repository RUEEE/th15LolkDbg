#pragma once
#include "pch.h"
#include "imguiClass.h"
#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <variant>
#include <list>

void AddrViewer(bool* p_open);
class GraphA;

class Address
{
private:
	std::string valueStr;
	unsigned int ID;
	static unsigned int cnt;//warning copy/assign can't work correctly
public:
	static const char* typeStr[];
	int count;
	DWORD base;
	bool is_bad_addr;
	int bad_count;
	std::vector<DWORD> offs;
	std::vector<DWORD> offVal;
	enum Addr_Type{AD_BYTE=0,AD_WORD=1,AD_DWORD=2,AD_QWORD=3,AD_FLOAT=4,AD_DOUBLE=5,AD_STRING=6
	,AD_VECTOR2=7,AD_VECTOR3=8,AD_VECTOR4=9};
	std::string description;
	Addr_Type type;
	int typeAdd;//string's length and is_hex
	const char* GetDescriptionWider();
	void GetPointer();
	void GetValueUnSafe(void* val);
	void GetValueStr(char* str, int bufsz);
	const char* GetValueStrUnSafe(){return valueStr.c_str();}
	Address(int c);
	void SetBuffer(int i);
	void GetID(){cnt++; ID=cnt;}//to copy/assign
};

class Graph_PointY
{
private:
	void PushPoint(float x);
	void DrawCircle(float x,float y,DWORD col,ImDrawList* dw, GraphA* graph);
	void DrawPoints_Tip(ImDrawList* drawList, GraphA* graph);//use graph to access things like scale
	void DrawPoints_Tip_Single(float x1, float y1, float x2, float y2, ImDrawList* drawList, GraphA* graph, DWORD color);
public:
	Address yAddr;
	DWORD color[4];//3 num used only when addr's type is vector
	int length;
	std::vector<float> ptX;//save the pointX
	std::vector<float> ptY;
	//when yAddr is vec,it will save like{y1,y2,y3,y4,,y1,y2,y3,y4}
	int ptBegin;//save the begin of the pt(ptX)
	friend class GraphA;
	Graph_PointY(Address addr):yAddr(addr),length(256){
	color[0]=IM_COL32(243,172,209,255);
	color[1]=IM_COL32(172,172,243,255);
	color[2]=IM_COL32(172,243,172,255);
	color[3]=IM_COL32(243,197,172,255);
	}
};

class GraphA
{
public:
	static std::list<GraphA> graphs;

	ImVec2 translationSrc;//the scrolling of the source rect
	ImVec2 scaleSrc;
	//ImVec2 canvas_p0,canvasSz; use when draw
	ImVec2 translationDst;//the scrolling of the point in Dst rect,usually =min( canvasSz.x * 5% , canvasSz.y * 5%)
	ImVec2 scrollingDst;//change when drag in the canvas

	ImVec2 minPoint,maxPoint;//to calculate scale/translationSrc

	bool is_grid_open;
	bool is_auto_resize;
	bool is_y_reversed;
	bool is_tipped;//true when tipbar visible

	int rate;//the draw frame to pass before update
	int drawCount;//use with delay
	std::string nameWithID;

	std::list<std::pair<Address,std::list<Graph_PointY>>> points;//x1{y1,y2,y3},x2{y1,y2,y3},...
		//x can be byte,short,int,float,double
		//y can be byte,short,int,float,double,vec2,vec3,vec4
	GraphA(std::string nameA):translationSrc(ImVec2(0,0)),scaleSrc(ImVec2(1,1)),translationDst(ImVec2(0,0)),scrollingDst(ImVec2(0,0)),
		minPoint(ImVec2(FLT_MAX,FLT_MAX)),maxPoint(ImVec2(-FLT_MAX,-FLT_MAX)),is_grid_open(true),is_auto_resize(true),
		is_y_reversed(true),is_tipped(false),rate(0),drawCount(0),nameWithID(nameA){};
	void Update();
	void AutoResize(ImVec2 cv0,ImVec2 cvsz);//calculate the translationDst,scaleSrc,translationSrc
	void Draw(ImDrawList* drawList,ImVec2 cv0,ImVec2 cvsz);//also draw the grid(if open)
	void Clear(){
	this->drawCount=0;for(auto &a:points){for(auto &b:a.second){b.ptBegin=0;b.ptX.clear();b.ptY.clear();}}
	scaleSrc = ImVec2(1, 1);scrollingDst = ImVec2(0, 0);translationDst = ImVec2(0, 0);translationSrc = ImVec2(0, 0);}
};
