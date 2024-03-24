//
//  globjects.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/24.
//

#ifndef globjects_h
#define globjects_h

#include <utility>

std::pair<unsigned int, unsigned int> get_cube_globjects()
{
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
    
    return std::make_pair(VBO_cube, VAO_cube);
}

std::pair<unsigned int, unsigned int> get_square_globjects()
{
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
    
    return std::make_pair(VBO_square, VAO_square);
}

std::pair<unsigned int, unsigned int> get_cubemap_globjects()
{
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
    
    return std::make_pair(VBO_cubemap, VAO_cubemap);
}

#endif /* globjects_h */
