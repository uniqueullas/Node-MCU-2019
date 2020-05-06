/*The code is written for AIT college project, called HEALTH MONITORING SYSTEM
   ECE dept Final year project
   board used is NODE MCU version 2.3
   micro contoller NodeMCU 1.0(ESP-12E MOdule)
   Upload speed 115200
   80 Mhz CPU frequency
   auto bootloader
   code uploaded to https://github.com/uniqueullas/Node-MCU-2019/blob/master/health_monitoring.ino
*/

// the last working code 06/05/2020 on board
// uploading to blynk.com maikinng it iot

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "peBRDC9wI9FEz-VwIqmlnebONFNcyb3q";
char ssid[] = "new";
char pass[] = "12345678";
BlynkTimer timer;

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9 // Lower resolution
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#include <Adafruit_SSD1306.h>
#define OLED_Address 0x3C // 0x3C device address of I2C OLED. Few other OLED has 0x3D
Adafruit_SSD1306 oled(128, 64); // create our screen object setting resolution to 128x64

WidgetLED led1(V2);
WidgetLED led2(V5);
const int eme = 12;
int emestate = 0;
int a = 0;
int lasta = 0;
int lastb = 0;
int LastTime = 0;
int ThisTime;
bool BPMTiming = false;
bool BeatComplete = false;
int BPM = 0;
int value = 0;
#define UpperThreshold 560
#define LowerThreshold 530

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  //timer.setInterval(1000L, sendSensor);
  Serial.println("AIT _ Health Monitoring Sysyem Logs");
  delay(2000);
  pinMode(eme, INPUT);
  sensors.begin();

  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  oled.clearDisplay();
  oled.setTextSize(2);
  Serial.println("Entering loop....!");
  Serial.println("Function :emergency initialized");
  Serial.println("Function :temparature initialized");
  Serial.println("Function :ecg initialized");
  Serial.println("Function :bpm initialized");
  Serial.println("Function :displai initialized");
  Serial.println("*******Running on loop*******");
}

void loop() {
  value = 0;
  emestate = digitalRead(eme);
  led1.off();
  while (emestate == LOW) {
    led1.on();
    emergency();
    emestate = digitalRead(eme);
  }
  
  int temp = temparature ();
  value = analogRead(0);
  //delay(200);
  Serial.print("value ");
  Serial.println(value);
  ecg(value);
  int BPM = bpm(value);
  displai(BPM, temp);
  upload(BPM, temp);
}

void emergency() {
  //Serial.println("Function calling-emergency");
  //Serial.println("...");
  oled.clearDisplay();
  tone(14, 2093, 200);
  delay(100);
  tone(14, 1000, 200);
  delay(100);
  Serial.println("emg");
  oled.writeFillRect(0, 50, 128, 16, BLACK);
  oled.setCursor(0, 1);
  oled.print("EMERGENCY!");
  oled.display();
  int temp = temparature ();
  int value = analogRead(0);
  delay(50);
  int BPM = bpm(value);
  displai(BPM, temp);
  String body = String("Emergency button pressed \r\nThe heart beat rate is:") + BPM + "\r\nThe Body temprature is:" + temp + "\r\nFor quick contact +91 82*******12";
  Serial.println(body);
  Blynk.email("ullas6558@gmail.com", "Subject: Emergency..!", body);
}

int temparature () {
  //Serial.println("Function calling-temparature");
  //Serial.println("...");
  sensors.requestTemperatures();
  int tempC = sensors.getTempCByIndex(0);
  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    //Serial.print("Temperature for the device 1 (index 0) is: ");
    //Serial.println(tempC);
    //Serial.println("...");
  }
  else {
    Serial.println("Error: Could not read temperature data");
  }
  return tempC;
}

int ecg(int value) {
  //Serial.println("Function calling-ecg");
  //Serial.println("...");
  if (a > 60) {
    oled.clearDisplay();
    a = 0;
    lasta = a;
  }
  ThisTime = millis();
  oled.setTextColor(WHITE);
  //int b = 60 - (value / 16);
  int b = map(value, 0, 1023, 0, 20);
  oled.writeLine(lasta, lastb, a, b, WHITE);
  oled.display();
  lastb = b;
  lasta = a;
  a = a + 3;
  return b;
}

int bpm(int value) {
  //Serial.println("Function calling-bpm");
  //Serial.println("...");
  if (value > UpperThreshold) {
    if (BeatComplete) {
      BPM = ThisTime - LastTime;
      BPM = int(60 / (float(BPM) / 1000));
      BPMTiming = false;
      BeatComplete = false;
    }
    if (BPMTiming == false) {
      LastTime = millis();
      BPMTiming = true;
      tone(14, 2093, 100);
    }
  }
  if ((value < LowerThreshold) & (BPMTiming))
    BeatComplete = true;
  return BPM;
}

void displai(int BPM, int temp) {
  //Serial.println("Function calling-displai");
  //Serial.println("...");
  oled.writeFillRect(0, 50, 128, 16, BLACK);
  //oled.setCursor(62, 0);
  //oled.print("EMR:");
  oled.setCursor(0, 30);
  oled.print("TMP:");
  oled.print((temp % 99));
  oled.setCursor(0, 51);
  oled.print("BPM:");
  oled.println(BPM);
  oled.display();
  Serial.print("TMP:");
  Serial.print(temp);
  Serial.print("   BPM:");
  Serial.println(BPM);
}

void upload(int BPM, int temp) {
  Blynk.virtualWrite(V0, BPM);
  Blynk.virtualWrite(V1, temp);
  // Blynk.virtualWrite(V3, ecg);

  //  if (bot == 1) {
  //    led2.on();
  //  }
  //  else {
  //    led2.off();
  //  }
  Blynk.run();
}
