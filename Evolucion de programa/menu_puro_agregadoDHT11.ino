#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 9       // Pin digital utilizado para la comunicación con el sensor DHT11
#define DHTTYPE DHT11   // Tipo de sensor DHT (DHT11 en este caso)

LiquidCrystal_I2C lcd(0x20, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

int upButton = 10;
int downButton = 11;
int selectButton = 12;
int menu = 0;
bool menuVisible = false;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  dht.begin();  // Inicializar el sensor DHT11
  updateScreen();
}

void loop() {
  if (!digitalRead(downButton)) {
    if (menuVisible) {
      menu++;
      if (menu > 4) {
        menu = 4;
      }
      updateMenu();
    }
    delay(100);
    while (!digitalRead(downButton));
  }
  if (!digitalRead(upButton)) {
    if (menuVisible) {
      menu--;
      if (menu < 1) {
        menu = 1;
      }
      updateMenu();
    }
    delay(100);
    while (!digitalRead(upButton));
  }
  if (!digitalRead(selectButton)) {
    if (menuVisible) {
      executeAction();
    } else {
      menuVisible = true;
      menu = 1;
      updateScreen();
    }
    delay(100);
    while (!digitalRead(selectButton));
  }
}

void updateScreen() {
  lcd.clear();
  if (menuVisible) {
    updateMenu();
  } else {
    showMainScreen();
  }
}

void showMainScreen() {
  lcd.setCursor(0, 0);
  lcd.print("Pantalla Principal");

  // Leer la temperatura y humedad del sensor DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  lcd.setCursor(0, 1);
  char tempStr[10];
  dtostrf(temperature, 4, 1, tempStr);
  lcd.print(tempStr);
  lcd.print((char)223);
  lcd.print("C");
  
  lcd.setCursor(10, 1);
  lcd.print(humidity);
  lcd.print(" %");
}


void updateMenu() {
  lcd.clear();
  switch (menu) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("> OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("  OPCION 3");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("> OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("  OPCION 3");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("> OPCION 3");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("  OPCION 3");
      lcd.setCursor(0, 3);
      lcd.print("> Volver");
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
      menuVisible = false;
      updateScreen();
      break;
  }
}

void action1() {
  lcd.clear();
  lcd.print("> Ejecutando #1");
  delay(1000);
}

void action2() {
  lcd.clear();
  lcd.print("> Ejecutando #2");
  delay(1000);
}

void action3() {
  lcd.clear();
  lcd.print("> Ejecutando #3");
  delay(1000);
}
