#include "End.h"

#define END_WIDTH 11
#define END_HEIGHT 8

unsigned int end_data[] =
{
     5,  6,144,145,146,  6,144,145,146,  6,  7,
    29, 30,168,169,170,197,168,169,170, 30, 31,
    53,196,196,196,196,196,196,196,196,196, 55,
    53,196,196,196,196,196,196,196,196,196, 55,
    53,196,196,196,196,196,196,196,196,196, 55,
    53,196,196,196,196,196,196,196,196,196, 55,
    53,196,196,196,196,196,196,196,196,196, 55,
    77, 78, 78, 78, 78, 78, 78, 78, 78, 78, 79
};


void End::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(END_WIDTH, END_HEIGHT, end_data, mapTextureID, 1.0f, 24, 9);

    // Initialize Game Objects
    state.player = new Entity();
    state.player->textureID = Util::LoadTexture("player_sheet.png");
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, -4, 0);
    state.player->startPoint = state.player->position;
    state.player->speed = 2.5f;

    state.player->animUp = new int[4]{ 1, 5, 9, 13 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };
    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 2, 6, 10, 14 };
    state.player->animIndices = state.player->animDown;
    state.player->animFrames = 4;
    state.player->animCols = 4;
    state.player->animRows = 7;

    state.player->height = 0.8;
    state.player->width = 0.8;
}


void End::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
}


void End::Render(ShaderProgram* program) {

    state.map->Render(program);

    state.player->Render(program);

    GLuint fontTextureID = Util::LoadTexture("font.png");
    Util::DrawText(program, fontTextureID, "You Win!", 0.75f, -0.25f, glm::vec3(3.3f, -3.5f, 0));
}