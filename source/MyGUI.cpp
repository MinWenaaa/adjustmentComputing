#include "MyGUI.h"
#include "windowPara.h"

void MyGUI::init(GLFWwindow* window) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsClassic();

	ImFont* font = io.Fonts->AddFontFromFileTTF(
		"imgui/Arial.ttf", 36, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
	);
	IM_ASSERT(font != nullptr);
	io.Fonts->Build();

}

void MyGUI::Render(int width, int height) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	Camera& camera = Camera::getInstance();
	ImGui::SetNextWindowPos(ImVec2(WindowParas::instance().SCREEN_WIDTH - width, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Fold");

	ImGui::Dummy(ImVec2(10, 20));
	//ImGui::TextWrapped(SchoolMap::getInstance().getCurrentName().c_str());
	ImGui::Dummy(ImVec2(10, 20));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, ImGui::GetContentRegionAvail().y - 4.5 * ImGui::GetFrameHeight()));
	if (ImGui::Button(camera.getMoving() ? "Rotation Mode" : "Moving Mode", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
		camera.changeMoving();
	}
	if (ImGui::Button("set as start point", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
		//SchoolMap::getInstance().setStart();
	}
	if (ImGui::Button("set as end point", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
		//SchoolMap::getInstance().setEnd();
	}
	if (ImGui::Button("Confirm", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
		//SchoolMap::getInstance().navigation();
	}
	//char buffer[256] = "";
	//ImGui::InputText("Input Field", buffer, sizeof(buffer));
	//std::string textU8 = buffer;

	//ImGui::Checkbox("Show Drag", &isShowDrag);
	//if (isShowDrag)
	//{
	//	float value = 10.0f;
	//	ImGui::DragFloat(u8"Value", &value);
	//}
	//ImGui::SliderFloat("float", &fValue, 0.0f, 1.0f);
	//ImGui::SameLine();
	//ImGui::Text("Value %f", fValue);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}