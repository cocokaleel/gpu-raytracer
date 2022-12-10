#include "shapegenerator.h"

void makeFaceCube(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight,
              int param1, int param2,
              std::vector<float>& data);

void makeTileCube(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    std::vector<float>& data);

std::vector<float> ShapeGenerator::generateCubeData(int param1, int param2) {
    std::vector<float> data;

    data.clear();

    //front face
    makeFaceCube(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f),
             param1, param2,
             data);

    //top face
    makeFaceCube(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, 0.5f),
             param1, param2,
             data);

    //bottom face
    makeFaceCube(glm::vec3(-0.5f,  -0.5f, 0.5f),
             glm::vec3( 0.5f,  -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             param1, param2,
             data);

    //right face
    makeFaceCube(glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             param1, param2,
             data);
    //left face
    makeFaceCube( glm::vec3( -0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( -0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              param1, param2,
              data);
    //back face
    makeFaceCube(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             param1, param2,
             data);
    return data;
}


void makeTileCube(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    std::vector<float>& data) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 ulNorm = glm::normalize(glm::cross(bottomLeft-topLeft, topRight-topLeft));
    glm::vec3 blNorm = glm::normalize(glm::cross(bottomRight-bottomLeft,topLeft-bottomLeft));
    glm::vec3 brNorm = glm::normalize(glm::cross(topRight-bottomRight,bottomLeft-bottomRight));
    glm::vec3 urNorm = glm::normalize(glm::cross(topLeft-topRight,bottomRight-topRight));

    ShapeGenerator::insertVec3(data, topLeft);
    ShapeGenerator::insertVec3(data, ulNorm);
    ShapeGenerator::insertVec3(data, bottomLeft);
    ShapeGenerator::insertVec3(data, blNorm);
    ShapeGenerator::insertVec3(data, bottomRight);
    ShapeGenerator::insertVec3(data, brNorm);


    ShapeGenerator::insertVec3(data, bottomRight);
    ShapeGenerator::insertVec3(data, brNorm);
    ShapeGenerator::insertVec3(data, topRight);
    ShapeGenerator::insertVec3(data, urNorm);
    ShapeGenerator::insertVec3(data, topLeft);
    ShapeGenerator::insertVec3(data, ulNorm);
}

void makeFaceCube(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight,
              int param1, int param2,
              std::vector<float>&data) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube

    glm::vec3 xDir = (topRight-topLeft)/float(param1);
    glm::vec3 yDir = (bottomLeft-topLeft)/float(param2);

    for (int x = 0; x < param1; x++) {
        for (int y = 0; y < param2; y++) {
            float i = x;
            float j = y;
            makeTileCube(topLeft+i*xDir+j*yDir, topLeft+(i+1)*xDir+j*yDir, topLeft+i*xDir+(j+1)*yDir, topLeft+(i+1)*xDir+(j+1)*yDir, data);
        }
    }


}
