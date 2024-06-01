#include "DisplayAnimator.h"

DisplayAnimator::DisplayAnimator()
    : display(nullptr), startNumber(0), endNumber(0), startTime(0), durationPer10Units(0), animating(false),
      currentAnimation(AnimationType::NONE), currentSegment(0), segmentDuration(0), lastSegmentUpdateTime(0) {}

void DisplayAnimator::setup(TwoDigitDisplay* displayInstance) {
    display = displayInstance;
}

void DisplayAnimator::animateTransition(int fromNumber, int toNumber, unsigned long durationPer10Units) {
    startNumber = fromNumber;
    endNumber = toNumber;
    startTime = millis();
    this->durationPer10Units = durationPer10Units;

    currentAnimation = AnimationType::TRANSITION;
    animating = true;
    #ifdef _DEBUG
    Serial.println("Transition animation started");
    #endif
}

void DisplayAnimator::animateClockwiseLoading(unsigned long segmentDurationMs) {
    startTime = millis();
    segmentDuration = segmentDurationMs;
    currentSegment = 0;
    lastSegmentUpdateTime = millis(); // Initialize segment update timing
    currentAnimation = AnimationType::CLOCKWISE_LOADING;
    animating = true;
    #ifdef _DEBUG
    Serial.println("Clockwise initialization animation started");
    #endif
}

void DisplayAnimator::animateCounterClockwiseLoading(unsigned long segmentDurationMs) {
    startTime = millis();
    segmentDuration = segmentDurationMs;
    currentSegment = 7; // Start from the last segment in the sequence
    lastSegmentUpdateTime = millis(); // Initialize segment update timing
    currentAnimation = AnimationType::COUNTERCLOCKWISE_LOADING;
    animating = true;
    #ifdef _DEBUG
    Serial.println("Counterclockwise initialization animation started");
    #endif
}

void DisplayAnimator::stop() {
    animating = false;
    currentAnimation = AnimationType::NONE;
    #ifdef _DEBUG
    Serial.println("Animation stopped");
    #endif
}

void DisplayAnimator::update() {
    if (!animating || display == nullptr) return;

    unsigned long currentTime = millis();
    #ifdef _DEBUG
    Serial.print("Elapsed Time: ");
    Serial.println(getElapsedTime());
    #endif

    switch (currentAnimation) {
        case AnimationType::TRANSITION: {
            updateTransitionAnimation(currentTime);
            break;
        }
        case AnimationType::CLOCKWISE_LOADING: {
            updateClockwiseLoadingAnimation(currentTime);
            break;
        }
        case AnimationType::COUNTERCLOCKWISE_LOADING: {
            updateCounterClockwiseLoadingAnimation(currentTime);
            break;
        }
        case AnimationType::NONE:
        default:
            break;
    }
}

void DisplayAnimator::updateTransitionAnimation(const unsigned long currentTime) {
    int distance = abs(endNumber - startNumber);
    unsigned long totalDuration = (distance * TRANSITION_DURATION_PER_10_UNITS) / 10;
    unsigned long elapsedTime = currentTime - startTime;

    if (elapsedTime >= totalDuration) {
        display->displayNumbers(endNumber / 10, endNumber % 10);
        animating = false;
        currentAnimation = AnimationType::NONE;
        #ifdef _DEBUG
        Serial.println("Transition animation completed");
        #endif
        return;
    }

    float progress = static_cast<float>(elapsedTime) / static_cast<float>(totalDuration);
    int currentNumber = startNumber + static_cast<int>(progress * (endNumber - startNumber));

    display->displayNumbers(currentNumber / 10, currentNumber % 10);
}

void DisplayAnimator::updateClockwiseLoadingAnimation(const unsigned long currentTime) {
    if (currentTime - lastSegmentUpdateTime >= segmentDuration) {
        lastSegmentUpdateTime = currentTime;
        currentSegment = (currentSegment + 1) % 8;
    }

    display->displaySegments(LOADING_ANIMATION_SEGMENT_SEQUENCE[currentSegment][0], LOADING_ANIMATION_SEGMENT_SEQUENCE[currentSegment][1]);
}

void DisplayAnimator::updateCounterClockwiseLoadingAnimation(const unsigned long currentTime) {
    if (currentTime - lastSegmentUpdateTime >= segmentDuration) {
        lastSegmentUpdateTime = currentTime;
        currentSegment = (currentSegment - 1 + 8) % 8;
    }

    display->displaySegments(LOADING_ANIMATION_SEGMENT_SEQUENCE[currentSegment][0], LOADING_ANIMATION_SEGMENT_SEQUENCE[currentSegment][1]);
}

unsigned long DisplayAnimator::getElapsedTime() const {
    return animating ? millis() - startTime : 0;
}

AnimationType DisplayAnimator::getCurrentAnimation() const {
    return currentAnimation;
}
