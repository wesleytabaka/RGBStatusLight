#include "Effect.h"
#include "Transition.h"

/* Pinout */
const int LED_R = 6;
const int LED_G = 10;
const int LED_B = 11;

/* State of each LED  */
int red = 0;
int green = 0;
int blue = 0;
int counter = 0;

String serialOutput = "";

/* Effect and transition variables */
float refreshRate = 33.00; // milliseconds

Effect effect = SOLID;
int effectCounter = 0;
int effectRate = 0;
int effectPeriodLength;

Transition transition = NOW;
int transitionCounter = 0;
int transitionDuration = 0;
int transitionPeriodLength;

int effect_basecolor_r = 0;
int effect_basecolor_g = 0;
int effect_basecolor_b = 0;
int effect_basecolor_r2 = 0;
int effect_basecolor_g2 = 0;
int effect_basecolor_b2 = 0;

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

void resetTransition(){
  transitionCounter = 0;  
}

void processEffect(){
//  Serial.println("Invoked processEffect( " + String(input_effect) + ", " + String(input_rate));
  int onoff = 0;
  float proportion = 0.00;
  int thisStep = 0;
  float r_component = 0.00;
  float g_component = 0.00;
  float b_component = 0.00;
  float goingUp = 0.00;
  float goingDown = 0.00;
  int cycleSectionPeriod = floor((float)effectPeriodLength / (float)3);

  switch(effect){
    case SOLID:
      delay(10000); // Don't waste time refreshing for SOLID effect.
      break;
    case BLINK:
      onoff = floor(effectCounter / (effectPeriodLength / 2));
      if(onoff == 0){
        setColor(effect_basecolor_r, effect_basecolor_g, effect_basecolor_b);
      }
      else {
        setColor(effect_basecolor_r2, effect_basecolor_g2, effect_basecolor_b2);
      }
      delay(floor(refreshRate)); // Delay for half of the effect.  Reduce the overall number of loop()s.
      break;
    case FLASH:
      proportion = (abs(((float)effectPeriodLength / (float)2) - (float)effectCounter) * (float)2) / (float)effectPeriodLength;
      setColor(floor(((float)effect_basecolor_r * proportion) + ((float)effect_basecolor_r2 * (1 - proportion))), floor(((float)effect_basecolor_g * proportion) + ((float)effect_basecolor_g2 * (1 - proportion))), floor(((float)effect_basecolor_b * proportion) + ((float)effect_basecolor_b2 * (1 - proportion))));
      delay(floor(refreshRate));
      break;
    case PULSE:
      proportion = (float)(effectPeriodLength - effectCounter - 1) / (float)effectPeriodLength;
      setColor(floor(((float)effect_basecolor_r * proportion) + (effect_basecolor_r2 * (1 - proportion))), floor(((float)effect_basecolor_g * proportion) + ((float)effect_basecolor_g2 * (1 - proportion))), floor(((float)effect_basecolor_b * proportion) + ((float)effect_basecolor_b2 * (1 - proportion))));
      delay(floor(refreshRate));
      break;
    case CYCLE:
      thisStep = (effectCounter % cycleSectionPeriod);
      r_component;
      g_component;
      b_component;
  
      goingUp = (float)(thisStep) / (float)cycleSectionPeriod;
      goingDown = (float)(cycleSectionPeriod - thisStep) / (float)cycleSectionPeriod;
      
      if(effectCounter >= 0 && effectCounter < (1 * cycleSectionPeriod)){
        r_component = (float)255 * goingDown; // Going down
        g_component = (float)255 * goingUp; // Going up
        b_component = 0;
      }
  
      if(effectCounter >= (1 * cycleSectionPeriod) && effectCounter < (2 * cycleSectionPeriod)){
        r_component = 0;
        g_component = (float)255 * goingDown; // Going down
        b_component = (float)255 * goingUp; // Going up
      }
  
      if(effectCounter >= (2 * cycleSectionPeriod) && effectCounter < (3 * cycleSectionPeriod)){
        r_component = (float)255 * goingUp; // Going up
        g_component = 0;
        b_component = (float)255 * goingDown; // Going down
      }

      if(effectCounter >= (3 * cycleSectionPeriod)){
        r_component = 255; // Going up
        g_component = 0;
        b_component = 0; // Going down
      }

      setColor(floor(r_component), floor(g_component), floor(b_component));
      delay(floor(refreshRate));
      break;
  }
  
  effectCounter = (effectCounter+1) % effectPeriodLength; // Increment effect counter, resetting so it never overflows.
  applyColor(); // Set the pins.  
}

void processTransition(){
  Serial.println("Invoked processTransition");
  if(transitionCounter <= transitionPeriodLength){
    // Transition to the correct first color if the effect is CYCLE.
    if(effect == CYCLE){
      transition_new_r = 255;
      transition_new_g = 0;
      transition_new_b = 0;
    }
    if(transition == FADE){
      // Calculate difference between old and new and prorate the color components along the transitionPeriodLength.
      float r_diff = (float)transition_new_r - (float)transition_old_r;
      float g_diff = (float)transition_new_g - (float)transition_old_g;
      float b_diff = (float)transition_new_b - (float)transition_old_b;

      Serial.println("r_diff: " + String(r_diff) + ", g_diff: " + String(g_diff) + ", b_diff: " + String(b_diff));
  
      int new_r = floor((float)transition_old_r + (r_diff * ((float)transitionCounter / (float)transitionPeriodLength)));
      int new_g = floor((float)transition_old_g + (g_diff * ((float)transitionCounter / (float)transitionPeriodLength)));
      int new_b = floor((float)transition_old_b + (b_diff * ((float)transitionCounter / (float)transitionPeriodLength)));

      Serial.println("new_r: " + String(new_r) + ", new_g: " + String(new_g) + ", new_b: " + String(new_b));
  
      setColor(new_r, new_g, new_b); // Again, this could interfere with effect.
      transitionCounter = (transitionCounter + 1); // Increment transition counter.
      applyColor();
      delay(floor(refreshRate));
    }
    else {
      setColor(transition_new_r, transition_new_g, transition_new_b);
      transitionCounter = transitionPeriodLength + 1; // Mark the "NOW" transition as done.  Make sure we're over the transitionPeriodLength.
      applyColor();
      // No delay.
    }
  }
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
      int field8 = input.indexOf(",", field7 + 1);
      int field9 = input.indexOf(",", field8 + 1);
      int field10 = input.indexOf(",", field9 + 1);

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
      String b = input.substring(field7 + 1, field8);
      String r2 = input.substring(field8 + 1, field9);
      String g2 = input.substring(field9 + 1, field10);
      String b2 = input.substring(field10 + 1);

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
      serialOutput += ",";
      serialOutput += r2;
      serialOutput += ",";
      serialOutput += g2;
      serialOutput += ",";
      serialOutput += b2;

      resetEffect();
      effect_basecolor_r = r.toInt();
      effect_basecolor_g = g.toInt();
      effect_basecolor_b = b.toInt();
      effect_basecolor_r2 = r2.toInt();
      effect_basecolor_g2 = g2.toInt();
      effect_basecolor_b2 = b2.toInt();
      effectPeriodLength = floor((float)effectRate / refreshRate);

      resetTransition();
      transition_old_r = abs(red-255);
      transition_old_g = abs(green-255);
      transition_old_b = abs(blue-255);
      transition_new_r = r.toInt();
      transition_new_g = g.toInt();
      transition_new_b = b.toInt();
      transitionPeriodLength = floor((float)transitionDuration / refreshRate);
      
      Serial.println(serialOutput);
    }
  }

  Serial.println("Before: Effect is " + String(effect) + ".  Effect rate is " + effectRate );

  // Continue processing the transition until it is done.
  if(transitionCounter <= transitionPeriodLength){
    processTransition();
  }
  else {
    processEffect();
  }
   
  Serial.println("After: Effect is " + String(effect) + ".  Effect rate is " + effectRate );
  
}
