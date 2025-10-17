#include <Arduino.h>

#define RED_PIN     25
#define GREEN_PIN   26
#define BLUE_PIN    27
#define BUTTON_PIN  4

#define RED_CHANNEL   0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL  2
#define PWM_FREQ      5000
#define PWM_RESOLUTION 8

enum Color {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  CYAN,
  WHITE,
  RED_GREEN_FLASH,
  RED_BLUE_FLASH,
  GREEN_BLUE_FLASH,
  YELLOW_BLUE_FLASH,
  MAGENTA_CYAN_FLASH,
  RED_CYAN_FLASH
};

enum Mode {
  SOLID,
  FLASH,
  PULSE
};

Color currentColor = RED;
Mode currentMode = SOLID;
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200;
bool buttonPressed = false;

void setColorPWM(int r, int g, int b) {
  ledcWrite(RED_CHANNEL, r);
  ledcWrite(GREEN_CHANNEL, g);
  ledcWrite(BLUE_CHANNEL, b);
}

void setColor(int r, int g, int b) {
  setColorPWM(r ? 0 : 255, g ? 0 : 255, b ? 0 : 255);
}

void setColorByEnum(Color color) {
  switch(color) {
    case RED:
      setColor(1, 0, 0);
      break;
    case GREEN:
      setColor(0, 1, 0);
      break;
    case BLUE:
      setColor(0, 0, 1);
      break;
    case YELLOW:
      setColorPWM(0, 255-140, 255); // Orange RGB(255, 140, 0)
      break;
    case MAGENTA:
      setColor(1, 0, 1);
      break;
    case CYAN:
      setColor(0, 1, 1);
      break;
    case WHITE:
      setColor(1, 1, 1);
      break;
    case RED_GREEN_FLASH:
      setColor(1, 1, 0);
      break;
    case RED_BLUE_FLASH:
      setColor(1, 0, 1);
      break;
    case GREEN_BLUE_FLASH:
      setColor(0, 1, 1);
      break;
    case YELLOW_BLUE_FLASH:
      setColor(1, 1, 1);
      break;
    case MAGENTA_CYAN_FLASH:
      setColor(1, 1, 1);
      break;
    case RED_CYAN_FLASH:
      setColor(1, 1, 1);
      break;
    default:
      break;
  }
}

void turnOff() {
  setColorPWM(255, 255, 255);
}

String getColorName(Color color) {
  switch(color) {
    case RED: return "RED";
    case GREEN: return "GREEN";
    case BLUE: return "BLUE";
    case YELLOW: return "YELLOW";
    case MAGENTA: return "MAGENTA";
    case CYAN: return "CYAN";
    case WHITE: return "WHITE";
    case RED_GREEN_FLASH: return "RED/GREEN FLASH";
    case RED_BLUE_FLASH: return "RED/BLUE FLASH";
    case GREEN_BLUE_FLASH: return "GREEN/BLUE FLASH";
    case YELLOW_BLUE_FLASH: return "YELLOW/BLUE FLASH";
    case MAGENTA_CYAN_FLASH: return "MAGENTA/CYAN FLASH";
    case RED_CYAN_FLASH: return "RED/CYAN FLASH";
    default: return "UNKNOWN";
  }
}

String getModeName(Mode mode) {
  switch(mode) {
    case SOLID: return "SOLID";
    case FLASH: return "FLASH";
    case PULSE: return "PULSE";
    default: return "UNKNOWN";
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  ledcSetup(RED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(GREEN_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(BLUE_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  
  ledcAttachPin(RED_PIN, RED_CHANNEL);
  ledcAttachPin(GREEN_PIN, GREEN_CHANNEL);
  ledcAttachPin(BLUE_PIN, BLUE_CHANNEL);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  turnOff();
  
  Serial.println("\n=== RGB LED Test Program ===");
  Serial.println("Press button to cycle: Solid -> Flash -> Pulse -> Next Color");
  Serial.println("13 colors total\n");
  
  Serial.println("Starting with: RED SOLID");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed && (currentTime - lastButtonPress > DEBOUNCE_DELAY)) {
      buttonPressed = true;
      lastButtonPress = currentTime;
      
      if (currentMode == SOLID) {
        currentMode = FLASH;
      } else if (currentMode == FLASH) {
        currentMode = PULSE;
      } else {
        currentMode = SOLID;
        currentColor = (Color)((currentColor + 1) % 13);
      }
      
      if (currentColor >= RED_GREEN_FLASH && currentMode == FLASH) {
        currentMode = PULSE;
      }
      
      Serial.println("\nButton pressed! " + getColorName(currentColor) + " " + getModeName(currentMode));
    }
  } else {
    buttonPressed = false;
  }
  
  if (currentMode == SOLID) {
    if (currentColor >= RED_GREEN_FLASH) {
      unsigned long alternateTime = currentTime % 250;
      if (alternateTime < 125) {
        switch(currentColor) {
          case RED_GREEN_FLASH:
            setColor(1, 0, 0);
            break;
          case RED_BLUE_FLASH:
            setColor(1, 0, 0);
            break;
          case GREEN_BLUE_FLASH:
            setColor(0, 1, 0);
            break;
          case YELLOW_BLUE_FLASH:
            setColor(1, 1, 0);
            break;
          case MAGENTA_CYAN_FLASH:
            setColor(1, 0, 1);
            break;
          case RED_CYAN_FLASH:
            setColor(1, 0, 0);
            break;
        }
      } else {
        switch(currentColor) {
          case RED_GREEN_FLASH:
            setColor(0, 1, 0);
            break;
          case RED_BLUE_FLASH:
            setColor(0, 0, 1);
            break;
          case GREEN_BLUE_FLASH:
            setColor(0, 0, 1);
            break;
          case YELLOW_BLUE_FLASH:
            setColor(0, 0, 1);
            break;
          case MAGENTA_CYAN_FLASH:
            setColor(0, 1, 1);
            break;
          case RED_CYAN_FLASH:
            setColor(0, 1, 1);
            break;
        }
      }
    } else {
      setColorByEnum(currentColor);
    }
  } else if (currentMode == FLASH) {
    if (currentColor >= RED_GREEN_FLASH) {
      unsigned long alternateTime = currentTime % 250;
      if (alternateTime < 125) {
        switch(currentColor) {
          case RED_GREEN_FLASH:
            setColor(1, 0, 0);
            break;
          case RED_BLUE_FLASH:
            setColor(1, 0, 0);
            break;
          case GREEN_BLUE_FLASH:
            setColor(0, 1, 0);
            break;
          case YELLOW_BLUE_FLASH:
            setColor(1, 1, 0);
            break;
          case MAGENTA_CYAN_FLASH:
            setColor(1, 0, 1);
            break;
          case RED_CYAN_FLASH:
            setColor(1, 0, 0);
            break;
        }
      } else {
        switch(currentColor) {
          case RED_GREEN_FLASH:
            setColor(0, 1, 0);
            break;
          case RED_BLUE_FLASH:
            setColor(0, 0, 1);
            break;
          case GREEN_BLUE_FLASH:
            setColor(0, 0, 1);
            break;
          case YELLOW_BLUE_FLASH:
            setColor(0, 0, 1);
            break;
          case MAGENTA_CYAN_FLASH:
            setColor(0, 1, 1);
            break;
          case RED_CYAN_FLASH:
            setColor(0, 1, 1);
            break;
        }
      }
    } else {
      unsigned long flashTime = currentTime % 1000;
      if (flashTime < 500) {
        setColorByEnum(currentColor);
      } else {
        turnOff();
      }
    }
  } else {
    if (currentColor >= RED_GREEN_FLASH) {
      float pulsePhase = (currentTime % 2000) / 2000.0;
      int brightness = (sin(pulsePhase * 2 * PI) + 1) * 127.5;
      int pwmValue = 255 - brightness;
      
      unsigned long alternateTime = currentTime % 500;
      if (alternateTime < 250) {
        switch(currentColor) {
          case RED_GREEN_FLASH:
            setColorPWM(pwmValue, 255, 255);
            break;
          case RED_BLUE_FLASH:
            setColorPWM(pwmValue, 255, 255);
            break;
          case GREEN_BLUE_FLASH:
            setColorPWM(255, pwmValue, 255);
            break;
          case YELLOW_BLUE_FLASH:
            setColorPWM(pwmValue, pwmValue, 255);
            break;
          case MAGENTA_CYAN_FLASH:
            setColorPWM(pwmValue, 255, pwmValue);
            break;
          case RED_CYAN_FLASH:
            setColorPWM(pwmValue, 255, 255);
            break;
        }
      } else {
        switch(currentColor) {
          case RED_GREEN_FLASH:
            setColorPWM(255, pwmValue, 255);
            break;
          case RED_BLUE_FLASH:
            setColorPWM(255, 255, pwmValue);
            break;
          case GREEN_BLUE_FLASH:
            setColorPWM(255, 255, pwmValue);
            break;
          case YELLOW_BLUE_FLASH:
            setColorPWM(255, 255, pwmValue);
            break;
          case MAGENTA_CYAN_FLASH:
            setColorPWM(255, pwmValue, pwmValue);
            break;
          case RED_CYAN_FLASH:
            setColorPWM(255, pwmValue, pwmValue);
            break;
        }
      }
    } else {
      float pulsePhase = (currentTime % 2000) / 2000.0;
      int brightness = (sin(pulsePhase * 2 * PI) + 1) * 127.5;
      int pwmValue = 255 - brightness;
      
      switch(currentColor) {
        case RED:
          setColorPWM(pwmValue, 255, 255);
          break;
        case GREEN:
          setColorPWM(255, pwmValue, 255);
          break;
        case BLUE:
          setColorPWM(255, 255, pwmValue);
          break;
        case YELLOW:
          setColorPWM(pwmValue, pwmValue, 255);
          break;
        case MAGENTA:
          setColorPWM(pwmValue, 255, pwmValue);
          break;
        case CYAN:
          setColorPWM(255, pwmValue, pwmValue);
          break;
        case WHITE:
          setColorPWM(pwmValue, pwmValue, pwmValue);
          break;
      }
    }
  }
}
