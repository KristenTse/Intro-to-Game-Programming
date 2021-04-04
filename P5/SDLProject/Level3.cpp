#include "Level3.h"

#define LEVEL3_ENEMY_COUNT 2
#define LEVEL3_WIDTH 23
#define LEVEL3_HEIGHT 9

unsigned int level3_data[] =
{
    22, 22, 22,  5, 18,  8,  4, 22, 22, 22, 22, 22,  5, 18, 19,  4, 23, 41,  0,  0,  0,  0, 10,
    41, 42, 41, 21,  5,  4, 23, 42, 41, 42, 41, 42, 21,  5,  4, 23, 50,  0, 44, 45, 46,  0, 10,
    10,  0,  0, 42, 21, 23, 41,  0,  0,  0,  0,  0, 41, 21, 23, 42, 60, 53, 54, 55, 56, 57, 10,
    10,  0,  0,  0, 41, 42,  0,  0,  0, 51, 52,  0,  0, 42, 41,  0, 70, 63, 64, 65, 66, 67, 10,
    10, 31, 32,  0,  0,  0,  0,  0, 39, 61, 62,  0,  0,  0,  0,  0, 31, 73, 74, 75, 76, 77, 10,
     6,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,
    16,  8, 13, 25, 39,  0,  0, 26, 11,  8, 13, 25, 32, 31,  0, 26, 11, 12,  8, 12, 12,  8, 12,
     8, 18, 14,  2,  2, 17,  6,  2, 15, 12, 14,  6,  2,  2, 17,  7, 15, 12, 12, 18,  9, 12, 12,
    19, 12,  9, 12, 12, 12,  8,  9, 12, 19,  8, 12, 18, 19,  8, 12, 12, 18,  9, 12,  8, 18, 12
};


void Level3::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 10, 8);

    // Initialize Game Objects

    // Initialize Background
    state.background = new Entity();
    state.background->textureID = Util::LoadTexture("background.png");
    state.background->entityType = BACKGROUND;
    state.background->position = glm::vec3(LEVEL3_WIDTH / 2, -5.5, 0);


    // Initialize Player
    state.player = new Entity();
    state.player->textureID = Util::LoadTexture("player_sheet.png");
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.5, -4, 0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.5f;

    state.player->animIdleRight = new int[8]{ 0, 1, 2, 3, 4, 5, 6, 7 };
    state.player->animIdleLeft = new int[8]{ 8, 9, 10, 11, 12, 13, 14, 15 };
    state.player->animRight = new int[8]{ 16, 17, 18, 19, 20, 21, 22, 23 };
    state.player->animLeft = new int[8]{ 24, 25, 26, 27, 28, 29, 30, 31 };
    state.player->animIndices = state.player->animIdleRight;
    state.player->animFrames = 8;
    state.player->animCols = 8;
    state.player->animRows = 4;

    state.player->width = 0.7f;
    state.player->jumpPower = 5.0f;


    // Initialize Enemies
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("snail_sheet.png");

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].textureID = enemyTextureID;

        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = IDLE;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].speed = 1;

        state.enemies[i].animLeft = new int[5]{ 0, 1, 2, 3, 4 };
        state.enemies[i].animRight = new int[5]{ 5, 6, 7, 8, 9 };
        state.enemies[i].animFrames = 5;
        state.enemies[i].animCols = 5;
        state.enemies[i].animRows = 2;

        state.enemies[i].height = 0.55;
        state.enemies[i].width = 0.8;
    }
    state.enemies[0].position = glm::vec3(5, -7, 0);
    state.enemies[1].position = glm::vec3(11, -7, 0);
}


void Level3::Update(float deltaTime) {
    state.background->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT;i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    if (state.player->position.x >= 16) {
        state.player->win = true;
    }
}


void Level3::Render(ShaderProgram* program) {
    state.background->Render(program);
    state.map->Render(program);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT;i++) {
        state.enemies[i].Render(program);
    }

    state.player->Render(program);
}