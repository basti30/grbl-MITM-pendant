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

  //Serial.println("Pendant connected");
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
  uint16_t middle_x = 2015;
  uint16_t middle_y = 2150;
  uint16_t deadband = 100;
  uint16_t max_v = 100;
  //deadband
  if(adc_x < middle_x + deadband && adc_x > middle_x - deadband){
    adc_x = 0;
  }
  
  if(adc_y < middle_y + deadband && adc_y > middle_y - deadband){
    adc_y = 0;
  }

  if(adc_x == 0 && adc_y == 0){
    return "";
  }

  float val_x = -(((float)adc_x) - middle_x) * 1 / 2000;// * (2000.0 / (100.0*100.0*100.0));
  float val_y = (((float)adc_y) - middle_y) * 1 / 2000;// * (2000.0 / (100.0*100.0*100.0));

  //float val_f_x = (abs(val_x) - abs((int)val_x)) * 100000;
  //float val_f_y = (abs(val_y) - abs((int)val_y)) * 100000;
  String res = "";
  if(adc_y == 0){
    res = "$J=G91 F500 X" + String(val_x); 
  }
  else if(adc_x == 0){
    res = "$J=G91 F500 Y" + String(val_y);
  }
  else{
    //$J=G91 G20 X0.5 F10
    res = "$J=G91 F100 X" + String(val_x) + "Y" + String(val_y);
    //sprintf (str, "G91 G0 X%d.%d Y%d.%d", (int)val_x, (int)val_f_x, (int)val_y, (int)val_f_y); 
  }
  
  Serial.print(adc_x);
  Serial.print("\t");
  Serial.print(adc_y);
  Serial.print("\t"); //*/
  return res;
}

void sendJog(String cmd){
  if(cmd != ""){
    //Serial3.println("G91 G0  X1");
    //Serial.println("P >>> G91 G0  X1");
    Serial3.println(cmd);
    Serial.print("P >>> ");
    Serial.println(cmd);
  }
}

void loop() {
  if (Serial3.available()) {
    Serial.write(Serial3.read());
  }
  if (Serial.available()) {
    Serial3.write(Serial.read());
  }

  if(nb_delay(200)){
    uint16_t rx_adc = avg_read(RX_PIN, 10);
    uint16_t ry_adc = avg_read(RY_PIN, 10);
    //static char command[40];
    //command[0] = '\0';
    sendJog(convertToCommand(rx_adc, ry_adc));
    //Serial.println("\t");
    //Serial3.println("G91 G0  X1");
    //Serial.println("P >>> G91 G0  X1");
  }//*/
  
  
}
