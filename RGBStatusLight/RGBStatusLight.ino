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

void loop() {

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
      String transition = input.substring(0, field2);
      String transitionDuration = input.substring(field2 + 1, field3);
      String effect = input.substring(field3 + 1, field4);
      String effectRate = input.substring(field4 + 1, field5);
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
  
      setColor(r.toInt(), g.toInt(), b.toInt());
      applyColor();
      Serial.println(serialOutput);
    }
  }
  
  delay(3000);
}
