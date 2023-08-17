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
int relayPin = 8;
float ppm = 0.0;
int menu = 0;
bool menuVisible = false;
bool relayManualOn = false;

const int POTENTIOMETER_PIN = A0;

const float ANALOG_MIN = 0.0;     // Valor mínimo de lectura analógica
const float ANALOG_MAX = 1023.0;  // Valor máximo de lectura analógica
const float PPM_MIN = 0.0;        // Valor mínimo de ppm
const float PPM_MAX = 1000.0;     // Valor máximo de ppm

void setup() {

  lcd.init();
  lcd.backlight();
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT); // Configura el pin del relé como salida
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

  if (!menuVisible) {
    showMainScreen();
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


  // Leer MQ135 !!!1

  lcd.setCursor(0, 3);
  lcd.print("Air Quality:");

  int potValue = analogRead(POTENTIOMETER_PIN);
  float voltage = potValue * (5.0 / 1023.0);

  ppm = mapFloat(potValue, ANALOG_MIN, ANALOG_MAX, PPM_MIN, PPM_MAX);

  lcd.setCursor(12, 3);
  lcd.print(getAirQuality(ppm));

  // Control del relé según la calidad del aire
  if (getAirQuality(ppm) == "Poor    ") {
    digitalWrite(relayPin, HIGH);  // Activa el relé
  } else if (getAirQuality(ppm) == "Moderate" && !relayManualOn) {
    digitalWrite(relayPin, LOW);   // Desactiva el relé
  }

  // Delay para dar tiempo a la visualización
  delay(500);
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

  if (getAirQuality(ppm) == "Moderate") {
    lcd.setCursor(0, 1);
    lcd.print("Relay Manual: ");
    lcd.setCursor(14, 1);

    if (relayManualOn) {
      lcd.print("ON ");
      digitalWrite(relayPin, HIGH);  // Activa el relé
    } else {
      lcd.print("OFF");
      digitalWrite(relayPin, LOW);   // Desactiva el relé
    }

    while (!digitalRead(selectButton)) {
      if (!digitalRead(selectButton)) {
        relayManualOn = !relayManualOn;  // Cambia el estado del relé manualmente
        lcd.setCursor(14, 1);

        if (relayManualOn) {
          lcd.print("ON ");
          digitalWrite(relayPin, HIGH);  // Activa el relé
        } else {
          lcd.print("OFF");
          digitalWrite(relayPin, LOW);   // Desactiva el relé
        }

        delay(100);
        while (!digitalRead(selectButton));
      }
    }
  }

  delay(1000);
}

float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

String getAirQuality(float ppm) {
  if (ppm <= 100) {
    return "Good    ";
  } else if (ppm <= 400) {
    return "Moderate";
  } else {
    return "Poor    ";
  }
}
