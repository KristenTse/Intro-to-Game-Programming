#include "Scene.h"


void Scene::SpawnArrow(Entity* obj) {
    state.arrows.push_back(new Entity());
    GLuint enemyTextureID = Util::LoadTexture("arrow_sheet.png");
    int idx = state.arrows.size() - 1;

    state.arrows[idx]->textureID = enemyTextureID;
    state.arrows[idx]->entityType = WEAPON;
    state.arrows[idx]->position = obj->position;
    state.arrows[idx]->speed = 5;

    state.arrows[idx]->animUp = new int[4]{ 1 };
    state.arrows[idx]->animDown = new int[4]{ 0 };
    state.arrows[idx]->animRight = new int[4]{ 3 };
    state.arrows[idx]->animLeft = new int[4]{ 2 };
    state.arrows[idx]->animFrames = 1;
    state.arrows[idx]->animCols = 4;
    state.arrows[idx]->animRows = 1;

    if (obj->animIndices == obj->animUp) {
        state.arrows[idx]->animIndices = state.arrows[idx]->animUp;
        state.arrows[idx]->movement.y = 1;
    }
    else if (obj->animIndices == obj->animDown) {
        state.arrows[idx]->animIndices = state.arrows[idx]->animDown;
        state.arrows[idx]->movement.y = -1;
    }
    else if (obj->animIndices == obj->animLeft) {
        state.arrows[idx]->animIndices = state.arrows[idx]->animLeft;
        state.arrows[idx]->movement.x = -1;
    }
    else if (obj->animIndices == obj->animRight) {
        state.arrows[idx]->animIndices = state.arrows[idx]->animRight;
        state.arrows[idx]->movement.x = 1;
    }

    if (obj->animIndices == obj->animUp || obj->animIndices == obj->animDown) {  //vertical arrow
        state.arrows[idx]->height = 0.8f;
        state.arrows[idx]->width = 0.5f;
    }
    else { //horizontal arrow
        state.arrows[idx]->height = 0.5f;
        state.arrows[idx]->width = 0.8f;
    }
}