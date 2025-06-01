#include "Timer.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Timer::Timer()
    : stopwatchRunning(false), spacePressed(false), holdingForReset(false),
    spaceHoldDuration(0.0f), resetThreshold(0.3f), savedTime(0.0f), elapsedTime(0.0f) {}

void Timer::Start() {
    stopwatchRunning = true;
    lastUpdateTime = std::chrono::steady_clock::now();
}

void Timer::Stop() {
    stopwatchRunning = false;
}

void Timer::Reset() {
    elapsedTime = 0.0f;
    stopwatchRunning = true;
    lastUpdateTime = std::chrono::steady_clock::now();
}

void Timer::Update() {
    if (stopwatchRunning) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = currentTime - lastUpdateTime;
        elapsedTime += delta.count();
        lastUpdateTime = currentTime;
    }

    // Handle spacebar hold logic for reset
    if (spacePressed) {
        spaceHoldDuration += 1.0f / 60.0f;  // Increment time for each frame (~60 FPS)
        if (spaceHoldDuration >= resetThreshold && holdingForReset) {
            elapsedTime = 0.0f;  // Reset time if held for long enough
        }
    }
}

void Timer::KeyCallback(int key, int action) {
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS) {
            spacePressed = true;
            spaceHoldDuration = 0.0f;
            holdingForReset = true;
            savedTime = elapsedTime;  // Save the current time
            elapsedTime = stopwatchRunning ? elapsedTime : 0.0f;  // Set to 0 if stopwatch was stopped
        }
        else if (action == GLFW_RELEASE) {
            if (spaceHoldDuration < resetThreshold) {
                // If hold is short, toggle stopwatch start/stop
                if (stopwatchRunning) {
                    stopwatchRunning = false; // Stop
                }
            }
            else {
                // If hold is long enough, reset the timer to 0
                elapsedTime = 0.0f;
                stopwatchRunning = true;  // Start immediately after reset
                lastUpdateTime = std::chrono::steady_clock::now();
            }
            spacePressed = false;
            holdingForReset = false;
        }
    }
}

// Function to return color based on spacebar hold duration and timer state
glm::vec3 Timer::GetTextColor() const {
    if (spacePressed) {
        if (spaceHoldDuration < resetThreshold) {
            return glm::vec3(1.0f, 0.0f, 0.0f); // Red
        }
        else {
            return glm::vec3(0.0f, 1.0f, 0.0f); // Green
        }
    }
    return glm::vec3(1.0f, 1.0f, 1.0f); // White (default color)
}
