#include "MyGUI.h"
#include "windowPara.h"
#include "solution.h"
#include <iostream>

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

    ImGui::SetNextWindowPos(ImVec2(WindowParas::instance().SCREEN_WIDTH - width, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));

    //static bool first = true;
    //if (first) {
    //    ImGui::SetNextWindowCollapsed(true, ImGuiCond_Always);
    //    first = false;
    //}

    ImGui::Begin("Fold");

    if (!inited) {
        float buttonWidth = 240.0f;
        float buttonHeight = 60.0f;
        float availWidth = ImGui::GetContentRegionAvail().x;
        float availHeight = ImGui::GetContentRegionAvail().y;

        // 预留空间，将按钮推到底部
        ImGui::Dummy(ImVec2(0, availHeight - buttonHeight-20));
        ImGui::SetCursorPosX((availWidth - buttonWidth) * 0.5f);
        if (ImGui::Button("Calculate", ImVec2(buttonWidth, buttonHeight))) {
            inited = true;
            solution::instance().readFile("source/data.txt");
            temp_status = 0;
        }
    }
    else {
        Camera& camera = Camera::getInstance();
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Text("Welcome to the 3D Visualization System!");
        ImGui::Separator();
        
        float textWidth = 580.0f;
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + textWidth);
        ImGui::TextWrapped("%s", getData(temp_status));
        ImGui::PopTextWrapPos();
        if (ImGui::Button(camera.getMoving() ? "Rotation Mode" : "Moving Mode", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            camera.changeMoving();
        }
        ImGui::Dummy(ImVec2(0, 10));
        if (ImGui::Button("Closed Traverse 1", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            temp_status = 1;
            solution::instance().change_focus(0);
        }
        ImGui::Dummy(ImVec2(0, 10));
        if (ImGui::Button("Closed Traverse 2", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            temp_status = 2;
            solution::instance().change_focus(1);
        }
        ImGui::Dummy(ImVec2(0, 10));
        if (ImGui::Button("Closed Traverse 3", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            temp_status = 3;
            solution::instance().change_focus(2);
        }
        ImGui::Dummy(ImVec2(0, 10));
        if (ImGui::Button("Connecting Traverse 1", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            temp_status = 4;
            solution::instance().change_focus(3);
        }

        static char buf[256] = "";
        ImGui::InputText("Input", buf, sizeof(buf));
		if (ImGui::Button("Confirm", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			point_name = buf;
			auto it = solution::instance().name2num.find(point_name);
			if (it != solution::instance().name2num.end()) {
				int index = it->second;
				solution::instance().focus_point(index);
				temp_status = 5;
			}
			else {
				std::cout << "Point not found!" << std::endl;
			}
		}

    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

const char* MyGUI::getData(int i) {  
   // std::cout << solution::instance().netinfo << std::endl;
   if (i == 0) return solution::instance().netinfo.c_str();  
   else if (i == 5) {
       if (point_name == "33") return "control point 33\nx:3371263.9690   y:558904.1270  z:32.1185";
	   else return "unknown point 1-2\nx:3371116.5608   y:558712.6905  z: 33.0607 \nLong axis (m):0.0072 Short axis (m):0.0053 Long axis orientation (dms):111.124826 Point error (m):0.0089\nElevation Mean Square Error(m):0.0004";
   }
   else return solution::instance().inf[i - 1].c_str();  

}