//
//  imgui.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/24.
//

#ifndef imgui_h
#define imgui_h

class MyImgui
{
public:
    bool show_demo_window;
    int imgui_shadowtype;
    
    MyImgui(GLFWwindow* window, bool in_show_demo_window=false, int in_imgui_shadowtype=0)
    {
        // Setup Dear ImGui context
        // ------------------------
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        ImGui::StyleColorsDark(); // Setup Dear ImGui style
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char* glsl_version = "#version 150";
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        show_demo_window = in_show_demo_window;
        imgui_shadowtype = in_imgui_shadowtype;
    };
    
    void newframe()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGuiIO& io = ImGui::GetIO();

        // 1. Show the big demo window
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(500, 100));
        ImGui::Begin("Rendering settings");
        
        const char* items[] = {
            "vanilla",
            "percentage-closer filtering (PCF)",
            "percentage colser soft shadows (PCSS)"
        };
        ImGui::Combo("Shadow mapping type", &imgui_shadowtype, items, IM_ARRAYSIZE(items));
        
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif /* imgui_h */
