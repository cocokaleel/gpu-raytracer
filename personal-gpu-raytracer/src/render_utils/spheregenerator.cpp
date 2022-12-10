#include "shapegenerator.h"
void makeWedgeSphere(float currentTheta, float nextTheta, int param1, std::vector<float>& data);
void makeTileSphere(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight,
              std::vector<float>& data);

std::vector<float> ShapeGenerator::generateSphereData(int param1, int param2) {
    std::vector<float> data;


    float thetaStep = glm::radians(360.f/fmax(param2, 3));
    float currentTheta, nextTheta;

    for (int i = 0; i<fmax(param2,3); i++) {
       float currentTheta = i * thetaStep;
       float nextTheta = (i+1) * thetaStep;
       makeWedgeSphere(currentTheta, nextTheta, fmax(param1, 3), data);
    }
    return data;
}

void makeTileSphere(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight,
              std::vector<float>& data) {


    glm::vec3 ulNorm = glm::normalize(topLeft);
    glm::vec3 blNorm = glm::normalize(bottomLeft);
    glm::vec3 brNorm = glm::normalize(bottomRight);
    glm::vec3 urNorm = glm::normalize(topRight);

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

void makeWedgeSphere(float currentTheta, float nextTheta, int param1, std::vector<float>& data) {

    float phiDivision = glm::radians(180.f)/param1;
    glm::vec3 tl, tr, bl, br; //vectors for storing the coordinates of each point in the faces
    float topPhi, bottomPhi;

    for (int i = 0; i < param1; i++) {
        topPhi = i*phiDivision;
        bottomPhi = (i+1)*phiDivision;

        tl = {0.5*sin(topPhi)*sin(currentTheta),   0.5*cos(topPhi),    0.5*sin(topPhi)*cos(currentTheta)}; //top phi, currentTheta
        tr = {0.5*sin(topPhi)*sin(nextTheta),      0.5*cos(topPhi),    0.5*sin(topPhi)*cos(nextTheta)}; //top phi, nextTheta
        bl = {0.5*sin(bottomPhi)*sin(currentTheta),0.5*cos(bottomPhi), 0.5*sin(bottomPhi)*cos(currentTheta)}; //bottomPhi, currentTheta
        br = {0.5*sin(bottomPhi)*sin(nextTheta),   0.5*cos(bottomPhi), 0.5*sin(bottomPhi)*cos(nextTheta)}; //bottomPhi, nextTheta

        makeTileSphere(tl,tr,bl,br, data);

    }


}
