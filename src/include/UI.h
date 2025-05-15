#pragma once

#include <vector>
#include <string>
#include <functional>

#include "TextRenderer.h"
#include "ImageRenderer.h"

// Drawable
struct DrawableDescription {
	glm::vec2 position = { 0.0f, 0.0f };
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };
	float scale = 1.0f;
	float alignmentX = 0.0f;
	float alignmentY = 0.0f;
};

struct Drawable {
	bool active = true;
	glm::vec2 position;
	glm::vec3 color;
	float scale;
	float alignmentX;
	float alignmentY;

	Drawable(const DrawableDescription& desc) :
		position(desc.position),
		color(desc.color),
		scale(desc.scale),
		alignmentX(desc.alignmentX),
		alignmentY(desc.alignmentY) {};
};

// Text
using TextId = int;

struct TextDescription : public DrawableDescription {
	std::string value = "";
};

struct Text : public Drawable {
	std::string value;

	Text(const TextDescription& desc) :
		Drawable(desc),
		value(desc.value) {};
};

// Image
using ImageId = int;
struct ImageDescription : public DrawableDescription {};

struct Image : public Drawable {
	GLuint texture;
	int width;
	int height;

	Image(const ImageDescription& desc) :
		Drawable(desc)
	{};
};

// Button
using ButtonId = int;

struct Button {
	ImageId image;
	std::function<void()> clickCallback;

	Button(ImageId _image, std::function<void()> _clickCallback) : image(_image), clickCallback(_clickCallback) {}
};

class UI {
public:
	void initialize(int canvasW, int canvasH);
	void reinitialize(int canvasW, int canvasH);
	void render();
	void mouseCallback(int button, int action, glm::vec2 position);

	TextId createText(const TextDescription& desc);
	ImageId createImage(const ImageDescription& desc, const char* filepath);
	ButtonId createButton(ImageId image, std::function<void()> clickCallback);

	inline Text& getText(TextId id) { return texts[id]; }
	inline Image& getImage(ImageId id) { return images[id]; }
	inline Button& getButton(ButtonId id) { return buttons[id]; }

private:
	int canvasW, canvasH;
	TextRenderer textRenderer;
	ImageRenderer imageRenderer;
	std::vector<Text> texts;
	std::vector<Image> images;
	std::vector<Button> buttons;
};	