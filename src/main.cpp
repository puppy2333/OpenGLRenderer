//
//  main.cpp
//  opengl_test
//
//  Created by 周宣辰 on 2024/2/9.
//

// OpenGL settings
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Load images (texture)
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

// cpp std
#include <iostream>
#include <map>
#include <memory>
#include <filesystem>
#include <random>

// UI (ref. https://github.com/ocornut/imgui)
#include <imui/imgui.h>
#include <imui/imgui_impl_glfw.h>
#include <imui/imgui_impl_opengl3.h>

// My files
#include "model.h"
#include "camera.h"
#include "shader_s.h"
#include "objects.h"
#include "light.h"
#include "const.h"
#include "texture.h"
#include "callbacks.h"
#include "utils.h"
//#include "globjects.h"
#include "myimgui.h"
#include "json.h"

int main()
{
    // glfw: initialize and configure
    glfwInit();

    // macOS only supports up to OpenGL 4.1
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation. Bugs about windows size exist in macOS,
    // see https://stackoverflow.com/questions/35715579/opengl-created-window-size-twice-as-large
    // ------------------------------------------------------------------------------------------
#ifdef __APPLE__
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH + UI_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    // GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
#else
    GLFWwindow* window = glfwCreateWindow(2 * SCR_WIDTH, 2 * SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
#endif
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 2 * SCR_WIDTH, 2 * SCR_HEIGHT);

    // Setup Dear ImGui context
    // ------------------------
    MyImgui myimgui(window, false, 0);
    
    // Set prefix
    // ----------
#ifdef __APPLE__
    std::filesystem::path prefix = std::filesystem::path("../");
    // For Xcode project
    // std::string prefix = "/Users/zhouxch/Playground/opengl_skybox/opengl_test/";
#else
//    std::string prefix = "D:/Learn_OpenGL/";
    std::string prefix = "../";
#endif

    // Set camera
    ourcamera.SetProjectMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
    
    // build and compile our shader programs
    // -------------------------------------
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;

    Shader lightshader(prefix / "shader" / "lightshader.vs", prefix / "shader" / "lightshader.fs");
    Shader screenshader(prefix / "shader" / "screenshader.vs", prefix / "shader" / "screenshader.fs");
    Shader depthmapshader(prefix / "shader" / "shadow_map" / "depthmapshader.vs", prefix / "shader" / "shadow_map" / "depthmapshader.fs");
    Shader blinnphongshader_shadow(prefix / "shader" / "blinnphongshader_shadow.vs", prefix / "shader" / "blinnphongshader_shadow.fs");
    Shader gbuffershader(prefix / "shader" / "gbuffershader.vs", prefix / "shader" / "gbuffershader.fs");
    Shader deferredrendershader(prefix / "shader" / "deferredrendershader.vs", prefix / "shader" / "deferredrendershader.fs");
    Shader objshader(prefix / "shader" / "objshader.vs", prefix / "shader" / "objshader.fs");

    // Shallow water equation, simulation
    Shader fluidsimulationshader(prefix / "shader" / "fluidsimulationshader.vs", prefix / "shader" / "fluidsimulationshader.fs");
    Shader swe_init_shader(prefix / "shader" / "swe" / "simulation" / "swe_shader.vs", prefix / "shader" / "swe" / "simulation" / "swe_init_shader.fs");
    Shader swe_v_advect_shader(prefix / "shader" / "swe" / "simulation" / "swe_shader.vs", prefix / "shader" / "swe" / "simulation" / "swe_v_advect_shader.fs");
    Shader swe_h_int_shader(prefix / "shader" / "swe" / "simulation" / "swe_shader.vs", prefix / "shader" / "swe" / "simulation" / "swe_h_int_shader.fs");
    Shader swe_v_int_shader(prefix / "shader" / "swe" / "simulation" / "swe_shader.vs", prefix / "shader" / "swe" / "simulation" / "swe_v_int_shader.fs");
    Shader swe_writebuffer_shader(prefix / "shader" / "swe" / "simulation" / "swe_shader.vs", prefix / "shader" / "swe" / "simulation" / "swe_writebuffer_shader.fs");

    // Shallow water equation, rendering
    Shader heightshader(prefix / "shader" / "swe" / "rendering" / "heightshader.vs", prefix / "shader" / "swe" / "rendering" / "heightshader_phong.fs");

    // Screen space ambient occlusion shader
    Shader ssaoshader(prefix / "shader" / "ssao" / "ssaoshader.vs", prefix / "shader" / "ssao" / "ssaoshader.fs");
    Shader ssaoblurshader(prefix / "shader" / "ssao" / "ssaoblurshader.vs", prefix / "shader" / "ssao" / "ssaoblurshader.fs");
    Shader inv_ssaoshader(prefix / "shader" / "ssao" / "inv_ssaoshader.vs", prefix / "shader" / "ssao" / "inv_ssaoshader.fs");

    // Screen space scattering shader
    Shader sss_shader(prefix / "shader" / "sss" / "sss_shader.vs", prefix / "shader" / "sss" / "sss_shader.fs");
    
    // Physics based rendering shader
    Shader pbr_shader(prefix / "shader" / "pbr" / "pbr.vs", prefix / "shader" / "pbr" / "pbr.fs");

    // Determine light position
    // ------------------------
    // Light light("light", glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(-4.0f, 8.0f, -6.0f));
    Light light("light", glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.0f, 0.0f, 5.0f));
    blinnphongshader_shadow.use();
    blinnphongshader_shadow.setVec3f("lightPos", light.Position);
    heightshader.use();
    heightshader.setVec3f("lightPos", light.Position);
    
    // Generate texture
    // ----------------
    unsigned int texture_cube = genTexture(prefix / "media" / "materials" / "container2.png", GL_CLAMP_TO_EDGE);
    // unsigned int texture_floor = genTexture(prefix + "media/material/wall.jpg", GL_REPEAT);
    unsigned int texture_floor = genTexture(prefix / + "media" / "materials" / "Asphalt031_4K-JPG/Asphalt031_4K-JPG_Color.jpg", GL_REPEAT);

    // Generate objects
    // ----------------
    Objects objects; // Should use this class to represent all the objects
    Cubes cubes;
    Quads quads;
    
    // High cube
    glm::mat4 cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -0.5f, -3.0f));
    cube_model = glm::scale(cube_model, glm::vec3(1.0f, 2.0f, 1.0f));
    cubes.addObject(cube_model, texture_cube);
    
    // cube
    cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.5f, -4.0f));
    // cubes.addObject(cube_model, texture_cube, true, true);
    
    // subsurface scattering cube
    cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.0f, 8.0f));
    // cubes.addObject(cube_model, texture_cube, true, true);
    
    // Light 2
    cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 1.0f, 7.0f));
    cubes.addObject(cube_model, texture_cube, true, true);
    
    // light
    cube_model = glm::translate(glm::mat4(1.0f), light.Position);
    cube_model = glm::scale(cube_model, glm::vec3(0.5f, 0.5f, 0.5f));
    cubes.addObject(cube_model, 0, false);
    
    // floor
    glm::mat4 floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    floor_model = glm::rotate(floor_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floor_model = glm::scale(floor_model, glm::vec3(10.0f, 10.0f, 10.0f));
    quads.addObject(floor_model, texture_floor);
    
    // mirror
    // glm::mat4 mirror_model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, -0.95f, 1.0f));
    glm::mat4 mirror_model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.7f, -0.95f, 1.0f));
    mirror_model = glm::rotate(mirror_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mirror_model = glm::scale(mirror_model, glm::vec3(4.0f, 4.0f, 4.0f));
    quads.addObject(mirror_model, 0, true, true);
    
    // Fluid height
    glm::mat4 height_model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -5.0f));
    height_model = glm::rotate(height_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    height_model = glm::scale(height_model, glm::vec3(1.0f, 1.0f, 1.0f));
    quads.addObject(height_model, 0);
    
    // Mesh
    //Meshes meshes(1, 1, 0.05);
    Meshes meshes(1, 1, 0.01);
    glm::mat4 mesh_model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -5.0f));
    mesh_model = glm::rotate(mesh_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mesh_model = glm::scale(mesh_model, glm::vec3(6.0f, 6.0f, 6.0f));
    meshes.addObject(mesh_model, 0);

    // Sphere
    Spheres spheres;
    for (int x = 0; x < 10; x+=2) {
        for (int y = 0; y < 10; y+=2) {
            glm::mat4 sphere_model = glm::translate(glm::mat4(1.0f), glm::vec3(x-5, y-5, 0.0f));
            spheres.addObject(sphere_model, 0);
        }
    }
    
    // load models
    // -----------
    std::vector<Model> models;
//    models.emplace_back(prefix + "media/Sponza/glTF/Sponza.gltf");
    // stbi_set_flip_vertically_on_load(true);
//    Model ourModel(prefix + "media/medieval_town/medieval_house_1/scene.gltf");
//    Model ourModel(prefix + "media/Sponza/glTF/Sponza.gltf");
    //Model ourModel(prefix + "media/sculpture/the_thinker_by_auguste_rodin/scene.gltf");

    ModelData model_data = load_json();
    models.emplace_back(prefix.string() + model_data.gltf_path);

    // Generate sample kernel for ssao
    // -–-----------------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 0.9 + 0.1
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    
    // Then randomly rotate the sample kernel
    // -–------------------------------------
    std::vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }
    GLuint noiseTexture = gen44RandomBuffer(ssaoNoise);
    
    // Shadow map
    // ----------
    unsigned int FBO_depthmap;
    glGenFramebuffers(1, &FBO_depthmap);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_depthmap);
        unsigned int texture_depth_framebuffer = genFrameBufferDepthTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_depth_framebuffer, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // G-Buffer
    // --------
    GLuint gBuffer = -1;
    GLuint gPosition = 0, gNormal = 0, gAlbedoSpec = 0, gShadow = 0;
    GLuint attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        // Position buffer
        gPosition = genGBufferRGBA16FTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        // Normal buffer
        gNormal = genGBufferRGBA16FTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        // Texture and specular value
        gAlbedoSpec = genGBufferRGBATexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
        // Texture and specular value
        gShadow = genGBufferRGBA32FTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gShadow, 0);
        // Tell OpenGL we are using color 123 to render
        glDrawBuffers(4, attachments);
        // Depth render buffer
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2 * SCR_WIDTH, 2 * SCR_HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Generate ssao & ssao_blur buffer
    // --------------------
    GLuint ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    GLuint ssaoColorBuffer = genGBufferRed16FTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    
    GLuint ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    //GLuint ssaoColorBufferBlur = genGBufferRGBATexture();
    GLuint ssaoColorBufferBlur = genGBufferRed16FTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    
    // Create SWE buffer1
    // ------------------
    GLuint sweFBO1;
    glGenFramebuffers(1, &sweFBO1);
    glBindFramebuffer(GL_FRAMEBUFFER, sweFBO1);
    GLuint sweBuffer1 = genGBufferSWETexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sweBuffer1, 0);
    
    // Create SWE buffer2
    // ------------------
    GLuint sweFBO2;
    glGenFramebuffers(1, &sweFBO2);
    glBindFramebuffer(GL_FRAMEBUFFER, sweFBO2);
    GLuint sweBuffer2 = genGBufferSWETexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sweBuffer2, 0);
    
    // Create height map buffer
    // ------------------------
    GLuint heightFBO;
    glGenFramebuffers(1, &heightFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, heightFBO);
    GLuint heightBuffer = genGBufferHeightTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightBuffer, 0);
    
    // OpenGL tests
    // ---------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    
    // Shader properties
    // -----------------
    screenshader.use();
    screenshader.setInt("screenTexture", 0);
    // -----------------
    glm::mat4 lightProjection = glm::perspective((float)glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 1.0f, 40.0f);
    glm::mat4 lightView = glm::lookAt(light.Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    depthmapshader.use();
    depthmapshader.setMat4f("lightProjection", lightProjection);
    depthmapshader.setMat4f("lightView", lightView);
    // -----------------
    blinnphongshader_shadow.use();
    blinnphongshader_shadow.setInt("diffuseTexture", 0);
    blinnphongshader_shadow.setInt("shadowMap", 1);
    blinnphongshader_shadow.setMat4f("lightProjection", lightProjection);
    blinnphongshader_shadow.setMat4f("lightView", lightView);
    // -----------------
    gbuffershader.use();
    gbuffershader.setInt("texture_diffuse1", 0);
    gbuffershader.setInt("shadowMap", 1);
    gbuffershader.setVec3f("lightPos", light.Position);
    gbuffershader.setMat4f("lightProjection", lightProjection);
    gbuffershader.setMat4f("lightView", lightView);
    // -----------------
    deferredrendershader.use();
    deferredrendershader.setInt("gPosition", 0);
    deferredrendershader.setInt("gNormal", 1);
    deferredrendershader.setInt("gAlbedoSpec", 2);
    deferredrendershader.setInt("gShadow", 3);
    deferredrendershader.setInt("ssaoColorBufferBlur", 4);
    // -----------------
    objshader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    objshader.setMat4f("model", model);
    glm::mat4 projection = ourcamera.GetProjectMatrix();
    objshader.setMat4f("projection", projection);
    // -----------------
    ssaoshader.use();
    ssaoshader.setInt("gPosition", 0);
    ssaoshader.setInt("gNormal", 1);
    ssaoshader.setInt("gShadow", 2);
    ssaoshader.setInt("noiseTexture", 3);
    // Send kernel
    for (GLuint i = 0; i < 64; ++i) {
        ssaoshader.setVec3f("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }
    ssaoshader.setMat4f("projection", projection);
    // -----------------
    inv_ssaoshader.use();
    inv_ssaoshader.setInt("gPosition", 0);
    inv_ssaoshader.setInt("gNormal", 1);
    inv_ssaoshader.setInt("gShadow", 2);
    inv_ssaoshader.setInt("noiseTexture", 3);
    // Send kernel
    for (GLuint i = 0; i < 64; ++i) {
        inv_ssaoshader.setVec3f("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }
    inv_ssaoshader.setMat4f("projection", projection);
    // -----------------
    pbr_shader.use();
    pbr_shader.setVec3f("light_pos", light.Position);
    pbr_shader.setVec3f("light_color", glm::vec3(100.0f, 100.0f, 100.0f));
    // Set material properties
    pbr_shader.setVec3f("albedo", glm::vec3(1.0f, 0.0f, 0.0f));
    pbr_shader.setFloat("ao", 1.0f);
    
    // glViewport(0, 0, 2 * SCR_WIDTH, 2 * SCR_HEIGHT);

    // Lambda function of rendering to gbuffer
    // ---------------------------------------
    auto renderToGbuffer = [&]() {
        // Render to GBuffer
        // -----------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glEnable(GL_DEPTH_TEST);
        // Setting g=1.0f is to init depth in gbuffer to 1.0f (farthest)
        glClearColor(0.2f, 1.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gbuffershader.use();
        gbuffershader.setVec3f("viewPos", ourcamera.Position);
        gbuffershader.setInt("imgui_shadowtype", myimgui.shadowtype);
        glm::mat4 view = ourcamera.GetViewMatrix();
        
        gbuffershader.setMVP(cubes.models[0], view);
        
        // Render cube
        for (int i = 0; i < cubes.num; i++) {
            if (cubes.textures[i] > 0) {
                gbuffershader.setModelMat(cubes.models[i]);
                gbuffershader.setBool("is_mirror", cubes.ismirror[i]);
                if (cubes.textures[i] > 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, cubes.textures[i]);
                }
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
                cubes.render();
            }
        }
        
        // Render floor
        for (int i = 0; i < quads.num; i++) {
            gbuffershader.setModelMat(quads.models[i]);
            gbuffershader.setBool("is_mirror", cubes.ismirror[i]);
            if (quads.textures[i] > 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, quads.textures[i]);
            }
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            quads.render();
        }
        
        // Render loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, model_data.translate);
        model = glm::scale(model, model_data.scale);
        gbuffershader.setModelMat(model);
        gbuffershader.setBool("is_mirror", false);
        for (Model m: models) {
            m.Draw(gbuffershader);
        }
    };
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        glm::mat4 view = ourcamera.GetViewMatrix();
        
        // Shadow
        // ------
        if (myimgui.shadowtype != 0) {
            // Render shadow map to frame buffer
            // ---------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, FBO_depthmap);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
        
            // Render cube
            depthmapshader.use();
            for (int i = 0; i < cubes.num; i++) {
                if (cubes.cast_shadow[i]) {
                    depthmapshader.setMat4f("model", cubes.models[i]);
                    cubes.render();
                }
            }
            
            // render the loaded model
            glm::mat4 model = glm::translate(glm::mat4(1.0f), model_data.translate);
            //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            model = glm::scale(model, model_data.scale);
            depthmapshader.setMat4f("model", model);
            for (Model m: models) {
                m.Draw(depthmapshader);
            }
            
            // Render floor
            for (int i = 0; i < quads.num; i++) {
                depthmapshader.setMat4f("model", quads.models[i]);
                quads.render();
            }
        }
        
        if (myimgui.ssao && myimgui.rendertype != 2) {
            renderToGbuffer();
            
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST); // Very important
            ssaoshader.use();
            ssaoshader.setViewMat(view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gShadow);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            quads.render();
            
            // 3. blur SSAO texture to remove noise
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            // glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            ssaoblurshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            quads.render();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        // Normal rendering
        // ----------------
        if (myimgui.rendertype == 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glEnable(GL_DEPTH_TEST);

            blinnphongshader_shadow.setMVP(cubes.models[0], view);
            blinnphongshader_shadow.setVec3f("viewPos", ourcamera.Position);
            blinnphongshader_shadow.setInt("imgui_shadowtype", myimgui.shadowtype);
            
            // Render cube
            for (int i = 0; i < cubes.num; i++) {
                if (cubes.textures[i] > 0) {
                    blinnphongshader_shadow.setModelMat(cubes.models[i]);
                    if (cubes.textures[i] > 0) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, cubes.textures[i]);
                    }
                    //glActiveTexture(GL_TEXTURE0);
                    //glBindTexture(GL_TEXTURE_2D, cubes.textures[i]);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
                    cubes.render();
                }
                else {
                    lightshader.setMVP(cubes.models[2], view);
                    cubes.render();
                }
            }
            
            // Render floor
            for (int i = 0; i < quads.num; i++) {
                blinnphongshader_shadow.setMVP(quads.models[i], view);
                if (quads.textures[i] > 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, quads.textures[i]);
                }
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
                quads.render();
            }
            
            // render the loaded model
            blinnphongshader_shadow.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, model_data.translate);
            //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));    // it's a bit too big for our scene, so scale it down
            model = glm::scale(model, model_data.scale);
            //model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
            blinnphongshader_shadow.setMVP(model, view);
            for (Model m: models) {
                m.Draw(blinnphongshader_shadow);
            }
        }
        // Deferred rendering
        // ------------------
        else if (myimgui.rendertype == 1) {
            renderToGbuffer();
    
            // Render to screen
            // ----------------
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            deferredrendershader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, gShadow);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            // send light relevant uniforms
            deferredrendershader.setVec3f("lightPos", light.Position);
            deferredrendershader.setVec3f("viewPos", ourcamera.Position);
            glm::mat4 vpmat = projection * view;
            deferredrendershader.setMat4f("VPMatrix", vpmat);
            deferredrendershader.setInt("numray", myimgui.numray);
            deferredrendershader.setBool("AO", myimgui.ssao);
            
            // finally render quad
            quads.render();
        }
        // Create SSAO texture
        // -------------------
        else if (myimgui.rendertype == 2) {
            renderToGbuffer();
            
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST); // Very important
            ssaoshader.use();
            ssaoshader.setViewMat(view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gShadow);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            quads.render();
            
            // 3. blur SSAO texture to remove noise
            // glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            ssaoblurshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            quads.render();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        // DEBUG: visualize depth map from light
        // -------------------------------------
        else if (myimgui.rendertype == 3) {
            // Shadowmap must be rendered before visualization
            assert(myimgui.shadowtype != 0);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            screenshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            
            // finally render quad
            quads.render();
        }
        // DEBUG mesh: render a curve
        // --------------------------
        else if (myimgui.rendertype == 4) {
            
            // Render floor
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            
            blinnphongshader_shadow.setMVP(cubes.models[0], view);
            blinnphongshader_shadow.setVec3f("viewPos", ourcamera.Position);
            blinnphongshader_shadow.setInt("imgui_shadowtype", myimgui.shadowtype);
            
            blinnphongshader_shadow.setMVP(quads.models[0], view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, quads.textures[0]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            quads.render();
            
            // Render fluid surface
            glBindFramebuffer(GL_FRAMEBUFFER, heightFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            fluidsimulationshader.use();
            quads.render();
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            heightshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, heightBuffer);
            heightshader.setMVP(meshes.models[0], view);
            meshes.render();
        }
        // Shallow water equation
        // ----------------------
        else if (myimgui.rendertype == 5) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            
            blinnphongshader_shadow.setMVP(cubes.models[0], view);
            blinnphongshader_shadow.setVec3f("viewPos", ourcamera.Position);
            blinnphongshader_shadow.setInt("imgui_shadowtype", myimgui.shadowtype);
            
            // Render floor
            blinnphongshader_shadow.setMVP(quads.models[0], view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, quads.textures[0]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            quads.render();

            // Render floor (deffered rendering) (prepare to be used in refraction of SWE surface)
//            for (int i = 0; i < quads.num; i++) {
//                gbuffershader.setModelMat(quads.models[i]);
//                gbuffershader.setBool("is_mirror", cubes.ismirror[i]);
//                if (quads.textures[i] > 0) {
//                    glActiveTexture(GL_TEXTURE0);
//                    glBindTexture(GL_TEXTURE_2D, quads.textures[i]);
//                }
//                glActiveTexture(GL_TEXTURE1);
//                glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
//                quads.render();
//            }

            // SWE initialization (periodically)
            // ---------------------------------
            // if (myimgui.swe_tick_count % 100 == 0) {
            //     glBindFramebuffer(GL_FRAMEBUFFER, sweFBO2);
            //     glClear(GL_COLOR_BUFFER_BIT);
            //     glDisable(GL_DEPTH_TEST);
            //     swe_init_shader.use();
            //     glActiveTexture(GL_TEXTURE0);
            //     glBindTexture(GL_TEXTURE_2D, sweBuffer1);
            //     quads.render();
            // }
            // myimgui.swe_tick_count++;

            // SWE initialization (by keyboard)
            // --------------------------------
            if (init_wave) {
                glBindFramebuffer(GL_FRAMEBUFFER, sweFBO2);
                glClear(GL_COLOR_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);
                swe_init_shader.use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sweBuffer1);
                quads.render();
                init_wave = false;
            }

            // SWE simulation
            // --------------

            // SWE advect
            glBindFramebuffer(GL_FRAMEBUFFER, sweFBO1);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            swe_v_advect_shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sweBuffer2);
            quads.render();

            // SWE height integration
            glBindFramebuffer(GL_FRAMEBUFFER, sweFBO2);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            swe_h_int_shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sweBuffer1);
            quads.render();

            // SWE velocity integration
            glBindFramebuffer(GL_FRAMEBUFFER, sweFBO1);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            swe_v_int_shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sweBuffer2);
            quads.render();
            
            // Swap buffers
            glBindFramebuffer(GL_FRAMEBUFFER, sweFBO2);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            swe_writebuffer_shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sweBuffer1);
            quads.render();
            
            // SWE rendering
            // -------------
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            heightshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sweBuffer2);
            heightshader.setMVP(meshes.models[0], view);
            heightshader.setVec3f("viewPos", ourcamera.Position);
            meshes.render();
        }
        // Inversed SSAO
        // -------------
        else if (myimgui.rendertype == 6) {
            renderToGbuffer();
            
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST); // Very important
            inv_ssaoshader.use();
            inv_ssaoshader.setViewMat(view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gShadow);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            quads.render();
            
            // 3. blur SSAO texture to remove noise
            // glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            ssaoblurshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            quads.render();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        // Subsurface scattering
        // ---------------------
        else if (myimgui.rendertype == 7) {
            renderToGbuffer();
            
            // Generate SSAO texture
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST); // Very important
            inv_ssaoshader.use();
            inv_ssaoshader.setViewMat(view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gShadow);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            quads.render();
            
            // Blur SSAO texture to remove noise
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            ssaoblurshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            quads.render();
            
            // Rendering floor
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            blinnphongshader_shadow.setMVP(quads.models[0], view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, quads.textures[0]);
            quads.render();
            
            // Rendering light
            lightshader.setMVP(cubes.models[3], view);
            cubes.render();
            
            // Rendering a cube
            sss_shader.setMVP(cubes.models[2], view);
            sss_shader.setVec3f("lightPos", glm::vec3(7.0f, 1.0f, 7.0f));
            sss_shader.setVec3f("viewPos", ourcamera.Position);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            cubes.render();
        }
        // Physically based rendering
        // --------------------------
        else if (myimgui.rendertype == 8) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glEnable(GL_DEPTH_TEST);
            
            pbr_shader.use();
            pbr_shader.setVec3f("cam_pos", ourcamera.Position);
            
            // Render sphere
            for (int x = 0; x < 5; x++) {
                for (int y = 0; y < 5; y++) {
                    pbr_shader.setMVP(spheres.models[y * 5 + x], view);
                    pbr_shader.setFloat("metallic", (float)(x+1) / 5.0f);
                    pbr_shader.setFloat("roughness", (float)(y+1) / 5.0f);
                    spheres.render();
                }
            }
        }
        // Start the Dear ImGui frame
        // --------------------------
        myimgui.newframe();

        if (!myimgui.opened_file_path.empty()) {
            // An object can be placed here
            models.emplace_back(myimgui.opened_file_path);
            myimgui.opened_file_path.clear();
        }
        if (myimgui.camera_moved) {
            ourcamera.Position.x = myimgui.camera_position[0];
            ourcamera.Position.y = myimgui.camera_position[1];
            ourcamera.Position.z = myimgui.camera_position[2];
            myimgui.camera_moved = false;
        }
        if (myimgui.camera_yaw_moved) {
            ourcamera.Yaw = myimgui.camera_yaw;
            myimgui.camera_yaw_moved = false;
            ourcamera.updateCameraVectors();
        }
        if (myimgui.camera_pitch_moved) {
            ourcamera.Pitch = myimgui.camera_pitch;
            myimgui.camera_pitch_moved = false;
            ourcamera.updateCameraVectors();
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    lightshader.del();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}
