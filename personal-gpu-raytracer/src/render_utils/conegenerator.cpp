#include "shapegenerator.h"

//pre-definition of functions
void makeCapWedge(float currTheta, float nextTheta, int param1, std::vector<float> &data);
void makePanel(float currTheta, float nextTheta, int param1, std::vector<float> &data);
void makeTile(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight,
              bool isCap,
              std::vector<float> &data);

std::vector<float> ShapeGenerator::generateConeData(int param1, int param2) {
    std::vector<float> data;

    data.clear();
//    data.reserve(phiTesselations * thetaTesselations * 6 * 3); //TODO fix this line?

    float thetaStep = glm::radians(360.f/fmax(param2, 3.f));
    float currentTheta, nextTheta;

    for (int i = 0; i<fmax(param2, 3); i++) {
       float currentTheta = i * thetaStep;
       float nextTheta = (i+1) * thetaStep;
       makePanel(currentTheta, nextTheta, fmax(param1, 3), data);
       makeCapWedge(currentTheta, nextTheta, fmax(param1, 3), data);
    }
    return data;
}


void makeCapWedge(float currTheta, float nextTheta, int param1, std::vector<float> &data) {
        glm::vec3 tl, tr, bl, br; //vectors for storing the coordinates of each point in the faces

        //Bottom Cap
        tl = {0.5*sin(currTheta),   -0.5,    0.5*cos(currTheta)}; //top phi, currentTheta
        tr = {0.5*sin(nextTheta),      -0.5,    0.5*cos(nextTheta)}; //top phi, nextTheta
        bl = {0, -0.5, 0}; //bottomPhi, currentTheta
        br = {0, -0.5, 0}; //bottomPhi, nextTheta

        glm::vec3 currDir = (tl-bl)/float(param1);
        glm::vec3 nextDir = (tr-br)/float(param1);

        for (int x = 0; x<param1; x++) {
            float i = x;
            makeTile(bl + i * nextDir, br + i * currDir, bl + (i+1)*nextDir, br+(i+1)*currDir, true, data);
        }

}


void makePanel(float currTheta, float nextTheta, int param1, std::vector<float> &data) {
    //make a panel on the side of the cone
    glm::vec3 tl, tr, bl, br; //vectors for storing the coordinates of each point in the faces

    tl = {0,   0.5,    0}; //top phi, currentTheta
    tr = {0,      0.5,    0}; //top phi, nextTheta
    bl = {0.5*sin(currTheta), -0.5, 0.5*cos(currTheta)}; //bottomPhi, currentTheta
    br = {0.5*sin(nextTheta),   -0.5, 0.5*cos(nextTheta)}; //bottomPhi, nextTheta

    glm::vec3 nextDir = (br-tr)/float(param1);
    glm::vec3 currDir = (bl-tl)/float(param1);


    for (int y = 0; y < param1; y++) {
        float j = y;
        makeTile(tl+j*currDir, tr+j*nextDir, tl+(j+1)*currDir, tl+(j+1)*nextDir, false, data);
    }
}

void makeTile(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight,
              bool isCap, std::vector<float> &data) {
    glm::vec3 ulNorm, blNorm, brNorm, urNorm;

    if (isCap) {
        //Bottom Cap
        ulNorm = {0, -1, 0};
        blNorm = {0, -1, 0};
        brNorm = {0, -1, 0};
        urNorm = {0, -1, 0};

    } else if (topLeft[1] > 0.499) {
        //tip normals
        //TODO un-jankify this

        //slightly shift positions down to get an arbitrary non-zero radius value to calculate normal on
        glm::vec3 leftShift = 0.01f*(bottomLeft - topLeft);
        glm::vec3 rightShift = 0.01f*(bottomRight - topRight);

        glm::vec3 newTopLeft = topLeft + leftShift;
        glm::vec3 newTopRight = topRight + rightShift;

        ulNorm = glm::normalize(glm::vec3{(newTopLeft[0]+newTopRight[0])/2, 0.125 - 0.25*(newTopLeft[1]), (newTopLeft[2]+newTopRight[2])/2});
        blNorm = glm::normalize(glm::vec3{bottomLeft[0], 0.125-0.25*bottomLeft[1], bottomLeft[2]});
        brNorm = glm::normalize(glm::vec3{bottomRight[0], 0.125-0.25*bottomRight[1], bottomRight[2]});
        urNorm = glm::normalize(glm::vec3{(newTopLeft[0]+newTopRight[0])/2, 0.125-0.25*newTopRight[1], (newTopLeft[2]+newTopRight[2])/2});

    } else {
        //Side Wraps

        ulNorm = glm::normalize(glm::vec3{topLeft[0], 0.125 - 0.25*topLeft[1], topLeft[2]});
        blNorm = glm::normalize(glm::vec3{bottomLeft[0], 0.125-0.25*bottomLeft[1], bottomLeft[2]});
        brNorm = glm::normalize(glm::vec3{bottomRight[0], 0.125-0.25*bottomRight[1], bottomRight[2]});
        urNorm = glm::normalize(glm::vec3{topRight[0], 0.125-0.25*topRight[1], topRight[2]});
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

