#include "Entity.h"


Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);

    width = 1;
    height = 1;
    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}


bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) { return false; }

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) { return true; }
    return false;
}


Entity* Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                return object;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                return object;
            }
        }
    }
    return nullptr;
}


Entity* Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                return object;
            }
            else if (velocity.x <= 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                return object;
            }
        }
    }
    return nullptr;
}


void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}


void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}


void Entity::AIWalker()
{
    switch (aiState) {
    case IDLE:
        aiState = MOVING;
        break;

    case MOVING:
        if (aiAxis == HORIZONTAL){
            if (movement.x == 0) {
                movement.x = -1;
            }
            else if (collidedLeft || collidedRight) {
                movement.x = -movement.x;
            }
        }
        else if (aiAxis == VERTICAL) {
            if (movement.y == 0) {
                movement.y = -1;
            }
            else if (collidedTop || collidedBottom) {
                movement.y = -movement.y;
            }
        }
        break;
    }
}


void Entity::AIWaitAndGo(Entity* player)
{
    switch (aiState) {
    case IDLE:
        if (glm::distance(position, player->position) < 3.0f) {
            aiState = MOVING;
        }
        movement = glm::vec3(0);
        break;

    case MOVING:
        if (glm::distance(position, player->position) >= 4.0f) {
            aiState = IDLE;
        }

        if (player->position.x < position.x) {
            movement.x = -1;
        }
        else {
            movement.x = 1;
        }

        if (player->position.y < position.y) {
            movement.y = -1;
        }
        else {
            movement.y = 1;
        }

        if (glm::length(movement) > 1.0f) {
            movement = glm::normalize(movement);
        }
        break;
    }
}


void Entity::AI(Entity* player)
{
    switch (aiType) {
    case WALKER:
        AIWalker();
        break;

    case WAITANDGO:
        AIWaitAndGo(player);
        break;
    }
}


void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map)
{
    if (isActive == false) { return; }

    if (animIndices != NULL) {
        if (glm::length(movement) != 0 || entityType == OBJECT) {
            animTime += deltaTime;

            if (animTime >= animSpeed)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;

    velocity.y = movement.y * speed;
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    if (entityType == ENEMY) {
        AI(player);

        if (movement.x > 0) {
            animIndices = animRight;
        }
        else if (movement.x < 0) {
            animIndices = animLeft;
        }
        else if (movement.y > 0) {
            animIndices = animUp;
        }
        else {
            animIndices = animDown;
        }
    }

    if (entityType == PLAYER) {
        Entity* objectHitY = CheckCollisionsY(objects, objectCount); // Fix if needed and get enemy if hit
        Entity* objectHitX = CheckCollisionsX(objects, objectCount); // Fix if needed and get enemy if hit

        if (objectHitY != nullptr || objectHitX != nullptr) {
            hit = true;
        }
    }

    if (entityType == WEAPON) {
        Entity* objectHitY = CheckCollisionsY(objects, objectCount); // Fix if needed and get enemy if hit
        Entity* objectHitX = CheckCollisionsX(objects, objectCount); // Fix if needed and get enemy if hit

        if (objectHitY != nullptr) {
            objectHitY->isActive = false;
        }
        if (objectHitX != nullptr) {
            objectHitX->isActive = false;
        }
        
        if (collidedBottom || collidedTop || collidedRight || collidedLeft) {
            isActive = false;
        }
    }

    if (entityType == OBJECT) {
        if (animIndices == animUp && animIndex == animFrames-1) {
            animTime = 0;
        }
    }
}


void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Entity::Render(ShaderProgram* program) {
    if (isActive == false) { return; }

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
