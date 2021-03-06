#include <libs>
#include <Gloom/Gloom.h>
#include <Gloom/Files.h>

#include <OpenGL/RT.h>

#include "extra.h"

using namespace gloom;

#include <imgui.h>

RenderTarget *rt;
void setup_esc_menu()
{

	rt = new RenderTarget(gloom::width, gloom::height, GL_RGB, GL_UNSIGNED_BYTE);
	
}

void esc_menu(bool *open)
{
	if (!*open)
		return;

	ImGuiIO &io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings;

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 220, 255, 255));

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(300, 0));

	ImGui::Begin("Boo", open, flags);

	ImGui::Text("(F10)");
	ImGui::Text("window %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
	ImGui::Text("fps %u", gloom::fps);

	//ImGui::Separator();
	ImGui::End();

	ImGui::PopStyleColor(4);
}