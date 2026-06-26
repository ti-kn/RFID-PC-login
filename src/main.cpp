#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

const byte BUTTON_PIN = 6;
const byte SDA_PIN = 10;
const byte RST_PIN = 9;

MFRC522 rfidReader(SDA_PIN, RST_PIN);


String masterID;
void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  rfidReader.PCD_Init();

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
    Serial.print("ID: ");
    Serial.println(idToCheck);
  } 
  else 
  {
    Serial.print("ID: ");
    Serial.println(idToCheck);
  }
  rfidReader.PICC_HaltA();

  delay(2000);
}
  

// Funktionen -->

void PrintCardUID()
{
  if (!rfidReader.PICC_IsNewCardPresent()) return;    // checks if new card is avaible
  if (!rfidReader.PICC_ReadCardSerial()) return;       // checks if it was able to read card
 
  Serial.print("Karten UID: ");
  for (int i = 0; i < rfidReader.uid.size; i++)
  {
    Serial.print(rfidReader.uid.uidByte[i]);
  }
  Serial.println();

  rfidReader.PICC_HaltA();    // puts card to sleep
}