#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "utils/sceneparser.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    RenderData metaData;

    GLuint m_shader; // Stores id of shader program
    GLuint m_filter_shader; //Stores id of filter shader program

    GLuint m_sphere_vao; // Stores id of vao for spheres
    GLuint m_sphere_vbo; // Stores id of vbo for spheres

    GLuint m_cube_vbo; // Stores id of vbo for cubes
    GLuint m_cube_vao;

    GLuint m_cone_vbo;
    GLuint m_cone_vao;

    GLuint m_cylinder_vbo;
    GLuint m_cylinder_vao;
    void paintGeometryPhong();

    //CONSTANTS
    GLuint m_defaultFBO = 2; //OS specific - the default display screen
    float m_rotationSpeed = 75;

    //FBO stuff
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    int m_fbo_width;//used to set the size of the FBO (should be the same size as the screen)
    int m_fbo_height;
    GLuint m_fbo; //where the default geometry is rendered to
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    void makeFBO(); //generate the FBO
    void paintFilteredFBO(GLuint texture);

    //store global sizes of data arrays, which is useful for rendering triangles
    //updated when parameters 1 and 2 are changed
    int unitSphereDataSize;
    int unitCubeDataSize;
    int unitConeDataSize;
    int unitCylinderDataSize;

    void setupViewAndCamera();
    void calculateProjectionMatrix();

    bool ready = false;

    glm::vec3 look;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 cameraRotMat;
    glm::mat4 cameraTransMat;
    glm::vec4 cameraPosition;

    void translateCamera(glm::vec3 translationMatrix, float deltaTime);
    void rotateCamera(glm::vec3 axis, float angle);

    void initializeBuffers();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;
};
