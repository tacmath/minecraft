#ifndef SUN_H
#define SUN_H

#include <glm/vec3.hpp>
#include "shader.h"
#include "texture.h"
#include "quad.h"

class Sun {
	glm::mat4	sunModel; //maybe have a shader for each
	glm::mat4	moonModel;
	glm::vec3	position;
	Shader		shader;
	Texture		texture;
	Quad        quad;
	int			ticks;

public:
	
	Sun() {
		ticks = 0;
		position = glm::vec3(0.0f);
		sunModel = glm::mat4(1.0f);
		moonModel = glm::mat4(1.0f);
		texture.LoadArray({"texture/sun.png", "texture/moon.png"}, 2);
		quad.Init(glm::vec2(-0.5f), glm::vec2(1.0f));
		shader.Load("shaders/sunVS.glsl", "shaders/sunFS.glsl");
		shader.Activate();
		shader.setInt("sunTexture", 2);
		Update();
	};

	~Sun() {
		shader.Delete();
	}

	void Draw() {
		shader.Activate();
		shader.setInt("phase", 0);
		shader.setMat4("model", sunModel);
		quad.Render();

		shader.setInt("phase", 1);
		shader.setMat4("model", moonModel);
		quad.Render();
	}

	void Update() {
		float angle;


		angle = glm::radians(ticks / 10.0f);
		position.z = -cos(angle);
		position.y = sin(angle);
		sunModel = glm::rotate(glm::translate(glm::mat4(1.0f), position), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		moonModel = glm::rotate(glm::translate(glm::mat4(1.0f), -position), angle, glm::vec3(1.0f, 0.0f, 0.0f));
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