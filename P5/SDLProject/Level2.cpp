#include "Level2.h"

#define LEVEL2_ENEMY_COUNT 1
#define LEVEL2_WIDTH 22
#define LEVEL2_HEIGHT 9

unsigned int level2_data[] =
{
    12, 12, 19, 12, 12,  8, 18, 12, 12, 12, 12,  9, 12, 18, 12, 19, 12,  8, 12, 12, 12, 12,
    22, 22, 22, 22, 22, 22, 22, 22,  5, 18,  4, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 
    10,  0,  0,  0,  0,  0,  0,  0, 11, 12, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    10,  0, 31,  0,  0,  0,  0,  0, 11,  9, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     2,  7,  3,  0,  0,  0, 26, 27, 21, 22, 23,  0,  0,  0, 31, 33, 31,  0, 31, 32, 31, 32,
    12, 12, 13, 24, 25,  0,  0, 37,  0,  0,  0,  0,  0,  0,  1,  2,  2,  6,  2,  2,  2,  2,
    12, 19, 13, 36, 32, 31, 32, 35, 31, 32, 31, 32, 26, 29, 11, 18,  9, 16,  8, 19,  9, 12,
    18, 12, 14,  2, 17,  2,  2,  6,  7,  2,  2,  2, 17,  7, 15, 19, 12, 12, 18, 12, 12, 12,
    12,  9, 12, 12, 12,  8, 12,  9, 12,  8, 12, 18, 19,  8, 12, 12, 18,  9, 12, 12, 18, 12
};


void Level2::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 10, 8);

    // Initialize Game Objects

    // Initialize Background
    state.background = new Entity();
    state.background->textureID = Util::LoadTexture("background.png");
    state.background->entityType = BACKGROUND;
    state.background->position = glm::vec3(LEVEL2_WIDTH / 2, -5.5, 0);


    // Initialize Player
    state.player = new Entity();
    state.player->textureID = Util::LoadTexture("player_sheet.png");
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.5f, -3, 0);
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
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("snail_sheet.png");
    state.enemies[0].textureID = enemyTextureID;

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].position = glm::vec3(7, -6, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 1;

    state.enemies[0].animLeft = new int[5]{ 0, 1, 2, 3, 4 };
    state.enemies[0].animRight = new int[5]{ 5, 6, 7, 8, 9 };
    state.enemies[0].animFrames = 5;
    state.enemies[0].animCols = 5;
    state.enemies[0].animRows = 2;

    state.enemies[0].height = 0.55;
    state.enemies[0].width = 0.8;
}


void Level2::Update(float deltaTime) {
    state.background->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT;i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    if (state.player->position.x > 16) {
        state.nextScene = 3;
    }
}


void Level2::Render(ShaderProgram* program) {
    state.background->Render(program);
    state.map->Render(program);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT;i++) {
        state.enemies[i].Render(program);
    }

    state.player->Render(program);
}