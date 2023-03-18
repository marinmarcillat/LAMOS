#include <Wire.h>
#include <DS3231.h>

DS3231 myclock;

#include <Preferences.h>
Preferences preferences;

byte year = 2023;
byte month = 3;
byte date = 14;
byte dOW = 2;
byte hour = 19;
byte minute = 0;
byte second = 0;

byte alarm_date = 14;
byte alarm_hour = 19;
byte alarm_minute = 1;
byte alarm_second = 0;

//int alarm1Pin = 2;
int relay_pin = 16;
int rst_mem = 26;

byte alarmMinute;
byte alarmBits;

void sleeping(){
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4,LOW);
  
  // Initialize DS3231
  Serial.println("Initialize DS3231");
  Wire.begin();
  myclock.setClockMode(false);
  myclock.setYear(year);
  myclock.setMonth(month);
  myclock.setDate(date);
  myclock.setDoW(dOW);
  myclock.setHour(hour);
  myclock.setMinute(minute);
  myclock.setSecond(second);

  // Disarm alarms and clear alarms for this example, because alarms is battery backed.
  // Under normal conditions, the settings should be reset after power and restart microcontroller.
  myclock.turnOffAlarm(1);
  myclock.setA1Time(
      alarm_date, alarm_hour, alarm_minute, alarm_second,
      0b00000000, false, false, true);
  // enable Alarm 1 interrupts
  myclock.turnOnAlarm(1);
  // clear Alarm 1 flag
  myclock.checkIfAlarm(1);

  
  // deactivate the second alarm 
  alarmMinute = 0xFF; // a value that will never match the time
  alarmBits = 0b01100000; // Alarm 2 when minutes match, i.e., never
  // Upload the parameters to prevent Alarm 2 entirely
  myclock.setA2Time(
      0, 0, alarmMinute,
      alarmBits, false, false, false);
  // disable Alarm 2 interrupt
  myclock.turnOffAlarm(2);
  // clear Alarm 2 flag
  myclock.checkIfAlarm(2);

  Serial.println("Going to sleep");
  esp_deep_sleep_start();
}

void setup()
{
  Serial.begin(115200);
  pinMode(relay_pin, OUTPUT);
  pinMode(rst_mem,  INPUT_PULLUP);

  digitalWrite(relay_pin, HIGH);
  delay(500);
  digitalWrite(relay_pin, LOW);

  //gestion mémoire NVS
  //Ouverture des namespace
  preferences.begin("my-app", false);

  if(analogRead(rst_mem)==0){
    preferences.putBool("flag", false);
  }
}

void loop()
{
	  // put your main code here, to run repeatedly:
  bool state = preferences.getBool("flag", false);
  Serial.print("\n valeur nvs :");
  Serial.printf("%d \n", state);
  
  if(state == true){
    Serial.println(" déclenchement");
    digitalWrite(relay_pin, HIGH);
    while(1); 
    
  }
  else{
    preferences.putBool("flag", true);
    Serial.println("mise en sommeil");
    sleeping();
  }
}
