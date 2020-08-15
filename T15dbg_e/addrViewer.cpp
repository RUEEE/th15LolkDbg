#include "pch.h"
#include "addrViewer.h"
#include "DrawShape.h"


extern float scale;
extern HANDLE current_process;
using namespace ImGui;

const char* Address::typeStr[] = { "byte","2 bytes","4 bytes","8 bytes","float","double","string","vec2","vec3","vec4" };

const char* Address::GetDescriptionWider()
{
	static char ch[300];
	sprintf_s(ch,300,"%d: %s###A~%x",count,description.c_str(),ID);
	return ch;
}

void Address::GetPointer()
{
	is_bad_addr=false;
	void* ptr;
	if (GetPtFinalSafe(this->base, this->offs, this->offVal))
	{
	ptr=(void*)offVal[offVal.size()-1];
	SIZE_T g;
	char testBuffer[255];
	#define TEST_ADDR(x) is_bad_addr=!ReadProcessMemory(current_process, (LPCVOID)ptr, testBuffer, sizeof(x), &g)
		switch (this->type)
		{
		case AD_BYTE:TEST_ADDR(BYTE); return;
		case AD_WORD:TEST_ADDR(WORD); return;
		case AD_DWORD:TEST_ADDR(DWORD); return;
		case AD_QWORD:TEST_ADDR(unsigned long long); return;
		case AD_FLOAT:TEST_ADDR(float); return;
		case AD_DOUBLE:TEST_ADDR(double); return;
		case AD_STRING: is_bad_addr = !ReadProcessMemory(current_process, (LPCVOID)ptr, testBuffer,typeAdd, &g);
		case AD_VECTOR2:TEST_ADDR(ImVec2); return;
		case AD_VECTOR3:TEST_ADDR(D3DXVECTOR3); return;
		case AD_VECTOR4:TEST_ADDR(ImVec4); return;
		}
	#undef TEST_ADDR(x) is_bad_addr=IsBadReadPtr((void*)ptr,sizeof(x))
	}
	is_bad_addr = true;
	bad_count = offVal.size();
	int n= (offs.size() - offVal.size() + 1);
	for(int i=0;i<n;i++)
		offVal.push_back(0);
	return;
}

void Address::GetValueUnSafe(void* val)//should call GetPointer() first
{
	//GetPointer();
	if(this->is_bad_addr)return;
	#define SET_VALUE(T,d,s) *(T*)d=*(T*)s
	void* pt=(void*)this->offVal[offVal.size() - 1];
	switch (this->type)
	{
		case AD_BYTE:SET_VALUE(BYTE,val,pt);return;
		case AD_WORD:SET_VALUE(WORD,val,pt);return;
		case AD_DWORD:SET_VALUE(DWORD,val,pt);return;
		case AD_QWORD:SET_VALUE(unsigned long long,val,pt);return;
		case AD_FLOAT:SET_VALUE(float,val,pt);return;
		case AD_DOUBLE:SET_VALUE(double,val,pt);return;
		case AD_STRING:
			memcpy(val,pt,this->typeAdd*sizeof(BYTE)); return;
		case AD_VECTOR2:SET_VALUE(ImVec2,val,pt); return;
		case AD_VECTOR3:SET_VALUE(D3DXVECTOR3,val,pt); return;
		case AD_VECTOR4:SET_VALUE(ImVec4,val,pt); return;
	}
	#undef SET_VALUE(T,d,s) *(T*)d=*(T*)s
}

void Address::GetValueStr(char* str,int bufsz=255)
{
#define TA if (this->is_bad_addr){strcpy_s(str,bufsz,"badAddr");break;}
	switch (this->type)
	{
	case AD_BYTE:  	{BYTE b;GetValueUnSafe(&b); TA	sprintf_s(str,bufsz,typeAdd?"%#02hhx":"%hhd",b);	break;			 }
	case AD_WORD:  	{WORD b;GetValueUnSafe(&b); TA	sprintf_s(str,bufsz,typeAdd?"%#04hx":"%hd",b);	break;				 }
	case AD_DWORD: 	{DWORD b;GetValueUnSafe(&b); TA	sprintf_s(str,bufsz,typeAdd?"%#08X":"%d",b);	break;					 }
	case AD_QWORD: 	{unsigned long long b;GetValueUnSafe(&b);TA	sprintf_s(str,bufsz,typeAdd?"%#016llX":"%lld",b);	break;	 }
																																	 
	case AD_FLOAT: 	{float b;GetValueUnSafe(&b); TA	sprintf_s(str,bufsz,"%g",b);	break;						 }
	case AD_DOUBLE:	{double b;GetValueUnSafe(&b); TA	sprintf_s(str,bufsz,"%lf",b);	break;					 }
	case AD_STRING:	{GetValueUnSafe(str); TA			break;					 }
	case AD_VECTOR2:{D3DXVECTOR2 b;GetValueUnSafe(&b);TA	sprintf_s(str,bufsz,"(%6.3g,%6.3g)",b.x,b.y);	break;	     }
	case AD_VECTOR3:{D3DXVECTOR3 b;GetValueUnSafe(&b);TA	sprintf_s(str,bufsz,"(%6.3g,%6.3g,%6.3g)",b.x,b.y,b.z);	break;	 }
	case AD_VECTOR4:{D3DXVECTOR4 b;GetValueUnSafe(&b);TA	sprintf_s(str,bufsz,"(%6.3g,%6.3g,%6.3g,%6.3g)",b.x,b.y,b.z,b.w);	break;	 }
	}
#undef TA if (this->is_bad_addr){str[0]=' ',str[1]='\0';break;}
	valueStr=std::string(str);
}

unsigned int Address::cnt = 0;
Address::Address(int c=0):base(0), is_bad_addr(1), offs(), offVal(), type(AD_DWORD), typeAdd(0)
{
	count=c;
	description="no description";
	cnt++;
	ID=cnt;
}

void Address:: SetBuffer(int i)
{
	if (i >= 256)i = 255;
	if (i <= 0)i = 1;
	typeAdd = i;
}

void Graph_PointY::PushPoint(float x)
{
	bool is_x_filled=false;;
	this->yAddr.GetPointer();
	if (!yAddr.is_bad_addr)
	{
		if (ptX.size() < this->length)
		{
			this->ptX.push_back(x);
		}else {
			ptX[ptBegin] = x;//delete the first point
			is_x_filled = true;
			//the ptBegin's adjust is in Y
		}
		float f1;
		#define GET_Y(T) T Y;yAddr.GetValueUnSafe(&Y);f1=(float)Y;goto SET_Y;
		switch (yAddr.type)
		{
		case Address::AD_BYTE:	 	{GET_Y(BYTE)}
		case Address::AD_WORD:	 	{GET_Y(WORD)}
		case Address::AD_DWORD:	 	{GET_Y(int)}
		case Address::AD_QWORD:	 	{GET_Y(long long int)}
		case Address::AD_FLOAT:	 	{GET_Y(float)}
		case Address::AD_DOUBLE: 	{GET_Y(double)}
		SET_Y:
			 if (!is_x_filled)
			 {ptY.push_back(f1);}
			 else{
				ptY[ptBegin]=f1;//delete the first point
				ptBegin++;
				if(ptBegin>=ptX.size())ptBegin=0;
			 }break;
		case Address::AD_VECTOR2:	{
			D3DXVECTOR2 v; yAddr.GetValueUnSafe(&v);
			if (!is_x_filled)
				{ptY.push_back(v.x); ptY.push_back(v.y);}
			else {
					ptY[ptBegin*2] = v.x;//delete the first point
					ptY[ptBegin*2+1]=v.y;
					ptBegin++;
					if (ptBegin >= ptX.size())ptBegin = 0;
			}break;
		}
		case Address::AD_VECTOR3:	{
			D3DXVECTOR3 v; yAddr.GetValueUnSafe(&v);
			if (!is_x_filled)
			{ptY.push_back(v.x); ptY.push_back(v.y); ptY.push_back(v.z);}else {
				ptY[ptBegin * 3] = v.x;//delete the first point
				ptY[ptBegin * 3 + 1] = v.y;
				ptY[ptBegin * 3 + 2] = v.z;
				ptBegin++;
				if (ptBegin >= ptX.size())ptBegin = 0;
				}break;
			}
		case Address::AD_VECTOR4: {
			D3DXVECTOR4 v; yAddr.GetValueUnSafe(&v);
				if (!is_x_filled)
				{
					ptY.push_back(v.x); ptY.push_back(v.y); ptY.push_back(v.z); ptY.push_back(v.w);
				}
				else {
					ptY[ptBegin * 4] = v.x;//delete the first point
					ptY[ptBegin * 4 + 1] = v.y;
					ptY[ptBegin * 4 + 2] = v.z;
					ptY[ptBegin * 4 + 3] = v.w;
					ptBegin++;
					if (ptBegin >= ptX.size())ptBegin = 0;
				}break;
			}
		default:break;
		}
		#undef GET_Y(T) T Y;yAddr.GetValueUnSafe(&Y);this->ptY.push_back((float)Y);break;
		
	}
}

void Graph_PointY::DrawPoints_Tip_Single(float x1, float y1, float x2, float y2, ImDrawList* drawList, GraphA* graph,DWORD color)
{
	float x1u,y1u,x2u,y2u;
	x1u = (x1 + graph->translationSrc.x) * graph->scaleSrc.x + graph->translationDst.x + graph->scrollingDst.x;
	x2u = (x2 + graph->translationSrc.x) * graph->scaleSrc.x + graph->translationDst.x + graph->scrollingDst.x;
	y1u = (y1 + graph->translationSrc.y) * graph->scaleSrc.y + graph->translationDst.y + graph->scrollingDst.y;
	y2u = (y2 + graph->translationSrc.y) * graph->scaleSrc.y + graph->translationDst.y + graph->scrollingDst.y;
	drawList->AddLine(ImVec2(x1u,y1u),ImVec2(x2u,y2u),color,3.0f);
	if (!graph->is_tipped)
	{
		ImVec2 msp=ImGui::GetMousePos();
		if (abs(msp.x - x1u) <= 4 && abs(msp.y - y1u) <= 4)
		{
			ImGui::BeginTooltip();
			Text("x:%6.4g\ny:%6.4g",x1,y1);
			ImGui::EndTooltip();
			graph->is_tipped = true;
		}
		else if (abs(msp.x - x2u) <= 4 && abs(msp.y - y2u) <= 4)
		{
			ImGui::BeginTooltip();
			Text("x:%6.4g\ny:%6.4g", x2, y2);
			ImGui::EndTooltip();
			graph->is_tipped = true;
		}
	}
	if (x1 < graph->minPoint.x)
		graph->minPoint.x = x1;
	if (x1 > graph->maxPoint.x)
		graph->maxPoint.x = x1;
	if (y1 < graph->minPoint.y)
		graph->minPoint.y = y1;
	if (y1 > graph->maxPoint.y)
		graph->maxPoint.y = y1;
}

void Graph_PointY::DrawCircle(float x, float y, DWORD col, ImDrawList* dw, GraphA* graph)
{
	ImVec2 c((x + graph->translationSrc.x) * graph->scaleSrc.x + graph->translationDst.x + graph->scrollingDst.x
	, (y + graph->translationSrc.y) * graph->scaleSrc.y + graph->translationDst.y + graph->scrollingDst.y);
	dw->AddCircle(c,5.0f,col,5,3.0f);
}

void Graph_PointY::DrawPoints_Tip(ImDrawList* drawList, GraphA* graph)
{
	graph->is_tipped=false;
	if(ptX.size()==0)return;
	if (yAddr.type < Address::AD_VECTOR2)//not vector
	{
		int last=ptX.size()-1;
		for (int i = ptBegin; i < last; i++)
		{
			DrawPoints_Tip_Single(ptX[i],ptY[i],ptX[i+1],ptY[i+1],drawList,graph,color[0]);
		}
		if (ptBegin != 0)
		{
			DrawPoints_Tip_Single(ptX[last], ptY[last], ptX[0], ptY[0], drawList, graph, color[0]);
			last=ptBegin-1;
			for (int i = 0; i < last; i++)
			{
				DrawPoints_Tip_Single(ptX[i], ptY[i], ptX[i + 1], ptY[i + 1], drawList, graph, color[0]);
			}
		}
		DrawCircle(ptX[last], ptY[last], color[0], drawList, graph);
	}
	else if(yAddr.type==Address::AD_VECTOR2){
		int last = ptX.size() - 1;
		for (int i = ptBegin,j=ptBegin*2; i < last; i++,j+=2)
		{
			DrawPoints_Tip_Single(ptX[i], ptY[j], ptX[i + 1], ptY[j + 2], drawList, graph, color[0]);
			DrawPoints_Tip_Single(ptX[i], ptY[j+1], ptX[i + 1], ptY[j + 3], drawList, graph, color[1]);
		}
		if (ptBegin != 0)
		{
			DrawPoints_Tip_Single(ptX[last], ptY[last*2], ptX[0], ptY[0], drawList, graph, color[0]);
			DrawPoints_Tip_Single(ptX[last], ptY[last*2+1], ptX[0], ptY[1], drawList, graph, color[1]);
			last = ptBegin - 1;
			for (int i = 0,j=0; i < last; i++,j+=2)
			{
				DrawPoints_Tip_Single(ptX[i], ptY[j], ptX[i + 1], ptY[j + 2], drawList, graph, color[0]);
				DrawPoints_Tip_Single(ptX[i], ptY[j + 1], ptX[i + 1], ptY[j + 3], drawList, graph, color[1]);
			}
		}
		DrawCircle(ptX[last], ptY[last * 2], color[0], drawList, graph);
		DrawCircle(ptX[last], ptY[last * 2 + 1], color[1], drawList, graph);
	}
	else if (yAddr.type == Address::AD_VECTOR3)
	{
		int last = ptX.size() - 1;
		for (int i = ptBegin, j = ptBegin * 3; i < last; i++, j += 3)
		{
			DrawPoints_Tip_Single(ptX[i], ptY[j], ptX[i + 1], ptY[j + 3], drawList, graph, color[0]);
			DrawPoints_Tip_Single(ptX[i], ptY[j + 1], ptX[i + 1], ptY[j + 4], drawList, graph, color[1]);
			DrawPoints_Tip_Single(ptX[i], ptY[j + 2], ptX[i + 1], ptY[j + 5], drawList, graph, color[1]);
		}
		if (ptBegin != 0)
		{
			DrawPoints_Tip_Single(ptX[last], ptY[last * 3], ptX[0], ptY[0], drawList, graph, color[0]);
			DrawPoints_Tip_Single(ptX[last], ptY[last * 3 + 1], ptX[0], ptY[1], drawList, graph, color[1]);
			DrawPoints_Tip_Single(ptX[last], ptY[last * 3 + 2], ptX[0], ptY[2], drawList, graph, color[1]);
			last = ptBegin - 1;
			for (int i = 0, j = 0; i < last; i++, j += 3)
			{
				DrawPoints_Tip_Single(ptX[i], ptY[j], ptX[i + 1], ptY[j + 3], drawList, graph, color[0]);
				DrawPoints_Tip_Single(ptX[i], ptY[j + 1], ptX[i + 1], ptY[j + 4], drawList, graph, color[1]);
				DrawPoints_Tip_Single(ptX[i], ptY[j + 2], ptX[i + 1], ptY[j + 5], drawList, graph, color[1]);
			}
		}
		DrawCircle(ptX[last], ptY[last * 3], color[0], drawList, graph);
		DrawCircle(ptX[last], ptY[last * 3 + 1], color[1], drawList, graph);
		DrawCircle(ptX[last], ptY[last * 3 + 2], color[2], drawList, graph);
		
	}
	else{//vector4
		int last = ptX.size() - 1;
		for (int i = ptBegin, j = ptBegin * 3; i < last; i++, j += 3)
		{
			DrawPoints_Tip_Single(ptX[i], ptY[j], ptX[i + 1], ptY[j + 4], drawList, graph, color[0]);
			DrawPoints_Tip_Single(ptX[i], ptY[j + 1], ptX[i + 1], ptY[j + 5], drawList, graph, color[1]);
			DrawPoints_Tip_Single(ptX[i], ptY[j + 2], ptX[i + 1], ptY[j + 6], drawList, graph, color[1]);
			DrawPoints_Tip_Single(ptX[i], ptY[j + 3], ptX[i + 1], ptY[j + 7], drawList, graph, color[1]);
		}
		if (ptBegin != 0)
		{
			DrawPoints_Tip_Single(ptX[last], ptY[last * 4], ptX[0], ptY[0], drawList, graph, color[0]);
			DrawPoints_Tip_Single(ptX[last], ptY[last * 4 + 1], ptX[0], ptY[1], drawList, graph, color[1]);
			DrawPoints_Tip_Single(ptX[last], ptY[last * 4 + 2], ptX[0], ptY[2], drawList, graph, color[1]);
			DrawPoints_Tip_Single(ptX[last], ptY[last * 4 + 3], ptX[0], ptY[3], drawList, graph, color[1]);
			last = ptBegin - 1;
			for (int i = 0, j = 0; i < last; i++, j += 3)
			{
				DrawPoints_Tip_Single(ptX[i], ptY[j], ptX[i + 1], ptY[j + 4], drawList, graph, color[0]);
				DrawPoints_Tip_Single(ptX[i], ptY[j + 1], ptX[i + 1], ptY[j + 5], drawList, graph, color[1]);
				DrawPoints_Tip_Single(ptX[i], ptY[j + 2], ptX[i + 1], ptY[j + 6], drawList, graph, color[1]);
				DrawPoints_Tip_Single(ptX[i], ptY[j + 3], ptX[i + 1], ptY[j + 7], drawList, graph, color[1]);
			}
		}
		DrawCircle(ptX[last], ptY[last * 4], color[0], drawList, graph);
		DrawCircle(ptX[last], ptY[last * 4 + 1], color[1], drawList, graph);
		DrawCircle(ptX[last], ptY[last * 4 + 2], color[2], drawList, graph);
		DrawCircle(ptX[last], ptY[last * 4 + 3], color[3], drawList, graph);
		
	}
}

void GraphA::Draw(ImDrawList* drawList, ImVec2 cv0, ImVec2 cvsz)
{
	this->AutoResize(cv0,cvsz);
	//grid
	float edy = cv0.y + cvsz.y, edx = cv0.x + cvsz.x;
	if (this->maxPoint.x != -FLT_MAX && this->is_grid_open)//have lines drawed
	{
		{
			
			if (is_y_reversed)
			{
				int powery = floor(log10f(-cvsz.y / this->scaleSrc.y));
				float dy_src = pow(10, powery);//src's dy
				float cv0y2 = (cv0.y - scrollingDst.y - translationDst.y) / scaleSrc.y - translationSrc.y;
				float sty_src = floor(cv0y2 / dy_src) * dy_src + dy_src;
				float sty = (sty_src + translationSrc.y) * scaleSrc.y + scrollingDst.y + translationDst.y;
				float dy = -dy_src * scaleSrc.y * 0.2f;//each grid have 5*5 small grids
				for (; sty < edy;)
				{
					char pos[20];
					sprintf_s(pos, 20, "%g", sty_src);
					drawList->AddText(ImVec2(cv0.x, sty), IM_COL32(200, 200, 200, 255), pos);
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 120), 1.0f); sty += dy;

					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					sty_src -= dy_src;
				}
			}
			else
			{
				int powery = floor(log10f(cvsz.y / this->scaleSrc.y));
				float dy_src = pow(10, powery);//src's dy
				float cv0y2 = (cv0.y - scrollingDst.y - translationDst.y) / scaleSrc.y - translationSrc.y;
				float sty_src = floor(cv0y2 / dy_src) * dy_src - dy_src;
				float sty = (sty_src + translationSrc.y) * scaleSrc.y + scrollingDst.y + translationDst.y;
				float dy = dy_src * scaleSrc.y * 0.2f;//each grid have 5*5 small grids
				for (; sty < edy;)
				{
					char pos[20];
					sprintf_s(pos, 20, "%g", sty_src);
					drawList->AddText(ImVec2(cv0.x, sty), IM_COL32(200, 200, 200, 255), pos);
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 120), 1.0f); sty += dy;

					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					drawList->AddLine(ImVec2(cv0.x, sty), ImVec2(edx, sty), IM_COL32(200, 200, 200, 40), 1.0f); sty += dy;
					sty_src += dy_src;
				}
			}
				
		}
		{
			int powerx = floor(log10f(fabsf(cvsz.x / this->scaleSrc.x)));
			float dx_src = pow(10, powerx);//src's dx
			float cv0x2 = (cv0.x - scrollingDst.x - translationDst.x) / scaleSrc.x - translationSrc.x;
			float stx_src = floor(cv0x2/dx_src) * dx_src - dx_src;
			float stx = (stx_src + translationSrc.x) * scaleSrc.x + scrollingDst.x + translationDst.x;
			float dx = dx_src * scaleSrc.x * 0.2f;//each grid have 5*5 small grids
			for (; stx < edx;)
			{
				char pos[20];
				sprintf_s(pos, 20, "%g", stx_src);
				drawList->AddText(ImVec2(stx,cv0.y), IM_COL32(200, 200, 200, 255), pos);
				drawList->AddLine(ImVec2(stx,cv0.y), ImVec2(stx, edy), IM_COL32(200, 200, 200, 120), 1.0f); stx += dx;
				drawList->AddLine(ImVec2(stx,cv0.y), ImVec2(stx, edy), IM_COL32(200, 200, 200, 40), 1.0f); stx += dx;
				drawList->AddLine(ImVec2(stx,cv0.y), ImVec2(stx, edy), IM_COL32(200, 200, 200, 40), 1.0f); stx += dx;
				drawList->AddLine(ImVec2(stx,cv0.y), ImVec2(stx, edy), IM_COL32(200, 200, 200, 40), 1.0f); stx += dx;
				drawList->AddLine(ImVec2(stx,cv0.y), ImVec2(stx,edy), IM_COL32(200, 200, 200, 40), 1.0f); stx += dx;
				stx_src += dx_src;
			}
		}
	}
	//init
	minPoint.x = FLT_MAX;
	minPoint.y = FLT_MAX;
	maxPoint.x = -FLT_MAX;
	maxPoint.y = -FLT_MAX;
	//axis
	float axOri = scaleSrc.x * translationSrc.x + translationDst.x + scrollingDst.x;
	float ayOri = scaleSrc.y * translationSrc.y + translationDst.y + scrollingDst.y;

	drawList->AddLine(ImVec2(axOri,ayOri),ImVec2(edx,ayOri),IM_COL32(255,0,0,255),1.0f);//x+
	drawList->AddLine(ImVec2(axOri,ayOri),ImVec2(cv0.x,ayOri),IM_COL32(155,0,0,255),1.0f);//x-
	if (is_y_reversed)
	{
		drawList->AddLine(ImVec2(axOri,ayOri),ImVec2(axOri,cv0.y),IM_COL32(255,255,0,255),1.0f);//y+
		drawList->AddLine(ImVec2(axOri,ayOri),ImVec2(axOri,edy),IM_COL32(155,155,0,255),1.0f);//y-
	}else{
		drawList->AddLine(ImVec2(axOri, ayOri), ImVec2(axOri, cv0.y), IM_COL32(155, 155, 0, 255), 1.0f);//y-
		drawList->AddLine(ImVec2(axOri, ayOri), ImVec2(axOri, edy), IM_COL32(255, 255, 0, 255), 1.0f);//y+
	}
	//point
	for (auto &x : this->points)
		for (auto& i : x.second)
			i.DrawPoints_Tip(drawList,this);
}

void GraphA::Update()
{
	if (rate!=0)
	{
		if (drawCount != 0)
		{
			drawCount++;
			if (drawCount >= rate)drawCount = 0;
			return;
		}
		drawCount++;
	}
	for (auto& i : this->points)
	{
		i.first.GetPointer();
		if (!i.first.is_bad_addr)
		{
			float x=0.0f;
#define GET_Y(T) T Y;i.first.GetValueUnSafe(&Y);x=Y;break;
			switch (i.first.type)
			{
			case Address::AD_BYTE: {GET_Y(BYTE)}
			case Address::AD_WORD: {GET_Y(WORD)}
			case Address::AD_DWORD: {GET_Y(int)}
			case Address::AD_QWORD: {GET_Y(long long int)}
			case Address::AD_FLOAT: {GET_Y(float)}
			case Address::AD_DOUBLE: {GET_Y(double)}
			default:return;
			}
#undef GET_Y(T) T Y;yAddr.GetValueUnSafe(&Y);x=Y;
			for (auto& j : i.second)
			{
				j.PushPoint(x);
			}
		}
	}
}

void GraphA::AutoResize(ImVec2 cv0, ImVec2 cvsz)
{
	if (minPoint.x == FLT_MAX)//no one point/only one point
	{
		scaleSrc=ImVec2(1,1);
		scrollingDst=ImVec2(0,0);
		translationDst=ImVec2(0,0);
		translationSrc=ImVec2(0,0);
	}else if (maxPoint.x - minPoint.x == 0 || maxPoint.y - minPoint.y == 0)
	{
		if (is_y_reversed)
		{
			scaleSrc = ImVec2(1, -1);
			float m = min(cvsz.x * 0.05f, cvsz.y * 0.05f);
			translationDst.x = m + cv0.x;
			translationDst.y = -m + cv0.y + cvsz.y;
			translationSrc.x = -minPoint.x;
			translationSrc.y = -minPoint.y;
		}else{
			scaleSrc = ImVec2(1, 1);
			float m = min(cvsz.x * 0.05f, cvsz.y * 0.05f);
			translationDst.x = m + cv0.x;
			translationDst.y = -m + cv0.y + cvsz.y;
			translationSrc.x = -minPoint.x;
			translationSrc.y = -minPoint.y;
		}

	}else{
		if (is_y_reversed)
		{
			float m = min(cvsz.x * 0.05f, cvsz.y * 0.05f);
			scaleSrc.x = (cvsz.x - m * 2.0f) / (maxPoint.x - minPoint.x);
			scaleSrc.y = -(cvsz.y - m * 2.0f) / (maxPoint.y - minPoint.y);
			translationDst.x = m + cv0.x;
			translationDst.y = -m + cv0.y+cvsz.y;
			translationSrc.x = -minPoint.x;
			translationSrc.y = -minPoint.y;
		}else{

			float m = min(cvsz.x * 0.05f, cvsz.y * 0.05f);
			scaleSrc.x = (cvsz.x - m * 2.0f) / (maxPoint.x - minPoint.x);
			scaleSrc.y = (cvsz.y - m * 2.0f) / (maxPoint.y - minPoint.y);
			translationDst.x = m + cv0.x;
			translationDst.y = m + cv0.y;
			translationSrc.x = -minPoint.x;
			translationSrc.y = -minPoint.y;
		}
	}
}

std::list<GraphA> GraphA::graphs;
void GraphWindow()
{
	ImGui::BeginTabBar("GrBar", ImGuiTabBarFlags_Reorderable);
	for(auto iter=GraphA::graphs.begin();iter!=GraphA::graphs.end();)
	{
		if (ImGui::BeginTabItem(iter->nameWithID.c_str(), 0))
		{

			ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
			if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
			if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			// Draw border and background color
			ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
			draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
			draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

			//draw
			draw_list->PushClipRect(canvas_p0, canvas_p1, true);
			iter->Update();
			iter->Draw(draw_list, canvas_p0, canvas_sz);
			draw_list->PopClipRect();
			//click in cv
			{
				ImGui::InvisibleButton("cvBt", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
				//const bool is_hovered = ImGui::IsItemHovered(); // Hovered
				const bool is_active = ImGui::IsItemActive();   // Held
				ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
					ImGui::OpenPopupContextItem("Cvct");
				if (ImGui::BeginPopup("Cvct"))
				{
					if (ImGui::MenuItem("Clear")) { ImGui::EndPopup(); iter->Clear(); }
					if (ImGui::MenuItem("Close", NULL, false)) { iter = GraphA::graphs.erase(iter); ImGui::EndPopup(); ImGui::EndTabItem(); continue; }
				}
				if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
				{
					iter->scrollingDst.x += io.MouseDelta.x;
					iter->scrollingDst.y += io.MouseDelta.y;
				}
			}
			ImGui::EndTabItem();
		}

		iter++;
	}
	EndTabBar();
	ImGui::SameLine();

}

void SingleAddrViewer(Address& addr)
{
	bool is_pointer=addr.offs.size()>=1;
	//address
	{
		char addrbuf[10];
		if (is_pointer)
		{
			sprintf_s(addrbuf, 10, "%08X", addr.offVal[addr.offVal.size()-1]);
			ImGui::InputText("adr", addrbuf, 10, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
		}
		else {
			sprintf_s(addrbuf, 10, "%08X", addr.base);
			if (ImGui::InputText("addr", addrbuf, 10, ImGuiInputTextFlags_CharsHexadecimal))
				sscanf_s(addrbuf, "%x", &addr.base);//auto changes
		}
		char valbuf[255];
		addr.GetValueStr(valbuf);
		SameLine();
		Text("=%s",valbuf);
	}
	ImGui::Separator();
	//description
	{
		char des[255];
		strcpy_s(des, 255, addr.description.c_str());
		ImGui::InputText("Description", des, 255);
		addr.description = std::string(des);
	}
	ImGui::Separator();
	//type
	{
		if (ImGui::Combo("ValueType", (int*)&(addr.type), Address::typeStr, IM_ARRAYSIZE(Address::typeStr)))
			ImGui::Separator();
		switch (addr.type)
		{
		case Address::AD_BYTE:
		case Address::AD_WORD:
		case Address::AD_DWORD:
		case Address::AD_QWORD:
			ImGui::Checkbox("hex value", (bool*)&(addr.typeAdd)); break;
		case Address::AD_STRING:
			if ((addr.typeAdd <= 0 || addr.typeAdd >= 201))addr.typeAdd = 1;
			ImGui::DragInt("length", &(addr.typeAdd), 1, 1, 200, "length: %d"); break;
		default:
			ImGui::Text(" ");
		}
	}
	//pointer check
	ImGui::Checkbox("pointer", &is_pointer);
	if (is_pointer)
	{
		if(addr.offs.size()==0)
			addr.offs.push_back(0);
		else
		{
			int i = addr.offs.size() - 1;
			//last offs
			{
				int* offs = (int*)&(addr.offs[i]);
				if (Button("<"))
					*offs -= 4;
				ImGui::SameLine();
				char lastOfsBuf[10];
				sprintf_s(lastOfsBuf, 10, *offs < 0 ? "-%X" : "%X", * offs < 0 ? (-*offs) : *offs);
				char name[15];
				sprintf_s(name, 15, "###offs%4d", i + 1);
				ImGui::SetNextItemWidth(80);
				if (InputText(name, lastOfsBuf, 10))
					if (!sscanf_s(lastOfsBuf, "%x", offs))offs = 0;//auto changes
				ImGui::SameLine();
				if (Button(">"))
					*offs += 4;
				ImGui::SameLine();
				if (addr.bad_count+2 <= i)
					Text("????????+%X=????????", *offs, addr.offVal[i + 1]);
				else
					if (*offs >= 0)
						Text("%08X+%X=%08X", addr.offVal[i], *offs, addr.offVal[i + 1]);
					else
						Text("%08X-%X=%08X", addr.offVal[i], -*offs, addr.offVal[i + 1]);
			}
			while (i--) {
				int* offs = (int*)&(addr.offs[i]);
				if (Button("<"))
					*offs -= 4;
				SameLine();
				char OfsBuf[10];
				sprintf_s(OfsBuf, 10, *offs < 0 ? "-%X" : "%X", * offs < 0 ? (-*offs) : *offs);
				char name[15];
				sprintf_s(name, 15, "###offs%4d", i + 1);
				ImGui::SetNextItemWidth(80);
				if (InputText(name, OfsBuf, 10))
					if (!sscanf_s(OfsBuf, "%x", offs))offs = 0;//auto changes
				SameLine();
				if (Button(">"))
					*offs += 4;
				SameLine();
				if (addr.bad_count - 1 <= i)
					Text("[????????+%X]=????????", *offs, addr.offVal[i + 1]);
				else if (addr.bad_count <= i)
					Text("[%08X+%X]=????????", addr.offVal[i], *offs);
				else
					if (*offs >= 0)
						Text("[%08X+%X]->%08X", addr.offVal[i], *offs, addr.offVal[i + 1]);
					else
						Text("[%08X-%X]->%08X", addr.offVal[i], -*offs, addr.offVal[i + 1]);
			}
			{
				DWORD* base = &addr.base;
				char OfsBuf[10];
				sprintf_s(OfsBuf, 10, "%08X", *base);
				char name[10];
				sprintf_s(name, 10, "offs %d:", i + 1);
				ImGui::SetNextItemWidth(80);
				if (InputText(name, OfsBuf, 10))
					if (!sscanf_s(OfsBuf, "%x", base))base = 0;//auto changes
				SameLine();
				if (addr.bad_count <= i)
					Text("->????????");
				else
					Text("->%08X", addr.offVal[i + 1]);
			}

			if (Button("add offset"))
				addr.offs.push_back(0);
			SameLine();
			if (Button("remove offset"))
				addr.offs.pop_back();
			addr.offVal.clear();
		}
	}
	else if(is_pointer==false){
		addr.offs.clear();
	}
}

void AddrViewer(bool* p_open)
{
	const static ImVec2 window_pos = ImVec2(GetVec(TransFromStageToScreen(100, 100)));
	const static ImVec2 window_pos_pivot = ImVec2(0, 0);//left top
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
	
	static std::list<std::pair<Address, bool>> addrs;//<addr,is_selected>
	static int totalSelected=0;
	static std::list<std::pair<Address, bool>>::iterator lastSelected = addrs.begin();
	static std::list<std::pair<Address, bool>>::iterator lastSecondSelected = addrs.begin();
	Begin("addrViewer", p_open);

	// Left
	{
		ImGui::BeginChild("left pane", ImVec2(400, 0), true,ImGuiWindowFlags_HorizontalScrollbar);
		Columns(3);
		int sort = 0;//0:no sort,1:by des(u),2:by addr(u),3,4 the similar

		Text("description");
		if (ImGui::IsItemClicked(0))
			sort=1;
		else if (ImGui::IsItemClicked(1))
			sort = 3;
		totalSelected=0;
		int dragN=0;
		for (auto x=addrs.begin();x!=addrs.end();x++)
		{
			ImGui::Selectable(x->first.GetDescriptionWider(), x->second, ImGuiSelectableFlags_SpanAllColumns);
			//select and drag to order
			if (x->second && ImGui::IsItemActive() && !ImGui::IsItemHovered())
			{
				dragN = ImGui::GetMouseDragDelta(0).y/ImGui::GetItemRectSize().y;//in order to avoid the multi select 
				
			}
			//end drag

			if (ImGui::IsItemDeactivated())
			{
				x->second=!x->second;
				lastSecondSelected=lastSelected;
				lastSelected = x;
				if (ImGui::GetIO().KeyShift && lastSecondSelected != addrs.end()){
					std::list<std::pair<Address, bool>>::iterator  it1, it2;
					if (lastSelected->first.count > lastSecondSelected->first.count)
						it1 = lastSecondSelected, it2 = lastSelected;
					else
						it2 = lastSecondSelected, it1 = lastSelected;
					for (auto iter = it1; iter != it2; iter++)
						iter->second = true;
					it2->second=true;
				}else if (ImGui::GetIO().KeyCtrl){
					x->second = true;
				}else if (ImGui::GetIO().KeyAlt){
					x->second = false;
				}else {
					for (auto &iter : addrs)
						iter.second = false;
					x->second = true;
				}
			}
		}
		NextColumn();
		Text("address");
		if (ImGui::IsItemClicked(0))
			sort = 2;
		else if(ImGui::IsItemClicked(1))
			sort=4;
		for (auto& x : addrs)
		{
			x.first.GetPointer();
			Text("%#08X",x.first.offVal[x.first.offVal.size()-1]);
		}
		NextColumn();
		Text("value");
		for (auto& x : addrs)
		{
			char ch[255];
			x.first.GetValueStr(ch);
			Text(ch);
			if (x.second)
				totalSelected++;
		}
		
		bool is_reOrdered=false;
		if (dragN > 0)//drag down
		{
			is_reOrdered=true;
			ImGui::ResetMouseDragDelta();
			for (int i = 0; i < dragN; i++)
			{
				auto ls = addrs.end()--;
				for (auto iter = addrs.begin(); iter != ls; iter++)
				{
					if (iter->second)
					{
						auto itNext = std::find_if(iter, addrs.end(), [=](auto i)->bool {return !i.second; });
						if (itNext != addrs.end())
						{
							auto itNN = itNext;
							itNN++;
							addrs.splice(itNN, addrs, iter, itNext);
							if (itNN != addrs.end())
								iter = itNN;
							else break;
						}
					}
				}
			}
		}
		else if (dragN < 0)//drag up
		{
			is_reOrdered = true;
			dragN=-dragN;
			ImGui::ResetMouseDragDelta();
			for (int i = 0; i < dragN; i++)
			{
				for (auto iter = addrs.begin(); iter != addrs.end(); iter++)
				{
					if (iter->second)
					{
						auto itNext = std::find_if(iter, addrs.end(), [=](auto i)->bool {return !i.second; });
						if (iter != addrs.begin())
						{
							auto itNN = iter;
							itNN--;
							addrs.splice(itNN, addrs, iter, itNext);
						}
						if(itNext!=addrs.end())
							iter = itNext;
						else break;
					}
				}
			}
		}
		switch (sort)
		{
			case 1:is_reOrdered = true;
			addrs.sort(
				[=](const std::pair<Address, bool>&A, const  std::pair<Address, bool>& B)->bool {
					return A.first.description > B.first.description; });break;
			case 2:is_reOrdered = true;
			addrs.sort(
				[=](const std::pair<Address, bool>&A, const  std::pair<Address, bool>& B)->bool {
					return A.first.offVal[A.first.offVal.size()-1] > B.first.offVal[B.first.offVal.size() - 1]; }); break;
			case 3:is_reOrdered = true;
			addrs.sort(
				[=](const std::pair<Address, bool>&A, const  std::pair<Address, bool>& B)->bool {
					return A.first.description < B.first.description; }); break;
			case 4:is_reOrdered = true;
			addrs.sort(
				[=](const std::pair<Address, bool>&A,const std::pair<Address, bool>& B)->bool {
					return A.first.offVal[A.first.offVal.size() - 1] < B.first.offVal[B.first.offVal.size() - 1]; }); break;
			default:break;
		}

		if (is_reOrdered)
		{
			int i = 0;
			for (auto& x : addrs)
			{
				x.first.count = i++;
			}
		}
		Columns(1);
		ImGui::EndChild();
	}
	ImGui::SameLine();

	// Right
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*16),ImGuiWindowFlags_AlwaysAutoResize); // Leave room for 1 line below us
		if(lastSelected==addrs.end())
			ImGui::Text("haven't select a address");
		else
		{
			Address &addr=lastSelected->first;
			ImGui::Text("Selected: %8d   Last2: %8d,%8d", totalSelected,
				lastSelected->first.count + 1, lastSecondSelected == addrs.end()?-1:lastSecondSelected->first.count+1);
			ImGui::Separator();
			if (ImGui::BeginTabBar("AddressTab", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Description"))
				{
					ImGui::TextWrapped("%s", addr.description.c_str());
					Text("val: %s", addr.GetValueStrUnSafe());
					ImGui::Separator();
					if(ImGui::Combo("ValueType", (int*)&(addr.type),Address::typeStr,IM_ARRAYSIZE(Address::typeStr)))
					ImGui::Separator();
					switch (addr.type )
					{
						case Address::AD_BYTE:
						case Address::AD_WORD:
						case Address::AD_DWORD:
						case Address::AD_QWORD:
							ImGui::Checkbox("hex value",(bool*)&(addr.typeAdd));break;
						case Address::AD_STRING:
							if((addr.typeAdd <= 0 || addr.typeAdd >= 201))addr.typeAdd = 1;
							ImGui::DragInt("length",&(addr.typeAdd),1,1,200,"length: %d");break;
						default:
							ImGui::Text(" ");
					}
					ImGui::Separator();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Details"))
				{
					SingleAddrViewer(addr);
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
		
	}

	//bottom
	
	{
		ImGui::BeginChild("plot view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		ImGui::Separator();

		static std::vector<ImVec2> scrolling{};
		static std::vector<std::string> tabNames={};
		static std::vector<std::list<ImVec2>> all_points={ };
		static std::vector<ImVec2> all_scales={};
		static std::vector<std::pair<ImVec2,ImVec2>> limCood={};//(minX,minY) (maxX,maxY)
		static std::vector<std::pair<Address,Address>> coodDes={};//(x,y) 
		static bool opt_enable_context_menu = true;
		static int nowSelectTab=-1;
		//>>>
		if (ImGui::Button("Add graph"))
			OpenPopup("add new graph");
		{
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("add new graph", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				if (lastSelected == addrs.end() || lastSecondSelected == addrs.end()
				|| lastSecondSelected->first.type == Address::AD_STRING ||
					lastSelected->first.type == Address::AD_STRING)
				{
					Text("please select two int/float/double/vector addresses to generate p(x,y)");
					if (Button("cancel"))
						ImGui::CloseCurrentPopup();
				}
				else {
					static int counter=0;
					static char p[40] = "Graph";
					Text("the addr selected:\nX=%s(%d)\nY=%s(%d)", 
					lastSecondSelected->first.description.c_str(),lastSecondSelected->first.count,
						lastSelected->first.description.c_str(), lastSelected->first.count);
					InputText("###GrN", p, 40);
					SameLine();
					if (ImGui::Button("apply"))
					{
						/*scrolling.push_back(ImVec2(0, 0));
						char label[50];
						sprintf_s(label,50, "%s###%d", p,counter); counter++;
						tabNames.push_back(std::string(label));
						all_points.push_back(std::list<ImVec2>());
						all_scales.push_back(ImVec2(1, 1));
						limCood.push_back(std::make_pair(ImVec2(0, 0), ImVec2(1, 1)));
						coodDes.push_back(std::make_pair(lastSecondSelected->first, lastSelected->first));*/
						char label[50];
						sprintf_s(label, 50, "%s###%d", p, counter); counter++;
						std::string s(label);
						GraphA G(s);
						Graph_PointY gpy(lastSelected->first);
						std::list<Graph_PointY> gpl;gpl.push_back(gpy);
						G.points.push_back(std::make_pair(lastSecondSelected->first, gpl));
						GraphA::graphs.push_back(G);
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}

		}
		Begin("gw");
		GraphWindow();
		End();
		//>>>
		SameLine();
		if (ImGui::Button("Remove graph"))
			OpenPopup("Remove graph");
		{
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Remove graph", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				if (nowSelectTab==-1)
					ImGui::CloseCurrentPopup();
				else{
					static char p[40] = "Graph";
					Text("sure to delete this graph: %s",tabNames[nowSelectTab].c_str());
					if (ImGui::Button("apply"))
					{
						scrolling.erase(scrolling.begin()+nowSelectTab);
						tabNames.erase(tabNames.begin()+nowSelectTab);
						all_points.erase(all_points.begin()+nowSelectTab);
						all_scales.erase(all_scales.begin()+nowSelectTab);
						limCood.erase(limCood.begin()+nowSelectTab);
						coodDes.erase(coodDes.begin()+nowSelectTab);
						nowSelectTab=-1;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
		}
		
		//>>>
		
		nowSelectTab=-1;
		/*if (ImGui::BeginTabBar("GrBar", ImGuiTabBarFlags_Reorderable))
		{
			for (int i = 0; i < tabNames.size(); i++)
			{
				if (BeginTabItem(tabNames[i].c_str(),0))
				{
					nowSelectTab=i;
					ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
					ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
					if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
					if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
					ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
					const float scaleP=64.0f;
					// Draw border and background color
					ImGuiIO& io = ImGui::GetIO();
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
					draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
					ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
					const bool is_hovered = ImGui::IsItemHovered(); // Hovered
					const bool is_active = ImGui::IsItemActive();   // Held
					const ImVec2 origin(canvas_p0.x + scrolling[i].x, canvas_p0.y + scrolling[i].y); // Lock scrolled origin
					const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

					// Add first and second point

					const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
					if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
					{
						scrolling[i].x += io.MouseDelta.x;
						scrolling[i].y += io.MouseDelta.y;
					}

					ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
					if (opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
						ImGui::OpenPopupContextItem("context");
					if (ImGui::BeginPopup("context"))
					{
						if (ImGui::MenuItem("Clear", NULL, false)) {all_points[i].clear();}
						ImGui::EndPopup();
					}
					// Draw grid + all lines in the canvas
					draw_list->PushClipRect(canvas_p0, canvas_p1, true);
					if (true)//enable gird
					{
						float GRID_STEP = scaleP * all_scales[i].x;
						for (float x = fmodf(scrolling[i].x, GRID_STEP),xp=-scaleP *(int)(scrolling[i].x/GRID_STEP);x < canvas_sz.x; x += GRID_STEP)
						{
							draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
							char pos[20];
							sprintf_s(pos,20,"%g", xp);
							draw_list->AddText(ImVec2(canvas_p0.x + x, canvas_p0.y),IM_COL32(200,200,200,255),pos);
							xp+= scaleP;
						}
						GRID_STEP = scaleP  * all_scales[i].y;
						for (float y = fmodf(scrolling[i].y, GRID_STEP), xp = -scaleP * (int)(scrolling[i].y / GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
						{
							draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
							char pos[20];
							sprintf_s(pos, 20, "%g", xp);
							draw_list->AddText(ImVec2(canvas_p0.x, canvas_p0.y+y), IM_COL32(200, 200, 200, 255), pos);
							xp += scaleP;
						}
						draw_list->AddLine(ImVec2(origin.x, origin.y-scrolling[i].y), ImVec2(origin.x,origin.y- scrolling[i].y+canvas_sz.y), IM_COL32(255, 255, 0, 255), 2.0f);
						draw_list->AddLine(ImVec2(origin.x - scrolling[i].x,origin.y), ImVec2(origin.x-scrolling[i].x + canvas_sz.x,origin.y), IM_COL32(0, 255, 0, 255), 2.0f);
					}
					auto j = all_points[i].begin();
					if (j != all_points[i].end())
					{
						limCood[i] = std::make_pair(*j,*j);
						auto p = j; p++;

						auto mouse=GetMousePos();
						bool is_drawed_tip=false;
						for (; p != all_points[i].end(); j++, p++)
						{
							ImVec2 pt1(origin.x + j->x * all_scales[i].x, origin.y + j->y * all_scales[i].y)
							,pt2(origin.x + p->x * all_scales[i].x, origin.y + p->y * all_scales[i].y);
							
							if(!is_drawed_tip)
								if (abs(mouse.x-pt1.x)<=5 && abs(mouse.y-pt1.y)<=5)
								{
									BeginTooltip();
									Text("(%5.3g,%5.3g)",j->x,j->y);
									EndTooltip();
									is_drawed_tip=true;
								}
								else if (abs(mouse.x - pt2.x) <= 5 && abs(mouse.y - pt2.y) <= 5)
								{
									BeginTooltip();
									Text("(%5.3g,%5.3g)", p->x, p->y);
									EndTooltip();
									is_drawed_tip = true;
								}
								draw_list->AddLine
								(pt1,pt2,IM_COL32(250, 140, 177, 255), 2.0f);
								if (p->x < limCood[i].first.x)limCood[i].first.x = p->x;
								if (p->x > limCood[i].second.x)limCood[i].second.x = p->x;
								if (p->y < limCood[i].first.y)limCood[i].first.y = p->y;
								if (p->y > limCood[i].second.y)limCood[i].second.y = p->y;
						}
						draw_list->AddCircle(ImVec2(origin.x + j->x * all_scales[i].x,origin.y + j->y * all_scales[i].y),5.0f,
						IM_COL32(191,166,255, 255),8,3.00f);
					}
					if (true)//auto resize
					{
						all_scales[i].x= canvas_sz.x/(limCood[i].second.x- limCood[i].first.x+0.0001f)*0.9f;
						all_scales[i].y= canvas_sz.y/(limCood[i].second.y- limCood[i].first.y+0.0001f)*0.9f;
						scrolling[i].x = canvas_sz.x * 0.05f -(limCood[i].first.x* all_scales[i].x);
						scrolling[i].y = canvas_sz.y * 0.05f -(limCood[i].first.y* all_scales[i].y);
						if(all_scales[i].x<=0.01f)all_scales[i].x=0.01f;
						if(all_scales[i].y<=0.01f)all_scales[i].y=0.01f;
					}
					auto &a= coodDes[i].first;
					auto &b= coodDes[i].second;
					a.GetPointer();
					b.GetPointer();
					if (!a.is_bad_addr && !b.is_bad_addr)
					{
						
						float x;
						switch (a.type)
						{
							case Address::AD_QWORD:
							{
								long long m;
								a.GetValueUnSafe((void*)&m);
								x = (float)m; break;
							}
							case Address::AD_DWORD:
							{
								int m;
								a.GetValueUnSafe((void*)&m);
								x=(float)m; break;
							}
							case Address::AD_WORD:
							{
								WORD m;
								a.GetValueUnSafe((void*)&m);
								x=(float)m; break;
							}
							case Address::AD_BYTE:
							{
								BYTE m;
								a.GetValueUnSafe((void*)&m);
								x=(float)m; break;
							}
							case Address::AD_FLOAT:
							{
								a.GetValueUnSafe((void*)&x); break;
							}
							case Address::AD_DOUBLE:
							{
								double m;
								a.GetValueUnSafe((void*)&m);
								x=m; break;
							}
							default:
							x=0;break;
						}
						float y;
						switch (b.type)
						{
							
						case Address::AD_QWORD:
						{
							long long m;
							b.GetValueUnSafe((void*)&m);
							y = (float)m; break;
						}
						case Address::AD_DWORD:
						{
							int m;
							b.GetValueUnSafe((void*)&m);
							y = (float)m; break;
						}
						case Address::AD_WORD:
						{
							WORD m;
							b.GetValueUnSafe((void*)&m);
							y = (float)m; break;
						}
						case Address::AD_BYTE:
						{
							BYTE m;
							b.GetValueUnSafe((void*)&m);
							y = (float)m; break;
						}
						case Address::AD_FLOAT:
						{
							b.GetValueUnSafe((void*)&y); break;
						}
						case Address::AD_DOUBLE:
						{
							double m;
							b.GetValueUnSafe((void*)&m);
							y = m; break;
						}
						default:
							y = 0; break;
						}
						all_points[i].push_back(ImVec2(x,y));
						if(all_points[i].size()>=256)
							all_points[i].pop_front();
					}

					draw_list->PopClipRect();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		*/
		ImGui::EndChild();
		
		{
			if (ImGui::Button("Add addressX"))
			{
				Address b(0);
				b.count=3;
				b.base = 0x004E73FC;
				b.type = b.AD_DWORD;
				b.description = std::string("Time");
				addrs.push_back(std::make_pair(b, false));

				Address a(0);
				a.base=0x004e9bb8;
				a.type=a.AD_FLOAT;
				a.offs.push_back(0x618);
				a.description=std::string("playerX");
				addrs.push_back(std::make_pair(a,false));

				a.count=1;
				a.GetID();
				a.offs[0]=0x61C;
				a.description = std::string("playerY");
				addrs.push_back(std::make_pair(a, false));

				a.GetID();
				a.count=2;
				a.type=a.AD_VECTOR2;
				a.offs[0]=0x618;
				a.description = std::string("player");
				addrs.push_back(std::make_pair(a, false));
			}
			if (ImGui::Button("Add address"))
				OpenPopup("add new address");
			//ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			//ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("add new address", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static Address addr1(0);
				static bool editing = 0;
				addr1 = Address(addrs.size() + 1);
				addr1.base = 0x0;
				addr1.type = addr1.AD_DWORD;
				if (!editing)
				{
					addrs.push_back(std::make_pair(addr1, false));
					editing = 1;
				}
				Address& adr = addrs.back().first;
				SingleAddrViewer(adr);
				if (ImGui::Button("apply") || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Enter)))
				{
					editing = 0;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}


			ImGui::SameLine();
			if (ImGui::Button("Delete address(del)") ||
				(IsKeyPressed(GetKeyIndex(ImGuiKey_Delete)) && IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))) {
				for (auto iter = addrs.begin(); iter != addrs.end();)
				{
					if (iter->second == true)
						iter = addrs.erase(iter);
					else
						iter++;
				}
				lastSelected = lastSecondSelected = addrs.end();
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}

