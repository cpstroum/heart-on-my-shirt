#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN 10     // Button connected to this pin
#define NEOPIXEL_PIN 6    // Pin connected to the NeoPixel
#define NUMPIXELS 4       // Number of pixels in the NeoPixel strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int currentEffect = 0;        // Index of the current lighting effect
const int numEffects = 3;     // Total number of lighting effects
bool lastButtonState = HIGH;  // Last button state
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds
unsigned long lastUpdateTime = 0;       // Timer for non-blocking effects

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up for button
  Serial.begin(9600); // Initialize Serial Monitor
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Check for button press with debounce
  if (isButtonPressed()) {
    currentEffect = (currentEffect + 1) % numEffects; // Cycle to the next effect
    Serial.print("Button pressed! New effect: ");
    Serial.println(currentEffect);
  }

  // Run the selected effect
  if (currentEffect == 0) {
    nonBlockingTravelingPulse(strip, NUMPIXELS, 100, 255, 0, 0); // Red pulse
  } else if (currentEffect == 1) {
    nonBlockingRainbow(strip, NUMPIXELS, 10); // Rainbow effect
  } else if (currentEffect == 2) {
    nonBlockingSparkle(strip, 255, 255, 255, 50); // Sparkle effect
  }
}

// Function to detect button press with debounce
bool isButtonPressed() {
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay && buttonState == LOW) {
    lastButtonState = buttonState;
    return true;
  }

  lastButtonState = buttonState;
  return false;
}

// Non-blocking traveling pulse effect
void nonBlockingTravelingPulse(Adafruit_NeoPixel& strip, uint16_t numPixels, uint8_t wait, uint8_t red, uint8_t green, uint8_t blue) {
  static int currentPixel = 0;
  static unsigned long lastUpdateTime = 0;

  if (millis() - lastUpdateTime > wait) {
    // Turn off the previous pixel
    strip.setPixelColor((currentPixel + numPixels - 1) % numPixels, strip.Color(0, 0, 0));
    // Turn on the current pixel
    strip.setPixelColor(currentPixel, strip.Color(red, green, blue));
    strip.show();

    // Move to the next pixel
    currentPixel = (currentPixel + 1) % numPixels;
    lastUpdateTime = millis();
  }
}

// Non-blocking rainbow effect
void nonBlockingRainbow(Adafruit_NeoPixel& strip, uint16_t numPixels, uint8_t wait) {
  static long firstPixelHue = 0;
  static unsigned long lastUpdateTime = 0;

  if (millis() - lastUpdateTime > wait) {
    for (uint16_t i = 0; i < numPixels; i++) {
      int pixelHue = firstPixelHue + (i * 65536L / numPixels);
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();

    // Update the starting point for the rainbow
    firstPixelHue += 256;
    lastUpdateTime = millis();
  }
}

// Non-blocking sparkle effect
void nonBlockingSparkle(Adafruit_NeoPixel& strip, uint8_t red, uint8_t green, uint8_t blue, uint8_t wait) {
  static unsigned long lastUpdateTime = 0;

  if (millis() - lastUpdateTime > wait) {
    int pixel = random(strip.numPixels()); // Pick a random pixel
    strip.setPixelColor(pixel, strip.Color(red, green, blue)); // Light it up
    strip.show();
    delay(50);
    strip.setPixelColor(pixel, strip.Color(0, 0, 0)); // Turn it off
    strip.show();

    lastUpdateTime = millis();
  }
}