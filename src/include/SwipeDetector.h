#pragma once

#include "glm/glm.hpp"

struct Swipe {
	glm::vec2 start;
	glm::vec2 end;
	float time;

	inline float getLength() const { return glm::distance(start, end); }
	inline glm::vec2 getDirection() const { return glm::normalize(end - start); }
};

class SwipeDetector {
public:
	void onMouseDown(glm::vec2 position);
	void onMouseUp(glm::vec2 position);
	bool checkSwipe();

	const Swipe& getSwipe() { return swipe; }

private:
	Swipe swipe;
	bool dragging = false;
	bool swipeReady = false;
};