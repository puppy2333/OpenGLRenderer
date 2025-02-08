//
//  imgui.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/24.
//

#ifndef imgui_h
#define imgui_h

// Project website: http://tinyfiledialogs.sourceforge.net
#include "tinyfd/tinyfiledialogs.h"

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

    // User opened file
    std::string opened_file_path;

    bool swe_init;
    int swe_tick_count;

    bool init_wave = false;

    float camera_position_old[3] = {0.0, 0.0, 0.0};
    float camera_position[3] = {0.0, 0.0, 0.0};
    bool camera_moved = false;

    float camera_yaw_old = 0.0;
    float camera_yaw = 0.0;
    bool camera_yaw_moved = false;

    float camera_pitch_old = 0.0;
    float camera_pitch = 0.0;
    bool camera_pitch_moved = false;

    // List of shadow types
    const char* shadowtype_list[4] = {
            "no shadow",
            "vanilla",
            "percentage-closer filtering (PCF)",
            "percentage colser soft shadows (PCSS)"
    };

    // List of render options
    const char* rendertype_list[8] = {
            "direct lightning",
            "screen space reflection",
            "DEBUG: vis ssao",
            "DEBUG: vis shadow map",
            "DEBUG: render height field",
            "shallow water equation",
            "DEBUG: vis inverse ssao",
            "subsurface scattering",
    };
    
    MyImgui(GLFWwindow* window, bool in_show_demo_window=false, 
            int in_shadowtype=0,
            bool in_rendertype=false,
            int in_numray=0)
    {
        // Setup Dear ImGui context
        // ------------------------
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); //(void)io;
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
        swe_init = false;
        ssao = false;
        swe_tick_count = 0;

#ifdef __linux__
        io.FontGlobalScale = 1.5f;
#endif

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
#ifdef __APPLE__
        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH, 0));
        ImGui::SetNextWindowSize(ImVec2(UI_WIDTH, SCR_HEIGHT));
#elif defined(__linux__)
        ImGui::SetNextWindowPos(ImVec2(10, 960));
        ImGui::SetNextWindowSize(ImVec2(600, 220));
#else
        ImGui::SetNextWindowPos(ImVec2(10, 960));
        ImGui::SetNextWindowSize(ImVec2(500, 180));
#endif
    };

    ~MyImgui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    void newframe()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGuiIO& io = ImGui::GetIO();

        // 1. Show the big demo window
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH, 0));
        ImGui::SetNextWindowSize(ImVec2(UI_WIDTH, SCR_HEIGHT));

        ImGui::Begin("Rendering settings", NULL, ImGuiWindowFlags_MenuBar);

        ImGui::Combo("Shadow mapping type", &shadowtype, shadowtype_list, IM_ARRAYSIZE(shadowtype_list));

        ImGui::Combo("Rendering type", &rendertype, rendertype_list, IM_ARRAYSIZE(rendertype_list));
        
        //ImGui::SeparatorText("Sliders");
        
        ImGui::Checkbox("Screen space ambient occlusion", &ssao);
        
        //ImGui::SliderInt("Num of rays", &numray, 1, 8);
        
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        
        ImGui::InputFloat3("Camera position", camera_position, "%.4f");
        if (
            abs(camera_position[0] - camera_position_old[0]) > 1e-4 or 
            abs(camera_position[1] - camera_position_old[1]) > 1e-4 or 
            abs(camera_position[2] - camera_position_old[2]) > 1e-4) 
        {
            camera_moved = true;
            std::copy(camera_position, camera_position + 3, camera_position_old);
            // camera_position_old[0] = camera_position[0];
            // camera_position_old[1] = camera_position[1];
            // camera_position_old[2] = camera_position[2];
        }

        ImGui::InputFloat("Camera yaw", &camera_yaw, 0.0f, 0.0f, "%.4f");
        if (abs(camera_yaw - camera_yaw_old) > 1e-4) 
        {
            camera_yaw_moved = true;
            camera_yaw_old = camera_yaw;
        }

        ImGui::InputFloat("Camera pitch", &camera_pitch, 0.0f, 0.0f, "%.4f");
        if (abs(camera_pitch - camera_pitch_old) > 1e-4) 
        {
            camera_pitch_moved = true;
            camera_pitch_old = camera_pitch;
        }

        static char buf[32];
        ImGui::InputText(" input", buf, IM_ARRAYSIZE(buf));

        // Menu Bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                    // char const * lFilterPatterns[2] = { "*.txt", "*.text" };

                    auto current_path = std::filesystem::current_path() / "../media/";

                    const char * file_path = tinyfd_openFileDialog(
                        "Select file", // title
                        current_path.c_str(),
                        0, // number of filter patterns
                        NULL, // char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
                        NULL, // optional filter description
                        0 // forbids multiple selections
                    );
                    if (file_path == nullptr) opened_file_path = "";
                    else opened_file_path = std::string(file_path);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Performance
        ImGui::SeparatorText("Performance");
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif /* imgui_h */
