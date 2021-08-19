// Purdue IEEE ROV Team
// Motor-Testing ESC Black Box
// [v0.1] [2021.01.22] [Tyler Stagge]
//    -This just uses the data from the potentiometer to drive a servo (in preparation for driving the lumineer ESC)

#include <Servo.h>

Servo esc;

int POT_IN = A0;
int potVal = 0;  //10-bit ADC => value ranges from 0 to 1023
double escSetpoint = 0; //Between 0 and 180
double CONV_FAC = 0.17578125; //180/1024;

void setup(void) {
    Serial.begin(9600);
    esc.attach(10, 1040, 1960); //Pin 9, Min and Max pulsewidths are 800 and 2000 microseconds
    //esc.write(90.1);
    //Serial.println(CONV_FAC);
}

void loop(void) {
    potVal = analogRead(POT_IN);
    //Serial.println(potVal);

    escSetpoint = potVal * CONV_FAC;
    Serial.println(escSetpoint);
    esc.write(escSetpoint);
}
