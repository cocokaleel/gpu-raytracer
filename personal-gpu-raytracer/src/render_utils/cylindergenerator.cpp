#include "shapegenerator.h"
void makeCapWedgeCylinder(float currTheta, float nextTheta, int param1, std::vector<float>& data);
void makePanelCylinder(float currTheta, float nextTheta, int param1, std::vector<float>&data);
void makeTileCylinder(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight,
                        bool isCap, std::vector<float>& data);

std::vector<float> ShapeGenerator::generateCylinderData(int param1, int param2) {
    std::vector<float> cylinderData;

    float thetaStep = glm::radians(360.f/fmax(param2, 3.f));
    float currentTheta, nextTheta;

    for (int i = 0; i<fmax(param2, 3); i++) {
       float currentTheta = i * thetaStep;
       float nextTheta = (i+1) * thetaStep;
       makePanelCylinder(currentTheta, nextTheta, fmax(param1, 3), cylinderData);
       makeCapWedgeCylinder(currentTheta, nextTheta, fmax(param1, 3), cylinderData);
    }

    return cylinderData;
}

void makeCapWedgeCylinder(float currTheta, float nextTheta, int param1, std::vector<float>& data) {
    glm::vec3 tl, tr, bl, br; //vectors for storing the coordinates of each point in the faces

    //Top Cap
    tl = {0,   0.5,    0}; //center of top cap
    tr = {0,   0.5,    0}; //center of top cap
    bl = {0.5*sin(currTheta), 0.5, 0.5*cos(currTheta)}; //bottom left, currentTheta
    br = {0.5*sin(nextTheta),   0.5, 0.5*cos(nextTheta)}; //bottom right, nextTheta

    glm::vec3 currDir = (bl-tl)/float(param1);
    glm::vec3 nextDir = (br-tr)/float(param1);

    for (int x = 0; x<param1; x++) {
        float i = x;
        makeTileCylinder(tl + i * currDir, tr + i * nextDir, tl + (i+1)*currDir, tr+(i+1)*nextDir, true, data);
    }

    //Bottom Cap
    tl = {0.5*sin(currTheta),   -0.5,    0.5*cos(currTheta)}; //top phi, currentTheta
    tr = {0.5*sin(nextTheta),      -0.5,    0.5*cos(nextTheta)}; //top phi, nextTheta
    bl = {0, -0.5, 0}; //bottomPhi, currentTheta
    br = {0, -0.5, 0}; //bottomPhi, nextTheta

    currDir = (tl-bl)/float(param1);
    nextDir = (tr-br)/float(param1);

    for (int x = 0; x<param1; x++) {
        float i = x;
        makeTileCylinder(bl + i * nextDir, br + i * currDir, bl + (i+1)*nextDir, br+(i+1)*currDir, true, data);
    }
}

void makePanelCylinder(float currTheta, float nextTheta, int param1, std::vector<float>& data) {
    //make a panel on the side of the cylinder
    glm::vec3 tl, tr, bl, br; //vectors for storing the coordinates of each point in the faces

    tl = {0.5*sin(currTheta),   0.5,    0.5*cos(currTheta)}; //top phi, currentTheta
    tr = {0.5*sin(nextTheta),      0.5,    0.5*cos(nextTheta)}; //top phi, nextTheta
    bl = {0.5*sin(currTheta), -0.5, 0.5*cos(currTheta)}; //bottomPhi, currentTheta
    br = {0.5*sin(nextTheta),   -0.5, 0.5*cos(nextTheta)}; //bottomPhi, nextTheta

    glm::vec3 xDir = (tr-tl);
    glm::vec3 yDir = (bl-tl)/float(param1);


    for (int y = 0; y < param1; y++) {
        float j = y;
        makeTileCylinder(tl+j*yDir, tl+xDir+j*yDir, tl+(j+1)*yDir, tl+xDir+(j+1)*yDir, false, data);
    }
}

////TODO fix normals
void makeTileCylinder(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight,
                        bool isCap, std::vector<float>& data) {
      // Task 5: Implement the makeTile() function for a Sphere
      // Note: this function is very similar to the makeTile() function for Cube,
      //       but the normals are calculated in a different way!

     glm::vec3 ulNorm, blNorm, brNorm, urNorm;
     if (isCap) {
         //if the cap is on the top (middle coordinate is 0.5 and not -0.5), have the normal face up
         int top = topLeft[1] > 0 ? 1 : -1;
         ulNorm = {0, top, 0};
         blNorm = {0, top, 0};
         brNorm = {0, top, 0};
         urNorm = {0, top, 0};

     } else {
         ulNorm = glm::normalize(glm::vec3{topLeft[0], 0, topLeft[2]});
         blNorm = glm::normalize(glm::vec3{bottomLeft[0], 0, bottomLeft[2]});
         brNorm = glm::normalize(glm::vec3{bottomRight[0], 0, bottomRight[2]});
         urNorm = glm::normalize(glm::vec3{topRight[0], 0, topRight[2]});
     }

     ShapeGenerator::insertVec3(data, topLeft);
     ShapeGenerator::insertVec3(data, ulNorm);
     ShapeGenerator::insertVec3(data, bottomLeft);
     ShapeGenerator::insertVec3(data, blNorm);
     ShapeGenerator::insertVec3(data, topRight);
     ShapeGenerator::insertVec3(data, urNorm);


     ShapeGenerator::insertVec3(data, bottomRight);
     ShapeGenerator::insertVec3(data, brNorm);
     ShapeGenerator::insertVec3(data, topRight);
     ShapeGenerator::insertVec3(data, urNorm);
     ShapeGenerator::insertVec3(data, bottomLeft);
     ShapeGenerator::insertVec3(data, blNorm);
}
