
//☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..☆.。.:*・°☆.。.:*・°☆.。.:*・°☆.。.:*・°☆*:..

#define DHTPIN 4       // Pin digital utilizado para la comunicación con el sensor DHT11
#define DHTTYPE DHT11   // Tipo de sensor DHT (DHT11 en este caso)

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

int BOTON_ARRIBA = 1;
int BOTON_ABAJO = 2;
int BOTON_SELECT = 3;

int pin_RELE = 8;

int ppm = 0.0;

volatile int f_wdt=1;

const int POTENTIOMETER_PIN = A0;

const float ANALOG_MIN = 0.0;     // Valor mínimo de lectura analógica
const float ANALOG_MAX = 1023.0;  // Valor máximo de lectura analógica
const float PPM_MIN = 0.0;        // Valor mínimo de ppm
const float PPM_MAX = 1000.0;     // Valor máximo de ppm

unsigned long debounceDelay = 100;  // Retardo de debounce en milisegundos
unsigned long lastDebounceTime = 0; // Último momento en que se detectó un cambio en el estado del botón SELECT
bool buttonState = HIGH;            // Estado actual del botón SELECT
bool lastButtonState = HIGH;        // Estado previo del botón SELECT

int menu = 0;
bool menuVisible = false;
bool relayManualOn = false;

//════════════════════════════════════════════════════════════════════════════════════════════════════════════

void setup() {

  Serial.begin(9600); 

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

  int potValue = analogRead(POTENTIOMETER_PIN);
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
      lcd.print("> OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("  ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("> OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("  ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  OPCION 2");
      lcd.setCursor(0, 2);
      lcd.print("> ACTIVAR RELE");
      lcd.setCursor(0, 3);
      lcd.print("  Volver");
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("  OPCION 1");
      lcd.setCursor(0, 1);
      lcd.print("  OPCION 2");
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
  lcd.print("> Ejecutando #1");

  File dataFile = SD.open("lecturas.txt", FILE_WRITE); // Abre el archivo para escritura
  
  if (dataFile) {
    unsigned long startTime = millis();
    unsigned long duration = 10000; // Duración de 10 segundos para tomar las lecturas
    
    while (millis() - startTime < duration) {
      float temperatura = dht.readTemperature();
      float humedad = dht.readHumidity();
      
      // Escribir los datos en la tarjeta microSD
      dataFile.print("Temperatura: ");
      dataFile.print(temperatura);
      dataFile.print("C, Humedad: ");
      dataFile.print(humedad);
      dataFile.println("%");
      dataFile.print("Ppm: ");
      dataFile.print(ppm);
      

      // Mostrar los datos en el Monitor Serie
      Serial.print("Temperatura: ");
      Serial.print(temperatura);
      Serial.print("C, Humedad: ");
      Serial.print(humedad);
      Serial.print("%");
      Serial.print(", Ppm: ");
      Serial.println(ppm);

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

// Watchdog Interrupt Service. This is executed when watchdog timed out.
ISR(WDT_vect) {
  if(f_wdt == 0) {
    // here we can implement a counter the can set the f_wdt to true if
    // the watchdog cycle needs to run longer than the maximum of eight
    // seconds.
    f_wdt=1;
  }
}

// Enters the arduino into sleep mode.
void enterSleep(void)
{
  // There are five different sleep modes in order of power saving:
  // SLEEP_MODE_IDLE - the lowest power saving mode
  // SLEEP_MODE_ADC
  // SLEEP_MODE_PWR_SAVE
  // SLEEP_MODE_STANDBY
  // SLEEP_MODE_PWR_DOWN - the highest power saving mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Now enter sleep mode.
  sleep_mode();

  // The program will continue from here after the WDT timeout

  // First thing to do is disable sleep.
  sleep_disable();

  // Re-enable the peripherals.
  power_all_enable();
}

// Setup the Watch Dog Timer (WDT)
void setupWatchDogTimer() {
  // The MCU Status Register (MCUSR) is used to tell the cause of the last
  // reset, such as brown-out reset, watchdog reset, etc.
  // NOTE: for security reasons, there is a timed sequence for clearing the
  // WDE and changing the time-out configuration. If you don't use this
  // sequence properly, you'll get unexpected results.

  // Clear the reset flag on the MCUSR, the WDRF bit (bit 3).
  MCUSR &= ~(1<<WDRF);

  // Configure the Watchdog timer Control Register (WDTCSR)
  // The WDTCSR is used for configuring the time-out, mode of operation, etc

  // In order to change WDE or the pre-scaler, we need to set WDCE (This will
  // allow updates for 4 clock cycles).

  // Set the WDCE bit (bit 4) and the WDE bit (bit 3) of the WDTCSR. The WDCE
  // bit must be set in order to change WDE or the watchdog pre-scalers.
  // Setting the WDCE bit will allow updates to the pre-scalers and WDE for 4
  // clock cycles then it will be reset by hardware.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /**
   *  Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
   *  WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
   *  0    0    0    0    |   2K cycles   | 16 ms
   *  0    0    0    1    |   4K cycles   | 32 ms
   *  0    0    1    0    |   8K cycles   | 64 ms
   *  0    0    1    1    |  16K cycles   | 0.125 s
   *  0    1    0    0    |  32K cycles   | 0.25 s
   *  0    1    0    1    |  64K cycles   | 0.5 s
   *  0    1    1    0    |  128K cycles  | 1.0 s
   *  0    1    1    1    |  256K cycles  | 2.0 s
   *  1    0    0    0    |  512K cycles  | 4.0 s
   *  1    0    0    1    | 1024K cycles  | 8.0 s
  */
  WDTCSR  = (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
  // Enable the WD interrupt (note: no reset).
  WDTCSR |= _BV(WDIE);
}

// Setup for the serial comms and the other things
void setup() {
  Serial.begin(9600);
  Serial.println("Initialising..."); delay(100);
  pinMode(LED_PIN, OUTPUT);
  setupWatchDogTimer();
  Serial.println("Initialisation complete."); delay(100);
}

// main loop
void loop() {
  // Wait until the watchdog have triggered a wake up.
  if(f_wdt != 1) {
    return;
  }

  // clear the flag so we can run above code again after the MCU wake up
  f_wdt = 0;

  // Re-enter sleep mode.
  enterSleep();

}
  
  menuVisible = true;  // Establece menuVisible en true para volver al menú
  ACTUALIZAR_PANTALLA();
}

//════════════════════════════════════════════════════════════════════════════════════════════════════════════

void OPCION_3() {
  lcd.clear();
  lcd.print("> Ejecutando #3");

  if (getAirQuality(ppm) == "Moderate") {
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
