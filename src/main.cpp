#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 13

// L298N
#define GATE_1 3
#define GATE_2 4

#define BUTTON_DEBOUNCE_MS 5

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(7);
ezButton limitSwitch2(6);
ezButton limitSwitch3(5);

ezButton volButton1(8); // sub vol
ezButton volButton2(9); // start
ezButton volButton3(10); // add vol


ezButton limit_switches[] = {limitSwitch1, limitSwitch2, limitSwitch3};
ezButton volume_buttons[] = {volButton1, volButton2, volButton3};

int VOLUME = 50; // ml

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
      } else if (i == 3) { // start button
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