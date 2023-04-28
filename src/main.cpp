#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(7);
ezButton limitSwitch2(6);
ezButton limitSwitch3(5);

ezButton volButton1(8); // sub vol
ezButton volButton2(9); // start
ezButton volButton3(10); // add vol

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 13

#define BUTTON_DEBOUNCE_MS 5


ezButton limit_switches[] = {limitSwitch1, limitSwitch2, limitSwitch3};
ezButton volume_buttons[] = {volButton1, volButton2, volButton3};

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

void handle_volume_buttons() {
  for (int i=0; i < 3; i++){
    if (limit_switches[i].isPressed()){

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

/* *** LCD FUNCTIONS *** */
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

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd_home();


  Serial.begin(9600);
  Serial.println("***SERIAL INIT***");

  // tmp test LED
  pinMode(LED_PIN, OUTPUT);

  setup_buttons(limit_switches);
}

void loop() {
  init_limit_switches_loop();
  handle_limit_switches();
}