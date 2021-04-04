#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 9

unsigned int menu_data[] =
{
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
    10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
     2,  2,  2,  2, 17,  2,  2,  6,  7,  2,  2,
    12,  9, 12, 12, 12, 12, 12, 12, 12,  8, 12
};


void Menu::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 10, 8);

    // Initialize Game Objects
    state.player = new Entity();

    state.background = new Entity();
    state.background->textureID = Util::LoadTexture("background.png");
    state.background->entityType = BACKGROUND;
    state.background->position = glm::vec3(MENU_WIDTH / 2, -5.5, 0);
}


void Menu::Update(float deltaTime) {
    state.background->Update(deltaTime, state.player, state.enemies, 0, state.map);
}


void Menu::Render(ShaderProgram* program) {
    state.background->Render(program);
    state.map->Render(program);
    Util::DrawText(program, Util::LoadTexture("font.png"), "Home Delivery", 1.0f, -0.5f, glm::vec3(2.0f, -2.5f, 0));
    Util::DrawText(program, Util::LoadTexture("font.png"), "Press enter to start", 0.75f, -0.4f, glm::vec3(1.75f, -5.0f, 0));
}