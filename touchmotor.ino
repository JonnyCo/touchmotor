#include <Wire.h>
#include "unit_rolleri2c.hpp"

UnitRollerI2C Roller;

// === SET THIS ===
// Start with 2000000 as a guess since your example used 2,000,000 counts.
// After you calibrate, update this to the true counts per revolution.
static const float COUNTS_PER_REV = 3600.0f;

// Optional: define a "zero" offset in counts (so angle=0 maps to your chosen home)
static int32_t zero_offset_counts = 0;

static int32_t degToCounts(float deg) {
  // Wrap degrees to [-180, 180) if you want; or remove wrapping if you want multi-turn.
  // deg = fmodf(deg + 180.0f, 360.0f) - 180.0f;

  float rev = deg / 360.0f;
  return zero_offset_counts + (int32_t)(rev * COUNTS_PER_REV);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  // XIAO ESP32S3 I2C on Grove shield: SDA=5, SCL=6
  Wire.begin(5, 6, 400000);

  Roller.begin(&Wire, 0x64, 5, 6, 400000);

  // Configure position mode once
  Roller.setOutput(0);
  Roller.setMode(ROLLER_MODE_POSITION);
  Roller.setPosMaxCurrent(100000);  // tune if needed
  Roller.setOutput(1);

  Serial.println("Send an angle in DEGREES (e.g., 90 or -45). Press Enter.");
  Serial.println("Optional commands: 'z' sets current position as 0 deg.");
}

void loop() {
  // Simple command parser:
  // - A number like "90" -> go to 90 degrees
  // - "z" -> set current position as zero reference (angle=0)
  if (Serial.available()) {
    // Peek first char to see if it's a command
    char c = Serial.peek();

    if (c == 'z' || c == 'Z') {
      Serial.read(); // consume 'z'
      // Set current readback as zero reference
      int32_t current_counts = (int32_t)(Roller.getPosReadback() / 100.0f);
      zero_offset_counts = current_counts;
      Serial.print("Zero set. zero_offset_counts=");
      Serial.println(zero_offset_counts);
      // flush rest of line
      while (Serial.available()) Serial.read();
      return;
    }

    // Otherwise parse a float angle
    float angle_deg = Serial.parseFloat();

    // consume the rest of the line (newline etc.)
    while (Serial.available()) Serial.read();

    int32_t target_counts = degToCounts(angle_deg);
    Roller.setPos(target_counts);

    // Print feedback
    int32_t actual_counts = (int32_t)(Roller.getPosReadback() / 100.0f);
    Serial.print("cmd_deg=");
    Serial.print(angle_deg, 2);
    Serial.print(" target_counts=");
    Serial.print(target_counts);
    Serial.print(" actual_counts=");
    Serial.println(actual_counts);
  }
}

