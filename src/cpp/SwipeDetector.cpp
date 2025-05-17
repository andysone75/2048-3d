#include "SwipeDetector.h"

#include <GLFW/glfw3.h>
#include <iostream>

void SwipeDetector::onMouseDown(glm::vec2 position) {
    dragging = true;
    swipe.start = position;
    swipe.time = static_cast<float>(glfwGetTime());
}

void SwipeDetector::onMouseUp(glm::vec2 position) {
    dragging = false;
}

void SwipeDetector::update(glm::vec2 position)
{
    if (dragging) {
        swipe.end = position;
        swipe.time = static_cast<float>(glfwGetTime()) - swipe.time;
        swipeReady = true;
    }
}

bool SwipeDetector::checkSwipeMove()
{
    return dragging;
}

bool SwipeDetector::checkSwipeRelease() {
    bool ready = swipeReady;
    swipeReady = false;
    return ready;
}

void SwipeDetector::cancelSwipe()
{
    dragging = false;
}
