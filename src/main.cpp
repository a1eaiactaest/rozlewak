#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>
#include <MFRC522.h>

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 2

// L298N
#define GATE_1 2
#define GATE_2 3

// MFRC522
#define RST_PIN 11
#define SS_PIN 12

#define BUTTON_DEBOUNCE_MS 5

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(6);
ezButton limitSwitch2(5);
ezButton limitSwitch3(4);

ezButton volButton1(7); // sub vol
ezButton volButton2(8); // start
ezButton volButton3(9); // add vol


ezButton limit_switches[] = {limitSwitch1, limitSwitch2, limitSwitch3};
ezButton volume_buttons[] = {volButton1, volButton2, volButton3};

int VOLUME = 50; // ml
int LAMP_MODE = 0; // default off

// LAMP
void set_lamp_mode(int mode){
  /*
    0 turn off
    1 turn on
  */

  if (mode != LAMP_MODE){
    if (mode == 1){
      digitalWrite(GATE_1, HIGH);
      digitalWrite(GATE_2, LOW);
      LAMP_MODE = 1;
    } else if (mode == 0){
      digitalWrite(GATE_1, LOW);
      digitalWrite(GATE_2, LOW);
      LAMP_MODE = 0;
    }
  }
  Serial.println(LAMP_MODE);
}

// LCD Functions

void lcd_myprint(String str) {
  lcd.clear();
  lcd.home();
  if (str.length() < 16){
    lcd.print(str);
  } else {
    lcd.setCursor(0,0);
    lcd.print(str.substring(0, 15));
    lcd.setCursor(0,1);
    lcd.print(str.substring(15));
  }
}

void lcd_home(){
  lcd.clear();
  lcd.home();
  lcd_myprint("Witaj przy Rozlewaku!");
}

void display_volume(){
  String volume_string = String("Vol: " + String(VOLUME));
  Serial.println(volume_string);
  lcd_myprint(volume_string);
}

// Button Functions

void setup_buttons(ezButton buttons[]) {
  for (int i=0; i < 3; i++){
    String s = "DEBOUNCED BUTTON " + i;
    Serial.println(s);
    buttons[i].setDebounceTime(BUTTON_DEBOUNCE_MS);
  }
}

void init_limit_switches_loop(){
  for (int i=0; i < 3; i++){
    limit_switches[i].loop();
  }
}

void init_volume_buttons_loop(){
  for (int i=0; i < 3; i++){
    volume_buttons[i].loop();
  }
}

void start_pouring_procedure(){
  if (LAMP_MODE == 0){
    set_lamp_mode(1);
  } else {
    set_lamp_mode(0);
  }
}

void handle_volume_buttons() {
  for (int i=0; i < 3; i++){
    if (volume_buttons[i].isPressed()){
      Serial.println("pressed");
      if (i == 0) { // sub button
        if (VOLUME > 50){
          VOLUME -= 50;
          display_volume();
        }
      } else if (i == 2) { // add button
        VOLUME += 50;
        display_volume();
      } else if (i == 1) { // start button
        Serial.println("POURING");
        start_pouring_procedure();
      }
      Serial.println(VOLUME);
    }
  }
}

void handle_limit_switches() {
  for (int i=0; i < 3; i++){
    if (limit_switches[i].isPressed()){
      digitalWrite(LED_PIN, HIGH);
    } else if (limit_switches[i].isReleased()){
      digitalWrite(LED_PIN, LOW);
    }
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("***SERIAL INIT***");

  pinMode(LED_PIN, OUTPUT);

  pinMode(GATE_1, OUTPUT);
  pinMode(GATE_2, OUTPUT);
  digitalWrite(GATE_1, LOW);
  digitalWrite(GATE_2, LOW);


  setup_buttons(limit_switches);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd_home();
  delay(3000);
  display_volume();
}

void loop() {
  init_limit_switches_loop();
  init_volume_buttons_loop();
  handle_limit_switches();
  handle_volume_buttons();

}