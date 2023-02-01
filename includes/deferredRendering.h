#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include "quad.h"
#include "shader.h"

class Deferred {
private:
	GLuint	gBuffer, gPosition, gNormal, gAlbedoSpec;
	Quad	quad;
	Shader	shader;
public:

	Deferred() {
		quad.Init();
		shader.Load("shaders/quadVS.glsl", "shaders/deferredFS.glsl");
		shader.Activate();
		shader.setInt("gPosition", 4);
		shader.setInt("gNormal", 5);
		shader.setInt("gAlbedoSpec", 6);
	};

	~Deferred() {
		quad.Delete();
		shader.Delete();
		Delete();
	}

	void Init(const glm::vec2& windowSize) {
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// - position color buffer
		glActiveTexture(GL_TEXTURE4);  //test a texture array later
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// - normal color buffer
		glActiveTexture(GL_TEXTURE5);
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// - color + specular color buffer
		glActiveTexture(GL_TEXTURE6);
		glGenTextures(1, &gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

		// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	template<typename T>
	void Draw(T drawFun) {
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawFun();
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shader.Activate();
		quad.Render();
	}

	void Delete() {
		glDeleteFramebuffers(1,&gBuffer);
		glDeleteBuffers(1, &gPosition);
		glDeleteBuffers(1, &gNormal);
		glDeleteBuffers(1, &gAlbedoSpec);
	}

};