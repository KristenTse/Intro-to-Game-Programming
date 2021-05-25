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
glm::mat4 weedMatrix, sunMatrix, cloudMatrix, sandMatrix;

//Initialize Objects
GLuint weedTextureID, sunTextureID, cloudTextureID, sandTextureID;
float weed_x = -6, weed_rotate = 0;
float sun_rotate = 0;
float cloud_x = 5;


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
    displayWindow = SDL_CreateWindow("2D Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.4f, 0.8f, 1.0f, 1.0f); //background color

    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sandTextureID = LoadTexture("sand.png");
    weedTextureID = LoadTexture("tumbleweed.png");
    sunTextureID = LoadTexture("sun.png");
    cloudTextureID = LoadTexture("cloud.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    //sand
    sandMatrix = glm::mat4(1.0f);
    sandMatrix = glm::scale(sandMatrix, glm::vec3(10.0f, 5.0f, 1.0f));
    sandMatrix = glm::translate(sandMatrix, glm::vec3(0.0f, -0.3f, 0.0f));

    //tumbleweed
    weed_x += 1.0f * deltaTime;
    weed_rotate += -100.0f * deltaTime;
    weedMatrix = glm::mat4(1.0f);
    weedMatrix = glm::scale(weedMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    weedMatrix = glm::translate(weedMatrix, glm::vec3(weed_x, -1.5f, 0.0f));
    weedMatrix = glm::rotate(weedMatrix, glm::radians(weed_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    //sun
    sun_rotate += -50.0f * deltaTime;
    sunMatrix = glm::mat4(1.0f);
    sunMatrix = glm::scale(sunMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    sunMatrix = glm::translate(sunMatrix, glm::vec3(2.0f, 1.5f, 0.0f));
    sunMatrix = glm::rotate(sunMatrix, glm::radians(sun_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    //cloud
    cloud_x -= 0.3f * deltaTime;
    cloudMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::scale(cloudMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    cloudMatrix = glm::translate(cloudMatrix, glm::vec3(cloud_x, 1.0f, 0.0f));

    //wrap around
    if (cloud_x < -5) {
        cloud_x = 5;
    }   
    if (weed_x > 6) {
        weed_x = -6;
    }
}

void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);


    program.SetModelMatrix(sandMatrix);
    glBindTexture(GL_TEXTURE_2D, sandTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(weedMatrix);
    glBindTexture(GL_TEXTURE_2D, weedTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(sunMatrix);
    glBindTexture(GL_TEXTURE_2D, sunTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(cloudMatrix);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
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