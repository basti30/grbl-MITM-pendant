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

#include "quadratureBluePill.h"

/*
Pins associated with each timer are
HardwareTimer timer(1); PA8-9 
HardwareTimer timer(3); PA6-7 
HardwareTimer timer(4); PB6-7 
*/


quadCounter  QC1(QUAD_TIMER_1);
quadCounter  QC3(QUAD_TIMER_3);
quadCounter  QC4(QUAD_TIMER_4);

#define SELECTOR PA5

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial3.begin(115200);
  pinMode(SELECTOR, INPUT_ANALOG);
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

float getStepsize(){
    uint16_t sel = avg_read(SELECTOR, 10);
    //0, 817, 1023, 1363, 2058, 4094
    const float steps[] = {0.01, 0.1, 0.5, 1, 2, 5};
    if(sel < 400){
      return steps[0];
    }else if(sel < 920){
      return steps[1];
    }else if(sel < 1150){
      return steps[2];
    }else if(sel < 1700){
      return steps[3];
    }else if(sel < 3000){
      return steps[4];
    }else
    {
      return steps[5];
    }
}

String convertToCommand(uint16_t adc_x, uint16_t adc_y){

/*
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
  Serial.print("\t"); */
  return "";//
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

#define EncPi 128
#define EncTwoPi 256 
int32_t get_delta(uint8_t encoder_id, int16_t new_angle) {
    static int32_t previous_angle[3] = {0, 0, 0};
    int32_t d = new_angle - previous_angle[encoder_id];
    d = d > EncPi ? d - EncTwoPi : (d < -EncPi ? d + EncTwoPi : d);
    previous_angle[encoder_id] = new_angle;
    return d; 
}

void loop() {
  if (Serial3.available()) {
    char c = Serial3.read();
    Serial.write(c);
  }
  if (Serial.available()) {
    Serial3.write(Serial.read());
  }

  if(nb_delay(300)){
    float stepsize = getStepsize();
    //encoder count
    uint16_t e0 = QC1.count()/4;
    uint16_t e1 = QC3.count()/4;
    uint16_t e2 = QC4.count()/4;
    //delta
    int32_t d0 = get_delta(0,e0);
    int32_t d1 = get_delta(1,e1);
    int32_t d2 = get_delta(2,e2);
    //positions
    static int32_t p0 = 0; 
    static int32_t p1 = 0;
    static int32_t p2 = 0;
    p0 += d0;
    p1 += d1;
    p2 += d2;
    Serial.print(p0);
    Serial.print(", ");
    Serial.print(e0);
    Serial.print("\t ");
    Serial.print(p1);
    Serial.print(", ");
    Serial.print(e1);
    Serial.print("\t ");
    Serial.print(p2);
    Serial.print(", ");
    Serial.print(e2);
    Serial.print("\t ");
    Serial.print(stepsize);
    Serial.print("\t ");
    
    Serial.println();
    //sendJog(convertToCommand(posX, posY));
  }
}
