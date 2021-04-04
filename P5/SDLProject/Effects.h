#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EffectType { NONE, FADEIN, FADEOUT, GROW, SHRINK, SHAKE };


class Effects {
	ShaderProgram program;
	EffectType currentEffect;
	float alpha, speed, size, timeLeft;

public:
	glm::vec3 viewOffset;
	Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
	void DrawOverlay();
	void Start(EffectType effectType, float effectspeed);
	void Update(float deltaTime);
	void Render();
};