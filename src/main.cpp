#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>
#include <MFRC522.h>
#include <Servo.h>

#define LCD_SDA A4
#define LCD_SCL A5

// lamp
#define L289N_IN1 2
#define L289N_IN2 3

// pump
#define L289N_IN3 4
#define L289N_IN4 5

#define BUTTON_DEBOUNCE_MS 50

#define LIMIT_SWITCH_PIN A6

LiquidCrystal_I2C lcd(0x27,16,2);

ezButton limitSwitch(LIMIT_SWITCH_PIN);
int limitSwitchState = -1;

ezButton volButton1(A4); // sub vol
ezButton volButton2(A3); // start
ezButton volButton3(A2); // add vol

ezButton volume_buttons[] = {volButton1, volButton2, volButton3};

int VOLUME = 50;

// set to off
int LAMP_MODE = -1;
int PUMP_MODE = -1;

void set_pump_mode(int mode){
  if (mode == 1){
    digitalWrite(L289N_IN3, HIGH);
    digitalWrite(L289N_IN4, LOW);
  } else if (mode == 0){
    digitalWrite(L289N_IN4, LOW);
    digitalWrite(L289N_IN3, LOW);
  }
  PUMP_MODE = mode;
}

void set_lamp_mode(int mode){
  if (mode == 1){
    digitalWrite(L289N_IN1, HIGH);
    digitalWrite(L289N_IN2, LOW);
  } else if (mode == 0){
    digitalWrite(L289N_IN1, LOW);
    digitalWrite(L289N_IN2, LOW);
  }
  LAMP_MODE = mode;
}

void lcd_print(String str){
  lcd.clear();
  lcd.home();
  if (str.length() < 16){
    lcd.print(str);
  } else {
    lcd.setCursor(0,0);
    lcd.print(str.substring(0, 15));
    lcd.setCursor(0, 1);
    lcd.print(str.substring(15));
  }
}

void lcd_home(){
  lcd.clear();
  lcd.home();
  lcd_print("Witaj przy Rozlewaku!");
}

void display_volume(){
  String volume_string = String("Vol: " + String(VOLUME));
  Serial.println(volume_string);
  lcd_print(volume_string);
}

void pour_for(int millis){
  set_pump_mode(1);
  delay(millis);
  set_pump_mode(-1);
}

void pour_volume(int volume) {
  int time = volume * 1000;
  pour_for(time);
}

void pouring_procedure(){
  set_lamp_mode(1);
  pour_volume(VOLUME);
  set_lamp_mode(0);
}



void setup() {
  Serial.begin(9600);
  pinMode(L289N_IN1, OUTPUT);
  pinMode(L289N_IN2, OUTPUT);
  pinMode(L289N_IN4, OUTPUT);
  digitalWrite(L289N_IN1, LOW);
  digitalWrite(L289N_IN2, LOW);
  digitalWrite(L289N_IN3, LOW);
  digitalWrite(L289N_IN4, LOW);

  limitSwitch.setDebounceTime(BUTTON_DEBOUNCE_MS);
  volButton1.setDebounceTime(BUTTON_DEBOUNCE_MS);
  volButton2.setDebounceTime(BUTTON_DEBOUNCE_MS);
  volButton3.setDebounceTime(BUTTON_DEBOUNCE_MS);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd_home();
}

void loop() {
  limitSwitch.loop();
  volButton1.loop();
  volButton2.loop();
  volButton3.loop();

  if (limitSwitch.isPressed()){
    limitSwitchState = 1;
  } else if (limitSwitch.isReleased()){
    limitSwitchState = -1;
  }
  Serial.println(limitSwitchState);

  while (limitSwitchState == 1) {
    display_volume();

    for (int i = 0; i < 3; i++){
      if (volume_buttons[i].isPressed()){
        if (i == 0){
          if (VOLUME > 50){
            VOLUME -= 50;
            display_volume();
          } else if (i == 2) {
            VOLUME += 50;
            display_volume();
          } else if (i == 1) {
            pouring_procedure();
          }
        }
      }
    }
  } while (limitSwitchState == -1) {
    set_pump_mode(-1);
    set_lamp_mode(-1);
  }
}