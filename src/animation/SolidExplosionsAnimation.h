#include "animation/Animation.h"

class SolidExplosionsAnimation : public Animation {
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    } else {
      if (param.state == AnimationState_Started) {
        tempColor = generateColor(0);
        tempLedIndex = random(pixelCount);
        startFlashPixelAnimation(tempLedIndex, tempColor);
      } else {
        float progress = calcProgress(param);
        led_index_t dLedIndex = calcAnimationIntensity() * progress;
        int leftLedIndex = ((int)tempLedIndex) - dLedIndex;
        int rightLedIndex = ((int)tempLedIndex) + dLedIndex;
        RgbColor color = changeColorBrightness(tempColor, NeoEase::SinusoidalIn(1.0 - 1.0 / calcAnimationIntensity() * dLedIndex));

        if (leftLedIndex >= 0) {
          startFlashPixelAnimation(leftLedIndex, color);
        }
        if (rightLedIndex < pixelCount) {
          startFlashPixelAnimation(rightLedIndex, color);
        }
      }
    }
  }

  void updateFlashPixelAnimation(const AnimationParam &param) {
    float progress = calcProgress(param);
    led_index_t ledIndex = param.index;
    RgbColor updatedColor;
    if (param.state == AnimationState_Completed) {
      updatedColor = ledColorAnimationState[ledIndex].endColor;
      ledColorAnimationState[ledIndex].startColor = updatedColor;
      ledColorAnimationState[ledIndex].endColor = BLACK;
      startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() - abs(ledIndex - tempLedIndex) * calcAnimationTime() / calcAnimationIntensity());
    } else {
      updatedColor = RgbColor::LinearBlend(ledColorAnimationState[ledIndex].startColor, ledColorAnimationState[ledIndex].endColor, progress);
    }
    strip->SetPixelColor(ledIndex, updatedColor);
  }

  void startFlashPixelAnimation(led_index_t ledIndex, RgbColor color) {
    unsigned int duration = calcAnimationTime() / calcAnimationIntensity();

    ledColorAnimationState[ledIndex].startColor = BLACK;
    ledColorAnimationState[ledIndex].endColor = color;
    Animation::animations->StartAnimation(ledIndex, duration, [=](const AnimationParam &param) { return this->updateFlashPixelAnimation(param); });
  }

  ScaleDescriptor getAnimationSpeedScale() { return ScaleDescriptor(SECONDS_TO_ANIMATION_TIME(0.3), SECONDS_TO_ANIMATION_TIME(3)); }
};
