#include <Wire.h>                     // i2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>        // LCD 1602 I2C용 라이브러리
#include <Adafruit_NeoPixel.h>        //네오픽셀 라이브러리 
#include "DHT.h"                      //온습도 센서 라이브러리
#define DHTPIN A0
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel RGB_LED = Adafruit_NeoPixel(12, 9, NEO_GRB);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // 접근주소: 0x3F or 0x27

int AA = 5;               //모터A 
int AB = 4;               //모터A
int BA = 6;               //모터B 
int BB = 7;               //모터B 

int ON = 1;
int OFF = 0;

float fMax_hum =0.0;                           //센서 기준값 선언 및 초기화
float fMax_temp = 0.0;
float fMin_temp = 0.0;
int Soil_moisture_reference = 0;

void setup() {
  fMax_hum  = 60.0;                            //센서 기준값 설정
  fMax_temp = 30.0;
  fMin_temp = 22.0;
  Soil_moisture_reference = 50;                //기준 수분값 설정 0 ~ 100%
  
  Serial.begin(9600);
  dht.begin();                                 // DHT 초기화
   
  lcd.init();                                  // LCD 초기화
  lcd.backlight();                             // 백라이트 켜기
  
  RGB_LED.begin();                             // Neopixel 초기화
  RGB_LED.setBrightness(200);                  //RGB_LED 밝기조절
  RGB_LED.show();

  pinMode(AA, OUTPUT);                         //모터 핀 설정          
  pinMode(AB, OUTPUT);
  pinMode(BA, OUTPUT);
  pinMode(BB, OUTPUT);
}

void loop() {
  
  float hum = dht.readHumidity();             //습도값 받아오기
  float temp = dht.readTemperature();         //온도값 받아오기
  int Soil_moisture = analogRead(A1);         //토양 수분값 받아오기
  int C_Soil_moisture = map(Soil_moisture, 1023, 0, 0, 100);   //토양 수분값 0~100으로 변환
  
  String strC_Soil = int2String(C_Soil_moisture);              //자릿수별 표시 변경

  if(C_Soil_moisture < Soil_moisture_reference)                //수분 값이 기준 보다 낮을 경우
  {
    WarrningLCD();
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("T : "); lcd.print((int)temp); lcd.print("C ");
    lcd.setCursor(8, 0);
    lcd.print("H : "); lcd.print((int)hum); lcd.print("% ");
    lcd.setCursor(0, 1);
    lcd.print("Soil_M : "); lcd.print(strC_Soil); lcd.print("   ");
  }

  if((temp > fMax_temp && hum > fMax_hum) || temp > fMax_temp)    // 온습도 둘다 높을때, 온도가 높을때 -> Fan ON , LED OFF
  {
    FanONOFF(ON);
    RGB_Color(RGB_LED.Color(0, 0, 0), 10);
  }
  else if(hum > fMax_hum)    //습도가 높을 때 -> Fan ON , LED OFF
  {
    FanONOFF(ON);
    RGB_Color(RGB_LED.Color(100, 100, 100), 10);
  }
  else if(temp < fMin_temp)  //온도가 낮을 때 -> Fan OFF , LED ON
  {
    FanONOFF(OFF);
    RGB_Color(RGB_LED.Color(100, 100, 100), 10);
  }
  else
  {
    FanONOFF(OFF);
    RGB_Color(RGB_LED.Color(0, 0, 0), 10);
  }
  
  delay(200);
  
}

void RGB_Color(float c, int wait) {

  for (int i = 0; i < RGB_LED.numPixels(); i++) 
  {  
     RGB_LED.setPixelColor(i, c);
     RGB_LED.show();
     delay(wait);
  }
}

String int2String(int val)
{
  String sval = "";

  int val2 = (val/10)%10;  //10의 자리
  int val3 = (val/100)%10; //100의 자리
  
  if(val3 >= 1)
  {
    sval = String(val);
  }
  else
  {
     if(val2 >= 1)
     {
      sval = " "+String(val);
     }
     else
     {
      sval = "  "+String(val);
     }
  }

  return sval;
}

void WarrningLCD(){ 
    lcd.clear();
    delay(200);
    lcd.backlight();                         // 백라이트 켜기
    lcd.setCursor(0, 0);                     // 0번째, 0라인
    lcd.print("Not enough water");
    lcd.setCursor(0, 1);                     // 1번째, 1라인
    lcd.print("supply water!!");
    delay(200);
}

void FanONOFF(int OnOff)
{
  if(OnOff == 1)
  {
      digitalWrite(AA, HIGH);
      digitalWrite(AB, LOW);
      digitalWrite(BA, HIGH);
      digitalWrite(BB, LOW);
  }
  else
  {
     digitalWrite(AA, LOW);
     digitalWrite(AB, LOW);
     digitalWrite(BA, LOW);
     digitalWrite(BB, LOW);
  }
}
