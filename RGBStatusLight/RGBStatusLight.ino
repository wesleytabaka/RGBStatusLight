#include "Effect.h"
#include "Transition.h"

/* Pinout */
const int LED_R = 9;
const int LED_G = 10;
const int LED_B = 11;

/* State of each LED  */
int red = 0;
int green = 0;
int blue = 0;
int counter = 0;

String serialOutput = "";

/* Effect and transition variables */
Effect effect = SOLID;
int effectCounter = 0;
int effectRate = 0;
int effectPeriodLength = 32; // Nice base 2 number.  Should be able to describe the effect function in 32 steps with okay resolution.

Transition transition = NOW;
int transitionCounter = 0;
int transitionDuration = 0;

int effect_basecolor_r = 0;
int effect_basecolor_g = 0;
int effect_basecolor_b = 0;

int transition_old_r = 0;
int transition_old_g = 0;
int transition_old_b = 0;
int transition_new_r = 0;
int transition_new_g = 0;
int transition_new_b = 0;

/* Startup */
void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Turn off on-board LED.
  digitalWrite(LED_BUILTIN, LOW); 

  // Initial condition
  setColor(0, 0, 0);
  applyColor();

  // Start serial
  Serial.begin(9600);

}

void setColor(int r, int g, int b){
  red = 255 - r;
  green = 255 - g;
  blue = 255 - b;
}

void applyColor(){
  //Serial.println("Invoked applyColor(): " + String(red) + "," + String(green) + "," + String(blue));
  analogWrite(LED_R, red);
  analogWrite(LED_G, green);
  analogWrite(LED_B, blue);
}

void resetEffect(){
  effectCounter = 0;  
}

void processEffect(){
//  Serial.println("Invoked processEffect( " + String(input_effect) + ", " + String(input_rate));
//
//  effect = input_effect;
//  effectRate = input_rate;
  
  if(effect == BLINK){
    int onoff = floor(effectCounter / (effectPeriodLength / 2));
    setColor(effect_basecolor_r * onoff, effect_basecolor_g * onoff, effect_basecolor_b * onoff);
    delay(floor(effectRate / effectPeriodLength));
  }
  if(effect == PULSE){
    float proportion = (float)(effectPeriodLength - effectCounter - 1) / (float)effectPeriodLength;
    setColor(floor((float)effect_basecolor_r * proportion), floor((float)effect_basecolor_g * proportion), floor((float)effect_basecolor_b * proportion));
    delay(floor(effectRate / effectPeriodLength));
  }
  if(effect == CYCLE){
    int thisStep = (effectCounter % 11);
    float r_component;
    float g_component;
    float b_component;

    float goingUp = (float)(thisStep) / (float)11;
    float goingDown = (float)(11 - thisStep) / (float)11;
    
    if(effectCounter >= 0 && effectCounter < 11){
      r_component = (float)255 * goingDown; // Going down
      g_component = (float)255 * goingUp; // Going up
      b_component = 0;
    }

    if(effectCounter >= 11 && effectCounter < 22){
      r_component = 0;
      g_component = (float)255 * goingDown; // Going down
      b_component = (float)255 * goingUp; // Going up
    }

    if(effectCounter >= 22 && effectCounter < 32){
      r_component = (float)255 * goingUp; // Going up
      g_component = 0;
      b_component = (float)255 * goingDown; // Going down
    }
    
    setColor(floor(r_component), floor(g_component), floor(b_component));
    delay(floor(effectRate / effectPeriodLength));
  }
  
  effectCounter = (effectCounter+1) % effectPeriodLength; // Increment effect counter, resetting so it never overflows.
  applyColor(); // Set the pins.  
}

void loop() {
  Serial.println("Invoked loop()");
  if(Serial.available()){    
    
    String input = Serial.readStringUntil('-');
    input.trim();
    if(input.length() > 0){
      int field2 = input.indexOf(",", 0);
      int field3 = input.indexOf(",", field2 + 1);
      int field4 = input.indexOf(",", field3 + 1);
      int field5 = input.indexOf(",", field4 + 1);
      int field6 = input.indexOf(",", field5 + 1);
      int field7 = input.indexOf(",", field6 + 1);

      // Color changing command:
      // TRANSITIONMODE: NOW: 0, FADE: 1
      // EFFECT: SOLID: 0, BLINK: 1, PULSE: 2, CYCLE: 3
      // example: 0,0,0,0,255,255,255
      transition = (Transition) input.substring(0, field2).toInt();
      transitionDuration = input.substring(field2 + 1, field3).toInt();
      effect = (Effect) input.substring(field3 + 1, field4).toInt();
      effectRate = input.substring(field4 + 1, field5).toInt();
      String r = input.substring(field5 + 1, field6);
      String g = input.substring(field6 + 1, field7);
      String b = input.substring(field7 + 1);

      serialOutput = "OK:";
      serialOutput += transition;
      serialOutput += ",";
      serialOutput += transitionDuration;
      serialOutput += ",";
      serialOutput += effect;
      serialOutput += ",";
      serialOutput += effectRate;
      serialOutput += ",";
      serialOutput += r;
      serialOutput += ",";
      serialOutput += g;
      serialOutput += ",";
      serialOutput += b;

      resetEffect();
      effect_basecolor_r = r.toInt();
      effect_basecolor_g = g.toInt();
      effect_basecolor_b = b.toInt();
      
      setColor(r.toInt(), g.toInt(), b.toInt());
      Serial.println(serialOutput);
    }
  }

  Serial.println("Before: Effect is " + String(effect) + ".  Effect rate is " + effectRate );

  if(effect == SOLID){
    applyColor(); // Set the pins. 
    delay(5000);
  }

  if(effect == BLINK){
    processEffect();
  }

  if(effect == PULSE){
    processEffect();
  }
  if(effect == CYCLE){
    processEffect();
  }
   
  Serial.println("After: Effect is " + String(effect) + ".  Effect rate is " + effectRate );
  
}
