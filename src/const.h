//
//  const.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/16.
//

#ifndef const_h
#define const_h

#include "camera.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int UI_WIDTH = 500;

// Camera
Camera ourcamera(glm::vec3(0.0f, 0.0f, 5.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool init_wave = false;

// C++ 20 has support for pi
constexpr double pi = 3.14159265358979323846;

#endif /* const_h */
