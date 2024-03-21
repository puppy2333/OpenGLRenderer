//
//  utils.h
//  opengl_test
//
//  Created by 周宣辰 on 2024/3/21.
//

#ifndef utils_h
#define utils_h

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

#endif /* utils_h */
