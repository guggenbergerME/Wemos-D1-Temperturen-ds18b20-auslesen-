#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>  
#include <DallasTemperature.h>
#include <OneWire.h>



///////////////////////////////////////////////////////////////////// WIRE Bus
#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


///////////////////////////////////////////////////////////////////// Kartendaten
const char* kartenID = "KartenName";
char mqtt_text[10];

////////////////////////////////////////////////// Temp. Sensor Messwert Variablen
// float wert_MAC_ADRESSE_DES_ds18b20;
float wert_28_92_D9_6D_1E_13_1_38;
char buffer1[10];

////////////////////////////////////////////////////////////////////// Sensoradressen onewire
DeviceAddress temp_28_92_D9_6D_1E_13_1_38 = { 0x28, 0x92, 0xD9, 0x6D, 0x1E, 0x13, 0x01, 0x38 }; 

///////////////////////////////////////////////////////////////////// Wifi einrichten
WiFiClient espClient;
PubSubClient client(espClient);

///////////////////////////////////////////////////////////////////// Intervall der Steuerung
unsigned long previousMillis_Temperatur = 0;
unsigned long interval_Temperatur = 30000; 
///////////////////////////////////////////////////////////////////////////


// Connect to the WiFi
const char* ssid = "XXX";
const char* password = "XXX";
const char* mqtt_server = "192.168.150.1";

///////////////////////////////////////////////////////////////////////////

//**************************************************************** VOID mqtt callback
void callback(char* topic, byte* payload, unsigned int length) {

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Verbindung zu mqtt IP: ");
    Serial.print(mqtt_server);
    Serial.println("");
    // Create a random client ID
    String clientId = "Wifi-Temperatur-Pellets";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Verbunden ....");
      /* ####################################################################
      SUBSCRIBE Eintraege
      Wird für jedes einzelne mqtt Device benötig um es anzusprechen
      */

          
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



//////////////////////////////////////////////////////////////////////////////////   SETUP
void setup() {

/////////////////////////////////////////////////////////////////////////// Serial
  Serial.begin(115200);

/////////////////////////////////////////////////////////////////////////// mqtt Server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

////////////////////////////////////////////////////////////////////////// Sensoren starten
sensors.begin();
sensors.setResolution(temp_28_92_D9_6D_1E_13_1_38, 9);


/////////////////////////////////////////////////////////////////////////// Wifi

  Serial.print("Verbindung zu SSID -> ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  // Wifi AP deaktivieren
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Erfolgreich mit dem WiFi verbunden!");
  Serial.print("DHCP Adresse der Karte : ");
  Serial.println(WiFi.localIP());
  Serial.print("ID der Karte: ");
  Serial.println(kartenID);


}

void temperartur_schreiben() {
///////////////////////////////////////////////////////////////////////////// Sensor schreiben
sensors.requestTemperatures();

/////////////////////////////////////////////////////////////////////////// Sensor VL Fussbodenheizung
wert_28_92_D9_6D_1E_13_1_38 = sensors.getTempC(temp_28_92_D9_6D_1E_13_1_38); 
   if ((wert_28_92_D9_6D_1E_13_1_38 == -127)||(wert_28_92_D9_6D_1E_13_1_38 == 85))  { 
     Serial.println("Sensor VL Fussbodenheizung : FEHLER");
     } 
    else 
        { 
          dtostrf(wert_28_92_D9_6D_1E_13_1_38,2, 1, buffer1); 
          Serial.println("Sensor  wert_28_92_D9_6D_1E_13_1_38 : ");
          Serial.println(buffer1);    
          client.publish("mqtt", buffer1);     
        }


} // Ende void

//****************************************************************************************** VOID LOOP
void loop()
{
  // mqtt Daten senden     
  if (!client.connected()) {
      reconnect();
    }
    client.loop();  

  ///////////////////////////////////////////////////////////////////////// Daten schreiben im Intervall Temperatur
  if (millis() - previousMillis_Temperatur > interval_Temperatur) {

      previousMillis_Temperatur = millis();   // aktuelle Zeit abspeichern
 
   // Temperatur auslesen
   temperartur_schreiben();
    }

delay(50);
}