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

#include "Entity.h"
#include "Map.h"
#include "Util.h"

#define ENEMY_COUNT 3

#define MAP_WIDTH 12
#define MAP_HEIGHT 8
unsigned int map_data[] =
{
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 8, 5, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 6, 7, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 6, 7, 1, 1, 1,
    1, 4, 4, 4, 4, 4, 4, 7, 1, 1, 1, 1,
};

struct GameState {
    Entity* player;
    Entity* enemies;
    Map* map;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fontTextureID = Util::LoadTexture("font.png");


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->textureID = Util::LoadTexture("bunny_sheet.png");
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -4, 0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.0f;

    state.player->animRight = new int[2]{ 4, 5 };
    state.player->animLeft = new int[2]{ 6, 7 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 2;
    state.player->animTime = 0;
    state.player->animCols = 8;
    state.player->animRows = 1;

    state.player->width = 0.6f;
    state.player->jumpPower = 6.0f;


    // Initialize Enemies
    state.enemies = new Entity[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiState = IDLE;
        state.enemies[i].speed = 1;
    }

    // Ground Enemies
    for (int i = 0; i < 2; i++) {
        state.enemies[i].textureID = Util::LoadTexture("red_enemy.png");
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);   
        state.enemies[i].height = 0.6f;
        state.enemies[i].width = 0.6f;
    }

    state.enemies[0].aiType = JUMPER;
    state.enemies[0].position = glm::vec3(6, -6, 0);
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].position = glm::vec3(10, -3, 0);

    // Flying Enemy
    state.enemies[2].textureID = Util::LoadTexture("flying_enemy_sheet.png");

    state.enemies[2].animIndices = new int[8]{ 1, 2, 3, 4, 5, 6, 7 };
    state.enemies[2].animFrames = 8;
    state.enemies[2].animTime = 0;
    state.enemies[2].animCols = 8;
    state.enemies[2].animRows = 1;

    state.enemies[2].height = 0.6f;

    state.enemies[2].aiType = FLYER;
    state.enemies[2].position = glm::vec3(1, -2, 0);


    // Initialize Map
    GLuint mapTextureID = Util::LoadTexture("platforms.png");
    state.map = new Map(MAP_WIDTH, MAP_HEIGHT, map_data, mapTextureID, 1.0f, 3, 3);
}


void ProcessInput() {
    state.player->movement = glm::vec3(0);

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
                if (state.player->collidedBottom) {
                    state.player->jump = true;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
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
        int RemainingEnemies = 0;
        for (int i = 0; i < ENEMY_COUNT;i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.map);

            if (state.enemies[i].isActive) {
                RemainingEnemies++;
            }
        }

        if (RemainingEnemies == 0) {
            state.player->win = true;
        }

        state.player->Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.map);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-5.5, 3.25, 0));
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    state.map->Render(&program);

    for (int i = 0; i < ENEMY_COUNT;i++) {
        state.enemies[i].Render(&program);
    }

    state.player->Render(&program);

    if (state.player->win) {
        Util::DrawText(&program, fontTextureID, "You Win!", 0.75f, -0.25f, glm::vec3(3.75f, -3.0f, 0));
    }
    else if (state.player->lose) {
        Util::DrawText(&program, fontTextureID, "You Lose!", 0.75f, -0.25f, glm::vec3(3.5f, -3.0f, 0));
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
        Render();
    }

    Shutdown();
    return 0;
}
