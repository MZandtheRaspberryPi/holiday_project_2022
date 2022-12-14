#ifndef ANIMATE_FACE_H
#define ANIMATE_FACE_H

#include "stdint.h"

void animationTriggerActionMode(void);

void displayFace(uint8_t left_eye_y_offset, uint8_t right_eye_y_offset, uint8_t mouth_y_offset);
void blinkEyes(const uint16_t counter, const uint16_t end_count);
void actuateMouth(const uint16_t counter, const uint16_t end_count);
void squintEyes(const uint16_t counter, const uint16_t end_count);
void animateFace();

#endif // ANIMATE_FACE_H