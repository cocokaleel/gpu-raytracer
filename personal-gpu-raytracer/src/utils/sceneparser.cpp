#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }


    // Populate renderData with global data, lights, and camera data

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();
    renderData.lights = fileReader.getLights();

    // Populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!

    SceneNode* rootNode = fileReader.getRootNode();
    renderData.shapes.clear();

    //start traverseTree with the identity matrix
    traverseTree(rootNode, &renderData.shapes, glm::mat4( 1.0f ));

    return true;
}

//Below copied from Lab 5
/**
 * @brief SceneParser::traverseTree traverses the tree which starts at rootNode (SceneNode). Every node has a list of
 * transformations (SceneTransformation), primitives (ScenePrimitive), and other nodes (SceneNode). During the traversal
 * of each node, each transformation, which stores a type of tranformation and the appropriate matrix, should be multiplied
 * with the passed in ctm. Then, each node should get traversed with this ctm passed in (depth-first traversal), and
 * afterwards, each primitive at this level should be added to shapes.
 * @param rootNode - the root node from the SceneNode xml
 * @param shapes - the primitives alongside their ctms
 * @param ctm - the cumulative transformation matrix at this point in a scene
 */
void SceneParser::traverseTree (SceneNode* rootNode, std::vector<RenderShapeData>* shapes, glm::mat4 ctm) {

    //calculate CTM for node
    //traverse all nodes of trees
    //add all primitives in this node to shapes

    glm::mat4 cumulative = ctm * accumulateTransformations(&rootNode->transformations);

    if (!rootNode->children.empty()) {
        for (SceneNode* node : rootNode->children) {
            traverseTree(node, shapes, cumulative);
        }
    }

    for (ScenePrimitive* prim : rootNode->primitives) {
        (*shapes).push_back(RenderShapeData {*prim, cumulative});
    }

}

/**
 * @brief SceneParser::accumulateTransformations go through a list of transformations and accumulate them into one
 * bigger ctm
 * @param transformations
 * @return
 */
glm::mat4 SceneParser:: accumulateTransformations(std::vector<SceneTransformation*>* transformations) {
    glm::mat4 cumulative (1.f);


    for (SceneTransformation* t : *transformations) {
            switch ( t->type) {
                case TransformationType::TRANSFORMATION_TRANSLATE:
                    cumulative *= glm::translate(t->translate);
                break;
                case TransformationType::TRANSFORMATION_SCALE:
                    cumulative *= glm::scale(t->scale);
                break;
                case TransformationType::TRANSFORMATION_ROTATE:
                    cumulative *= glm::rotate(t->angle, t->rotate);
                break;
                case TransformationType::TRANSFORMATION_MATRIX:
                    cumulative *= t->matrix;
                break;
            }
    }

    return cumulative;
}
