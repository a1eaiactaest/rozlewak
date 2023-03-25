#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton limitSwitch1(7);

#define LCD_SDA A4
#define LCD_SCL A5

#define LED_PIN 13

/* *** LIMIT SWITCHES *** */

void init_limit_switches_loop() {
  // apparently this must be called
  limitSwitch1.loop();
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

  limitSwitch1.setDebounceTime(50); // miliseconds

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  init_limit_switches_loop();

  if (limitSwitch1.isPressed()) {
    digitalWrite(LED_PIN, HIGH);
  }

  if (limitSwitch1.isReleased()) {
    digitalWrite(LED_PIN, LOW);
  }
}