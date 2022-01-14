#include "RTClib.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

//servo
const int pin_servo = 5;
Servo myservo;

//suhu
OneWire pin_DS18B20(3);
DallasTemperature DS18B20(&pin_DS18B20);

//LCD
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//RTC
RTC_DS3231 rtc;
char dataHari[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
String hari;
int tanggal, bulan, tahun, jam, menit, detik;
float suhu;

//waterpump
const int pin_waterpump1 = 8;
const int pin_waterpump2 = 12;

//waterlevel
#define pin_waterlevel A1 
int nilai = 0;

void setup () {
 //RTC
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("RTC Tidak Ditemukan");
    Serial.flush();
    abort();
  }
  
  //Servo
  myservo.attach(pin_servo);

  //Atur Waktu
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //  rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  //Suhu
  Serial.begin(9600);
  DS18B20.begin();

  //LCD
  lcd.init();

  //waterpump
  pinMode(pin_waterpump1, OUTPUT);
  pinMode(pin_waterpump2, OUTPUT);
}

int bacaAir(){
  nilai = analogRead(pin_waterlevel);
  return nilai;
}

void loop () {
  //RTC
  DateTime now = rtc.now();
  hari    = dataHari[now.dayOfTheWeek()];
  //tanggal = now.day(), DEC;
  //bulan   = now.month(), DEC;
  //tahun   = now.year(), DEC;
  jam     = now.hour(), DEC;
  menit   = now.minute(), DEC;
  detik   = now.second(), DEC;

  /*Serial.println(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun);
  Serial.println(String() + jam + ":" + menit + ":" + detik);
  Serial.println(String() + "Suhu: " + suhu + " C");
  Serial.println();
  */
  //waterlevel  
  int ketinggian = bacaAir();
  Serial.print("Ketinggian Air :" );
  Serial.println(ketinggian);
  
  //waterpump penurun suhu
  if(suhu >= 27){
    if(ketinggian >= 110){
      digitalWrite(pin_waterpump2, LOW);
      digitalWrite(pin_waterpump1, HIGH);
    }
    else if (ketinggian <= 100){
      digitalWrite(pin_waterpump2, HIGH);
      digitalWrite(pin_waterpump1, LOW);
    }
  }else if(suhu < 27){
    if(ketinggian >= 110){
      digitalWrite(pin_waterpump1, HIGH);
      digitalWrite(pin_waterpump2, HIGH);
    }else if(ketinggian <= 100){
      digitalWrite(pin_waterpump1, LOW);
      digitalWrite(pin_waterpump2, HIGH);
    }
  }
  /*
  //waterpump kuras air
  if(hari == "Rabu" & jam == 7 & menit == 30 & detik == 1){
    if(ketinggian >= 120){
      digitalWrite(pin_waterpump2, LOW);
      digitalWrite(pin_waterpump1, HIGH);
    }
    else if (ketinggian <= 115){
      digitalWrite(pin_waterpump2, HIGH);
      digitalWrite(pin_waterpump1, LOW);
    }if(hari == "Rabu" & jam == 7 & menit == 33  & detik == 1){
      if(ketinggian >= 120){
      digitalWrite(pin_waterpump2, HIGH);
      digitalWrite(pin_waterpump1, HIGH);
      }
    }
  }*/

  //Suhu
  DS18B20.requestTemperatures();
  suhu = DS18B20.getTempCByIndex(0);
  Serial.print("Suhu Celsius: ");
  Serial.print(suhu);
  
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(String() + hari + "," + "air : " + ketinggian);
  lcd.setCursor(0,1);
  lcd.print(String() + jam + ":" + menit + ":" + detik);
  lcd.print("  ");
  lcd.print(suhu);

  //Servo
  if(jam == 7 & menit == 15 & detik == 1){
    kasih_pakan(1);    
  }
  if(jam == 15 & menit == 10 & detik == 1){
    kasih_pakan(1);    
  }
  if(jam == 21 & menit == 15 & detik == 1){
    kasih_pakan(1);    
  }
}

void kasih_pakan(int jumlah){
  for(int i = 1; i <= jumlah; i++){
  myservo.write(180);
  delay(500);  
  myservo.write(25);
  delay(500);  
  } 
}
