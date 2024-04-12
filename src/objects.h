//
//  objects.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/12.
//

#ifndef objects_h
#define objects_h

#include <vector>
#include <utility>

float* getCube();
float* getCubeWithUV();
float* getQuad();
float* getCubeMap();

// Object base class
// -----------------
class Objects
{
public:
    float* vertexarray;
    unsigned int VBO;
    unsigned int VAO;
    
    unsigned int num = 0;
    std::vector<glm::mat4> models;
    std::vector<unsigned int> textures;
    std::vector<bool> cast_shadow;
    std::vector<bool> ismirror;
    
    Objects() {};
    ~Objects() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    };
    
    void addObject(glm::mat4 in_model, unsigned int in_texture=0, bool in_cast_shadow=true, bool in_ismirrior=false) {
        num++;
        models.push_back(in_model); // TODO: change to std::move()
        textures.push_back(in_texture);
        cast_shadow.push_back(in_cast_shadow);
        ismirror.push_back(in_ismirrior);
    };
    
    virtual void _getVBOVAO() {};
    virtual void render() {};
};

// Object base class
// -----------------
class Cubes: public Objects
{
public:
    Cubes() {
        _getCubeWithUV();
        _getVBOVAO();
    };
    void _getCube();
    void _getCubeWithUV();
    void _getVBOVAO() override;
    void render() override; // override;
};

// Object base class
// -----------------
class Quads: public Objects
{
public:
    Quads() {
        _getQuad();
        _getVBOVAO();
    };
    void _getQuad();
    void _getVBOVAO() override;
    void render() override;
};

// Object base class
// -----------------
class Meshes: public Objects
{
public:
    float h;
    float w;
    int h_n;
    int w_n;
    float dx;
    unsigned int * indices;
    unsigned int EBO;
    
    Meshes(float in_h, float in_w, float in_dx) {
        h = in_h;
        w = in_w;
        dx = in_dx;
        h_n = int(h / dx);
        w_n = int(w / dx);
        std::cout << "h_n, w_n: " << h_n << " " << w_n << std::endl;
        _getMesh2();
        _getVBOVAOEBO();
    };
    void _getMesh();
    void _getMesh2();
    void _getVBOVAOEBO();
    void render() override;
};

// Object base class
// -----------------
class Skyboxes: public Objects
{
public:
    Skyboxes() {
        _getSkybox();
        _getVBOVAO();
    };
    void _getSkybox();
    void _getVBOVAO() override;
};

void Cubes::_getVBOVAO()
{
    // cubeVBO
    // -------
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 288 * sizeof(float), vertexarray, GL_STATIC_DRAW);
    
    // cubeVAO
    // -------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void Quads::_getVBOVAO()
{
    // squareVBO
    // ---------
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), vertexarray, GL_STATIC_DRAW);
    
    // squareVAO
    // ---------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void Meshes::_getVBOVAOEBO()
{
    // squareVBO
    // ---------
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, 8 * (h_n+1) * (w_n+1) * sizeof(float), vertexarray, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, 8 * 6 * h_n * w_n * sizeof(float), vertexarray, GL_STATIC_DRAW);
    
    // squareVAO
    // ---------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    // EBO
    // ---
//    glGenBuffers(1, &EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Skyboxes::_getVBOVAO()
{
    // Cubemap VAO, VBO
    // ----------------
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), vertexarray, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Cubes::render()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Quads::render()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Meshes::render()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 8 * 6 * h_n * w_n);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glDrawElements(GL_TRIANGLES, 8 * (h_n+1) * (w_n+1), GL_UNSIGNED_INT, 0);
}

// Vertices functions
// ------------------
void Cubes::_getCube()
{
    vertexarray = new float[180] {
    //  ------- pos --------  - texture -
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
}

void Cubes::_getCubeWithUV()
{
    vertexarray = new float[288] {
        //------- pos ------- ----- normal ----- -- texture --
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
    };
}

void Quads::_getQuad()
{
    vertexarray = new float[48] {
    //  ------- pos ------- ----- normal ----- -- texture --
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
         1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,
    };
//    vertexarray = new float[48] {
//    //  ------- pos ------- ----- normal ----- -- texture --
//        -0.1f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
//         0.2f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
//        -0.4f,  0.7f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
//        -0.3f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,
//         0.9f, -0.9f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
//         0.8f,  0.4f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,
//    };
}

void Meshes::_getMesh()
{
    auto getVertexIdx = [&](int x, int y) -> unsigned int {
        return y * (this->w_n + 1) + x;
    };
    auto getTriangleIdx = [&](int x, int y) -> unsigned int {
        return y * this->w_n + x;
    };
    
    vertexarray = new float[8 * (h_n+1) * (w_n+1)];
    for (int y = 0; y < h_n+1; y++) {
        for (int x = 0; x < w_n+1; x++) {
            unsigned int idx = getVertexIdx(x, y);
            // position
            vertexarray[8 * idx    ] = x * dx;
            vertexarray[8 * idx + 1] = y * dx;
            vertexarray[8 * idx + 2] = 0.0f;
            // normal
            vertexarray[8 * idx + 3] = 0.0f;
            vertexarray[8 * idx + 4] = 0.0f;
            vertexarray[8 * idx + 5] = 1.0f;
            // texture
            vertexarray[8 * idx + 6] = x * dx;
            vertexarray[8 * idx + 7] = y * dx;
        }
    }
    
    indices = new unsigned int[h_n * w_n * 6];
    for (int y = 0; y < h_n; y++) {
        for (int x = 0; x < w_n; x++) {
            unsigned int tri_idx = getTriangleIdx(x, y);
            // First triangle
            indices[6 * tri_idx    ] = getVertexIdx(x, y);
            indices[6 * tri_idx + 1] = getVertexIdx(x+1, y);
            indices[6 * tri_idx + 2] = getVertexIdx(x, y+1);
            // Second triangle
            indices[6 * tri_idx + 3] = getVertexIdx(x, y+1);
            indices[6 * tri_idx + 4] = getVertexIdx(x+1, y);
            indices[6 * tri_idx + 5] = getVertexIdx(x, y+1);
        }
    }
}

void Meshes::_getMesh2()
{
    auto getTriangleIdx = [&](int x, int y) -> unsigned int {
        return y * this->w_n + x;
    };
    
    vertexarray = new float[8 * 6 * h_n * w_n];
    
    int offset_x[] = {0, 1, 1, 0, 1, 0};
    int offset_y[] = {0, 0, 1, 0, 1, 1};
    
    for (int y = 0; y < h_n; y++) {
        for (int x = 0; x < w_n; x++) {
            unsigned int idx = getTriangleIdx(x, y);
            for (int i = 0; i < 6; i++) {
                // position
                vertexarray[8 * 6 * idx + i * 8    ] = (x + offset_x[i]) * dx;
                vertexarray[8 * 6 * idx + i * 8 + 1] = (y + offset_y[i]) * dx;
                vertexarray[8 * 6 * idx + i * 8 + 2] = 0.0f;
                // normal
                vertexarray[8 * 6 * idx + i * 8 + 3] = 0.0f;
                vertexarray[8 * 6 * idx + i * 8 + 4] = 0.0f;
                vertexarray[8 * 6 * idx + i * 8 + 5] = 1.0f;
                // texture
                vertexarray[8 * 6 * idx + i * 8 + 6] = (x + offset_x[i]) * dx;
                vertexarray[8 * 6 * idx + i * 8 + 7] = (y + offset_y[i]) * dx;
            }
        }
    }
}

void Skyboxes::_getSkybox()
{
    vertexarray = new float[108] {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}


#endif /* objects_h */
