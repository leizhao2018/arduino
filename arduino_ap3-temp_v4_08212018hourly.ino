// Temperature sensor part//////
//leizhao 01252019:add the function of showing mess in screen. Cassie 09232018: Adjusted frequency of reports to hourly. Bingwei 08212018: Improving program work with either AP3 or RS485 monitor. Bingwei //
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_1 6
#define ONE_WIRE_BUS_2 8

OneWire oneWire_in(ONE_WIRE_BUS_1);
OneWire oneWire_out(ONE_WIRE_BUS_2);

DallasTemperature sensor_inhouse(&oneWire_in);
DallasTemperature sensor_outhouse(&oneWire_out);
// End Temperature part///////

//screen part ///////
#include<LiquidCrystal.h>

LiquidCrystal lcd(12,11,10,9,1,0);
//screen part end///

// AP3 part///////////
#include <SoftwareSerial.h>

#define SSerialRX        4  //Serial Receive pin
#define SSerialTX        5  //Serial Transmit pin
#define SSerialTxControl 3   //RS485 Direction control

SoftwareSerial mySerial(SSerialRX, SSerialTX); // RX, TX
// End AP3 part//////////


void setup() {
  // Temperature sensor part//////
  //Serial.begin(9600);
  sensor_inhouse.begin();
  sensor_outhouse.begin();
  
  // AP3 part///////////
  mySerial.begin(9600);      // open the serial port at 9600 bps:
  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(SSerialTxControl, OUTPUT); 
  digitalWrite(SSerialTxControl, LOW);  // Init Transceiver
  lcd.begin(16,2);


  
  //testing part////
  
  
}

void loop() {  
  delay(5000);
  int var = 0;
  int t1 = 0;
  int t2 = 0;
  float Tins = 0.0;
  float Touts = 0.0;
  float Tinmin = 100.0;
  float Toutmin = 100.0;
  float Tinmax = 0.0;
  float Toutmax = 0.0;
  String m1= "000000000";
  String m2= "000000000";
  String m3= "000";
  String m4= "000";
  //Test serial communication
  //Serial.println("Done...");
  
  while(var < 60){
    digitalWrite(LED_BUILTIN, HIGH);
    // do something repetitive 200 times
    // Requesting temperatures... 
    sensor_inhouse.requestTemperatures();
    sensor_outhouse.requestTemperatures();
    float Tin = sensor_inhouse.getTempCByIndex(0)+30.0;
    float Tout = sensor_outhouse.getTempCByIndex(0)+30.0;
    
    //Serial.println(var);
    
    if (0 < Tin && Tin < 100){
      Tins += Tin;
      //Serial.print("t1");
      //Serial.print(": ");
      //Serial.println(Tin);
      Tinmin = min(Tinmin,Tin);
      Tinmax = max(Tinmax,Tin);
      t1++;
    }
    if (0 < Tout && Tout < 100){
      Touts += Tout;
      //Serial.print("t2");
      //Serial.print(": ");
      //Serial.println(Tout);
      Toutmin = min(Toutmin,Tout);
      Toutmax = max(Toutmax,Tout);
      t2++;
    }
    
    var++;
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  // Create message
  if (t1 > 0){
    m1 = String(int((Tins/t1)*10));
    m3 = String(int((Tins/t1)*10)-300);
    m1 = m1 + String(int(Tinmin*10));
    m1 = m1 + String(int(Tinmax*10));
  }
  
  if (t2 > 0){
    m2 = String(int((Touts/t2)*10));
    m4 = String(int((Touts/t2)*10)-300);
    m2 = m2 + String(int(Toutmin*10));
    m2 = m2 + String(int(Toutmax*10));
  }
  
  String mes = String(m1+m2);
  String mes2 = String(m3+m4);
  
  //Serial.println(mes);
  
  // Satellite transfer
  ap3transmes(mes,mes2);

  //digitalWrite(LED_BUILTIN, LOW);  
  delay(3600000);
  
}

void ap3transmes(String message,String message2) {
  
  //char ms = message;
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);  
  
  digitalWrite(SSerialTxControl, HIGH);  // Enable RS485 Transmit
  String showtin = "$001," + message2 + "#";
  delay(500);
  mySerial.print(showtin);        // for the monitor
  delay(1000);
  
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  mySerial.print("\n");
  delay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);  
  
  
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  mySerial.print("\n"); 
  delay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW); 
  delay(1000);  
  
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //mySerial.write("!!!sudo:o00200000>>");  
  String mess = String("ylb "+message);
  mySerial.print(mess);
  mySerial.print("\n");
 // delay(5000);                       // wait for a second
 
  //screen part ///////
  lcd.setCursor(0,0);
  lcd.print(showtin);
  lcd.setCursor(0,1);
  lcd.print(mess);
  //screen part end///
  
  digitalWrite(SSerialTxControl, LOW);  // Disable RS485 Transmit
  
  //Serial.end()
  digitalWrite(LED_BUILTIN, LOW);  
  //delay(600000);
}
