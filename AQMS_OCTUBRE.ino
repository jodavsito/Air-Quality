
//☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。
#include <Wire.h>                              //☆
#include <LiquidCrystal_I2C.h>                 //☆
#include <DHT.h>                               //☆
#include <SD.h>                                //☆
#include <SPI.h>                               //☆
#include <LowPower.h>                         //☆
//☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。

#define DHTPIN 4       // Pin digital utilizado para la comunicación con el sensor DHT11
#define DHTTYPE DHT11   // Tipo de sensor DHT (DHT11 en este caso)
#define interruptPin 0 //Pin que vamos a usar para despertar al Arduino

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

int BOTON_ARRIBA = 1;
int BOTON_ABAJO = 2;
int BOTON_SELECT = 3;

int pin_RELE = 5;

int ppm = 0.0;
const int MQ135_PIN = A0;

unsigned long debounceDelay = 50;  // Retardo de debounce en milisegundos
unsigned long lastDebounceTime = 0; // Último momento en que se detectó un cambio en el estado del botón SELECT
bool buttonState = HIGH;            // Estado actual del botón SELECT
bool lastButtonState = HIGH;        // Estado previo del botón SELECT

int menu = 0;
bool menuVisible = false;
bool relayManualOn = true;


const float ANALOG_MIN = 0.0;     // Valor mínimo de lectura analógica
const float ANALOG_MAX = 1023.0;  // Valor máximo de lectura analógica
const float PPM_MIN = 0.0;        // Valor mínimo de ppm
const float PPM_MAX = 1000.0;     // Valor máximo de ppm


//════════════════════════════════════════════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);//iniciar Serial comunicacion
  pinMode(LED_BUILTIN,OUTPUT); //usamos el led en el pin 13 para indicar cuando el arduino esta en modo sleep
  pinMode(interruptPin,INPUT_PULLUP);//Establecer el pin 0 a la entrada usando la resistencia pullup buildin
  digitalWrite(LED_BUILTIN,HIGH);//Prendiendo el LED b
  lcd.init();
  lcd.backlight();
  pinMode(BOTON_ARRIBA, INPUT_PULLUP);
  pinMode(BOTON_ABAJO, INPUT_PULLUP);
  pinMode(BOTON_SELECT, INPUT_PULLUP);
  pinMode(pin_RELE, OUTPUT);

  dht.begin();  // Inicializar el sensor DHT11
  SD.begin(10); 
  ACTUALIZAR_PANTALLA();
  
}

void loop() {
  
    if (!digitalRead(BOTON_ABAJO)) {
      if (menuVisible) {
        menu++;
        if (menu > 4) {
          menu = 4;
        }
        ACTUALIZAR_MENU();
      }
      delay(100);
      while (!digitalRead(BOTON_ABAJO));
    }
    if (!digitalRead(BOTON_ARRIBA)) {
      if (menuVisible) {
        menu--;
        if (menu < 1) {
          menu = 1;
        }
        ACTUALIZAR_MENU();
      }
      delay(100);
      while (!digitalRead(BOTON_ARRIBA));
    }
    if (botonSelectPresionado()) {
      if (menuVisible) {
        executeAction();
      } else {
        menuVisible = true;
        menu = 1;
        ACTUALIZAR_PANTALLA();
      }
      delay(100);
      while (botonSelectPresionado());
    }
    if (!menuVisible) {
      MOSTRAR_PANTALLA_INICIO();
    }
   
} 

void ACTUALIZAR_PANTALLA() {
  lcd.clear();
  if (menuVisible) {
    ACTUALIZAR_MENU();
  } else {
    MOSTRAR_PANTALLA_INICIO();
  }
}


bool botonSelectPresionado() {
  
  bool estadoActual = digitalRead(BOTON_SELECT);
  if (estadoActual != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (estadoActual != buttonState) {
      buttonState = estadoActual;
      if (buttonState == LOW) {
        lastButtonState = buttonState;
        return true;
      }
    }
  }
  lastButtonState = estadoActual;
  return false;
}

void MOSTRAR_PANTALLA_INICIO() {

  // Leer la temperatura y humedad del sensor DHT11
  
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  lcd.setCursor(0, 0);
  char tempStr[10];
  dtostrf(temperatura, 4, 1, tempStr);
  lcd.print(tempStr);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(10, 0);
  lcd.print(humedad);
  lcd.print(" %");


  lcd.setCursor(0, 3);
  lcd.print("Air Quality:");
  
  int potValue = analogRead(MQ135_PIN);
  float voltage = potValue * (5.0 / 1023.0);

  ppm = mapFloat(potValue, ANALOG_MIN, ANALOG_MAX, PPM_MIN, PPM_MAX);

  lcd.setCursor(12, 3);
  lcd.print(getAirQuality(ppm));

  // Control del relé según la calidad del aire
  if (getAirQuality(ppm) == "Poor    ") {
    digitalWrite(pin_RELE, LOW);  // Activa el relé
  } else if (getAirQuality(ppm) == "Moderate" && !relayManualOn) {
    digitalWrite(pin_RELE, HIGH);
  }
}



void ACTUALIZAR_MENU() {
  lcd.clear();
  switch (menu) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("> DESCARGAR DATOS");
      lcd.setCursor(0, 1);
      lcd.print("  MODO SLEEP: PRONTO");
      lcd.setCursor(0, 2);
      lcd.print("  ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("  DESCARGAR DATOS");
      lcd.setCursor(0, 1);
      lcd.print("> MODO SLEEP: PRONTO");
      lcd.setCursor(0, 2);
      lcd.print("  ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  MODO SLEEP: PRONTO");
      lcd.setCursor(0, 2);
      lcd.print("> ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("  DESCARGAR DATOS");
      lcd.setCursor(0, 1);
      lcd.print("  MODO SLEEP: PRONTO");
      lcd.setCursor(0, 2);
      lcd.print("  ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("> Volver");
      break;
  }
}

void executeAction() {
  switch (menu) {
    case 1:
      OPCION_1();
      break;
    case 2:
      OPCION_2();
      break;
    case 3:
      OPCION_3();
      break;
    case 4:
      menuVisible = false;
      ACTUALIZAR_PANTALLA();
      break;
  }
}

void OPCION_1() {
  lcd.clear();
  lcd.print("> DESCARGANDO DATOS");

  File dataFile = SD.open("lecturas.txt", FILE_WRITE); // Abre el archivo para escritura

  if (dataFile) {
    unsigned long startTime = millis();
    unsigned long duration = 10000; // Duración de 10 segundos para tomar las lecturas
    int lecturaCounter = 1; // Inicializa el contador de lecturas

    while (millis() - startTime < duration) {
      float temperatura = dht.readTemperature();
      float humedad = dht.readHumidity();

      // Escribir los datos en la tarjeta microSD
      dataFile.print("Lectura ");
      dataFile.print(lecturaCounter);
      dataFile.println(":");
      dataFile.print("Temperatura: ");
      dataFile.print(temperatura);
      dataFile.print("C, Humedad: ");
      dataFile.print(humedad);
      dataFile.println("%");
      dataFile.print("Ppm: ");
      dataFile.print(ppm);
      dataFile.println();

      // Mostrar los datos en el Monitor Serie
      Serial.print("Lectura ");
      Serial.print(lecturaCounter);
      Serial.println(":");
      Serial.print("Temperatura: ");
      Serial.print(temperatura);
      Serial.print("C, Humedad: ");
      Serial.print(humedad);
      Serial.print("%");
      Serial.print(", Ppm: ");
      Serial.println(ppm);

      lecturaCounter++; // Incrementa el contador de lecturas
      delay(1000); // Intervalo de 1 segundo entre cada lectura
    }

    dataFile.close(); // Cierra el archivo después de completar las lecturas
  }

  lcd.clear();
  lcd.print("Datos guardados");
  delay(1000);
  menuVisible = true;  // Establece menuVisible en true para volver al menú
  ACTUALIZAR_PANTALLA();
}


//════════════════════════════════════════════════════════════════════════════════════════════════════════════

void OPCION_2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("> Modo Sleep");
  lcd.setCursor(0, 1);
  lcd.print("Presiona SELECT");
  lcd.setCursor(0, 2);
  lcd.print("para despertar");

  // Espera hasta que se presione el botón SELECT para despertar
  while (digitalRead(BOTON_SELECT)) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  // Mostrar mensaje de despertar
  lcd.clear();
  lcd.print("¡Desperté!");
  delay(1000);
  lcd.clear();
}

//════════════════════════════════════════════════════════════════════════════════════════════════════════════

void OPCION_3() {
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Relay Manual: ");
  lcd.setCursor(14, 1);

  if (relayManualOn) {
    lcd.print("ON ");
    digitalWrite(pin_RELE, LOW);  // Activa el relé
  } else {
    lcd.print("OFF");
    digitalWrite(pin_RELE, HIGH);   // Desactiva el relé
  }

  while (!digitalRead(BOTON_SELECT)) {
    if (!digitalRead(BOTON_SELECT)) {
      relayManualOn = !relayManualOn;  // Cambia el estado del relé manualmente
      lcd.setCursor(14, 1);

      if (relayManualOn) {
        lcd.print("ON ");
        digitalWrite(pin_RELE, LOW);  // Activa el relé
      } else {
        lcd.print("OFF");
        digitalWrite(pin_RELE, HIGH);   // Desactiva el relé
      }

      delay(100);
      while (!digitalRead(BOTON_SELECT));
    }
  }

  delay(1000);
}
//════════════════════════════════════════════════════════════════════════════════════════════════════════════

float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

String getAirQuality(float ppm) {
  if (ppm <= 100) {
    return "Good    ";
  } else if (ppm <= 300) {
    return "Moderate";
  } else {
    return "Poor    ";
  }
}
