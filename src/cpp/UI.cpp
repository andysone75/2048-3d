#include "UI.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void UI::initialize(int canvasW, int canvasH) {
	this->canvasW = canvasW;
	this->canvasH = canvasH;
	textRenderer.initialize(canvasW, canvasH);
	imageRenderer.initialize(canvasW, canvasH);
}

void UI::reinitialize(int canvasW, int canvasH) {
	this->canvasW = canvasW;
	this->canvasH = canvasH;
	textRenderer.reinitialize(canvasW, canvasH);
	imageRenderer.reinitialize(canvasW, canvasH);
}

void UI::render() {
	for (const Text& text : texts) {
		if (!text.active) continue;
		textRenderer.draw(
			text.value,
			text.position.x,
			text.position.y,
			text.scale,
			text.color,
			text.alignmentX);
	}

	for (const Image& image : images) {
		if (!image.active) continue;
		imageRenderer.draw(
			image.texture,
			image.width,
			image.height,
			image.position.x,
			image.position.y,
			image.scale,
			image.color,
			image.alignmentX,
			image.alignmentY);
	}
}

void UI::mouseCallback(int button, int action, glm::vec2 position) {
	if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_RELEASE)
		return;

	for (const Button& button : buttons) {
		if (!getImage(button.image).active) continue;

		const Image& image = getImage(button.image);
		
		glm::vec2 min = image.position;
		min.x -= image.width * image.scale * image.alignmentX;
		min.y -= image.height * image.scale * image.alignmentY;
		
		glm::vec2 max = min;
		max.x += image.width * image.scale;
		max.y += image.height * image.scale;

		//glm::vec2 pos = position * image.scale; // with this command button clicks don't work when alignmentX == 1, but without it buttons "triggers" have sizes as if no scaling applied
		glm::vec2 pos = position;
		pos.y = canvasH - pos.y;

		if (min.x <= pos.x && max.x >= pos.x &&
			min.y <= pos.y && max.y >= pos.y)
		{
			button.clickCallback();
		}
	}
}

TextId UI::createText(const TextDescription& desc) {
	texts.emplace_back(desc);
	return texts.size() - 1;
}

ImageId UI::createImage(const ImageDescription& desc, const char* filepath) {
	images.emplace_back(desc);
	int index = images.size() - 1;

	Image& image = images[index];
	
	glGenTextures(1, &image.texture);
	glBindTexture(GL_TEXTURE_2D, image.texture);

	int nrChannels;
	unsigned char* data = stbi_load(filepath, &image.width, &image.height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << filepath << std::endl;
		return -1;
	}

	GLenum format = GL_RGB;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return index;
}

ButtonId UI::createButton(ImageId image, std::function<void()> clickCallback) {
	buttons.emplace_back(image, clickCallback);
	return buttons.size() - 1;
}
