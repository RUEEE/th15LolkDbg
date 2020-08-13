#include "pch.h"
#include "addrViewer.h"
#include "DrawShape.h"
#include <tuple>
#include <sstream>

using namespace ImGui;
extern float scale;
extern HANDLE current_process;

const char* Address::typeStr[] = { "byte","2 bytes","4 bytes","8 bytes","float","double","string","vec2","vec3","vec4" };

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

const char* Address::GetDescription()
{
	std::stringstream ss;
	ss<<count+1<<": "<<this->description;
	descriptionTotal=std::string(ss.str());
	return descriptionTotal.c_str();
}

Address::Address(int c=0):base(0), is_bad_addr(1), offs(), offVal(), type(AD_DWORD), typeAdd(0)
{
	count=c;
	description="no description";
}

void Address:: SetBuffer(int i)
{
	if (i >= 256)i = 255;
	if (i <= 0)i = 1;
	typeAdd = i;
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
	Separator();
	//description
	{
		char des[255];
		strcpy_s(des, 255, addr.description.c_str());
		ImGui::InputText("Description", des, 255);
		addr.description = std::string(des);
	}
	Separator();
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
	Separator();
	//pointer check
	Checkbox("pointer", &is_pointer);
	Separator();
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
				SameLine();
				if (Button(">"))
					*offs += 4;
				SameLine();
				char lastOfsBuf[10];
				sprintf_s(lastOfsBuf, 10, *offs < 0 ? "-%X" : "%X", * offs < 0 ? (-*offs) : *offs);
				char name[10];
				sprintf_s(name, 10, "offs %d:", i + 1);
				ImGui::SetNextItemWidth(80);
				if (InputText(name, lastOfsBuf, 10))
					if (!sscanf_s(lastOfsBuf, "%x", offs))offs = 0;//auto changes
				SameLine();
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
				if (Button(">"))
					*offs += 4;
				SameLine();
				char OfsBuf[10];
				sprintf_s(OfsBuf, 10, *offs < 0 ? "-%X" : "%X", * offs < 0 ? (-*offs) : *offs);
				char name[10];
				sprintf_s(name, 10, "offs %d:", i + 1);
				ImGui::SetNextItemWidth(80);
				if (InputText(name, OfsBuf, 10))
					if (!sscanf_s(OfsBuf, "%x", offs))offs = 0;//auto changes
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
		int i=0;
		totalSelected=0;
		for (auto x=addrs.begin();x!=addrs.end();x++)
		{
			x->first.count=i;
			if (ImGui::Selectable(x->first.GetDescription(), x->second, ImGuiSelectableFlags_SpanAllColumns))
			{
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
			i++;
		}
		NextColumn();
		for (auto& x : addrs)
		{
			x.first.GetPointer();
			Text("%#08X",x.first.offVal[x.first.offVal.size()-1]);
		}
		NextColumn();
		for (auto& x : addrs)
		{
			char ch[255];
			x.first.GetValueStr(ch);
			Text(ch);
			if (x.second)
				totalSelected++;
		}
		
		Columns(1);
		ImGui::EndChild();
	}
	ImGui::SameLine();

	// Right
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*16)); // Leave room for 1 line below us
		if(lastSelected==addrs.end())
			ImGui::Text("haven't select a address");
		else
		{
			Address &addr=lastSelected->first;
			ImGui::Text("Selected: %8d   Last2: %8d,%8d", totalSelected,
				lastSelected->first.count + 1, lastSecondSelected == addrs.end()?-1:lastSecondSelected->first.count+1);
			ImGui::Separator();
			if (ImGui::BeginTabBar("AddressViewer_Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Description"))
				{
					ImGui::TextWrapped("%s",addr.description.c_str());
					Text("val: %s", addr.GetValueStrUnsafe());
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
					Separator();
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
		Separator();

		static std::vector<ImVec2> scrolling{};
		static std::vector<std::string> tabNames={};
		static std::vector<std::list<ImVec2>> all_points={ };
		static std::vector<ImVec2> all_scales={};
		static std::vector<std::pair<ImVec2,ImVec2>> limCood={};//(minX,minY) (maxX,maxY)
		static std::vector<std::pair<Address,Address>> coodDes={};//(x,y) 
		static bool opt_enable_context_menu = true;
		//>>>
		if (ImGui::Button("Add graph"))
			OpenPopup("add new graph");
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("add new graph", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (lastSelected == addrs.end() || lastSecondSelected == addrs.end())
			{
				Text("please select two int or float or double addresses to generate p(x,y)");
				if (Button("cancel"))
					ImGui::CloseCurrentPopup();
			}else{
				static char p[40]="Graph";
				InputText("name:",p,40);
				SameLine();
				Text("%s@n",p);
				if (ImGui::Button("apply"))
				{
					scrolling.push_back(ImVec2(0,0));
					char label[50];
					sprintf_s(label,50,"%s@%d",p,tabNames.size()+1);
					tabNames.push_back(std::string(label));
					all_points.push_back(std::list<ImVec2>());
					all_scales.push_back(ImVec2(1,1));
					limCood.push_back(std::make_pair(ImVec2(0,0),ImVec2(1,1)));
					coodDes.push_back(std::make_pair(lastSecondSelected->first,lastSelected->first));
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		//>>>
		if (ImGui::BeginTabBar("##TabBar"))
		{
			for (int i = 0; i < tabNames.size(); i++)
			{
				if (BeginTabItem(tabNames[i].c_str(),0))
				{
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
						for (; p != all_points[i].end(); j++, p++)
						{
							draw_list->AddLine
							(ImVec2(origin.x + j->x * all_scales[i].x, origin.y + j->y * all_scales[i].y),
								ImVec2(origin.x + p->x * all_scales[i].x, origin.y + p->y * all_scales[i].y),
								IM_COL32(250, 140, 177, 255), 2.0f);
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
		



		ImGui::EndChild();
		{
			if (ImGui::Button("Add address(A)") ||
				(ImGui::IsKeyPressed('A') && IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)))
				OpenPopup("add new address");
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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
				if (ImGui::Button("apply"))
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
	End();
}

