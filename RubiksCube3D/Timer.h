#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <glm/vec3.hpp>

class Timer {
public:
    Timer();
    void Start();
    void Stop();
    void Reset();
    void Update();
    void KeyCallback(int key, int action);
    glm::vec3 GetTextColor() const;

    float GetElapsedTime() const { return elapsedTime; }
    bool IsRunning() const { return stopwatchRunning; }

private:
    std::chrono::steady_clock::time_point lastUpdateTime;
    float elapsedTime;  // Time elapsed in seconds
    bool stopwatchRunning;
    bool spacePressed;
    bool holdingForReset;
    float spaceHoldDuration;
    float resetThreshold;
    float savedTime;
};

#endif // TIMER_H
