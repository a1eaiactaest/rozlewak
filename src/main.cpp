#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(7);
ezButton limitSwitch2(6);
ezButton limitSwitch3(5);

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 13

#define BUTTON_DEBOUNCE_MS 100

/* *** LIMIT SWITCHES *** */
struct LIMIT_SWITCHES {
  ezButton limitSwitch1, limitSwitch2, limitSwitch3;
};

LIMIT_SWITCHES switches = {limitSwitch1, limitSwitch2, limitSwitch3};

void setup_limit_switches() {
  limitSwitch1.setDebounceTime(BUTTON_DEBOUNCE_MS);
  limitSwitch2.setDebounceTime(BUTTON_DEBOUNCE_MS);
  limitSwitch3.setDebounceTime(BUTTON_DEBOUNCE_MS);
}

void init_limit_switches_loop() {
  // apparently this must be called
  limitSwitch1.loop();
  limitSwitch2.loop();
  limitSwitch3.loop();
}

void handle_limit_switches() {
  init_limit_switches_loop();

  // switch 1 
  if (limitSwitch1.isPressed()) {
    digitalWrite(LED_PIN, HIGH);
  }

  if (limitSwitch1.isReleased()) {
    digitalWrite(LED_PIN, LOW);
  }

  // switch 2 
  if (limitSwitch2.isPressed()) {
    digitalWrite(LED_PIN, HIGH);
  }

  if (limitSwitch2.isReleased()) {
    digitalWrite(LED_PIN, LOW);
  }

  // switch 3 
  if (limitSwitch3.isPressed()) {
    digitalWrite(LED_PIN, HIGH);
  }

  if (limitSwitch3.isReleased()) {
    digitalWrite(LED_PIN, LOW);
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
}

void loop() {
  handle_limit_switches();
}