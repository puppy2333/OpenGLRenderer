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
    
    // Shadow type
    int shadowtype;
    // Screen space reflection
    int rendertype;
    // Num of ray for SSR
    int numray;
    // Screen space ambient occlusion
    bool ssao;
    
    MyImgui(GLFWwindow* window, bool in_show_demo_window=false, 
            int in_shadowtype=0,
            bool in_rendertype=false,
            int in_numray=0)
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
        shadowtype = in_shadowtype;
        rendertype = in_rendertype;
        numray = in_numray;
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
        //ImGui::SetNextWindowPos(ImVec2(10, 10));
#ifdef __APPLE__
        ImGui::SetNextWindowPos(ImVec2(10, 440));
#else
        ImGui::SetNextWindowPos(ImVec2(10, 960));
#endif
        ImGui::SetNextWindowSize(ImVec2(500, 180));
        ImGui::Begin("Rendering settings");
        
        const char* shadowtype_list[] = {
            "no shadow",
            "vanilla",
            "percentage-closer filtering (PCF)",
            "percentage colser soft shadows (PCSS)"
        };
        ImGui::Combo("Shadow mapping type", &shadowtype, shadowtype_list, IM_ARRAYSIZE(shadowtype_list));
        
        const char* rendertype_list[] = {
            "direct lightning",
            "screen space reflection",
            "DEBUG: vis ssao",
            "DEBUG: vis shadow map",
            "Test render height field"
        };
        ImGui::Combo("Rendering type", &rendertype, rendertype_list, IM_ARRAYSIZE(rendertype_list));
        
        //ImGui::SeparatorText("Sliders");
        
        ImGui::Checkbox("Screen space ambient occlusion", &ssao);
        
        //ImGui::SliderInt("Num of rays", &numray, 1, 8);
        
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        
        ImGui::SeparatorText("Performance");
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif /* imgui_h */
