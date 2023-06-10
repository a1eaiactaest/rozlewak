#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>
#include <Servo.h>
#include <MFRC522.h>

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 2

// L298N
#define L289N_IN1 2
#define L289N_IN2 3

#define L289N_IN4 5 // use

#define SERVO_PIN 8

#define BUTTON_DEBOUNCE_MS 5

int angles[] = {60, 90, 120};

Servo myservo;

int servo_pos = 0;

// check back of L2C chip for address
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(A7);
ezButton limitSwitch2(A6);
ezButton limitSwitch3(A5);

int limitSwitchesState[] = {0,0,0};

ezButton volButton1(A4); // sub vol
ezButton volButton2(A3); // start
ezButton volButton3(A2); // add vol


ezButton limit_switches[] = {limitSwitch1, limitSwitch2, limitSwitch3};
ezButton volume_buttons[] = {volButton1, volButton2, volButton3};

int VOLUME = 50; // ml
int LAMP_MODE = 0; // default off
int PUMP_MODE = 0; // default off

int **zip(int *arr1, int *arr2, int length){
  int **ret = new int*[length];
  for (int i = 0; i < length; i++){
    ret[i] = new int[2];
    ret[i][0] = arr1[i];
    ret[i][1] = arr2[i];
  }
  return ret;
}

void set_pump_mode(int mode){
  if (mode != PUMP_MODE){
    if (mode == 1){
      digitalWrite(L289N_IN4, HIGH);
      PUMP_MODE = 1;
    } else if (mode == 0){
      digitalWrite(L289N_IN4, LOW);
      PUMP_MODE = 0;
    }
  }
}

// LAMP
void set_lamp_mode(int mode){
  /*
    0 turn off
    1 turn on
  */

  if (mode != LAMP_MODE){
    if (mode == 1){
      digitalWrite(L289N_IN1, HIGH);
      digitalWrite(L289N_IN2, LOW);
      LAMP_MODE = 1;
    } else if (mode == 0){
      digitalWrite(L289N_IN1, LOW);
      digitalWrite(L289N_IN2, LOW);
      LAMP_MODE = 0;
    }
  }
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


void reset_servo(){
  myservo.write(0);
}

void setup_servo(){
  myservo.attach(SERVO_PIN);
  myservo.writeMicroseconds(1500);
  reset_servo();
}

void map_positions(int *modify){
  for (int i = 0; i < 3; i++){
    Serial.print(limitSwitchesState[i]);
    if (limitSwitchesState[i] != 0){
      modify[i] = angles[i];
    }
  }
  Serial.println();
}

void pour_for(int milis){
  set_pump_mode(1);
  delay(milis);
  set_pump_mode(0);
}

void pour_volume(int volume){
  // 50ml * 10s 
  int time = volume * 10;
  pour_for(time);
}

void set_servo_positions(){
  int acc = 0;
  for (int i = 0; i < 3; i++){
    if (limitSwitchesState[i] == 1){
      acc += 1;
    }
  }

  int positions[acc];

  map_positions(positions);

  Serial.println("START OF POSITIONS");
  for (int i = 0; i < acc; i++){
    Serial.println(positions[i]);
    myservo.write(positions[i]);
    delay(1000);
    // ****** POUR HERE ******
    pour_volume(VOLUME);
  }
  Serial.println("END OF POSITIONS");
  set_lamp_mode(0);
}

void start_pouring_procedure(){
  if (LAMP_MODE == 0){
    set_lamp_mode(1);
  } else {
    set_lamp_mode(0);
  }
  set_servo_positions();
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
    }
  }
}

void handle_limit_switches() {
  for (int i=0; i < 3; i++){
    if (limit_switches[i].isPressed()){
      limitSwitchesState[i] = 1;
      digitalWrite(LED_PIN, HIGH);
    } else if (limit_switches[i].isReleased()){
      digitalWrite(LED_PIN, LOW);
      limitSwitchesState[i] = 0;
    }
  }
  /*
  for (int i=0; i < 3; i++){
    Serial.print(limitSwitchesState[i]);
  }
  Serial.println();
  */
}



void setup() {
  Serial.begin(9600);
  Serial.println("***SERIAL INIT***");

  pinMode(LED_PIN, OUTPUT);

  pinMode(L289N_IN1, OUTPUT);
  pinMode(L289N_IN2, OUTPUT);
  pinMode(L289N_IN4, OUTPUT);
  digitalWrite(L289N_IN1, LOW);
  digitalWrite(L289N_IN2, LOW);
  digitalWrite(L289N_IN4, LOW);


  setup_buttons(limit_switches);
  setup_servo();

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