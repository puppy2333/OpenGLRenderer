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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// cpp std
#include <iostream>
#include <map>
#include <memory>

// UI (ref. https://github.com/ocornut/imgui)
#include <imui/imgui.h>
#include <imui/imgui_impl_glfw.h>
#include <imui/imgui_impl_opengl3.h>

// My files
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
    
    // Determine light position
    // ------------------------
    Light light("light", glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(-4.0f, 8.0f, -2.0f));
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
    
    glm::mat4 cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cube_model = glm::scale(cube_model, glm::vec3(1.0f, 2.0f, 1.0f));
    cubes.addObject(cube_model, texture_cube);
    
    cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.5f, 2.0f));
    cubes.addObject(cube_model, texture_cube);
    
    cube_model = glm::translate(glm::mat4(1.0f), light.Position);
    cube_model = glm::scale(cube_model, glm::vec3(0.5f, 0.5f, 0.5f));
    cubes.addObject(cube_model, texture_cube);
    
    glm::mat4 floor_model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floor_model =glm::translate(floor_model, glm::vec3(0.0f, 0.0f, 6.0f));
    floor_model = glm::scale(floor_model, glm::vec3(10.0f, 10.0f, 10.0f));
    quads.addObject(floor_model, texture_floor);
    
    std::vector<std::string> faces {
        prefix + "media/skybox/right.jpg",
        prefix + "media/skybox/left.jpg",
        prefix + "media/skybox/top.jpg",
        prefix + "media/skybox/bottom.jpg",
        prefix + "media/skybox/front.jpg",
        prefix + "media/skybox/back.jpg"
    };
    //unsigned int texture_cubemap = genCubeMapTexture(faces);
    
    // transparent window locations
    // ----------------------------
    std::vector<glm::vec3> windows {
        glm::vec3(-1.5f, 0.0f, -1.48f),
        glm::vec3( 1.5f, 0.0f, 1.51f),
        glm::vec3( 0.0f, 0.0f, 1.7f),
        glm::vec3(-0.3f, 0.0f, -3.3f),
        glm::vec3( 0.5f, 0.0f, -1.6f)
    };
    
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
    GLuint gPosition, gNormal, gAlbedoSpec;
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    if (myimgui.ssr) {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        // Position buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // Normal buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        // Texture and specular value
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        // Tell OpenGL we are using color 123 to render
        glDrawBuffers(3, attachments);
        
    }
    
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
    blinnphongshader_shadow.use();
    blinnphongshader_shadow.setInt("diffuseTexture", 0);
    blinnphongshader_shadow.setInt("shadowMap", 1);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        // 1. Render shadow map to frame buffer
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_depthmap);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
    
        // Configure depth map
        glm::mat4 lightProjection = glm::perspective((float)glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 1.0f, 100.0f);
        glm::mat4 lightView = glm::lookAt(light.Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Render cube
        depthmapshader.use();
        depthmapshader.setMat4f("lightProjection", lightProjection);
        depthmapshader.setMat4f("lightView", lightView);
        depthmapshader.setMat4f("model", cubes.models[0]);
        cubes.render();
    
        // Render cube2
        depthmapshader.setMat4f("model", cubes.models[1]);
        cubes.render();
    
        // Render floor
        depthmapshader.setMat4f("model", quads.models[0]);
        quads.render();
        
        // Render to screen
        // ----------------
        if (not myimgui.ssr) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glEnable(GL_DEPTH_TEST);

            // Render cube
            glm::mat4 view = ourcamera.GetViewMatrix();
            blinnphongshader_shadow.setMVP(cubes.models[0], view);
            blinnphongshader_shadow.setMat4f("lightProjection", lightProjection);
            blinnphongshader_shadow.setMat4f("lightView", lightView);
            blinnphongshader_shadow.setVec3f("viewPos", ourcamera.Position);
            blinnphongshader_shadow.setInt("imgui_shadowtype", myimgui.imgui_shadowtype);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_cube);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            cubes.render();
            
            // Render cube2
            blinnphongshader_shadow.setMVP(cubes.models[1], view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_cube);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            cubes.render();
            
            // Render light
            lightshader.setMVP(cubes.models[2], view);
            cubes.render();
            
            // Render floor
            blinnphongshader_shadow.setMVP(quads.models[0], view);
            glActiveTexture(GL_TEXTURE0); // bind floor texture
            glBindTexture(GL_TEXTURE_2D, texture_floor);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            quads.render();
        }
        // 2. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        else {
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glm::mat4 view = ourcamera.GetViewMatrix();
            
            // Render cube
            blinnphongshader_shadow.setMVP(cubes.models[0], view);
            blinnphongshader_shadow.setMat4f("lightProjection", lightProjection);
            blinnphongshader_shadow.setMat4f("lightView", lightView);
            blinnphongshader_shadow.setVec3f("viewPos", ourcamera.Position);
            blinnphongshader_shadow.setInt("imgui_shadowtype", myimgui.imgui_shadowtype);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_cube);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            cubes.render();
            
            // Render cube2
            blinnphongshader_shadow.setMVP(cubes.models[1], view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_cube);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            cubes.render();
            
            // Render light
            lightshader.setMVP(cubes.models[2], view);
            cubes.render();
            
            // Render floor
            blinnphongshader_shadow.setMVP(quads.models[0], view);
            glActiveTexture(GL_TEXTURE0); // bind floor texture
            glBindTexture(GL_TEXTURE_2D, texture_floor);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
            quads.render();
            
            // Draw
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        
        
        
        // Start the Dear ImGui frame
        // --------------------------
        myimgui.newframe();
        
        // MVP matrixes
        // ------------
        //glm::mat4 model = glm::mat4(1.0f);
        //glm::mat4 view = ourcamera.GetViewMatrix();
        //glm::mat4 projection = glm::perspective((float)glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 1.0f, 200.0f);
        
//            // Cube
//            // ----
//            model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
//            cubeshader.setMVP(model, view);
//            cubeshader.setVec3f("viewPos", ourcamera.Position);
//            glBindVertexArray(VAO_cube);
//            glActiveTexture(GL_TEXTURE0); // bind diffuse map
//            glBindTexture(GL_TEXTURE_2D, texture_cube);
//            glActiveTexture(GL_TEXTURE1); // bind specular map
//            glBindTexture(GL_TEXTURE_2D, texture_cube_specular);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//
//            // Light
//            // -----
//            model = glm::translate(glm::mat4(1.0f), light.Position);
//            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
//            lightshader.setMVP(model, view);
//            glBindVertexArray(VAO_cube);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//
//            // Floor
//            // -----
//            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
//            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//            model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
//            floorshader.setMVP(model, view);
//            floorshader.setVec3f("viewPos", ourcamera.Position);
//            glBindVertexArray(VAO_square);
//            glActiveTexture(GL_TEXTURE0); // bind floor texture
//            glBindTexture(GL_TEXTURE_2D, texture_floor);
//            glDrawArrays(GL_TRIANGLES, 0, 6);
 
        // Skybox
        // ------
//        glDepthFunc(GL_LEQUAL);
//        cubemapshader.use();
//        glm::mat4 view_cubemap = glm::mat4(glm::mat3(view));
//        cubemapshader.setMat4f("view", view_cubemap);
//        cubemapshader.setMat4f("projection", projection);
//        glBindVertexArray(VAO_cubemap);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        glDepthFunc(GL_LESS);
        
        // Reflect object
        // --------------
//        model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.5f, 0.0f));
//        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
//        reflectshader.setMVP(model, view);
//        reflectshader.setVec3f("cameraPos", ourcamera.Position);
//        glBindVertexArray(VAO_cube);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Refract object
        // --------------
//        model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.5f, 0.0f));
//        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
//        refractshader.setMVP(model, view);
//        refractshader.setVec3f("cameraPos", ourcamera.Position);
//        glBindVertexArray(VAO_cube);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Grass
        // -----
//        model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//        cubeshader.setMVP(model, view);
//        cubeshader.setVec3f("viewPos", ourcamera.Position);
//        glBindVertexArray(VAO_square);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture_grass);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Window
        // sort the transparent windows before rendering
        // ---------------------------------------------
//        transparentshader.setMVP(model, view);
//        transparentshader.setVec3f("viewPos", ourcamera.Position);
//        glBindVertexArray(VAO_square);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture_window);
//        
//        std::map<float, glm::vec3> sorted;
//        for (unsigned int i = 0; i < windows.size(); i++) {
//            float distance = glm::length(ourcamera.Position - windows[i]);
//            sorted[distance] = windows[i];
//        }
//        
//        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, it->second);
//            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//            transparentshader.setMat4f("model", model);
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//        }
        
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

void codebuffer()
{
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    screenshader.use();
//    glBindVertexArray(VAO_square);
//    glDisable(GL_DEPTH_TEST);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
}
