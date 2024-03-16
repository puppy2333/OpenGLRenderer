//
//  light.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/16.
//

#ifndef light_h
#define light_h

#include "shader_s.h"

class Light
{
public:
    std::string Name;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Position;
    
    Light(std::string name = "light",
          glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f),
          glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
          glm::vec3 specular = glm::vec3(0.3f, 0.3f, 0.3f),
          glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
    {
        Name = name; 
        Ambient = ambient;
        Diffuse = diffuse;
        Specular = specular;
        Position = position;
    }
    
    void shaderSetLight(Shader& shader)
    {
        shader.use();
        shader.setVec3f(Name + ".ambient", Ambient);
        shader.setVec3f(Name + ".diffuse", Diffuse);
        shader.setVec3f(Name + ".specular", Specular);
        shader.setVec3f(Name + ".position", Position);
    }
};


#endif /* light_h */
