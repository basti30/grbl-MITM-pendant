/*
*****grbl "Man in the Middle" Jog Controller Pendant*****

  Uses interrupt-based rotary encoder code by Simon Merrett, based on insight from Oleg Mazurov, Nick Gammon, rt, Steve Spence
  Based on SoftwareSerialExample sketch by Tom Igoe
  Receives from the hardware serial, sends to software serial.
  Receives from software serial, sends to hardware serial.

  The circuit:
   RX is digital pin 10 (connect to TX of other device)
   TX is digital pin 11 (connect to RX of other device)

*/

#define RX_PIN PA0
#define RY_PIN PA1
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  pinMode(RX_PIN, INPUT_ANALOG);
  pinMode(RY_PIN, INPUT_ANALOG);
  Serial3.begin(115200);
}

inline bool nb_delay(int period){
  static unsigned long time_then = 0;
  if(millis() >= time_then){
    time_then = millis() + period;
    return true;
  }
  return false;
}

uint16_t avg_read(int pin, int n){
  uint32_t sum=0;
  for(int i=0; i<n; i++){
    sum += analogRead(pin);
  }
  return sum/n;
}

String convertToCommand(uint16_t adc_x, uint16_t adc_y){
  static bool jogging = false;
  uint16_t middle_x = 2044;
  uint16_t middle_y = 2124;
  uint16_t deadband = 200;
  uint16_t max_v = 100;

  if(adc_x < middle_x + deadband && adc_x > middle_x - deadband){
    adc_x = 0;
  }
  
  if(adc_y < middle_y + deadband && adc_y > middle_y - deadband){
    adc_y = 0;
  }

  if(adc_x == 0 && adc_y == 0){
    if(jogging){
      jogging = false;
      //Serial3.write(0x85);
      //Serial3.println("G4P0");
      //Serial.println("P >>> G4P0");
    }
    return "";
  }
  jogging = true;

  float mag_x = (adc_x > middle_x) ? -1 : (adc_x < middle_x) ?  1 : 0;
  float mag_y = (adc_y > middle_y) ?  1 : (adc_y < middle_y) ? -1 : 0;

  float vel_x = (((float)adc_x) - middle_x) * 700 / 2000;
  float vel_y = (((float)adc_y) - middle_y) * 700 / 2000;

  String res = "";
  if(adc_y == 0){
    res = "$J=G91 F"+ String(abs(vel_x+400)) + "X" + String(mag_x); 
  }
  else if(adc_x == 0){
    res = "$J=G91 F"+ String(abs(vel_y+400)) + "Y" + String(mag_y); 
  }
  else{
    res = "$J=G91 F"+ String(max(abs(vel_x), abs(vel_y))+400) + "X" + String(mag_x) + "Y" + String(mag_y); 
  }
  
  /*Serial.print(mag_x);
  Serial.print("\t");
  Serial.print(mag_y);
  Serial.print("\t");
  Serial.print(vel_x);
  Serial.print("\t");
  Serial.print(vel_y);
  Serial.print("\t"); 
  return "";//*/
  return res;
}

bool sendJog(String cmd){
  if(cmd != ""){
    Serial3.println(cmd);
    Serial.print("P >>> ");
    Serial.println(cmd);
    return false;
  }
  return true;
}


void loop() {
  if (Serial3.available()) {
    char c = Serial3.read();
    Serial.write(c);
  }
  if (Serial.available()) {
    Serial3.write(Serial.read());
  }

  if(nb_delay(80)){
    uint16_t rx_adc = avg_read(RX_PIN, 10);
    uint16_t ry_adc = avg_read(RY_PIN, 10);
    sendJog(convertToCommand(rx_adc, ry_adc));
  }
}
