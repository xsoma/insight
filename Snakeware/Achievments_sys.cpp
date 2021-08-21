#include "Achievment_sys.h"
#include "imgui/imgui.h"

bool ActiveMessage = false;

int time_global = 1;

int duration_global = 1;

void ResetGlobalTime()
{
	time_global = 1;
	duration_global = 1;
}
void Achievments::Create_message(const char text, int time, int duration)
{

	
	if (duration > duration_global)
	{
		duration_global++;
		ActiveMessage = true;
	}
	

	if (duration < duration_global)
	{
		ActiveMessage = false;
	}

	ImGui::SetNextWindowSize(ImVec2{ 165, 30 }, ImGuiSetCond_Once);

	if (ImGui::Begin("MessageGeneral", &ActiveMessage, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
		ImGui::Text(&text);
	}
	ImGui::End();
}