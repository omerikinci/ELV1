/*
 * Arduino Electronic Load Project (FINAL VERSION - 20x4 EDITION)
 * -----------------------------------------------------------
 * - OPTIMIZED FOR 20x4 DISPLAY (Professional Look) -
 * - ENGLISH INTERFACE FOR YOUTUBE / GLOBAL USE -
 * - HIGH VOLTAGE PROTECTION & MAX 20V SET LIMIT -
 * - FIXED: Display Flickering (Titreme) and Overwrite Issues -
 * - PREMIUM FEATURES: Fan Animation, Progress Bar, Haptic Sound -
 * -----------------------------------------------------------
 * * * PIN CONNECTIONS:
 * A0: Current Sense (1V = 1A)
 * A1: Voltage Sense (Max 50V)
 * A2: Set Current Pot (Max 5A)
 * A3: Set Voltage Pot (Max 20V)
 * D8: Load Control MOSFET (Safety Cutoff - HIGH triggers OFF)
 * D2: Reset Button (Active LOW)
 * D5: Buzzer
 * D9: DS18B20 One-Wire Bus
 * D11: Fan MOSFET (Low-Side Switching)
 * SDA/SCL: 20x4 I2C LCD Display
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- LCD Settings (UPDATED FOR 20x4) ---
LiquidCrystal_I2C lcd(0x27, 20, 4);  // 20 Columns, 4 Rows

// --- Pin Definitions ---
const int CURRENT_PIN = A0;
const int VOLTAGE_PIN = A1;
const int SET_CURRENT_PIN = A2;
const int SET_VOLTAGE_PIN = A3;
const int INDICATOR_POT_PIN = A7;

const int RESET_BUTTON_PIN = 2;
const int BUZZER_PIN = 5;
const int LOAD_CONTROL_PIN = 8;    // D8 - Safety Cutoff MOSFET
const int ONE_WIRE_BUS_PIN = 9;    // D9 - DS18B20 Sensors
const int FAN_MOSFET_PIN = 11;     // D11 - Fan MOSFET

// --- Control Logic ---
const int LOAD_ON = LOW;       // Normal Operation (D8 LOW)
const int LOAD_OFF = HIGH;     // Fault State (D8 HIGH)
const int FAN_ON = HIGH;
const int FAN_OFF = LOW;

// --- Calibration & Settings ---
const float V_REF = 5.0;  
const float VOLTAGE_DIVIDER_RATIO = 11.0;  
const float MIN_CURRENT_THRESHOLD = 0.05;  

// --- Thermal Control Settings ---
const float FAN_ON_TEMP_THRESHOLD = 55.0;
const float FAN_OFF_TEMP_THRESHOLD = 45.0;
const float TEMP_FAULT_LIMIT = 65.0;  

// --- PROTECTION LIMITS ---
const float HIGH_VOLTAGE_PROTECTION = 21.0;  

// --- Global Variables ---
float actualVoltage = 0.0;
float actualCurrent = 0.0;
float setVoltage = 0.0;
float setCurrent = 0.0;
float power = 0.0;
int indicatorPotValue = 0;

bool faultState = false;
String faultMessage = "";

// --- Temp & Fan Variables ---
OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature sensors(&oneWire);
float tempS2 = 0.0; // S2 (MOSFET Temp)
float tempS1 = 0.0; // S1 (Resistor Temp)
bool isFanOn = false;

// Sabit etiketleri sadece bir kez yazdırmak için bayraklar
bool labelsPrinted = false;

// --- Timing Variables for Async Temp Reading ---
unsigned long lastTempUpdate = 0;
const unsigned long TEMP_UPDATE_INTERVAL = 1000; // 1 saniyede bir sıcaklık oku

// --- Custom Characters & Animation ---
byte charFan1[8] = {0x04,0x0E,0x1F,0x04,0x1F,0x0E,0x04,0x00}; // Pervane +
byte charFan2[8] = {0x1B,0x0E,0x04,0x0E,0x1B,0x00,0x00,0x00}; // Pervane x
byte charBar[8]  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // Full Block

unsigned long lastFanAnimTime = 0;
const int FAN_ANIM_DELAY = 250; // 250ms de bir yön değişir
int fanFrame = 0; // 0 veya 1


void setup() {
  Serial.begin(9600);

  // Set Pin Modes
  pinMode(LOAD_CONTROL_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_MOSFET_PIN, OUTPUT);
  
  // Initial States
  digitalWrite(LOAD_CONTROL_PIN, LOAD_ON);    // Load ON
  digitalWrite(BUZZER_PIN, LOW);             // Buzzer OFF
  digitalWrite(FAN_MOSFET_PIN, FAN_OFF);     // Fan OFF
  
  // Init LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Create Custom Characters
  lcd.createChar(0, charFan1);
  lcd.createChar(1, charFan2);
  lcd.createChar(2, charBar); // Progress Bar Block

  // --- SHOW LOADING SCREEN ---
  lcd.setCursor(2, 1);
  lcd.print("SYSTEM STARTING");
  
  // Progress Bar Animation (20 Karakterlik Çubuk)
  lcd.setCursor(0, 2);
  for (int i = 0; i < 20; i++) {
    lcd.write(2); // Block karakterini yaz
    delay(75);    // Hız ayarı (Toplam ~1.5sn sürer)
  }
  
  lcd.clear();
  
  // --- WELCOME SCREEN (Short) ---
  lcd.setCursor(2, 0);  
  lcd.print("ELECTRONIC LOAD");
  lcd.setCursor(4, 1);
  lcd.print("Version 2.0"); // V2.0 PRO
  delay(1500);
  
  // Init Sensors
  sensors.begin();
  
  // ASYNC MODE: İşlemciyi bekletme
  sensors.setWaitForConversion(false); 
  sensors.requestTemperatures(); // İlk çevrimi başlat
  
  delay(100); 

  faultState = false;
  lcd.clear();
  // İlk çalıştırmada sabit etiketleri yazdır
  printFixedLabels(); 
}

/**
 * @brief Ekrana sabit etiketleri (Set:, Act:, Pwr:, Mos:, Res:, Pot:) sadece bir kez yazar.
 * Bu, ana döngüde (loop) tekrarlanan yazımı ve titremeyi önler.
 */
void printFixedLabels() {
  // Line 1: Set Values Labels
  lcd.setCursor(0, 0);
  lcd.print("Set: ");
  // Line 2: Actual Values Labels
  lcd.setCursor(0, 1);
  lcd.print("Act: ");
  // Line 3: Power & Pot Labels
  lcd.setCursor(0, 2);
  lcd.print("Pwr: "); // POWER -> PWR KISALTILDI
  lcd.setCursor(11, 2); // POT Sola hizalandı
  lcd.print("Pot:"); 
  // Line 4: Temps Labels
  lcd.setCursor(0, 3);
  lcd.print("Mos:");
  lcd.setCursor(8, 3); // Res Sola hizalandı
  lcd.print("Res:");
  
  labelsPrinted = true;
}


void loop() {
  
  // 1. Check Reset Button
  if (faultState == true) {
    if (digitalRead(RESET_BUTTON_PIN) == LOW) {  
      delay(50); // Debounce
      if (digitalRead(RESET_BUTTON_PIN) == LOW) {
        // Reset Faults
        tone(BUZZER_PIN, 2000, 100); // Bip sesi ile onay
        faultState = false;
        faultMessage = "";
        digitalWrite(LOAD_CONTROL_PIN, LOAD_ON);  
        digitalWrite(BUZZER_PIN, LOW);            
        lcd.clear();
        lcd.setCursor(3, 1);
        lcd.print("SYSTEM ACTIVE");
        delay(1000);
        lcd.clear();
        // Reset sonrası sabit etiketleri yeniden yazdır
        printFixedLabels(); 
        return;
      }
    }
  }

  // 2. Read Sensors
  readSensors();
  readTemperatures();

  // 3. Fan Control
  controlFan();

  // 4. Fault Check
  if (faultState == false) {
    
    // *** HIGH VOLTAGE PROTECTION (>21V) ***
    if (actualVoltage > HIGH_VOLTAGE_PROTECTION) {  
      setFault("FAULT: HIGH VOLTAGE");
    }
    // Over Current
    else if (actualCurrent > setCurrent) {
      setFault("FAULT: OVER CURRENT");
    }  
    // Low Voltage
    else if (actualVoltage < setVoltage && actualCurrent > MIN_CURRENT_THRESHOLD) {
      setFault("FAULT: LOW VOLTAGE");
    }
    // Over Temp
    else if (tempS1 > TEMP_FAULT_LIMIT || tempS2 > TEMP_FAULT_LIMIT) {
      setFault("FAULT: OVER HEATING");
    }
  }

  // 5. Update Display
  if (faultState) {
    displayFault();
  } else {
    // Sabit etiketler (Set:, Act:, Pwr: vb.) zaten yazıldıysa, sadece dinamik değerleri güncelle
    updateDynamicValues();
  }
}

/**
 * @brief Triggers a system fault.
 */
void setFault(String message) {
  faultState = true;
  faultMessage = message;
  
  digitalWrite(LOAD_CONTROL_PIN, LOAD_OFF); // Cut Power
  digitalWrite(BUZZER_PIN, HIGH);
  lcd.clear(); // Hata durumunda ekranı temizle (Mos vb. kalmasın)
}

/**
 * @brief Reads all analog inputs.
 */
void readSensors() {
  // Set Current (Max 5A)
  int setAmperRaw = analogRead(SET_CURRENT_PIN);
  setCurrent = map(setAmperRaw, 0, 1023, 0, 500) / 100.0;

  // Set Voltage (Max 20V Limit)
  int setVoltajRaw = analogRead(SET_VOLTAGE_PIN);
  setVoltage = map(setVoltajRaw, 0, 1023, 0, 2000) / 100.0;  

  // Actual Current
  int actualCurrentRaw = analogRead(CURRENT_PIN);
  float actualCurrentVolts = (actualCurrentRaw / 1023.0) * V_REF;
  actualCurrent = actualCurrentVolts;  

  // Actual Voltage
  int actualVoltajRaw = analogRead(VOLTAGE_PIN);
  float pinVoltage = (actualVoltajRaw / 1023.0) * V_REF;
  actualVoltage = pinVoltage * VOLTAGE_DIVIDER_RATIO;
  
  power = actualVoltage * actualCurrent;
  indicatorPotValue = analogRead(INDICATOR_POT_PIN);
}

void readTemperatures() {
  // Sadece 1 saniyede bir işlem yap (Non-blocking)
  if (millis() - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
    lastTempUpdate = millis();
    
    // 1. Sensörde HAZIR olan değeri oku (Bir önceki istekten gelen)
    float temp1 = sensors.getTempCByIndex(1);
    float temp0 = sensors.getTempCByIndex(0);

    if (temp0 != DEVICE_DISCONNECTED_C && temp0 > -127.0) {
      tempS2 = temp0; // Index 0 -> S2 (Resistor)
    }
    if (temp1 != DEVICE_DISCONNECTED_C && temp1 > -127.0) {
      tempS1 = temp1; // Index 1 -> S1 (MOSFET)
    }
    
    // 2. Bir SONRAKİ okuma için sensöre "Çevrime Başla" emri ver
    // WaitForConversion(false) olduğu için işlemciyi bekletmez, hemen döner.
    sensors.requestTemperatures();
  }
}

/**
 * @brief Controls Fan based on temp.
 */
void controlFan() {
  if (tempS1 > FAN_ON_TEMP_THRESHOLD || tempS2 > FAN_ON_TEMP_THRESHOLD) {
    isFanOn = true;
  }  
  else if (tempS1 < FAN_OFF_TEMP_THRESHOLD && tempS2 < FAN_OFF_TEMP_THRESHOLD) {
    isFanOn = false;
  }
  
  if (isFanOn) {
    digitalWrite(FAN_MOSFET_PIN, FAN_ON);  
  } else {
    digitalWrite(FAN_MOSFET_PIN, FAN_OFF);  
  }
}

/**
 * @brief Sadece dinamik değerleri günceller, sabit etiketlere dokunmaz. (20x4 Optimized)
 */
void updateDynamicValues() {
  // Line 1: Set Values
  // Başlangıç: Kolon 5 (Set: 01234)
  lcd.setCursor(5, 0); 
  lcd.print(String(setVoltage, 1));
  lcd.print("V "); // Sabit uzunluk koruma (Tek boşluk)
  // Başlangıç: Kolon 11
  lcd.setCursor(11, 0); 
  lcd.print(String(setCurrent, 2));
  lcd.print("A "); // Sabit uzunluk koruma (Tek boşluk)

  // Line 2: Actual Values
  // Başlangıç: Kolon 5 (Act: 01234)
  lcd.setCursor(5, 1);
  lcd.print(String(actualVoltage, 1));
  lcd.print("V "); // Sabit uzunluk koruma
  // Başlangıç: Kolon 11
  lcd.setCursor(11, 1);
  lcd.print(String(actualCurrent, 2));
  lcd.print("A "); // Sabit uzunluk koruma

  // Line 3: Power & Pot
  // Başlangıç: Kolon 5 (Pwr: 01234)
  lcd.setCursor(5, 2);
  lcd.print(String(power, 1));
  lcd.print("W "); // Sabit uzunluk koruma - Tek boşluk (P harfini korur)
  
  // Pot değerini güncelle. Başlangıç: Kolon 15 (Pot: 12345)
  lcd.setCursor(15, 2); 
  lcd.print(indicatorPotValue);
  
  // Taşmayı önlemek için akıllı boşluk (Toplam 5 karakter alanı: 15-19)
  if (indicatorPotValue < 10) lcd.print("    ");      // 1 rakam + 4 boşluk
  else if (indicatorPotValue < 100) lcd.print("   "); // 2 rakam + 3 boşluk
  else if (indicatorPotValue < 1000) lcd.print("  "); // 3 rakam + 2 boşluk
  else lcd.print(" ");                                // 4 rakam + 1 boşluk

  // Line 4: Temps & Fan Status
  // Mos Temp. Başlangıç: Kolon 4 (Mos: 0123)
  lcd.setCursor(4, 3);
  lcd.print(String((int)tempS1)); 
  lcd.print("C "); // Sabit uzunluk koruma
  
  // Res Temp. Başlangıç: Kolon 12 (Res: 8901)
  lcd.setCursor(12, 3);
  lcd.print(String((int)tempS2));
  lcd.print("C ");
  
  // Fan Status. Başlangıç: Kolon 16
  lcd.setCursor(16, 3);
  if (isFanOn) {
    // Animasyon Mantığı
    if (millis() - lastFanAnimTime >= FAN_ANIM_DELAY) {
      lastFanAnimTime = millis();
      fanFrame = !fanFrame; // 0 -> 1, 1 -> 0 Geçiş
    }
    
    lcd.print("FAN");
    lcd.write(fanFrame); // 0 veya 1 nolu karakteri bas
  } else {
    lcd.print("    "); // Temizle
  }
}

/**
 * @brief Displays Fault Screen (Centered)
 */
void displayFault() {
  // Line 0: Header (Centered)
  // "SYSTEM HALTED!" is 14 chars. (20-14)/2 = 3
  lcd.setCursor(3, 0);
  lcd.print("SYSTEM HALTED!");

  // Line 1: Error Message (Dynamic)
  lcd.setCursor(0, 1);
  lcd.print(faultMessage); // e.g. "FAULT: HIGH VOLTAGE"
  // Clear rest of line
  for (int i = faultMessage.length(); i < 20; i++) {
    lcd.print(" ");
  }

  // Line 2: Frozen Values
  lcd.setCursor(1, 2); // Indent slightly
  lcd.print("V:");
  lcd.print(String(actualVoltage, 1));
  lcd.print("  A:");
  lcd.print(String(actualCurrent, 2));
  lcd.print("    ");

  // Line 3: Instruction
  // "Press Reset..." is 14 chars.
  lcd.setCursor(3, 3);
  lcd.print("Press Reset...");
}