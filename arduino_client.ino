#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <SPI.h>
#include <MFRC522.h>

const char* ssid = "BoteroAguilar"; 
const char* pass = "ejecutivo2020";

//Para 1 LED
#define RED D0 
#define GREEN D1
#define BLUE D2

#define RST_PIN D3
#define SS_PIN D4

MFRC522 rfid(SS_PIN, RST_PIN);

void setup()
{
	SPI.begin();
	rfid.PCD_Init();
	
    pinMode(RED,OUTPUT);
    pinMode(GREEN,OUTPUT);
    pinMode(BLUE,OUTPUT);

	digitalWrite(RED,LOW);
    digitalWrite(GREEN,LOW);
    digitalWrite(BLUE,LOW);
    Serial.begin(9600);
    delay(10);

    //Conexion a la red WiFi
    Serial.println();
    Serial.print("Conectando a la red ");
    Serial.println(ssid);

    WiFi.begin(ssid,pass); //Inicia la conexion 
    while(WiFi.status() != WL_CONNECTED) //sigue intentando conectarse
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWifi conectado");
}

int value_r = LOW;
int value_g = LOW;
int value_b = LOW;

void loop()
{
	if (!rfid.PICC_IsNewCardPresent()) {
		delay(10);
		return;
	}
	if (!rfid.PICC_ReadCardSerial()) {
		return;
	}

	Serial.print(F("PICC type: "));
	MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
	Serial.println(rfid.PICC_GetTypeName(piccType));

	// Check is the PICC of Classic MIFARE type
	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
		piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
		piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
		Serial.println(F("Your tag is not of type MIFARE Classic."));
		return;
	}

	Serial.println("card detected");

	byte card_id[4];
	memcpy(card_id, rfid.uid.uidByte, 4);
	char card_hex[9];
	formatHex(card_id, 4, card_hex);
	
    WiFiClient client;
	HTTPClient http;

	char request[1024];
	char* format = "http://192.168.0.8:5000/card/%s";
	sprintf(request, format, card_hex);
	Serial.printf("requesting: %s\n", request);

	bool result = http.begin(client, request);
	if (!result) {
		delay(1000);
		return;
	}

	int httpCode = http.GET();
	Serial.printf("HTTP: %d\n", httpCode);
	bool success = false;
	if (httpCode > 0) {
		String payload = http.getString();
		Serial.println(payload);
		success = 0 == payload.compareTo("True");
	}
	http.end();

	Serial.println("END");
	if (success) {
		digitalWrite(GREEN, HIGH);
	} else {
		digitalWrite(RED, HIGH);
	}
	delay(2000);
	digitalWrite(GREEN, LOW);
	digitalWrite(RED, LOW);
    
}

void formatHex(byte *buffer, byte size, char* destination) {
	byte cursor = 0;
	for (byte idx = 0; idx < size; idx++) {
		byte data = buffer[idx];
		destination[cursor] = toHexChar(data & 0x0F);
		destination[cursor+1] = toHexChar(data >> 4);
		cursor += 2;
	}
	destination[cursor] = 0;
}
char toHexChar(byte value) {
	if (value < 0x0A) {
		return '0' + value;
	} else {
		return 'A' + value - 0x0A;
	}
}



