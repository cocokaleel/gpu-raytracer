#include "shapegenerator.h"


//// Inserts a glm::vec3 into a vector of floats.
//// This will come in handy if you want to take advantage of vectors to build your shape!
void ShapeGenerator::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
