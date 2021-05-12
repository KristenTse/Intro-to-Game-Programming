#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Effects.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Instructions.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "End.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[6];

Effects* effects;

GLuint fontTextureID;

float xHUD, yHUD;
int lives = 3;

Mix_Music* music;
Mix_Chunk* shoot;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Final Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.18f, 0.16f, 0.23f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fontTextureID = Util::LoadTexture("font.png");

    // Create and switch to level
    sceneList[0] = new Menu();
    sceneList[1] = new Instructions();
    sceneList[2] = new Level1();
    sceneList[3] = new Level2();
    sceneList[4] = new Level3();
    sceneList[5] = new End();
    SwitchToScene(sceneList[0]);

    //Effects
    effects = new Effects(projectionMatrix, viewMatrix);
    effects->Start(FADEIN, 1.0f);

    // Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("bg_music.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 5);

    shoot = Mix_LoadWAV("shoot.wav");
}


void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

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
                if (currentScene != sceneList[0] && 
                    currentScene != sceneList[1] && 
                    currentScene != sceneList[5] && 
                    !currentScene->state.player->lose) {
                    currentScene->SpawnArrow(currentScene->state.player);
                    Mix_PlayChannel(-1, shoot, 0);
                }

                break;
            case SDLK_RETURN:
                if (currentScene == sceneList[0]) {
                    currentScene->state.nextScene = 1;
                }
                else if (currentScene == sceneList[1]) {
                    currentScene->state.nextScene = 2;
                }
                break;
            }
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);


    if (keys[SDL_SCANCODE_UP]) {
        currentScene->state.player->movement.y = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animUp;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        currentScene->state.player->movement.y = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animDown;
    }
    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }

    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
bool lastCollidedBottom = false;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        currentScene->Update(FIXED_TIMESTEP);

        lastCollidedBottom = currentScene->state.player->collidedBottom;

        effects->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    if (currentScene->state.player->hit) {
        lives--;
        currentScene->state.player->hit = false;

        if (lives <= 0) {
            currentScene->state.player->lose = true;
        }
        else {
            currentScene->state.player->position = currentScene->state.player->startPoint;
        }
    }

    //move camera opposite to player, move up into view
    viewMatrix = glm::mat4(1.0f);
    if (currentScene == sceneList[0] || currentScene == sceneList[1]) {
        xHUD = 5;
    }
    else {
        xHUD = currentScene->state.player->position.x;
    }

    if (currentScene == sceneList[0] || currentScene == sceneList[1]) {
        yHUD = -3.75;
    }
    else {
        yHUD = currentScene->state.player->position.y;
    }

    viewMatrix = glm::translate(viewMatrix, glm::vec3(-xHUD, -yHUD, 0));
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);
    currentScene->Render(&program);
    effects->Render();

    if (currentScene != sceneList[0] && currentScene != sceneList[1]) {
        Util::DrawText(&program, fontTextureID, "Lives: " + std::to_string(lives), 0.5f, -0.25f, glm::vec3(xHUD - 4.5f, yHUD + 3.25f, 0));
        
        if (currentScene->state.player->lose) {
            Util::DrawText(&program, fontTextureID, "You Lose!", 0.75f, -0.25f, glm::vec3(xHUD - 2, yHUD + 0.5f, 0));
            currentScene->state.player->isActive = false;
        }
    }

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
        if (currentScene->state.nextScene >= 0) {
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        }
        Render();
    }

    Shutdown();
    return 0;
}
