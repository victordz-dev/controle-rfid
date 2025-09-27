#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

#define LED_VERMELHO 2
#define LED_AMARELO 3
#define LED_VERDE 4

#define servoPin 5

MFRC522 mfrc522(SS_PIN, RST_PIN);

Servo servoMotor;

LiquidCrystal lcd(8, 7, A0, A1, A2, A3);

String storedUID = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  lcd.begin(16, 2);
  
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  servoMotor.attach(servoPin);
  servoMotor.write(0);
  
  digitalWrite(LED_AMARELO, HIGH);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  printMessage("Aproxime o", "cartao");
}

void loop() {

  Serial.println("MSG: Aproxime o cartao...");
  printMessage("Aproxime o", "cartao");
  
  while (!mfrc522.PICC_IsNewCardPresent()) {delay(1000); return;};
  while (!mfrc522.PICC_ReadCardSerial()) {delay(1000); return;};
  
  String currentUID = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    currentUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    currentUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  
  currentUID.toUpperCase();

  if (storedUID == "") {
    storedUID = currentUID;
    
    Serial.println("MSG: Aguarde...");
    printMessage("Aguarde", "");

    sendUID(storedUID);
    String ledCommand = "";
    delay(1000);
    
    while (Serial.available() == 0) {}
    
    ledCommand = Serial.readStringUntil('\n');

    if (ledCommand == "green") {
      piscarLed(LED_VERDE, 3);
      Serial.println("MSG: Acesso Liberado");
      printMessage("Acesso", "Liberado");
      servoMotor.write(180);
      delay(3000);
      servoMotor.write(0);
    } else {
      piscarLed(LED_VERMELHO, 3);
      Serial.println("MSG: Acesso Negado");
      printMessage("Acesso", "Negado");
    }

    mfrc522.PICC_HaltA();
    
    delay(1000);
    storedUID = "";
  }
}

void piscarLed(int ledPin, int vezes) {
  digitalWrite(LED_AMARELO, LOW);
  for (int i = 0; i < vezes; i++) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
  digitalWrite(LED_AMARELO, HIGH);
}

void sendUID(String uid) {
  Serial.print("MAIN_UID:");
  Serial.println(uid);
}

void printMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}
