// Purdue IEEE ROV Team
// Motor-Testing ESC Black Box
// [v0.1] [2021.01.22] [Tyler Stagge]
//    -This just uses the data from the potentiometer to drive a servo (in preparation for driving the lumineer ESC)
// [v0.2] [2021.01.23] [Tyler Stagge]
//    -Using ESC class instead of Servo
//    -Replaced global constants with #define

#include <ESC.h>

#define PIN_POT (A0)
#define PIN_LED (7)  // ESC LED override (uses I2C)
#define PIN_TLM (1)  // ESC telemetry
#define PIN_PWM (9)

#define ESC_MIN (1000)
#define ESC_MAX (2000)
#define ESC_ARM (200)

ESC esc (PIN_PWM, ESC_MIN, ESC_MAX, ESC_ARM);

int potVal = 0;  //10-bit ADC => value ranges from 0 to 1023
double escSetpoint = 0; //[us]

void setup(void) {
    Serial.begin(9600);
    esc.arm();
    delay(1000);
}

void loop(void) {
    potVal = analogRead(PIN_POT);
    //Serial.println(potVal);

    escSetpoint = map(potVal, 0, 1023, ESC_MIN,ESC_MAX);  // Basically just linear interpolation?
    Serial.println(escSetpoint);
    esc.speed(escSetpoint);
    delay(15);
}
