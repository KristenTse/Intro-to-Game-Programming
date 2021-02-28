#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

//Loading an image with STB_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;
glm::mat4 ballMatrix, pLeftMatrix, pRightMatrix;


// Start at 0, 0, 0
glm::vec3 pLeft_position = glm::vec3(-4.75, 0, 0);
glm::vec3 pRight_position = glm::vec3(4.75, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);

// Don’t go anywhere (yet).
glm::vec3 pLeft_movement = glm::vec3(0, 0, 0);
glm::vec3 pRight_movement = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);

// Speed
float p_speed = 3.5f;
float ball_speed = 5.0f;

// Sizes
glm::vec3 p_scale = glm::vec3(0.5f, 2.0f, 1.0f);
glm::vec3 ball_scale = glm::vec3(0.5f, 0.5f, 1.0f);
float p_height = 1.0f * p_scale.y;
float p_width = 1.0f * p_scale.x;
float ball_side = 1.0f * ball_scale.x;

// Window in OpenGl Units
float uWidth = 5.0f;
float uHeight = 3.75f;


GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    pLeftMatrix = glm::mat4(1.0f);
    pRightMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-uWidth, uWidth, -uHeight, uHeight, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f); //drawing color

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //background color

    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
    pLeft_movement = glm::vec3(0); //reset player movement to 0
    pRight_movement = glm::vec3(0); 

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                if (ball_movement.x == 0 && ball_movement.y == 0) { //ALSO CHECK BALL POSITION FOR RESTART
                    float choices[2] = { -1.0, 1.0 };
                    ball_movement.x = choices[rand() % 2];

                    if (ball_position.x != 0.0f) {
                        //reset values
                        pLeft_position = glm::vec3(-4.75, 0, 0);
                        pRight_position = glm::vec3(4.75, 0, 0);
                        ball_position = glm::vec3(0, 0, 0);
                        ball_movement = glm::vec3(0, 0, 0);
                        p_speed = 3.5f;
                    }
                }
                break;
            }
            break; // SDL_KEYDOWN

        }
    }

    //player movement
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    float bound = uHeight - (p_height/2);
    if (keys[SDL_SCANCODE_W] && pLeft_position.y < bound) {
        pLeft_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S] && pLeft_position.y > -bound) {
        pLeft_movement.y = -1.0f;
    }

    if (keys[SDL_SCANCODE_UP] && pRight_position.y < bound) {
        pRight_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && pRight_position.y > -bound) {
        pRight_movement.y = -1.0f;
    }
}


void handleHitBounds() {
    //upper and lower bounds collision detection
    float bound = uHeight - (ball_side / 2);    
    if (ball_position.y > bound) {
        ball_movement.y = -ball_movement.y;
        ball_position.y = bound;
    }
    else if (ball_position.y < -bound) {
        ball_movement.y = -ball_movement.y;
        ball_position.y = -bound;
    }

    //side bounds collision detection
    bound = uWidth - (ball_side / 2);
    if (ball_position.x > bound || ball_position.x < -bound) {
        ball_movement = glm::vec3(0, 0, 0);
        p_speed = 0.0f;
    }
}


bool isColliding(glm::vec3 pPos, float &colOffset) {
    float xdist = fabs(pPos.x - ball_position.x) - ((ball_side + p_width) / 2);
    float ydist = fabs(pPos.y - ball_position.y) - ((ball_side + p_height) / 2);
    colOffset = ball_position.y - pPos.y;
    return ((xdist < 0) && (ydist < 0));
}


void handleHitPaddle() {
    float colOffset;
    if (isColliding(pLeft_position, colOffset)) {
        ball_position.x = pLeft_position.x + p_width;
    }
    else if (isColliding(pRight_position, colOffset)) {
        ball_position.x = pRight_position.x - p_width;
    }
    else {
        return;
    }

    ball_movement.x = -ball_movement.x;
    if (colOffset > 0.0f) {
        ball_movement.y = 1;
    }
    else {
        ball_movement.y = -1;
    }

    if (glm::length(ball_movement) > 1.0f) {
        ball_movement = glm::normalize(ball_movement);
    }
}


float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    //objects
    pLeftMatrix = glm::mat4(1.0f);
    pRightMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);

    //Add (direction * units per second * elapsed time)
    pLeft_position += pLeft_movement * p_speed * deltaTime;
    pRight_position += pRight_movement * p_speed * deltaTime;
    ball_position += ball_movement * ball_speed * deltaTime;

    //translate objects
    pLeftMatrix = glm::translate(pLeftMatrix, pLeft_position);
    pRightMatrix = glm::translate(pRightMatrix, pRight_position);
    ballMatrix = glm::translate(ballMatrix, ball_position);

    //collision detection
    handleHitBounds();
    handleHitPaddle();

    //scale objects
    pLeftMatrix = glm::scale(pLeftMatrix, p_scale);
    pRightMatrix = glm::scale(pRightMatrix, p_scale);
    ballMatrix = glm::scale(ballMatrix, ball_scale);
}

void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(pLeftMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(pRightMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(ballMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
