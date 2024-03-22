//
//  main.cpp
//  opengl_test
//
//  Created by 周宣辰 on 2024/2/9.
//

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <map>
#include <memory>

#include "camera.h"
#include "shader_s.h"
#include "objects.h"
#include "light.h"
#include "const.h"
#include "texture.h"
#include "callbacks.h"
#include "utils.h"

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

    std::string prefix = "/Users/zhouxch/Playground/opengl_skybox/opengl_test/";
    
    // build and compile our shader programs
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
    
    // Determine light position
    // ------------------------
    Light light("light", glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(2.0, 2.0, 2.0));
    light.shaderSetLight(cubeshader);
    light.shaderSetLight(floorshader);
    
    // cubeVBO
    // -------
    float* vertices_cube = nullptr;
    getCubeWithUV(vertices_cube);
    
    unsigned int VBO_cube;
    glGenBuffers(1, &VBO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, 288 * sizeof(float), vertices_cube, GL_STATIC_DRAW);
    
    // cubeVAO
    // -------
    unsigned int VAO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glBindVertexArray(VAO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    // squareVBO
    // ---------
    float* vertices_square = nullptr;
    getSquare(vertices_square);
    
    unsigned int VBO_square;
    glGenBuffers(1, &VBO_square);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_square);
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), vertices_square, GL_STATIC_DRAW);
    
    // squareVAO
    // ---------
    unsigned int VAO_square;
    glGenVertexArrays(1, &VAO_square);
    glBindVertexArray(VAO_square);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    // Cubemap VAO, VBO
    // ----------------
    float* vertices_cubemap = nullptr;
    getCubeMap(vertices_cubemap);
    
    unsigned int VBO_cubemap;
    glGenBuffers(1, &VBO_cubemap);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cubemap);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), vertices_cubemap, GL_STATIC_DRAW);
    
    unsigned int VAO_cubemap;
    glGenVertexArrays(1, &VAO_cubemap);
    glBindVertexArray(VAO_cubemap);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Generate texture
    // ----------------
    unsigned int texture_cube, texture_cube_specular;
    genTexture(texture_cube, prefix + "media/container2.png", GL_CLAMP_TO_EDGE);
    genTexture(texture_cube_specular, prefix + "media/container2_specular.png", GL_CLAMP_TO_EDGE);
    
    unsigned int texture_floor;
    genTexture(texture_floor, prefix + "media/wall.jpg", GL_REPEAT);
    
    unsigned int texture_grass;
    genTexture(texture_grass, prefix + "media/grass.png", GL_CLAMP_TO_EDGE);
    
    unsigned int texture_window;
    genTexture(texture_window, prefix + "media/blending_transparent_window.png", GL_CLAMP_TO_EDGE);

    std::vector<std::string> faces {
        prefix + "media/skybox/right.jpg",
        prefix + "media/skybox/left.jpg",
        prefix + "media/skybox/top.jpg",
        prefix + "media/skybox/bottom.jpg",
        prefix + "media/skybox/front.jpg",
        prefix + "media/skybox/back.jpg"
    };
    unsigned int texture_cubemap;
    genCubeMapTexture(texture_cubemap, faces);
    
    // transparent window locations
    // ----------------------------
    std::vector<glm::vec3> windows {
        glm::vec3(-1.5f, 0.0f, -1.48f),
        glm::vec3( 1.5f, 0.0f, 1.51f),
        glm::vec3( 0.0f, 0.0f, 1.7f),
        glm::vec3(-0.3f, 0.0f, -3.3f),
        glm::vec3( 0.5f, 0.0f, -1.6f)
    };
    
    // Frame buffer
    // ------------
    unsigned int FBO_depthmap;
    glGenFramebuffers(1, &FBO_depthmap);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_depthmap);
        unsigned int texture_depth_framebuffer;
        genFrameBufferDepthTexture(texture_depth_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_depth_framebuffer, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
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
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        // 1. Render to frame buffer
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_depthmap);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
    
            // Configure depth map
            GLfloat near_plane = 1.0f, far_plane = 15.0f;
            glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 model = glm::mat4(1.0f);
            
            depthmapshader.use();
            depthmapshader.setMat4f("lightProjection", lightProjection);
            depthmapshader.setMat4f("lightView", lightView);
            depthmapshader.setMat4f("model", model);
        
            // Render cube
            glBindVertexArray(VAO_cube);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        
            // Render light
            model = glm::translate(glm::mat4(1.0f), light.Position);
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            depthmapshader.setMat4f("model", model);
            glBindVertexArray(VAO_cube);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        
            // Floor
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
            depthmapshader.setMat4f("model", model);
            glBindVertexArray(VAO_square);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        // 2. Render to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenshader.use();
        glBindVertexArray(VAO_square);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_depth_framebuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        

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
    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteVertexArrays(1, &VAO_square);
    glDeleteBuffers(1, &VBO_cube);
    glDeleteBuffers(1, &VBO_square);
    cubeshader.del();
    lightshader.del();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}
