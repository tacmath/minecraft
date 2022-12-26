#ifndef SUN_H
#define SUN_H

#include <glm/vec3.hpp>
#include "shader.h"
#include "texture.h"
#include "quad.h"

class Sun {
	glm::vec3	position;
	Shader		shader;
	Texture		texture;
	Quad        quad;
	int			ticks;

public:
	
	Sun() {
		ticks = 0;
		position = glm::vec3(0.0f);
		texture.Load("texture/sun.png", 2);
		quad.Init(glm::vec2(-0.5f), glm::vec2(1.0f));
		shader.Load("shaders/sunVS.glsl", "shaders/sunFS.glsl");
		shader.Activate();
		shader.setInt("sunTexture", 2);
		Update();
	};

	void Draw() {
		shader.Activate();
		quad.Render();
	}

	void Update() {
		float angle;

		angle = glm::radians(ticks / 1.0f);
		position.z = -cos(angle);
		position.y = sin(angle);
		shader.Activate();
		shader.setVec3("position", position);
	}

	void SetView(glm::mat4 &view) {
		shader.Activate();
		shader.setMat4("view", view);
	}

	void SetProjection(glm::mat4& projection) {
		shader.Activate();
		shader.setMat4("projection", projection);
	}

	void SetTicks(int ticks) {
		this->ticks = ticks;
		Update();
	}

	void tick() {
		ticks++;
		Update();
	}
};

#endif