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

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    // ------------------------
    MyImgui myimgui(window, false, 0);
    
    // Set prefix
    // ----------
    std::string prefix = "/Users/zhouxch/Playground/opengl_skybox/opengl_test/";
    
    // build and compile our shader programs
    // -------------------------------------
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    Shader cubeshader(prefix + "shader/cubeshader.vs", prefix + "shader/cubeshader.fs");
    Shader floorshader(prefix + "shader/floorshader.vs", prefix + "shader/floorshader.fs");
    Shader lightshader(prefix + "shader/lightshader.vs", prefix + "shader/lightshader.fs");
    Shader transparentshader(prefix + "shader/transparent_shader.vs", prefix + "shader/transparent_shader.fs");
    Shader cubemapshader(prefix + "/shader/skyboxshader.vs", prefix + "shader/skyboxshader.fs");
    Shader reflectshader(prefix + "/shader/reflectshader.vs", prefix + "shader/reflectshader.fs");
    Shader refractshader(prefix + "/shader/refractshader.vs", prefix + "shader/refractshader.fs");
    Shader screenshader(prefix + "/shader/screenshader.vs", prefix + "shader/screenshader.fs");
    Shader depthmapshader(prefix + "/shader/depthmapshader.vs", prefix + "shader/depthmapshader.fs");
    Shader blinnphongshader_shadow(prefix + "/shader/blinnphongshader_shadow.vs", prefix + "shader/blinnphongshader_shadow.fs");
    Shader gbuffershader(prefix + "/shader/gbuffershader.vs", prefix + "shader/gbuffershader.fs");
    Shader deferredrendershader(prefix + "/shader/deferredrendershader.vs", prefix + "shader/deferredrendershader.fs");
    Shader objshader(prefix + "/shader/objshader.vs", prefix + "/shader/objshader.fs");
    
    // Determine light position
    // ------------------------
    Light light("light", glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(-4.0f, 8.0f, -6.0f));
    light.shaderSetLight(cubeshader);
    light.shaderSetLight(floorshader);
    blinnphongshader_shadow.use();
    blinnphongshader_shadow.setVec3f("lightPos", light.Position);
    
    // Generate texture
    // ----------------
    unsigned int texture_cube = genTexture(prefix + "media/container2.png", GL_CLAMP_TO_EDGE);
    //unsigned int texture_cube_specular = genTexture(prefix + "media/container2_specular.png", GL_CLAMP_TO_EDGE);
    unsigned int texture_floor = genTexture(prefix + "media/wall.jpg", GL_REPEAT);
    //unsigned int texture_grass = genTexture(prefix + "media/grass.png", GL_CLAMP_TO_EDGE);
    //unsigned int texture_window = genTexture(prefix + "media/blending_transparent_window.png", GL_CLAMP_TO_EDGE);

    // Generate objects
    // ----------------
    Cubes cubes;
    Quads quads;
    
    // High cube
    //glm::mat4 cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 0.0f));
    glm::mat4 cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -0.5f, -3.0f));
    cube_model = glm::scale(cube_model, glm::vec3(1.0f, 2.0f, 1.0f));
    cubes.addObject(cube_model, texture_cube);
    
    // cube
    //cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -0.5f, 5.0f));
    cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.5f, -2.0f));
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
    glm::mat4 mirror_model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -0.9f, 1.0f));
    mirror_model = glm::rotate(mirror_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mirror_model = glm::scale(mirror_model, glm::vec3(4.0f, 4.0f, 4.0f));
    quads.addObject(mirror_model, 0, true, true);
    
    // load models
    // -----------
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);
    Model ourModel(prefix + "media/medieval_town/medieval_house_1/scene.gltf");
    
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
        gShadow = genGBufferRGBA16FTexture();
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
    
    // OpenGL tests
    // ---------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    
    // Shader properties
    // -----------------
    cubeshader.use();
    cubeshader.setInt("material.diffuse", 0);
    cubeshader.setInt("material.specular", 1);
    cubeshader.setFloat("material.shininess", 64.0f);
    // ---------------
    floorshader.use();
    floorshader.setInt("material.diffuse", 0);
    floorshader.setVec3f("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    floorshader.setFloat("material.shininess", 1.0f);
    // -----------------
    cubemapshader.use();
    cubemapshader.setInt("skybox", 0);
    // -----------------
    reflectshader.use();
    reflectshader.setInt("skybox", 0);
    // -----------------
    refractshader.use();
    refractshader.setInt("skybox", 0);
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
    // -----------------
    objshader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    objshader.setMat4f("model", model);
    glm::mat4 projection = glm::perspective((float)glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 40.0f);
    objshader.setMat4f("projection", projection);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        glm::mat4 view = ourcamera.GetViewMatrix();
        
        // 1. Render shadow map to frame buffer
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_depthmap);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
    
        // Render cube
        depthmapshader.use();
        for (int i = 0; i < cubes.num; i++) {
            if (cubes.cast_shadow[i] == true) {
                depthmapshader.setMat4f("model", cubes.models[i]);
                cubes.render();
            }
        }
        
        // render the loaded model
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        depthmapshader.setMat4f("model", model);
        ourModel.Draw(depthmapshader);
        
        // Render floor
        for (int i = 0; i < quads.num; i++) {
            depthmapshader.setMat4f("model", quads.models[i]);
            quads.render();
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
            model = glm::translate(model, glm::vec3(0.0f, -1.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));    // it's a bit too big for our scene, so scale it down
            blinnphongshader_shadow.setMVP(model, view);
            ourModel.Draw(blinnphongshader_shadow);
            
        }
        // Deferred rendering
        // ------------------
        else if (myimgui.rendertype == 1) {
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
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -1.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));    // it's a bit too big for our scene, so scale it down
            gbuffershader.setModelMat(model);
            gbuffershader.setBool("is_mirror", false);
            ourModel.Draw(gbuffershader);
    
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
            // send light relevant uniforms
            deferredrendershader.setVec3f("lightPos", light.Position);
            deferredrendershader.setVec3f("viewPos", ourcamera.Position);
            glm::mat4 vpmat = projection * view;
            deferredrendershader.setMat4f("VPMatrix", vpmat);
            deferredrendershader.setInt("numray", myimgui.numray);
            
            // finally render quad
            quads.render();
        }
        else if (myimgui.rendertype == 2) {
            // Render to screen
            // ----------------
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            screenshader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            
            // finally render quad
            quads.render();
        }
        
        // Start the Dear ImGui frame
        // --------------------------
        myimgui.newframe();
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    cubeshader.del();
    lightshader.del();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}
