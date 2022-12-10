#include <vector>
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

class ShapeGenerator
{
    //The methods called upon to generate the primitive object data that gets stored in the VBOs
public:
    static std::vector<float> generateSphereData(int param1, int param2);
    static std::vector<float> generateCubeData(int param1, int param2);
    static std::vector<float> generateCylinderData(int param1, int param2);
    static std::vector<float> generateConeData(int param1, int param2);
    static void insertVec3(std::vector<float> &data, glm::vec3 v);
};
