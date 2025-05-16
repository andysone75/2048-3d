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
	for (const std::unique_ptr<Drawable>& drawable : drawables)
		drawable->draw();
}

void UI::mouseCallback(int button, int action, glm::vec2 position) {
	if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_RELEASE)
		return;

	for (int i = static_cast<int>(buttons.size() - 1); i >= 0; i--) {
		const Button& button = buttons[i];
		if (!getImage(button.image).active) continue;
		const Image& image = getImage(button.image);

		glm::vec2 min = image.position;
		min.x -= image.width * image.scale.x * image.alignmentX;
		min.y -= image.height * image.scale.y * image.alignmentY;

		glm::vec2 max = min;
		max.x += image.width * image.scale.x;
		max.y += image.height * image.scale.y;

		glm::vec2 pos = position;
		pos.y = canvasH - pos.y;

		if (min.x <= pos.x && max.x >= pos.x &&
			min.y <= pos.y && max.y >= pos.y)
		{
			button.clickCallback();
			break;
		}
	}
}

TextId UI::createText() {
	return createText(TextDescription());
}

TextId UI::createText(const TextDescription& desc) {
	TextId id = drawables.size();
	drawables.push_back(std::make_unique<Text>(Text(desc, textRenderer)));
	return id;
}

ImageId UI::createImage(const char* filepath) {
	return createImage(ImageDescription(), filepath);
}

ImageId UI::createImage(const ImageDescription& desc, const char* filepath) {
	ImageId id = drawables.size();
	drawables.push_back(std::make_unique<Image>(Image(desc, imageRenderer)));
	Image& image = getImage(id);

	if (filepath != nullptr) {
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
	}
	else {
		image.width = 100;
		image.height = 100;
	}

	return id;
}

ButtonId UI::createButton(ImageId image, std::function<void()> clickCallback) {
	buttons.emplace_back(image, clickCallback);
	return buttons.size() - 1;
}
