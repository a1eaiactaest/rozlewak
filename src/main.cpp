#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(7);
ezButton limitSwitch2(6);
ezButton limitSwitch3(5);

ezButton volButton1(8);
ezButton volButton2(9);
ezButton volButton3(10);

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 13

#define BUTTON_DEBOUNCE_MS 100

struct BUTTONS {
  ezButton button1, button2, button3;
};

ezButton get_struct_buttons(BUTTONS s, int i) {
  switch(i) {
    case 0: return s.button1;
    case 1: return s.button2;
    case 2: return s.button3;
    default: return -1;
  }
}

/* *** VOLUME BUTTONS *** */
BUTTONS volume_buttons = {volButton1, volButton2, volButton3};

BUTTONS limit_switches = {limitSwitch1, limitSwitch2, limitSwitch3};

void setup_buttons(BUTTONS buttons) {
  for (int i=0; i < 3; i++){
    ezButton cbutton = get_struct_buttons(buttons, i);
    cbutton.setDebounceTime(BUTTON_DEBOUNCE_MS);
  }
}

void init_buttons_loop(BUTTONS buttons) {
  for (int i=0; i < 3; i++){
    Serial.println("INIT");
    ezButton cbutton = get_struct_buttons(buttons, i);
    cbutton.loop();
  }
}

void handle_volume_buttons() {

}

void handle_limit_switches() {
  init_buttons_loop(limit_switches);

  for (int i = 0; i < 3; i++){
    ezButton cbutton = get_struct_buttons(limit_switches, i);
    if (cbutton.isPressed()){
      digitalWrite(LED_PIN, HIGH);
    } else if (cbutton.isReleased()) {
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
  handle_limit_switches();
}