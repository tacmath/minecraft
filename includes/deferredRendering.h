#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include "quad.h"
#include "shader.h"

class Deferred {
private:
	GLuint	gBuffer, gPosition, gNormal, gAlbedoAo, rboDepth;
	glm::ivec2 bufferSize;
	Quad	quad;
	Shader	shader;
public:

	Deferred() {
		quad.Init();

		std::vector<std::string> shaderOption = { "SHADOW" };

		shader.Load(shaderOption, "shaders/quadVS.glsl", "shaders/deferredFS.glsl");
		shader.Activate();
		shader.setInt("shadowMap", 3);
		shader.setInt("gPosition", 4);
		shader.setInt("gNormal", 5);
		shader.setInt("gAlbedoAo", 6);
	};

	~Deferred() {
		quad.Delete();
		shader.Delete();
		Delete();
	}

	void Init(const glm::ivec2& bufferSize) {
		this->bufferSize = bufferSize;
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// - position color buffer
		glActiveTexture(GL_TEXTURE4);  //test a texture array later

		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, bufferSize.x, bufferSize.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bufferSize.x, bufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// - normal color buffer
		glActiveTexture(GL_TEXTURE5);
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, bufferSize.x, bufferSize.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// - color + specular color buffer
		glActiveTexture(GL_TEXTURE6);
		glGenTextures(1, &gAlbedoAo);
		glBindTexture(GL_TEXTURE_2D, gAlbedoAo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoAo, 0);

		// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	template<typename T>
	void Draw(T drawFun) { // test by passing worldArea
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawFun();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		shader.Activate();
		quad.Render();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, bufferSize.x, bufferSize.y, 0, 0, bufferSize.x, bufferSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Reload(const glm::ivec2& bufferSize) {
		Delete();
		Init(bufferSize);
	}

	void Delete() {
		glDeleteFramebuffers(1,&gBuffer);
		glDeleteBuffers(1, &rboDepth);
		glDeleteBuffers(1, &gPosition);
		glDeleteBuffers(1, &gNormal);
		glDeleteBuffers(1, &gAlbedoAo);
	}

	Shader GetShader() {
		return shader;
	}

	void InitUniforms(Camera& camera) {
		shader.Activate();
		shader.setInt("shadowMap", 3);
		shader.setInt("gPosition", 4);
		shader.setInt("gNormal", 5);
		shader.setInt("gAlbedoAo", 6);
		shader.setMat4("view", camera.view);
	}

	void LoadViewMatrix(Camera& camera) {
		shader.Activate();
		shader.setMat4("view", camera.view);
	}

	void ReloadShader(std::vector<std::string> shaderOption, Camera& camera) {
		shader.Delete();
		shader.Load(shaderOption, "shaders/quadVS.glsl", "shaders/deferredFS.glsl");
		InitUniforms(camera);
	}
};