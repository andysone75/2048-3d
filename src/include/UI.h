#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "TextRenderer.h"
#include "ImageRenderer.h"

// Drawable
using DrawableId = int;

struct DrawableDescription {
	glm::vec2 position = { 0.0f, 0.0f };
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec2 scale = { 1.0f, 1.0f };
	float alignmentX = 0.5f;
	float alignmentY = 0.5f;
};

struct Drawable {
	bool active = true;
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 scale;
	float alignmentX;
	float alignmentY;

	Drawable(const DrawableDescription& desc) :
		position(desc.position),
		color(desc.color),
		scale(desc.scale),
		alignmentX(desc.alignmentX),
		alignmentY(desc.alignmentY) {};

	virtual ~Drawable() = default;
	virtual void draw() const = 0;
};

// Text
using TextId = DrawableId;

struct TextDescription : public DrawableDescription {
	std::string value = "";
};

struct Text : public Drawable {
	std::string value;
	const TextRenderer& renderer;

	Text(const TextDescription& desc, const TextRenderer& renderer) :
		Drawable(desc),
		value(desc.value),
		renderer(renderer)
	{};

	void draw() const override {
		if (!active) return;
		renderer.draw(
			value,
			position.x,
			position.y,
			scale,
			color,
			alignmentX);
	}
};

// Image
using ImageId = DrawableId;
struct ImageDescription : public DrawableDescription {};

struct Image : public Drawable {
	GLuint texture = 0;
	int width;
	int height;
	const ImageRenderer& renderer;

	Image(const ImageDescription& desc, const ImageRenderer& renderer) :
		Drawable(desc),
		renderer(renderer)
	{};

	void draw() const override {
		if (!active) return;
		renderer.draw(
			texture,
			width,
			height,
			position.x,
			position.y,
			scale,
			color,
			alignmentX,
			alignmentY);
	}
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

	TextId createText();
	TextId createText(const TextDescription& desc);
	ImageId createImage(const char* filepath = nullptr);
	ImageId createImage(const ImageDescription& desc, const char* filepath = nullptr);
	ButtonId createButton(ImageId image, std::function<void()> clickCallback);

	inline Text& getText(TextId id) { return dynamic_cast<Text&>(*drawables.at(id)); }
	inline Image& getImage(ImageId id) { return dynamic_cast<Image&>(*drawables.at(id)); }
	inline Button& getButton(ButtonId id) { return buttons[id]; }

private:
	int canvasW, canvasH;
	TextRenderer textRenderer;
	ImageRenderer imageRenderer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	std::vector<Button> buttons;
};	