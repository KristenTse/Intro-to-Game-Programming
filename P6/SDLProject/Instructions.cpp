#include "Instructions.h"

#define INST_WIDTH 11
#define INST_HEIGHT 8

unsigned int inst_data[] =
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


void Instructions::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(INST_WIDTH, INST_HEIGHT, inst_data, mapTextureID, 1.0f, 24, 9);

    // Initialize Game Objects
    state.player = new Entity();
}


void Instructions::Update(float deltaTime) {
}


void Instructions::Render(ShaderProgram* program) {

    state.map->Render(program);

    GLuint fontTextureID = Util::LoadTexture("font.png");
    Util::DrawText(program, fontTextureID, "Instructions", 1.0f, -0.5f, glm::vec3(2.35f, -2.25f, 0));
    Util::DrawText(program, fontTextureID, "Use the spacebar to shoot arrows.", 0.6f, -0.35f, glm::vec3(1.0f, -3.25f, 0));
    Util::DrawText(program, fontTextureID, "Kill all enemies to open the next", 0.6f, -0.35f, glm::vec3(1.0f, -4.25f, 0));
    Util::DrawText(program, fontTextureID, "door.", 0.6f, -0.35f, glm::vec3(1.0f, -4.75, 0));

    Util::DrawText(program, fontTextureID, "Press enter to start", 0.75f, -0.4f, glm::vec3(1.68f, -6.0f, 0));
}