//온습도 센서 라이브러리
#include "DHT.h"                            
#define DHTPIN A0
#define DHTTYPE DHT11
#define fMax_temp   24            //온도 기준값 설정 (최대)
#define fMin_temp   20             //온도 기준값 설정 (최소)
#define fMax_hum    45         //습도 기준값 설정

#include <Adafruit_NeoPixel.h> //네오픽셀 라이브러리 
#include <Wire.h>                         // i2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>            // LCD 1602 I2C용 라이브러리

// 팬 pin 설정
#define AA 5
#define AB 4
#define BA 6
#define BB 7

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);       // 접근주소: 0x3F or 0x27
Adafruit_NeoPixel RGB_LED =
Adafruit_NeoPixel(12, 9, NEO_GRB);

 byte char_celsius[8] = {     // 온도 단위 출력
  0b10000,
  0b00110,
  0b01001,
  0b01000,
  0b01000,
  0b01000,
  0b01001,
  0b00110,
};

unsigned long p_time = 0;
unsigned long c_time;

void setup() { 
    Serial.begin(9600);   // Serial 초기화
    dht.begin();      // DHT11 센서 초기화
    lcd.init();           // LCD 초기화
    lcd.backlight();
    lcd.createChar(3, char_celsius);     // 온도 단위 출력
    pinMode(AA, OUTPUT);       //모터 핀 설정        
    pinMode(AB, OUTPUT);
    pinMode(BA, OUTPUT);
    pinMode(BB, OUTPUT);

    RGB_LED.begin();            // Neopixel 초기화
    RGB_LED.setBrightness(200); //RGB_LED 밝기조절
    RGB_LED.show();
}

void loop() {
    c_time = millis();
    
    float temp = dht.readTemperature();        //온도값 받아오기
    float hum = dht.readHumidity();            //습도값 받아오기
    int Soil_moisture = analogRead(A1);   //Soil_Moisture에 아날로그값 저장
    int C_Soil_moisture = map(Soil_moisture, 1023, 0, 0, 100);   //토양 수분값 0~100으로 변환
    
    Serial.print("[DHT11] → Temp : "); Serial.print((int)temp); Serial.print("[°C],\t");
    Serial.print("Humidity : ");    Serial.print((int)hum); Serial.println("%");
    Serial.print("Resistance: ");   Serial.print(Soil_moisture);    Serial.print("[Ω],\t");
    Serial.print("Humidity[Soil] : ");   Serial.print(C_Soil_moisture); Serial.println("%");
    Serial.print(c_time-p_time);
    
    // 기준 온/습도 값보다 보다 높을 경우
    if(temp > fMax_temp || C_Soil_moisture > fMax_hum)      FAN_ON();
    else                                                               FAN_OFF();

    if(temp < fMin_temp) // 설정 온도 값보다 보다 낮을 경우
    RGB_Color(RGB_LED.Color(255, 255, 255), 0); //LED ON
    else    RGB_Color(RGB_LED.Color(0, 0, 0), 0);       //LED OFF

    // 토양 수분이 35%이하일때
    // 이전 펌프 작동으로부터 최소 20초의 간격을 두고 1초간 펌프 작동
    if((C_Soil_moisture < 35) && (c_time-p_time > 20000)){
        digitalWrite(AA, HIGH);
        digitalWrite(AB, LOW);
        lcd.print("Water PUMP ON... ");
        delay(1000);
        digitalWrite(AA, LOW);
        digitalWrite(AB, LOW);
        lcd.clear();
        p_time = c_time;
    }
    
    disp(temp, hum, Soil_moisture, C_Soil_moisture);
    delay(10);
}



void disp(float temp, float hum, int so, int cso){
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");    lcd.print((int)temp);    lcd.write(byte(3));
    lcd.setCursor(0, 1);
    lcd.print("Humid[AIR] : ");    lcd.print((int)hum);    lcd.print("%");
    delay(2000);
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("Resist: ");    lcd.print(so);    lcd.print(" Ohms");
    lcd.setCursor(0, 1);
    lcd.print("Humid[Soil]: ");    lcd.print(cso);    lcd.print("%");
    delay(2000);
    lcd.clear();
}

void FAN_ON(){
        digitalWrite(BA, HIGH);
        digitalWrite(BB, LOW);
}
void FAN_OFF(){
        digitalWrite(BA, LOW);
        digitalWrite(BB, LOW);
}

void RGB_Color(float c, int wait) {
  for (int i = 0; i < RGB_LED.numPixels(); i++) {
    RGB_LED.setPixelColor(i, c);
    RGB_LED.show();
    delay(wait);
  }
}
