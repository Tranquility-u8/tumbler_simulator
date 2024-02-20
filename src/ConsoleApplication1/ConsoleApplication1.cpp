#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <learnopengl/shader.h>
#include <learnopengl/model.h>

#include "Tumbler.h"
#include "Particle.h"
#include "Ball.h"



/* META DATA */
#define NR_TUMBLER 5
#define NR_WALL 6
#define NR_BALL 30

// room
float leftPosition[] = {
    -2.5f,  2.5f,  2.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    -2.5f,  2.5f, -2.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    -2.5f, -2.5f, -2.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    -2.5f, -2.5f, -2.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    -2.5f, -2.5f,  2.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    -2.5f,  2.5f,  2.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
};

float backPosition[] = {
    -2.5f, -2.5f, -2.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     2.5f,  2.5f, -2.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
     2.5f, -2.5f, -2.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     2.5f,  2.5f, -2.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -2.5f, -2.5f, -2.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    -2.5f,  2.5f, -2.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
};

float rightPosition[] = {
     2.5f,  2.5f,  2.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
     2.5f, -2.5f, -2.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
     2.5f,  2.5f, -2.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
     2.5f, -2.5f, -2.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
     2.5f,  2.5f,  2.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
     2.5f, -2.5f,  2.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
};

float bottomPosition[] = {
    -2.5f, -2.5f, -2.5f,  0.0f, 1.0f,  0.0f, 1.0f,  0.0f,
     2.5f, -2.5f, -2.5f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f,
     2.5f, -2.5f,  2.5f,  1.0f, 0.0f,  0.0f, 1.0f,  0.0f,
     2.5f, -2.5f,  2.5f,  1.0f, 0.0f,  0.0f, 1.0f,  0.0f,
    -2.5f, -2.5f,  2.5f,  0.0f, 0.0f,  0.0f, 1.0f,  0.0f,
    -2.5f, -2.5f, -2.5f,  0.0f, 1.0f,  0.0f, 1.0f,  0.0f,
};

float topPosition[] = {
    -2.5f,  2.5f, -2.5f,  0.0f, 1.0f,  0.0f,  -1.0f,  0.0f,
     2.5f,  2.5f,  2.5f,  1.0f, 0.0f,  0.0f,  -1.0f,  0.0f,
     2.5f,  2.5f, -2.5f,  1.0f, 1.0f,  0.0f,  -1.0f,  0.0f,
     2.5f,  2.5f,  2.5f,  1.0f, 0.0f,  0.0f,  -1.0f,  0.0f,
    -2.5f,  2.5f, -2.5f,  0.0f, 1.0f,  0.0f,  -1.0f,  0.0f,
    -2.5f,  2.5f,  2.5f,  0.0f, 0.0f,  0.0f,  -1.0f,  0.0f,
};

float wallsPos[NR_WALL] = { // for collision detection
    -2.5,
    2.5,
    -2.5,
    2.5,
    -2.5,
    2.5,
};

glm::vec3 wallsDir[NR_WALL] = { // for collision detection
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(-1.0, 0.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, -1.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(0.0, 0.0, -1.0),
};

glm::vec3 roomPosition = glm::vec3(0.0f, 0.0f, 0.0f);

// lamp
glm::vec3 lampPos = glm::vec3(0.0, 2.45, 0.0);
glm::vec3 lightPos = glm::vec3(0.0, 2.0, 0.0); // avoid potential distortion in shadow mapping

// tumbler
glm::vec3 tumblerPositions[NR_TUMBLER] = {
    glm::vec3(0.0, -2.25, 0.0),
    glm::vec3(1.0, -2.25, 1.0),
    glm::vec3(1.0, -2.25, -1.0),
    glm::vec3(-1.0, -2.25, 1.0),
    glm::vec3(-1.0, -2.25, -1.0),
};
glm::mat4 tumblerModel = glm::mat4(1.0);
Tumbler tumblers[NR_TUMBLER];
Model::AABB basicTumblerBox;
Model::AABB tumblerBox[NR_TUMBLER];

// balls (include fireballs)
vector<Ball> balls;

// fireballs
int texture1;
int texture2;
bool summonFlame;
const GLchar* varyings[7] = {
    "type1",
    "age1",
    "alpha1",
    "size1",
    "life1",
    "position1",
    "velocity1"
};
vector<ParticleSystem> flames;
unsigned int nr_point = 1;

// ashes (for those dropping balls)
vector<ParticleSystem> ashes;

// window
const GLuint SCR_WIDTH = 1250;
const GLuint SCR_HEIGHT = 1250;

// mouse
float xpos;
float ypos;
float zpos;
glm::vec3 target;

bool isPressedMouse = false;
int lastMoveTumbler = -1;
int lastRotateTumbler = -1;

// keyboard
bool isPressedB = false;
bool wasPressedB = false;
bool isSummonBall = false;

bool isPressedF = false;
bool wasPressedF = false;
bool isSummonFireball = false;

// camera
float near = 0.1f;
float far = 100.0f;

float fov = 45.0f;
float yaw = -90.0f;
float pitch = 0.0f;

float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

//glm::vec3 cameraPos = glm::vec3(0.0f, -2.0f, 5.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view;
glm::mat4 projection;
const glm::mat4 model = glm::mat4(1.0);

// time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// shadow
float near_plane = 1.0f;
float far_plane = 30.0f;

const GLuint SHADOW_WIDTH = 2048;
const GLuint SHADOW_HEIGHT = 2048;

// blur
bool bloom = true;
float exposure = 1.0f;

unsigned int quadVAO = 0;
unsigned int quadVBO;
unsigned int amount = 20; // times to blur

/* FUNCTIONS */
// I/O
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void summonFireball();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int loadTexture(char const* path);
unsigned int loadCubeTexture(const char* path[]);

void releaseBuffer(std::vector<GLuint> vaos, std::vector<GLuint> vbos);

// render
void renderConfig(Shader& shader, int hdrFBO, int ubo, int depthCubeMap);
void renderScene(Shader& shader,
    int lvao, int bvao, int rvao, int cvao, int fvao, int tex,
    Model& lamp,
    Model& tumbler,
    Model::AABB& basicTumblerBox,
    Model& ball
);
void renderRoom(Shader& shader, int lvao, int bvao, int rvao, int cvao, int fvao, int tex);
void renderLamp(Shader shader, Model& lamp);
void renderTumbler(Shader shader, Model& tumbler, Model::AABB basicTumblerBox);
void renderBall(Shader& shader, Model& ball);
void renderFlame(Shader& normalShader, Shader& feedbackShader, Shader particleShader, int texture1, int texture2);
void shaderConfig(Shader& shader, int depthMapFBO);
void renderQuad();
void blurScene(Shader& blurShader, Shader& blendShader, unsigned int pingpongFBO[], unsigned int colorBuffers[], unsigned int pingpongColorBuffers[2]);

// update
void updateScene(Model& tumbler, Model& ball);
void updateTumbler(Model& tumbler);
void updateBall(Model& ball);

// collision
bool checkCollisionBetweenTwoBox(const Model::AABB& a, const Model::AABB& b);
bool checkCollisionBetweenBoxAndPoint(const Model::AABB& a, const glm::vec3& p);
bool checkCollisionBetweenLowerBoxAndPoint(const Model::AABB& a, const glm::vec3& p);

void moveTumbler(int tumblerId, float xposIn, float yposIn);
void rotateTumbler(int tumblerId, float xposIn, float yposIn);

/* PROGRAM ENTRY*/
int main()
{
    /* GLFW CONFIGURATION */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cg_2023", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // glfw config
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* CREATE ENTITY */
    // create shader
    Shader normalShader("normal.vs", "normal.fs", "normal.gs");
    Shader depthShader("depth.vs", "depth.fs", "depth.gs");
    Shader feedbackShader("feedback.vs", "feedback.fs", "feedback.gs", varyings, 7);
    Shader particleShader("particle.vs", "particle.fs");
    Shader blurShader("blur.vs", "blur.fs");
    Shader blendShader("blend.vs", "blend.fs");

    // create left wall
    unsigned int leftVBO, leftVAO;
    glGenVertexArrays(1, &leftVAO);
    glGenBuffers(1, &leftVBO);

    glBindVertexArray(leftVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leftVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftPosition), leftPosition, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create back wall
    unsigned int backVBO, backVAO;
    glGenVertexArrays(1, &backVAO);
    glGenBuffers(1, &backVBO);

    glBindVertexArray(backVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backPosition), backPosition, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create right wall
    unsigned int rightVBO, rightVAO;
    glGenVertexArrays(1, &rightVAO);
    glGenBuffers(1, &rightVBO);

    glBindVertexArray(rightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightPosition), rightPosition, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create ceiling
    unsigned int ceilingVBO, ceilingVAO;
    glGenVertexArrays(1, &ceilingVAO);
    glGenBuffers(1, &ceilingVBO);

    glBindVertexArray(ceilingVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ceilingVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(topPosition), topPosition, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create floor
    unsigned int floorVBO, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bottomPosition), bottomPosition, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int boardTex = loadTexture("resource/texture/board.jpg");

    // create lamp
    Model lamp("resource/model/ball/ball.obj");

    // create tumbler
    Model tumbler("resource/model/tumbler/tumbler.obj");
    basicTumblerBox = tumbler.calculateAABB();
    for (int i = 0; i < NR_TUMBLER; i++) {
        tumblers[i] = Tumbler(tumblerPositions[i], basicTumblerBox.min, basicTumblerBox.max);
    }

    // create ball
    Model ball("resource/model/ball/ball.obj");
   
    std::vector<int> numbers;
    for (int i = 0; i < NR_BALL; ++i) {
        numbers.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g); // generate random positions
    std::vector<int> selectedNumbers(numbers.begin(), numbers.begin() + NR_BALL);

    for (int i = 0; i < NR_BALL; i++) {
        int z = selectedNumbers[i] / 25;
        int tmp = selectedNumbers[i] % 25;
        int y = tmp / 5;
        int x = tmp % 5;
        balls.emplace_back(Ball(glm::vec3(x - 2.0, 2.0 - z, y - 2.0)));
    }
    
    // create particle system (flame)
    summonFlame = false;
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    unsigned int texture1 = loadTexture("resource/texture/flame.bmp");
    unsigned int texture2 = loadTexture("resource/texture/particle.bmp");
    
    /* SHADER CONFIGURATION */
    unsigned int UBO;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);

    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(glm::mat4));
    normalShader.use();

    normalShader.setFloat("material.shininess", 32.0f);
    normalShader.setInt("material.diffuse", 0);
    normalShader.setInt("material.specular", 1);

    normalShader.setInt("texture1", 2);
    normalShader.setInt("depthMap", 3);
    normalShader.setInt("isDrop", 0);

    normalShader.setInt("nr_point", nr_point);
    normalShader.setVec3("plights[0].position", lightPos);
    normalShader.setVec3("plights[0].ambient", 0.1, 0.1, 0.1);
    normalShader.setVec3("plights[0].diffuse", 0.6, 0.6, 0.6);
    normalShader.setVec3("plights[0].specular", 0.7, 0.7, 0.7);
    normalShader.setFloat("plights[0].constant", 1.0);
    normalShader.setFloat("plights[0].linear", 0.05);
    normalShader.setFloat("plights[0].quadratic", 0.02);

    glUniformBlockBinding(normalShader.ID, glGetUniformBlockIndex(normalShader.ID, "Matrices"), 0);

    blurShader.use();
    blurShader.setInt("scene", 0);

    blendShader.use();
    blendShader.setInt("scene", 0);
    blendShader.setInt("bloomBlur", 1);

    /* SHADOW MAPPING */
    // create fbo
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // create depth cubemap texture
    GLuint depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (GLuint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // attach cubemap to fbo
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    /* BLUR */
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // create 2 color buffers
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    // create renderbuffer
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create fbo for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorBuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }


    // tips
    cout << "================================" << endl;
    cout << "   MOVE: WASD SHIFT SPACE" << endl;
    cout << "   ROTATE YOUR VIEW: Q E" << endl;
    cout << "   SUMMON ANY BALLS:  B" << endl;
    cout << "   SUMMON FIREBALLS:  F" << endl;
    cout << "   HOLD LEFT MOUSE BUTTON\n   TO PUSH OR MOVE TUMBLER" << endl;
    cout << "================================" << endl;

    /* MAIN LOOP*/
    while (!glfwWindowShouldClose(window)) {

        // time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // shadow mapping
        shaderConfig(depthShader, depthMapFBO);
        renderScene(depthShader,
            leftVAO, backVAO, rightVAO, ceilingVAO, floorVAO, boardTex,
            lamp,
            tumbler,
            basicTumblerBox,
            ball
        );

        // render the scene
        renderConfig(normalShader, hdrFBO, UBO, depthCubemap); 
        renderScene(normalShader,
            leftVAO, backVAO, rightVAO, ceilingVAO, floorVAO, boardTex,
            lamp,
            tumbler,
            basicTumblerBox,
            ball
        );
        updateScene(tumbler, ball);

        // input (read depth component for picking before we move to next stage)
        processInput(window);
        
        // blur and blend the scene
        renderFlame(normalShader, feedbackShader, particleShader, texture1, texture2);
        blurScene(blurShader, blendShader, pingpongFBO, colorBuffers, pingpongColorBuffers);

        // swap front and back fbo
        glfwSwapBuffers(window);
        
        // poll event
        glfwPollEvents();
        
    }
    
    // release
    std::vector<GLuint> vaos;
    std::vector<GLuint> vbos;

    vaos.push_back(leftVAO);
    vaos.push_back(backVAO);
    vaos.push_back(rightVAO);
    vaos.push_back(ceilingVAO);
    vaos.push_back(floorVAO);

    vbos.push_back(leftVBO);
    vbos.push_back(backVBO);
    vbos.push_back(rightVBO);
    vbos.push_back(ceilingVBO);
    vbos.push_back(floorVBO);
    
    releaseBuffer(vaos, vbos);

    glfwTerminate();

    return 0;

}

/* RENDER */
void renderConfig(Shader& shader, int hdrFBO, int ubo, int depthCubemap) {

    // bind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    // bind depth map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

    // init shader
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("cameraPos", cameraPos);
    shader.setFloat("far_plane", far_plane);
}

void renderScene(Shader& shader, 
    int lvao, int bvao, int rvao, int cvao, int fvao, int tex,
    Model& lamp,
    Model& tumbler,
    Model::AABB& basicTumblerBox,
    Model& ball
    ){
    renderRoom(shader, lvao, bvao, rvao, cvao, fvao, tex);
    renderLamp(shader, lamp);
    renderTumbler(shader, tumbler, basicTumblerBox);
    if (isSummonBall)
        renderBall(shader, ball);
}

void renderRoom(Shader& shader, int lvao, int bvao, int rvao, int cvao, int fvao, int tex){

    // set model matrix
    glm::mat4 roomModel = glm::translate(model, roomPosition);

    shader.setMat4("model", roomModel);
    // set lighting mode
    shader.setFloat("lightingMode", 0.0);

    shader.setFloat("colorId", 1.0);
    glBindVertexArray(lvao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.setFloat("colorId", 2.0);
    glBindVertexArray(bvao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.setFloat("colorId", 3.0);
    glBindVertexArray(rvao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.setFloat("colorId", 0.0);
    glBindVertexArray(cvao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.setFloat("lightingMode", 1.0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(fvao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderLamp(Shader shader, Model& lamp) {
    
    // set model matrix
    shader.setMat4("model", glm::scale(glm::translate(model, lampPos), glm::vec3(0.02, 0.008, 0.02)));
    // set lighting mode
    shader.setFloat("lightingMode", 2.0);
    shader.setFloat("colorId", 0.0);
    // render
    lamp.Draw(shader);
}

void renderTumbler(Shader shader, Model& tumbler, Model::AABB basicTumblerBox) {

    // set lighting mode
    shader.setFloat("lightingMode", 1.0);
    for (int i = 0; i < NR_TUMBLER; i++) {
        // set model matrix
        tumblerModel = glm::scale(glm::translate(model, tumblers[i].position) * tumblers[i].rotateMatrix, glm::vec3(5.0, 5.0, 5.0));
        shader.setMat4("model", tumblerModel);
        // render
        if(!tumblers->isBurned)
            tumbler.Draw(shader);
        // calculate box
        tumblerBox[i].min = glm::vec3(tumblerModel * glm::vec4(basicTumblerBox.min, 1.0));
        tumblerBox[i].max = glm::vec3(tumblerModel * glm::vec4(basicTumblerBox.max, 1.0));

    }
}

void renderBall(Shader& shader, Model& ball) {
    
    int num = balls.size();
    for (int i = 0; i < num; i++) {
        // set model matrix
        glm::mat4 ballModel = glm::scale(glm::translate(model, balls[i].position), glm::vec3(0.005, 0.005, 0.005));
        shader.setMat4("model", ballModel);
        shader.setFloat("colorId", balls[i].getColor());
        // for fireball
        if (balls[i].type == Ball::BallType::Fireball) {
            shader.setFloat("lightingMode", 2.0);
        }
        else {
            shader.setFloat("lightingMode", 0.0);
        }
        // for dropping ball
        if (balls[i].type == Ball::BallType::Dropball) {
            shader.setInt("isDrop", 1);
            shader.setFloat("time", balls[i].lifetime);
        }
        else {
            shader.setInt("isDrop", 0);
        }
        if (balls[i].type == Ball::BallType::DeadBall) continue;
        ball.Draw(shader);
    }
}

void renderFlame(Shader& normalShader, Shader &feedbackShader, Shader particleShader, int texture1, int texture2) {

    if (summonFlame) {

        // create flame
        flames.emplace_back(ParticleSystem(feedbackShader, particleShader, texture1, texture2));
        summonFlame = false;

        // create pointlight
        normalShader.use();
        normalShader.setVec3("plights[" + to_string(nr_point) + "]position", balls[NR_BALL + flames.size() - 1].position);
        normalShader.setVec3("plights[" + to_string(nr_point) + "].ambient", 0.05, 0.05, 0.05);
        normalShader.setVec3("plights[" + to_string(nr_point) + "].diffuse", 0.3, 0.28, 0.28);
        normalShader.setVec3("plights[" + to_string(nr_point) + "].specular", 0.05, 0.05, 0.05);
        normalShader.setFloat("plights[" + to_string(nr_point) + "].constant", 1.0);
        normalShader.setFloat("plights[" + to_string(nr_point) + "].linear", 0.5);
        normalShader.setFloat("plights[" + to_string(nr_point) + "].quadratic", 0.3);
        nr_point++;
        normalShader.setInt("nr_point", nr_point);
    }
    for (int i = 0; i < flames.size(); i++) {
        
        // reset light pos
        normalShader.use();
        normalShader.setVec3("plights[" + to_string(i + 1) + "]position", balls[NR_BALL + i].position);
        
        // calculate flame's pos and dir
        glm::vec3 p = balls[NR_BALL + i].position;
        glm::vec3 v0 = glm::vec3(0, 1, 0);
        glm::vec3 v1 = glm::vec3(-1.0) * glm::normalize(balls[NR_BALL + i].velocity); 
        glm::vec3 axis = glm::cross(v0, v1);
        float angle = acos(glm::dot(glm::normalize(v0), glm::normalize(v1)));

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
        glm::mat4 flameModel = glm::translate(model, p) * rotationMatrix;
        flames[i].renderSystem(feedbackShader, particleShader, deltaTime, flameModel, view, projection);
    }

    // render ashes for dropping ball
    for (int i = 0; i < balls.size(); i++) {
        if (balls[i].type == Ball::BallType::Dropball) {
            if (balls[i].startDrop) 
                balls[i].initDrop(feedbackShader, particleShader, texture1, texture2, 300.0f, 0.5f);        
            balls[i].ashe.render(feedbackShader, particleShader, deltaTime, view, projection);
        }
    }
    // render ashes for burned tumbler
    for (int i = 0; i < NR_TUMBLER; i++) {
        if (tumblers[i].isBurned) {
            if(!tumblers[i].isPrepare)
                tumblers[i].initBurned(feedbackShader, particleShader, texture1, texture2, 200.0f, 1.0f);
            tumblers[i].ashe.render(feedbackShader, particleShader, deltaTime, view, projection);
        }
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void shaderConfig(Shader& shader, int depthMapFBO) {

    // bind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    shader.use();
    
    // init shader
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    // set shader matrix (6 dirs)
    for (unsigned int i = 0; i < 6; ++i)
        shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    shader.setVec3("lightPos", lightPos);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("cameraPos", cameraPos);
    shader.setFloat("far_plane", far_plane);
}

void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void blurScene(Shader& blurShader, Shader& blendShader, unsigned int pingpongFBO[], unsigned int colorBuffers[], unsigned int pingpongColorBuffers[2]) {
    
    // gauss-blur
    bool horizontal = true, first_iteration = true;
    blurShader.use();
    glActiveTexture(GL_TEXTURE0);
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blurShader.setInt("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    // blend
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    blendShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
    blendShader.setInt("bloom", bloom);
    blendShader.setFloat("exposure", exposure);
    renderQuad();
}


/* UPDATE */
void updateScene(Model& tumbler, Model& ball) {
    updateTumbler(tumbler);
    if(isSummonBall)
        updateBall(ball);
}

void updateTumbler(Model& tumbler) {
    for (int i = 0; i < NR_TUMBLER; i++) {
        // update position
        tumblers[i].update(deltaTime);
    }
}

void updateBall(Model& ball) {

    int num = balls.size();
    // update position
    for (int i = 0; i < num; i++) {
        balls[i].update(deltaTime);
    }
    // detect collision
    for (int i = 0; i < num; i++) {

        bool isHit = false;

        // hit the wall
        for (int j = 0; j < NR_WALL; j++) {
            if (Ball::checkCollisionBetweenBallAndWall(balls[i], wallsDir[j], wallsPos[j])) {
                isHit = true;
                Ball::updateBetweenBallAndWall(balls[i], wallsDir[j]);
                break;
            }
        }
        if (isHit) continue;

        // hit the ball
        for (int k = i + 1; k < num; k++) {
            if (Ball::checkCollisionBetweenBalls(balls[i], balls[k])) {
                isHit = true;
                Ball::updateBetweenBalls(balls[i], balls[k]);
                break;
            }

        }
        if (isHit) continue;

        //hit the tumbler
        for (int z = 0; z < NR_TUMBLER; z++) {
            tumblers[z].updateBox();
            int result = Ball::checkCollisionBetweenBallAndTumbler(balls[i], tumblers[z], wallsDir);
            if (result != -1) {
                //cout << "hit the box" << z << " at " << result << endl;
                Ball::updateBetweenBallAndTumbler(balls[i], tumblers[z], wallsDir[result]);
                break;
            }
        }
    }

}

/* I/O */
void processInput(GLFWwindow* window)
{
    float cameraSpeed = static_cast<float>(4.0 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float xoffset = 0;
    float yoffset = 0;
    
    // move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        xoffset -= cameraSpeed * 10;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        xoffset += cameraSpeed * 10;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

    yaw += xoffset;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    
    // summon balls
    isPressedB = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
    if (isPressedB && !wasPressedB) {
        isSummonBall = !isSummonBall;
    }
    wasPressedB = isPressedB;

    // summon fireball
    isPressedF = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (isPressedF && !wasPressedF) {
        summonFireball();
    }
    wasPressedF = isPressedF;

    // pickup logic
    glReadPixels(xpos, SCR_HEIGHT - ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zpos);

    double x = (xpos * 2.0) / SCR_WIDTH - 1.0;
    double y = 1.0f - (ypos * 2.0) / SCR_HEIGHT;
    double z = zpos * 2.0 - 1.0;
    double w = (2.0 * near * far) / (far + near - z * (far - near));
    glm::vec4 mvp(x * w, y * w, z * w, w);
    target = glm::inverse(view) * glm::inverse(projection) * mvp;
   
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        isPressedMouse = true;
        bool isHappenend = false;
        for (int i = 0; i < NR_TUMBLER; i++) {
            if (checkCollisionBetweenBoxAndPoint(tumblerBox[i], glm::vec3(target))) {
                isHappenend = true;
                rotateTumbler(i, xpos, ypos);
                //cout << "You are pushing the box" << i << endl;
                break;
            }
            else if (checkCollisionBetweenLowerBoxAndPoint(tumblerBox[i], glm::vec3(target))) {
                isHappenend = true;
                moveTumbler(i, xpos, ypos);
                //cout << "You are moving the box" << i << endl;
                break;
            }
        }
        if (!isHappenend) {
            lastMoveTumbler = -1;
            lastRotateTumbler = -1;
            //cout << "You are doing nothing" << endl;
        }
    }
    else {
        lastMoveTumbler = -1;
        lastRotateTumbler = -1;
        isPressedMouse = false;
    }  

}

void summonFireball() {
    cout << "summon fireball!" << endl;
    glm::vec3 p = glm::vec3(cameraPos.x, cameraPos.y, 2.0);
    glm::vec3 v = target - p;
    balls.emplace_back(Ball(p, Ball::BallType::Fireball, v));
    summonFlame = true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    xpos = xposIn;
    ypos = yposIn;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubeTexture(const char* path[])
{
    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    unsigned char* data;
    int width, height, nrComponents;
    for (int i = 0; i < 6; i++) {
        data = stbi_load(path[i], &width, &height, &nrComponents, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return skyboxTex;
}

void releaseBuffer(std::vector<GLuint> vaos, std::vector<GLuint> vbos) {
    for (auto vao : vaos) {
        glDeleteVertexArrays(1, &vao);
    }
    for (auto vbo : vbos) {
        glDeleteBuffers(1, &vbo);
    }

}

/* COLLISION */
bool checkCollisionBetweenTwoBox(const Model::AABB& a, const Model::AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool checkCollisionBetweenBoxAndPoint(const Model::AABB& a, const glm::vec3& p) {
    return (a.min.x - 0.1 <= p.x && a.max.x + 0.1 >= p.x) &&
        (a.min.y + 0.4 <= p.y && a.max.y + 0.4 >= p.y) &&
        (a.min.z - 0.1 <= p.z && a.max.z + 0.1 >= p.z);
}

bool checkCollisionBetweenLowerBoxAndPoint(const Model::AABB& a, const glm::vec3& p) {
    return (a.min.x - 0.2 <= p.x && a.max.x + 0.2 >= p.x) &&
        (a.min.y + 0.4 >= p.y) &&
        (a.min.z - 0.2 <= p.z && a.max.z + 0.2 >= p.z);
}

void moveTumbler(int tumblerId, float xposIn, float yposIn) {
    
    if (tumblerId == -1 || tumblerId != lastMoveTumbler)
    {
        lastX = xposIn;
        lastY = yposIn;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xposIn;
    lastY = yposIn;
    lastMoveTumbler = tumblerId;

    float x_sensitivity = 0.004f;
    float y_sensitivity = 0.02f;
    xoffset *= x_sensitivity;
    yoffset *= y_sensitivity;

    tumblers[tumblerId].position -= glm::cross(cameraUp,cameraFront) * xoffset;
    tumblers[tumblerId].position += cameraFront * yoffset;
}

void rotateTumbler(int tumblerId, float xposIn, float yposIn) {

    if (tumblerId == -1 || tumblerId != lastRotateTumbler)
    {
        lastX = xposIn;
        lastY = yposIn;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xposIn;
    lastY = yposIn;
    lastRotateTumbler = tumblerId;

    float x_abs = xoffset > -xoffset ? xoffset : -xoffset;
    float y_abs = yoffset > -yoffset ? yoffset : -yoffset;
    if (x_abs >= y_abs + 5) {
        glm::vec3 dir = cameraFront;
        if (xoffset < 0) dir = -dir;
        tumblers[tumblerId].push(dir, x_abs - y_abs);
    }else if (y_abs >= x_abs + 5){
        tumblers[tumblerId].push(glm::cross(cameraUp, cameraFront), y_abs - x_abs);
    }
    
}