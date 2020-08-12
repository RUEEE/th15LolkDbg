#include "pch.h"
#include "imguiClass.h"
#include "handle.h"
#include "DrawShape.h"
using namespace ImGui;
extern float scale;
extern bool is_in_game;
ImVec2 GetVec(Vector2 vec)
{
    return ImVec2(vec.x,vec.y);
}

static void ShowOverlay()
{
    static float BgAlpha=1.0f;
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2(GetVec(TransFromStageToScreen(200,0)));
    ImVec2 window_pos_pivot = ImVec2(0,0);//left top
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(BgAlpha);
    SetNextWindowContentSize(ImVec2(200*scale,432* scale));
    ImGuiWindowFlags window_flags =ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar
    | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::Begin("sv", NULL, window_flags);
    if (ImGui::CollapsingHeader("General", NULL, ImGuiTreeNodeFlags_DefaultOpen))
    {
        static const char* diff[]={u8"easy",u8"normal",u8"hard",u8"lunatic",u8"extra",u8"unknown"};
        Text(
        "diff: %s\nhiScore: \t\t\t%13lld\nscore: \t\t\t  %13lld\nlife: %9d\tbomb: %9d\npower: %8d\tvalue: %8d\nnowG: %9d\tgraze: %8d\n",
            (*GameGeneral::difficulty<=5 && *GameGeneral::difficulty>=0)?diff[*GameGeneral::difficulty]:diff[5],
            (unsigned long long)*GameGeneral::hiScore, (unsigned long long)*GameGeneral::score,
            *GameGeneral::life,*GameGeneral::bomb,
            *GameGeneral::power, *GameGeneral::mvalue,
            *GameGeneral::graze_present, *GameGeneral::graze
            );
        Separator();
        if (DragInt("life", (int*)GameGeneral::life, 1.0f, 0, 9, "life: %d") && is_in_game)
            GameGeneral::LifeChanged();
        if (DragInt("bomb", (int*)GameGeneral::bomb, 1.0f, 0, 9, "bomb: %d") && is_in_game)
            GameGeneral::BombChanged();
        if (DragInt("power", (int*)GameGeneral::power, 1.0f, 0, 400, "power: %d") && is_in_game)
            GameGeneral::PowerChanged();
        Separator();
        if (TreeNodeEx("more things",ImGuiTreeNodeFlags_DefaultOpen))
        {
            static const char* playerType[] = { u8"reimu",u8"marisa",u8"sanae",u8"reisen",u8"unknown" };
            Text(
            "playerType: %s\nframePassed:   %9d\ngameSpeed: \t%9.2f\ntotalReCount:  %9d\nnowReCount:    %9d\ndeathCount:    %9d\ncreditUsed:    %9d\n",
            (*GameGeneral::playerType <= 4 && *GameGeneral::playerType >= 0)?playerType[*GameGeneral::playerType]:playerType[4],
            *GameGeneral::frame_passed, *GameGeneral::gameSpeed, *GameGeneral::retry_count_total, *GameGeneral::retry_count_now,
            *GameGeneral::death_count, *GameGeneral::credit_used);
            TreePop();
        }
        Separator();
        if (TreeNodeEx("keyBoard", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const ImVec4 color_pressed(0.375f,0.375f,0.9f,0);//rgba
            const ImVec4 color_pressedG(1.0f,0.375f,0.375f,0);//rgba
            const ImVec4 color_unpressed(0.25f, 0.25f, 0.25f, 0);
            const ImVec4 color_none(0,0,0,0);
            ImGui::Columns(7,0,false);
            for(int i=0;i<7;i++)
                SetColumnWidth(i,32);

                if(GameGeneral::is_pressed_GZ)
                    ColorButton("Z", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("Z",GameGeneral::is_pressed_Z?color_pressed:color_unpressed,ImGuiColorEditFlags_NoTooltip);
                if (GameGeneral::is_pressed_GShift)
                    ColorButton("S1", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("S1",GameGeneral::is_pressed_Shift?color_pressed:color_unpressed,ImGuiColorEditFlags_NoTooltip);
            ImGui::NextColumn();
                if(GameGeneral::is_pressed_GX)
                    ColorButton("X", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("X",GameGeneral::is_pressed_X?color_pressed:color_unpressed,ImGuiColorEditFlags_NoTooltip);
                if (GameGeneral::is_pressed_GShift)
                    ColorButton("S2", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("S2",GameGeneral::is_pressed_Shift?color_pressed:color_unpressed,ImGuiColorEditFlags_NoTooltip);
            ImGui::NextColumn();
                if (GameGeneral::is_pressed_GC)
                    ColorButton("C", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("C", GameGeneral::is_pressed_C ? color_pressed : color_unpressed, ImGuiColorEditFlags_NoTooltip);
                if (GameGeneral::is_pressed_GShift)
                    ColorButton("S3", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("S3", GameGeneral::is_pressed_Shift ? color_pressed : color_unpressed, ImGuiColorEditFlags_NoTooltip);
            ImGui::NextColumn();
                ColorButton("N1", color_none, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder);
                if (GameGeneral::is_pressed_GLeft)
                    ColorButton("L", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("L", GameGeneral::is_pressed_Left ? color_pressed : color_unpressed, ImGuiColorEditFlags_NoTooltip);
            ImGui::NextColumn();
                if (GameGeneral::is_pressed_GUp)
                    ColorButton("U", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("U", GameGeneral::is_pressed_Up ? color_pressed : color_unpressed, ImGuiColorEditFlags_NoTooltip);
                if (GameGeneral::is_pressed_GDown)
                    ColorButton("D", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("D", GameGeneral::is_pressed_Down ? color_pressed : color_unpressed, ImGuiColorEditFlags_NoTooltip);
            ImGui::NextColumn();
                ColorButton("N2", color_none, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder);
                if (GameGeneral::is_pressed_GRight)
                    ColorButton("R", color_pressedG, ImGuiColorEditFlags_NoTooltip);
                else ColorButton("R", GameGeneral::is_pressed_GRight ? color_pressed : color_unpressed, ImGuiColorEditFlags_NoTooltip);

            ImGui::Columns(1);
            TreePop();
        }
        
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Bullet",NULL,ImGuiTreeNodeFlags_DefaultOpen))
    {
        Text("Bullet num: %5d", BulletHandle::bullet_num);
        Checkbox(u8"bulletHitBox", &BulletHandle::is_HitBox_visible);
        Checkbox(u8"bulletSelectBox", &BulletHandle::is_SelectBox_visible);
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Others"))
    {
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (TreeNodeEx("style setting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static bool is_style_editer_visible = false;
            if (Button("style editor"))
                is_style_editer_visible = !is_style_editer_visible;
            if (is_style_editer_visible)
            {
                Begin("Style editer");
                ShowStyleEditor();
                End();
            }

            static bool is_metrics_visible = false;
            if (Button("metrics"))
                is_metrics_visible = !is_metrics_visible;
            if (is_metrics_visible)
            {
                //Begin("Style editer");
                ShowMetricsWindow(&is_metrics_visible);
                //End();
            }

            DragFloat("bg alpha", &BgAlpha, 0.01f, 0, 1, "background alpha: %.2f");
            TreePop();
        }
    }
    ImGui::End();
}

void DefaultWindow()
{
    ShowOverlay();
}




void DrawImGui()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();

	DefaultWindow();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
}
