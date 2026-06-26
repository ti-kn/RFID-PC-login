#include <Arduino.h>
#include "LedControl.h"
#include <SPI.h>
#include <MFRC522.h>

const byte BUTTON_PIN = 6;
const byte SDA_PIN = 10;
const byte RST_PIN = 9;

LedControl matrix = LedControl(2, 4, 3, 1); // Pins: DIN, CLK, CS, Anzahl angeschlossener Module
MFRC522 rfidReader(SDA_PIN, RST_PIN);

const byte arrowRight[] = 
{
  0b00000000,
  0b00011000, 
  0b00011000, 
  0b00011000, 
  0b01111110, 
  0b00111100, 
  0b00011000, 
  0b00000000
};

const byte confirmed[] =
{
  0b00000000, 
  0b00010000, 
  0b00100110, 
  0b01000000, 
  0b01000000, 
  0b00100110, 
  0b00010000, 
  0b00000000
};

const byte denied[] =
{
  0b00000000,
  0b01000000, 
  0b00100110, 
  0b00010000, 
  0b00010000, 
  0b00100110, 
  0b01000000, 
  0b00000000
};

void DrawAtMatrix(const byte data[])
{
  for (int pixelCount = 0; pixelCount < 8; pixelCount++)
  {
    matrix.setRow(0, pixelCount, data[pixelCount]);
  }
}

String masterID;
void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  rfidReader.PCD_Init();

  matrix.shutdown(0, false);       // Display 0 aufwecken
  matrix.setIntensity(0, 1);       // Helligkeit für Modul 0 stellen (0 - 15)
  matrix.clearDisplay(0);

  DrawAtMatrix(arrowRight);

  Serial.println("Karte einlesen");
  while (true)
  {
    if (rfidReader.PICC_IsNewCardPresent())
    {
      if (rfidReader.PICC_ReadCardSerial())
      {
        for (int i = 0; i < rfidReader.uid.size; i++)
        {
          masterID += rfidReader.uid.uidByte[i] < 0x10 ? " 0" : " ";
          masterID += String(rfidReader.uid.uidByte[i], HEX);
        }
        masterID.toUpperCase();

        Serial.print("Erfolgreich eingelesen! KartenID: ");
        Serial.println(masterID);
        break;
      }
    }
    
  } 
}

String idToCheck;
void loop() 
{
  if (!rfidReader.PICC_IsNewCardPresent()) return;   
  if (!rfidReader.PICC_ReadCardSerial()) return; 

  idToCheck = "";
  for (int i = 0; i < rfidReader.uid.size; i++)
  {
    idToCheck += rfidReader.uid.uidByte[i] < 0x10 ? " 0" : " ";
    idToCheck += String(rfidReader.uid.uidByte[i], HEX);
  }
  idToCheck.toUpperCase();

  if (idToCheck == masterID)
  {
    DrawAtMatrix(confirmed);
    Serial.print("ID: ");
    Serial.println(idToCheck);
  } 
  else 
  {
    DrawAtMatrix(denied);
    Serial.print("ID: ");
    Serial.println(idToCheck);
  }
  rfidReader.PICC_HaltA();

  delay(2000);
  DrawAtMatrix(arrowRight);
}
  

// Funktionen -->

void PrintCardUID()
{
  if (!rfidReader.PICC_IsNewCardPresent()) return;    // Überprüft ob neue Karte vorhanden ist
  if (!rfidReader.PICC_ReadCardSerial()) return;       // Überprüft ob RFID gelesen werden konnte
 
  Serial.print("Karten UID: ");
  for (int i = 0; i < rfidReader.uid.size; i++)
  {
    Serial.print(rfidReader.uid.uidByte[i]);
  }
  Serial.println();

  rfidReader.PICC_HaltA();    // Karte schlafen legen (nicht in der loop die ganze Zeit auf dem SerialMonitor ausgeben)
}