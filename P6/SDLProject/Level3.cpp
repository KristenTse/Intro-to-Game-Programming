#include "Level3.h"

#define LEVEL3_ENEMY_COUNT 14
#define LEVEL3_WIDTH 19
#define LEVEL3_HEIGHT 21

unsigned int level3_data[] =
{
     5,  6,  6,  6,144,145,146,147,148,149,  6,  6,  6,  6,  6,  6,  6,  6,  7,
    29, 30, 30, 30,168,169,170,171,172,173, 30, 30, 30, 30, 30,  0, 30, 30, 31,
    53,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,124, 55,
    53,193,196,196,196,196,196,196,196,196,196,196,192,196,196,196,196,196, 55,
    53,196,196,  8, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 79,
    53,196,196, 27,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,
    53,196,196, 80, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31,
    53,196,196,196,196,196,196,195,196,196,196,196,196,196,196,196,196,196, 55,
    53,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,193,196, 55,
    53,195,196,  8, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  9,196,196, 55,
    53,196,196, 27,  5,  6,  6,  7, 25, 25, 25,  5,  6,  6,  7, 28,196,196, 55,
    53,196,196, 56, 51, 30, 30, 52,  6,  6,  6, 51, 30, 30, 52, 57,196,196, 55,
    53,196,196, 80, 81,192,196, 80, 30, 30, 30, 81,196,196, 80, 81,196,196, 55,
    53,196,196,196,196,196,196,196,196,196,196,196,196,195,196,196,196,195, 55,
    53,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196, 55,
     3, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  9,196,196, 55,
     5, 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  28,196,196, 55,
    29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 81,196,196, 55,
    53,196,196,196,196,196,196,196,196,196,192,196,196,196,196,196,196,196, 55,
    53,196,196,196,193,196,196,196,196,196,196,196,196,196,196,196,196,196, 55,
    77, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 79
};


void Level3::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 24, 9);

    // Initialize Game Objects

    // Initalize Door
    state.door = new Entity();
    state.door->textureID = Util::LoadTexture("door_sheet.png");
    state.door->entityType = OBJECT;
    state.door->position = glm::vec3(15, -1, 0);

    state.door->animUp = new int[4]{ 0, 1, 2, 3 }; //open
    state.door->animDown = new int[4]{ 0, 0, 0, 0 }; //closed
    state.door->animIndices = state.door->animDown;
    state.door->animFrames = 4;
    state.door->animCols = 4;
    state.door->animRows = 1;


    // Initalize Lever
    state.lever = new Entity();
    state.lever->textureID = Util::LoadTexture("switch_sheet.png");
    state.lever->entityType = OBJECT;
    state.lever->position = glm::vec3(17, -2, 0);

    state.lever->animUp = new int[7]{ 0, 1, 2, 3, 4, 5, 6 }; //open
    state.lever->animDown = new int[7]{ 0, 0, 0, 0, 0, 0, 0 }; //closed
    state.lever->animIndices = state.lever->animDown;
    state.lever->animFrames = 7;
    state.lever->animCols = 7;
    state.lever->animRows = 1;
    state.lever->animTime = 0.13f;


    // Initialize Player
    state.player = new Entity();
    state.player->textureID = Util::LoadTexture("player_sheet.png");
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.5, -18.5, 0);
    state.player->startPoint = state.player->position;
    state.player->speed = 2.5f;

    state.player->animUp = new int[4]{ 1, 5, 9, 13 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };
    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 2, 6, 10, 14 };
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animCols = 4;
    state.player->animRows = 7;

    state.player->height = 0.8;
    state.player->width = 0.8;

    
    // Initialize Enemies
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("green_slime_sheet.png");

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].textureID = enemyTextureID;

        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiState = IDLE;
        state.enemies[i].speed = 1;

        state.enemies[i].animUp = new int[4]{ 1, 5, 9, 13 };
        state.enemies[i].animDown = new int[4]{ 0, 4, 8, 12 };
        state.enemies[i].animLeft = new int[4]{ 2, 6, 10, 14 };
        state.enemies[i].animRight = new int[4]{ 3, 7, 11, 15 };
        state.enemies[i].animFrames = 4;
        state.enemies[i].animCols = 4;
        state.enemies[i].animRows = 4;

        state.enemies[i].height = 0.7f;
        state.enemies[i].width = 0.9f;
    }

    for (int i = 0; i < 9; i++) {
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiAxis = VERTICAL;
    }

    state.enemies[0].position = glm::vec3(6, -18.5, 0);
    state.enemies[1].position = glm::vec3(11, -18.5, 0);
    state.enemies[2].position = glm::vec3(9, -13, 0);
    state.enemies[3].position = glm::vec3(5.5, -7, 0);
    state.enemies[4].position = glm::vec3(12.5, -7, 0);
    state.enemies[5].position = glm::vec3(7, -2, 0);
    state.enemies[6].position = glm::vec3(12, -2, 0);

    state.enemies[7].aiAxis = HORIZONTAL;
    state.enemies[7].position = glm::vec3(1.5, -10.5, 0);
    state.enemies[8].aiAxis = HORIZONTAL;
    state.enemies[8].position = glm::vec3(16.5, -10.5, 0);


    //Red Enemies
    enemyTextureID = Util::LoadTexture("red_slime_sheet.png");

    for (int i = 9; i < 14; i++) {
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].aiType = WAITANDGO;
        state.enemies[i].speed = 2;
    }
    state.enemies[9].position = glm::vec3(5.5, -12, 0);
    state.enemies[10].position = glm::vec3(12.5, -12, 0);
    state.enemies[11].position = glm::vec3(16.5, -18.5, 0);
    state.enemies[12].position = glm::vec3(1.5, -2.5, 0);
    state.enemies[13].position = glm::vec3(9, -7.5, 0);
}


void Level3::Update(float deltaTime) {

    state.door->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    state.lever->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    int RemainingEnemies = 0;
    for (int i = 0; i < LEVEL3_ENEMY_COUNT;i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

        if (state.enemies[i].isActive) RemainingEnemies++;
    }
    if (RemainingEnemies == 0) {
        state.lever->animIndices = state.lever->animUp;
        state.door->animIndices = state.door->animUp;

        if (state.player->position.x > (state.door->position.x - 0.5f) &&
            state.player->position.x < (state.door->position.x + 0.5f) &&
            state.player->position.y >= (state.door->position.y - 0.5f))
            state.nextScene = 5;
    }

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    for (int i = 0; i < state.arrows.size(); i++) {
        state.arrows[i]->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
}


void Level3::Render(ShaderProgram* program) {

    state.map->Render(program);
    state.door->Render(program);
    state.lever->Render(program);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT;i++) {
        state.enemies[i].Render(program);
    }

    state.player->Render(program);

    for (int i = 0; i < state.arrows.size(); i++) {
        state.arrows[i]->Render(program);
    }
}