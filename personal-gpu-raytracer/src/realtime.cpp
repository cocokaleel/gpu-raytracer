#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "render_utils/shapegenerator.h"
#include "utils/shaderloader.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this

    //calculate the initial view, camera, and projection matrices
    setupViewAndCamera();
    calculateProjectionMatrix();
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

//    m_defaultFBO = 2; //set the default framebuffer (the screen) to 2

    //set the variables that will set the size of the FBO texture and renderbuffers
    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram("./resources/shaders/default.vert","./resources/shaders/default.frag");
    m_filter_shader = ShaderLoader::createShaderProgram("./resources/shaders/filter.vert","./resources/shaders/filter.frag");

    // Generate VBOs
    glGenBuffers(1, &m_sphere_vbo);
    glGenBuffers(1, &m_cube_vbo);
    glGenBuffers(1, &m_cone_vbo);
    glGenBuffers(1, &m_cylinder_vbo);

    // Generate VAOs
    glGenVertexArrays(1, &m_sphere_vao); //generate 1 vao (with positions and normals)
    glGenVertexArrays(1, &m_cone_vao); //generate 1 vao (with positions and normals)
    glGenVertexArrays(1, &m_cylinder_vao); //generate 1 vao (with positions and normals)
    glGenVertexArrays(1, &m_cube_vao); //generate 1 vao (with positions and normals)

    //Bind and fill VBOs, then unbind
    initializeBuffers();

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0)); //set up the first attribute
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat))); //set up the second attribute with a 3x offset

    // Clean-up bindings
    glBindVertexArray(0);
    // Task 10: Set the texture.frag uniform for our texture
     glUseProgram(m_filter_shader);
     //sets filter shader rendered_geometry_sampler to use the texture at active slot 0
     glUniform1i(glGetUniformLocation(m_filter_shader, "rendered_geometry_sampler"), 0);
     //pass in the size of the FBO to the filter shader for use by the kernel filters
     glUseProgram(0);

    // Fullscreen quad that the filter shaders write to
    std::vector<GLfloat> fullscreen_quad_data =
    { //     POSITIONS    //
        -1.f,  1.f, 0.0f,
              0.f, 1.f,
        -1.f, -1.0f, 0.0f,
              0.f, 0.f,
         1.f, -1.0f, 0.0f,
              1.f, 0.f,
         1.f,  1.f, 0.0f,
              1.f, 1.f,
        -1.f,  1.f, 0.0f,
              0.f, 1.f,
         1.f, -1.f, 0.0f,
              1.f, 0.f,
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Set up the fullscreen quad vao with an attribute for position as well as a uv coordinate
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //generate FBOs to get rendered to before the fullscreen quad
    makeFBO();

    ready = true;
}

//Initialize the different shape VBOs
void Realtime::initializeBuffers() {

    //SPHERE
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    // Generate sphere data
    std::vector<float> unitSphereData = ShapeGenerator::generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
    unitSphereDataSize = unitSphereData.size();
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER,unitSphereDataSize * sizeof(GLfloat),unitSphereData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glBindVertexArray(m_sphere_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(0*3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(1*3*sizeof(GLfloat)));

    //unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //CONE
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);
    std::vector<float> unitConeData = ShapeGenerator::generateConeData(settings.shapeParameter1, settings.shapeParameter2);
    unitConeDataSize = unitConeData.size();
    glBufferData(GL_ARRAY_BUFFER,unitConeDataSize * sizeof(GLfloat),unitConeData.data(), GL_STATIC_DRAW);
    glBindVertexArray(m_cone_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(0*3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(1*3*sizeof(GLfloat)));
    //unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //CYLINDER
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);
    std::vector<float> unitCylinderData = ShapeGenerator::generateCylinderData(settings.shapeParameter1, settings.shapeParameter2);
    unitCylinderDataSize = unitCylinderData.size();
    glBufferData(GL_ARRAY_BUFFER,unitCylinderData.size() * sizeof(GLfloat),unitCylinderData.data(), GL_STATIC_DRAW);
    glBindVertexArray(m_cylinder_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(0*3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(1*3*sizeof(GLfloat)));
    //unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //CUBE
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    std::vector<float>unitCubeData = ShapeGenerator::generateCubeData(settings.shapeParameter1, settings.shapeParameter2);
    unitCubeDataSize = unitCubeData.size();
    glBufferData(GL_ARRAY_BUFFER,unitCubeData.size() * sizeof(GLfloat),unitCubeData.data(), GL_STATIC_DRAW);
    glBindVertexArray(m_cube_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(0*3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6.f * sizeof(GLfloat), reinterpret_cast<void*>(1*3*sizeof(GLfloat)));
    //unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

//create the geometry with Phong lighting
void Realtime::paintGeometryPhong() {
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Bind full-screen quad
    glBindVertexArray(m_fullscreen_vao);

    // Activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);

    glUniform4fv(glGetUniformLocation(m_shader, "cameraPositionWorld"), 1, &cameraPosition[0]);


    //pass in the height angle and width angle
    float heightAngle = metaData.cameraData.heightAngle;
    float widthAngle = heightAngle * float(width()) / float(height());
    glUniform1f(glGetUniformLocation(m_shader, "widthAngle"), widthAngle);
    glUniform1f(glGetUniformLocation(m_shader, "heightAngle"), heightAngle);

    // Pass in m_view and m_proj
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "viewMatrixInverse"), 1, GL_FALSE, &glm::inverse(viewMatrix)[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

    //pass in ka, ks, and kd (global coefficients for ambient, specular, and diffuse lighting)
    glUniform1f(glGetUniformLocation(m_shader, "ka"), metaData.globalData.ka);
    glUniform1f(glGetUniformLocation(m_shader, "kd"), metaData.globalData.kd);
    glUniform1f(glGetUniformLocation(m_shader, "ks"), metaData.globalData.ks);


    //Pass in light positional, directional, and color data into the fragment (and technically vertex) shader as uniforms
    int lightCounter = 0;
    for (SceneLightData light : metaData.lights) {
        GLint dir = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].dir").c_str());
        glUniform4fv(dir, 1, &light.dir[0]);
        GLint color = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].color").c_str());
        glUniform4fv(color, 1, &light.color[0]);
        GLint pos = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].pos").c_str());
        glUniform4fv(pos, 1, &light.pos[0]);
        GLint attenuationFunction = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].attenuation").c_str());
        glUniform3fv(attenuationFunction, 1, &light.function[0]);
        GLint lightType = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].lightType").data());
        //pass in light type corresponding to the type of light
        //TODO make this an enum in frag shader??
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            //DIRECTIONAL LIGHT
            glUniform1i(lightType, 0);
        } else if (light.type == LightType::LIGHT_POINT) {
            //POINT LIGHT
            glUniform1i(lightType, 1);
        } else {
            //SPOTLIGHT
            glUniform1i(lightType, 2);

            GLint angle = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].angle").data());
            glUniform1f(angle, light.angle);

            GLint penumbra = glGetUniformLocation(m_shader, ("lights[" + std::to_string(lightCounter) + "].penumbra").data());
            glUniform1f(penumbra, light.penumbra);
        }
        lightCounter++;
        if (lightCounter > 8) {
            std::cerr << "This file has more than 8 lights in it. That's not allowed!!" << std::endl;
            exit(1); //Perhaps making the program exit is a bit dramatic but hey, you shouldn't have put that file in!
        }
    }

    glUniform1i(glGetUniformLocation(m_shader, "numLights"), lightCounter);

    int shapeCounter = 0;
    for (RenderShapeData shape : metaData.shapes) {

        //pass in all the shape-specific information to the first full-screen quad shader
        GLint matrix = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].ctm").c_str());
        GLint matrixInv = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].ctmInv").c_str());
        GLint normalMat = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].normalMat").c_str());
        GLint type = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].type").c_str());
        GLint cAmbient = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].cAmbient").c_str());
        GLint cDiffuse = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].cDiffuse").c_str());
        GLint cSpecular = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].cSpecular").c_str());
        GLint shininess = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].shininess").c_str());
        GLint cReflective = glGetUniformLocation(m_shader, ("shapes[" + std::to_string(shapeCounter) + "].cReflective").c_str());

        //pass in shape matrix and inverse matrix because inverting is costly
        glUniformMatrix4fv(matrix, 1, GL_FALSE, &shape.ctm[0][0]);
        glUniformMatrix3fv(normalMat, 1, GL_FALSE, &glm::inverse(glm::transpose(glm::mat3(shape.ctm)))[0][0]);
        glUniformMatrix4fv(matrixInv, 1, GL_FALSE, &glm::inverse(shape.ctm)[0][0]);


        // Pass in shininess and color components
        glUniform1f(shininess, shape.primitive.material.shininess);
        glUniform4fv(cSpecular, 1, &shape.primitive.material.cSpecular[0]);
        glUniform4fv(cAmbient, 1, &shape.primitive.material.cAmbient[0]);
        glUniform4fv(cDiffuse, 1, &shape.primitive.material.cDiffuse[0]);
        glUniform4fv(cReflective, 1, &shape.primitive.material.cReflective[0]);

        //pass in the shape type
        switch(shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_SPHERE:
            glUniform1i(type, 0);
            break;

//        case PrimitiveType::PRIMITIVE_CONE:

//        break;
//    case PrimitiveType::PRIMITIVE_CUBE:
//        break;
//    case PrimitiveType::PRIMITIVE_CYLINDER:
//        break;
        default:
            std::cerr << "Attempted to take on a shape that hasn't been modeled yet" << std::endl;
            exit(1);
            break;
        }

        shapeCounter++;
    }
    glUniform1i(glGetUniformLocation(m_shader, "numShapes"), shapeCounter);

    glDrawArrays(GL_TRIANGLES, 0, 6); //draw the fullscreen quad (6 vertices)
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind buffer (though none should have been bound)
    // Unbind Vertex Array
    glBindVertexArray(0);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    // Bind our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Call glViewport
    glViewport(0, 0, size().width()*m_devicePixelRatio, size().height()*m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGeometryPhong();

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Call paintTexture to draw our FBO color attachment texture
    paintFilteredFBO(m_fbo_texture);
}

void Realtime::paintFilteredFBO(GLuint texture) {
    glUseProgram(m_filter_shader);

    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    glUniform1i(glGetUniformLocation(m_filter_shader, "perPixelFilter"), settings.perPixelFilter);
    glUniform1i(glGetUniformLocation(m_filter_shader, "perPixelExtraCreditFilter"), settings.extraCredit1);
    glUniform1i(glGetUniformLocation(m_filter_shader, "kernelFilter"), settings.kernelBasedFilter);

    glBindVertexArray(m_fullscreen_vao);
    // Bind "texture" to slot 0
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6); //6, because it's the size of the fullscreen vao vbo dataset
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//adapted from Lab 11, set up a blank FBO
//Set up the blank that FBO will intercept the rendered geometry from the Phong shaders
void Realtime::makeFBO() {
    //pass the new height and width into the filter shader
    glUseProgram(m_filter_shader);
    glUniform1i(glGetUniformLocation(m_filter_shader, "image_height"), m_fbo_height);
    glUniform1i(glGetUniformLocation(m_filter_shader, "image_width"), m_fbo_width);
    glUseProgram(0);

        // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
        glGenTextures(1, &m_fbo_texture);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
        //put in an empty image into the new framebuffer texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        //set scaling parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Generate and bind a renderbuffer of the right size, set its format, then unbind
        glGenRenderbuffers(1, &m_fbo_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Generate and bind an FBO
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

        // Unbind the FBO by re-binding the default FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    //recalculate the view and perspective matrices as well as the camera matrix
    calculateProjectionMatrix();

    m_devicePixelRatio = this->devicePixelRatio();

    //set the variables that will set the size of the FBO texture and renderbuffers
    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    //remake FBO
    makeFBO();
}

void Realtime::sceneChanged() {
    bool success = SceneParser::parse(settings.sceneFilePath, metaData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
        exit(1);
    }

    //calculate the view, projection, and camera position matrices from scratch
    setupViewAndCamera();
    calculateProjectionMatrix();
    update(); // asks for a PaintGL() call to occur
}

//calculate the view, perspective, and camera matrices
//only called at the beginning of each scene
void Realtime::setupViewAndCamera() {
    //If in need of debugging, use glm::lookAt() and glm::perspective()

    //View matrix
    SceneCameraData cameraData = metaData.cameraData;
    look = cameraData.look;
    glm::vec4 w = glm::normalize(-cameraData.look);
    glm::vec4 v = glm::normalize(cameraData.up - glm::dot(cameraData.up, w)*w);
    glm::vec3 u = glm::cross(glm::vec3(v),glm::vec3(w));

    //camera rotation matrix (initialized COLUMN-WISE)
    cameraRotMat = {u[0],v[0],w[0],0,
                              u[1],v[1],w[1],0,
                              u[2],v[2],w[2],0,
                              0,0,0,1};

    cameraTransMat = {1,0,0,0,
                                0,1,0,0,
                                0,0,1,0,
                                -cameraData.pos[0], -cameraData.pos[1], -cameraData.pos[2], 1};

    viewMatrix = cameraRotMat * cameraTransMat;

    cameraPosition = (glm::inverse(viewMatrix)*glm::vec4(0,0,0,1));
}

//(re)calculates the projection matrix that gets passed into the shaders
void Realtime::calculateProjectionMatrix() {
    //Projection Matrix
    float scaleX,scaleY,scaleZ;
    float heightAngle = metaData.cameraData.heightAngle;
    float widthAngle = heightAngle * float(width()) / float(height());
    scaleX = 1.f / (settings.farPlane * glm::tan(widthAngle*0.5f));
    scaleY = 1.f/ (settings.farPlane * glm::tan(heightAngle*0.5f));

    glm::mat4 scalingMatrix = {scaleX,  0,      0,  0,
                              0,        scaleY, 0,  0,
                              0,        0,      1.f/settings.farPlane, 0,
                              0,        0,      0,  1.f}; //remember that glm is in column-major format

    glm::mat4 unhingingMatrix; //perspective to parallel projection!

    float c = -settings.nearPlane/settings.farPlane;

    unhingingMatrix = {1, 0, 0, 0,
                       0, 1, 0, 0,
                       0,0, 1.f/(1.f+c), -1.f,
                       0,0, -c/(1.f+c), 0};

    glm::mat4 openGLMat = {1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, -2, 0,
                          0, 0, -1, 1};

    projectionMatrix = openGLMat * unhingingMatrix*scalingMatrix;
}

void Realtime::settingsChanged() {
    if (ready == true) {
        makeCurrent();
        initializeBuffers();
    }

    //recalculate the projection matrix in case the near/far plane has changed
    calculateProjectionMatrix();
    update(); // asks for a PaintGL() call to occur
}

//translates the view matrix in the direction of the translation and the camera in the opposite direction
void Realtime::translateCamera(glm::vec3 translationVector, float deltaTime) {
    glm::vec3 trans = 5.f * deltaTime * normalize(translationVector);
    glm::mat4 translationMatrix = {1.f, 0,0,0,
                                  0,1.f,0,0,
                                  0,0,1,0,
                                  trans.x, trans.y, trans.z, 1.f};

    cameraTransMat = translationMatrix * cameraTransMat;

    viewMatrix = cameraRotMat * cameraTransMat;

    cameraPosition = (glm::inverse(viewMatrix)*glm::vec4(0,0,0,1));
}

//Uses Rodrigues' formula to rotate the camera and view matrix the angle about the specified axis
void Realtime::rotateCamera(glm::vec3 axis, float angle) {
    //slow down the rotation
    angle /= -m_rotationSpeed;
    float cos = glm::cos(angle);
    float sin = glm::sin(angle);
    axis = normalize(axis);
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    //calculate the Rodrigues rotation matrix
    glm::mat3 rotationMatrix = {cos+x*x*(1-cos), x*y*(1-cos)+z*sin, x*z*(1-cos)-y*sin,
                               x*y*(1-cos)-z*sin, cos+y*y*(1-cos), y*z*(1-cos)+x*sin,
                               x*z*(1-cos)+y*sin, y*z*(1-cos)-x*sin, cos+z*z*(1-cos)};
    look = rotationMatrix * look;

    glm::vec3 up = metaData.cameraData.up;
    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - glm::dot(up, w)*w);
    glm::vec3 u = glm::cross(v,w);

    //camera rotation matrix (initialized COLUMN-WISE)
    cameraRotMat = {u[0],v[0],w[0],0,
                    u[1],v[1],w[1],0,
                    u[2],v[2],w[2],0,
                    0,0,0,1};

    //update the rotation matrix
    viewMatrix = cameraRotMat * cameraTransMat;

    cameraPosition = (glm::inverse(viewMatrix)*glm::vec4(0,0,0,1));
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        if (m_mouseDown) {
            rotateCamera(glm::vec3{0,1,0}, deltaX);
            rotateCamera(glm::cross(glm::vec3(look),glm::vec3(metaData.cameraData.up)), deltaY);
        }

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    glm::mat4 translationMatrix;
    if (m_keyMap[Qt::Key::Key_W]) {
        //W key: 'push in': camera moves into the direction of the look vector

        //negative look, because we are modifying the view matrix, and then the inverse is used
        // to calculate the camera position, which will end up being (+) look
        glm::vec3 normLook = -normalize(look);

        translateCamera(normLook, deltaTime);

    } else if (m_keyMap[Qt::Key::Key_S]) {
        //S key: 'pull out': camera moves away from the direction of the look vector
        glm::vec3 normLook = normalize(look);

        translateCamera(normLook, deltaTime);
    } else if (m_keyMap[Qt::Key::Key_A]) {
        //A Key: Translates the camera in the left direction, perpendicular to the look and up vectors
        glm::vec3 perp = glm::cross(glm::vec3(look), glm::vec3(metaData.cameraData.up));

        translateCamera(perp, deltaTime);

    } else if (m_keyMap[Qt::Key::Key_D]) {
        //D Key: Translates the camera in the right direction, also perpendicular to the look and up vectors.
        //This movement should be opposite to that of pressing A

        //A Key: Translates the camera in the left direction, perpendicular to the look and up vectors
        glm::vec3 perp = -glm::cross(glm::vec3(look), glm::vec3(metaData.cameraData.up));

        translateCamera(perp, deltaTime);

    } else if (m_keyMap[Qt::Key::Key_Space]) {
        //Space bar: Translates the camera along the world space vector (0,1,0)
        translateCamera(glm::vec3{0,-1,0}, deltaTime);

    } else if (m_keyMap[Qt::Key::Key_Control]) {
        // Translates the camera along the world space vector (0, -1, 0)
        translateCamera(glm::vec3{0,1,0}, deltaTime);
    }


    update(); // asks for a PaintGL() call to occur
}
