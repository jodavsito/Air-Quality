#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 9  // Pin digital utilizado para el sensor DHT
#define DHTTYPE DHT11  // Tipo de sensor utilizado (DHT11 o DHT22)

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x20, 20, 4);

int upButton = 10;
int downButton = 11;
int selectButton = 12;
int menu = 1;
bool menuShown = false;
bool showTempHum = true; 
float temperature = 0;
float humidity = 0;


void setup() {
  
  lcd.init();
  lcd.backlight();
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  lcd.print("Pantalla Principal");
  dht.begin();
  
}

void loop() {

  if (!menuShown) {

    if (showTempHum) {
    float t = dht.readTemperature();
    if (!isnan(t)) {
      temperature = t;
    }
    float h = dht.readHumidity();
    if (!isnan(h)) {
      humidity = h;
    }
    lcd.setCursor(0, 1);
    char tempStr[10];
    dtostrf(temperature, 4, 1, tempStr);
    lcd.print(tempStr);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0, 2);
    lcd.print(humidity);
    lcd.print(" %");
  }
         
    lcd.setCursor(0, 1);
    if (!digitalRead(selectButton)) {
      lcd.clear();
      updateMenu();
      menuShown = true;
      delay(100);
    }
  }
  else {
    if (!digitalRead(downButton)){
      menu++;
      updateMenu();
      delay(100);
      while (!digitalRead(downButton));
    }
    if (!digitalRead(upButton)){
      menu--;
      updateMenu();
      delay(100);
      while(!digitalRead(upButton));
    }
    if (!digitalRead(selectButton)){
      executeAction();
      updateMenu();
      showTempHum = true; 
      delay(100);
      while (!digitalRead(selectButton));
    }
  }
}

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(">MenuItem1");
      lcd.setCursor(0, 1);
      lcd.print(" MenuItem2");
      lcd.setCursor(0, 2);
      lcd.print(" MenuItem3");
      lcd.setCursor(0, 3);
      lcd.print(" Salir del menu");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" MenuItem1");
      lcd.setCursor(0, 1);
      lcd.print(">MenuItem2");
      lcd.setCursor(0, 2);
      lcd.print(" MenuItem3");
      lcd.setCursor(0, 3);
      lcd.print(" Salir del menu");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" MenuItem1");
      lcd.setCursor(0, 1);
      lcd.print(" MenuItem2");
      lcd.setCursor(0, 2);
      lcd.print(">MenuItem3");
      lcd.setCursor(0, 3);
      lcd.print(" Salir del menu");
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" MenuItem1");
      lcd.setCursor(0, 1);
      lcd.print(" MenuItem2");
      lcd.setCursor(0, 2);
      lcd.print(" MenuItem3");
      lcd.setCursor(0, 3);
      lcd.print(">Salir del menu");
      break;
    case 5:
      menu = 4;
      break;
  }
}

void executeAction() {
  switch (menu) {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
    case 3:
      action3();
      break;
    case 4:
      backToMain();
      break;
  }
}

void action1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">Executing #1");
  delay(1500);
}

void action2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">Executing #2");
  delay(1500);
}

void action3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">Executing #3");
  delay(1500);
}

void backToMain() {
  menuShown = false;
  lcd.clear();
  delay(100);
  updateMenu();
  lcd.print("Pantalla Principal");
  showTempHum = true;
}
