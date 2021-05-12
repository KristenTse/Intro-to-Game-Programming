#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 8

unsigned int menu_data[] =
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


void Menu::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 24, 9);

    // Initialize Game Objects
    state.player = new Entity();
}


void Menu::Update(float deltaTime) {
}


void Menu::Render(ShaderProgram* program) {

    state.map->Render(program);

    GLuint fontTextureID = Util::LoadTexture("font.png");
    Util::DrawText(program, fontTextureID, "Dungeon Escape", 1.0f, -0.5f, glm::vec3(1.75f, -3.0f, 0));
    Util::DrawText(program, fontTextureID, "Press enter to start", 0.75f, -0.4f, glm::vec3(1.68f, -5.0f, 0));
}