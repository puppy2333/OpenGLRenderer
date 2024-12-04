#ifndef polyhedron_h
#define polyhedron_h

#include <vector>
#include <utility>
#include <memory>
#include <iostream>

#include <glm/glm.hpp>

#include <const.h>

// Polyhedron base class
// ---------------------
class Polyhedron
{
public:
    unsigned int vbo;
    unsigned int vao;
    
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation_aixs;
    float rotation_degree;

    glm::mat4 model_mat;
    unsigned int texture;

    ObjectType object_type;
    
    Polyhedron(
        ObjectType init_object_type, 
        glm::vec3 init_position = glm::vec3(0.0f), 
        glm::vec3 init_scale = glm::vec3(1.0f), 
        glm::vec3 init_rotation_aixs = glm::vec3(1.0f),
        float init_rotation_degree = 0.0f): 
            object_type(init_object_type), position(init_position), scale(init_scale), rotation_aixs(init_rotation_aixs), rotation_degree(init_rotation_degree)
    {
        model_mat = glm::translate(glm::mat4(1.0f), position);
        model_mat = glm::rotate(model_mat, glm::radians(rotation_degree), rotation_aixs);
        model_mat = glm::scale(model_mat, scale);
    };
    ~Polyhedron() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    };
    
    virtual void render() {};

protected:
    virtual void get_vbo_vao(float* vertex_array_ptr) {};
};

// Cube class
// ----------
class Cube: public Polyhedron
{
public:
    Cube(
        ObjectType init_object_type, 
        glm::vec3 init_position = glm::vec3(0.0f), 
        glm::vec3 init_scale = glm::vec3(1.0f), 
        glm::vec3 init_rotation_aixs = glm::vec3(1.0f),
        float init_rotation_degree = 0.0f);
    void get_vbo_vao(float* vertex_array_ptr) override;
    void render() override; // override;
};

Cube::Cube(
    ObjectType init_object_type, 
    glm::vec3 init_position, 
    glm::vec3 init_scale, 
    glm::vec3 init_rotation_aixs,
    float init_rotation_degree): 
        Polyhedron(init_object_type, init_position, init_scale, init_rotation_aixs, init_rotation_degree)
{
    float* vertex_array_ptr = new float[288] {
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

    get_vbo_vao(vertex_array_ptr);

    delete vertex_array_ptr;
}

void Cube::get_vbo_vao(float* vertex_array_ptr)
{
    // cubeVBO
    // -------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 288 * sizeof(float), vertex_array_ptr, GL_STATIC_DRAW);
    
    // cubeVAO
    // -------
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void Cube::render()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Quad class
// ----------
class Quad: public Polyhedron
{
public:
    Quad(
        ObjectType init_object_type, 
        glm::vec3 init_position = glm::vec3(0.0f), 
        glm::vec3 init_scale = glm::vec3(1.0f), 
        glm::vec3 init_rotation_aixs = glm::vec3(1.0f),
        float init_rotation_degree = 0.0f,
        float texture_repeat_times = 1.0f);
    void get_vbo_vao(float* vertex_array_ptr) override;
    void render() override; // override;
};

Quad::Quad(
    ObjectType init_object_type, 
    glm::vec3 init_position, 
    glm::vec3 init_scale, 
    glm::vec3 init_rotation_aixs,
    float init_rotation_degree,
    float texture_repeat_times): 
        Polyhedron(init_object_type, init_position, init_scale, init_rotation_aixs, init_rotation_degree)
{
    float* vertex_array_ptr = new float[48] {
    //  ------- pos ------- ----- normal ----- -- texture --
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  texture_repeat_times,  0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  texture_repeat_times,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  texture_repeat_times,
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  texture_repeat_times,  0.0f,
         1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  texture_repeat_times,  texture_repeat_times,
    };

    get_vbo_vao(vertex_array_ptr);

    delete vertex_array_ptr;
}

void Quad::get_vbo_vao(float* vertex_array_ptr)
{
    // squareVBO
    // ---------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), vertex_array_ptr, GL_STATIC_DRAW);
    
    // squareVAO
    // ---------
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void Quad::render()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Mesh class
// ----------
class Mesh: public Polyhedron
{
public:
    float h;
    float w;
    int h_n;
    int w_n;
    float dx;
    unsigned int * indices;
    
    Mesh(
        ObjectType init_object_type, 
        glm::vec3 init_position = glm::vec3(0.0f), 
        glm::vec3 init_scale = glm::vec3(1.0f), 
        glm::vec3 init_rotation_aixs = glm::vec3(1.0f),
        float init_rotation_degree = 0.0f,
        float in_h = 1, 
        float in_w = 1, 
        float in_dx = 0.01);
    void get_vbo_vao(float* vertex_array_ptr);
    void render() override;
};

Mesh::Mesh(
    ObjectType init_object_type, 
    glm::vec3 init_position, 
    glm::vec3 init_scale, 
    glm::vec3 init_rotation_aixs,
    float init_rotation_degree,
    float in_h, 
    float in_w, 
    float in_dx): 
        Polyhedron(init_object_type, init_position, init_scale, init_rotation_aixs, init_rotation_degree)
{
    h = in_h;
    w = in_w;
    dx = in_dx;
    h_n = int(h / dx);
    w_n = int(w / dx);
    std::cout << "h_n, w_n: " << h_n << " " << w_n << std::endl;

    auto getTriangleIdx = [&](int x, int y) -> unsigned int {
        return y * this->w_n + x;
    };
    
    float* vertex_array_ptr = new float[8 * 6 * h_n * w_n];
    
    int offset_x[] = {0, 1, 1, 0, 1, 0};
    int offset_y[] = {0, 0, 1, 0, 1, 1};
    
    for (int y = 0; y < h_n; y++) {
        for (int x = 0; x < w_n; x++) {
            unsigned int idx = getTriangleIdx(x, y);
            for (int i = 0; i < 6; i++) {
                // position
                vertex_array_ptr[8 * 6 * idx + i * 8    ] = (x + offset_x[i]) * dx;
                vertex_array_ptr[8 * 6 * idx + i * 8 + 1] = (y + offset_y[i]) * dx;
                vertex_array_ptr[8 * 6 * idx + i * 8 + 2] = 0.0f;
                // normal
                vertex_array_ptr[8 * 6 * idx + i * 8 + 3] = 0.0f;
                vertex_array_ptr[8 * 6 * idx + i * 8 + 4] = 0.0f;
                vertex_array_ptr[8 * 6 * idx + i * 8 + 5] = 1.0f;
                // texture
                vertex_array_ptr[8 * 6 * idx + i * 8 + 6] = (x + offset_x[i]) * dx;
                vertex_array_ptr[8 * 6 * idx + i * 8 + 7] = (y + offset_y[i]) * dx;
            }
        }
    }

    get_vbo_vao(vertex_array_ptr);

    delete vertex_array_ptr;
}

void Mesh::get_vbo_vao(float* vertex_array_ptr)
{
    // squareVBO
    // ---------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * 6 * h_n * w_n * sizeof(float), vertex_array_ptr, GL_STATIC_DRAW);
    
    // squareVAO
    // ---------
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void Mesh::render()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6 * h_n * w_n);
}

// Skybox class
// ------------
class Skybox: public Polyhedron
{
public:
    Skybox();
    void get_vbo_vao(float* vertex_array_ptr) override;
};

Skybox::Skybox():
    Polyhedron(Light, glm::vec3(0.0), glm::vec3(1.0f), glm::vec3(1.0f), 0.0)
{
    float* vertex_array_ptr = new float[108] {
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

    get_vbo_vao(vertex_array_ptr);

    delete vertex_array_ptr;
}

void Skybox::get_vbo_vao(float* vertex_array_ptr)
{
    // Cubemap VAO, VBO
    // ----------------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), vertex_array_ptr, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


#endif