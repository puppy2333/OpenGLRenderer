//
//  main.cpp
//  opengl_test
//
//  Created by 周宣辰 on 2024/2/9.
//

#define GL_SILENCE_DEPRECATION
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#include <iostream>
#include <map>

#include "camera.h"
#include "shader_s.h"
#include "objects.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera ourcamera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

void genTexture(unsigned int& texture, std::string path, GLenum handle_edge);
void genCubeMapTexture(unsigned int& texture, std::vector<std::string> faces);
void printMat4(glm::mat4 & matrix);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

    std::string prefix = "/Users/zhouxch/Playground/opengl_test/opengl_test/";
    
    // build and compile our shader program
    std::cout << std::filesystem::current_path();
    Shader cubeshader(prefix + "shader/cubeshader.vs", prefix + "shader/cubeshader.fs");
    Shader lightshader(prefix + "shader/lightshader.vs", prefix + "shader/lightshader.fs");
    Shader transparentshader(prefix + "shader/transparent_shader.vs", prefix + "shader/transparent_shader.fs");
    Shader cubemapshader(prefix + "/shader/skyboxshader.vs", prefix + "shader/skyboxshader.fs");
    
    // Setup vertices and indices
    float* vertices_cube = nullptr;
    getCubeWithUV(vertices_cube);
    
    float* vertices_square = nullptr;
    unsigned int* indices_square = nullptr;
    getSquare(vertices_square, indices_square);
    
    // cubeVBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 216 * sizeof(float), vertices_cube, GL_STATIC_DRAW);
    
    // cubeVAO
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    //glBindVertexArray(0);
    
    // lightVAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glBindVertexArray(0);
    
    // squareVBO
    unsigned int VBO_square;
    glGenBuffers(1, &VBO_square);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_square);
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), vertices_square, GL_STATIC_DRAW);
    
    // squareVAO
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
    
    // Bind texture
    unsigned int texture_cube, texture_cube_specular;
    genTexture(texture_cube, prefix + "media/container2.png", GL_CLAMP_TO_EDGE);
    genTexture(texture_cube_specular, prefix + "media/container2_specular.png", GL_CLAMP_TO_EDGE);
    
    unsigned int texture_floor;
    genTexture(texture_floor, prefix + "media/wall.jpg", GL_REPEAT);
    
    unsigned int texture_grass;
    genTexture(texture_grass, prefix + "media/grass.png", GL_REPEAT);
    
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
    std::vector<glm::vec3> windows
    {
        glm::vec3(-1.5f, 0.0f, -1.48f),
        glm::vec3( 1.5f, 0.0f, 1.51f),
        glm::vec3( 0.0f, 0.0f, 1.7f),
        glm::vec3(-0.3f, 0.0f, -3.3f),
        glm::vec3( 0.5f, 0.0f, -1.6f)
    };
    
    glEnable(GL_DEPTH_TEST);
    
    // Early-z
    glEnable(GL_DEPTH_CLAMP);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //glEnable(GL_CULL_FACE);
    
    // material properties
    cubeshader.use();
    cubeshader.setInt("material.diffuse", 0);
    cubeshader.setInt("material.specular", 1);
    cubeshader.setFloat("material.shininess", 64.0f);
    cubeshader.setVec3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    cubeshader.setVec3f("light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeshader.setVec3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    cubeshader.setVec3f("light.position", glm::vec3(3.0f, 3.0f, -3.0f));
    
    cubemapshader.use();
    cubemapshader.setInt("skybox", 0);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Projection
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
        glm::mat4 view = ourcamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective((float)glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 1.0f, 100.0f);
        
        // Cube
        cubeshader.use();
        cubeshader.setMat4f("model", model);
        cubeshader.setMat4f("view", view);
        cubeshader.setMat4f("projection", projection);
        cubeshader.setVec3f("viewPos", ourcamera.Position);
        glBindVertexArray(cubeVAO);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_cube);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_cube_specular);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Light
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 3.0f, -3.0f));
        
        lightshader.use();
        lightshader.setMat4f("model", model);
        lightshader.setMat4f("view", view);
        lightshader.setMat4f("projection", projection);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Floor
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        
        cubeshader.use();
        cubeshader.setMat4f("model", model);
        cubeshader.setMat4f("view", view);
        cubeshader.setMat4f("projection", projection);
        cubeshader.setVec3f("viewPos", ourcamera.Position);
        
        glBindVertexArray(VAO_square);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_floor);
        glDrawArrays(GL_TRIANGLES, 0, 6);
 
        // Skybox
        glDepthFunc(GL_LEQUAL);
        cubemapshader.use();
        glm::mat4 view_cubemap = glm::mat4(glm::mat3(view));
        cubemapshader.setMat4f("view", view_cubemap);
        cubemapshader.setMat4f("projection", projection);
        glBindVertexArray(VAO_cubemap);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        
        // Grass
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        
        cubeshader.use();
        cubeshader.setMat4f("model", model);
        cubeshader.setMat4f("view", view);
        cubeshader.setMat4f("projection", projection);
        cubeshader.setVec3f("viewPos", ourcamera.Position);
        
        glBindVertexArray(VAO_square);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_grass);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Window
        // sort the transparent windows before rendering
        transparentshader.use();
        transparentshader.setMat4f("model", model);
        transparentshader.setMat4f("view", view);
        transparentshader.setMat4f("projection", projection);
        transparentshader.setVec3f("viewPos", ourcamera.Position);
        
        glBindVertexArray(VAO_square);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_window);
        
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++) {
            float distance = glm::length(ourcamera.Position - windows[i]);
            sorted[distance] = windows[i];
        }
        
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            transparentshader.setMat4f("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &VAO_square);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_square);
    cubeshader.del();
    lightshader.del();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float cameraSpeed = 0.025f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(FORWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(BACKWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(RIGHT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(UP, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(DOWN, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(MOUSE_LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(MOUSE_RIGHT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(MOUSE_UP, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        ourcamera.ProcessKeyboard(MOUSE_DOWN, cameraSpeed);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void genTexture(unsigned int& texture, std::string path, GLenum handle_edge)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, handle_edge);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, handle_edge);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    stbi_image_free(data);
}

void genCubeMapTexture(unsigned int& texture, std::vector<std::string> faces)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void printMat4(glm::mat4 & matrix)
{
    std::cout << "\n";
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            std::cout << matrix[x][y] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    ourcamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ourcamera.ProcessMouseScroll(static_cast<float>(yoffset));
}
