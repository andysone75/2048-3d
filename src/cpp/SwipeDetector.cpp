#include "SwipeDetector.h"

#include <GLFW/glfw3.h>
#include <iostream>

void SwipeDetector::onMouseDown(glm::vec2 position) {
    dragging = true;
    swipe.start = position;
    swipe.time = static_cast<float>(glfwGetTime());
}

void SwipeDetector::onMouseUp(glm::vec2 position) {
    if (dragging) {
        dragging = false;
        swipe.end = position;
        swipe.time = static_cast<float>(glfwGetTime()) - swipe.time;
        swipeReady = true;
    }
}

bool SwipeDetector::checkSwipe() {
    bool ready = swipeReady;
    swipeReady = false;
    return ready;
}