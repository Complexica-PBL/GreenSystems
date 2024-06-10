#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int RELAY_PIN_1 = 16;
int module_1_count_water_cicle = 3;
int module_1_count_water_cicle_counted = 0;
int module_1_soil;

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

int air_sensorValue;

// Global variable
int lux_global;
int hum_global;
int temp_global;
bool water_global;
int air_global;
int water_count=0;






#include <SoftwareSerial.h>
#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include <ArduinoJson.h>

const char* ssid = "Cristian’s iPhone 11 Pro Max";
const char* password = "multevrei";

char server[] = "172.20.10.4";
int port = 5004;
const char* route = "receive";
WiFiEspClient client;

char newServer[] = "172.20.10.4";
int newPort = 5005;
const char* newRoute = "telegram";
WiFiEspClient newClient;

int send_mess_temp_var_cicle = 3;
int send_mess_temp_var = 0;


const int buzzer = 14;  //buzzer to arduino pin 9

#define sensorPower 17
#define sensorPin A8
int water_val = 0;

int water_readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  digitalWrite(RELAY_PIN_1, HIGH);
  delay(10);                          // wait 10 milliseconds
  water_val = analogRead(sensorPin);  // Read the analog value form sensor

  digitalWrite(sensorPower, LOW);  // Turn the sensor OFF
  return water_val;                // send current reading
}



void setup() {
  Serial.begin(9600);
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.println("© GreenSystems | UTM PBL spring 2024");
  delay(1000);
  Serial.println("Initialising...");
  tone(buzzer, 1000);  // Send 1KHz sound signal...
  delay(20);
  noTone(buzzer);
  delay(300);
  tone(buzzer, 1000);  // Send 1KHz sound signal...
  delay(20);
  noTone(buzzer);
  delay(300);
  tone(buzzer, 1000);  // Send 1KHz sound signal...
  delay(20);
  noTone(buzzer);

  delay(2000);
  //set water pins reading (4 for this unit)
  pinMode(22, INPUT);
  pinMode(23, INPUT);
  pinMode(24, INPUT);
  pinMode(25, INPUT);

  pinMode(RELAY_PIN_1, OUTPUT);

  // Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);

  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);

   digitalWrite(RELAY_PIN_1, HIGH);

  pinMode(buzzer, OUTPUT);  // Set buzzer - pin 9 as an output

  Wire.begin();

  lightMeter.begin();

 Serial.begin(115200);

  // Initialize serial for ESP-01 (TX1/RX1 on Mega)
  Serial1.begin(115200);
  WiFi.init(&Serial1);

}

boolean any_module_connected = false;
boolean module_1_connected = false;
boolean module_2_connected = false;
boolean module_3_connected = false;
boolean module_4_connected = false;


void loop() {


  digitalWrite(RELAY_PIN_1, HIGH);
  if (!any_module_connected) {
     
    Serial.println("Searching for modules...");
  }
  Serial.println("---------------------------------------");
  int module_presence_pinState1 = digitalRead(22);
  int module_presence_pinState2 = digitalRead(23);
  int module_presence_pinState3 = digitalRead(24);
  int module_presence_pinState4 = digitalRead(25);

  module_1_connected = (module_presence_pinState1 != 0);
  if (module_1_connected) {
    Serial.println("Detected module at port [1]");


    int water_sensor_level_module_1 = water_readSensor();
    while (water_sensor_level_module_1 < 50) {  // Loop will continue if no water is detected
      digitalWrite(RELAY_PIN_1, HIGH);
      Serial.println("---------------------------------------");
      Serial.println("ALERT [no water]");  // Print alert message
      if(water_count==10){
         sendJsonPayload(newServer, newPort, newRoute, createTelegramJsonPayload("! [Alert] - No water, please refill"));
         water_count=0;
      }
      water_count++;

      water_global=false;
      tone(buzzer, 1000);                  // Send 1KHz sound signal...
      delay(50);
      noTone(buzzer);
      delay(300);
      tone(buzzer, 1000);  // Send 1KHz sound signal...
      delay(50);
      noTone(buzzer);                                    // Stop sound...
      delay(4000);                                       // Wait for a second before checking again
      water_sensor_level_module_1 = water_readSensor();  // Re-read the water sensor level
    }
water_global=true;

    Serial.print("  -- Module [1] Water level ");
    if (water_sensor_level_module_1 < 200) {
      Serial.print("[ LOW ]  - ");
    }

    else if (water_sensor_level_module_1 >= 200) {
      Serial.print("[ NORMAL ]  - ");
    }

    Serial.println(water_sensor_level_module_1);



    module_1_soil = analogRead(A15);
    Serial.print("  -- Module [1] soil - ");
    Serial.println(module_1_soil);


 digitalWrite(RELAY_PIN_1, HIGH);
    if (module_1_soil < 700) {
      module_1_count_water_cicle_counted++;
      if (module_1_count_water_cicle_counted == module_1_count_water_cicle) {
        Serial.println("Watering ....");
        digitalWrite(RELAY_PIN_1, LOW);
        delay(3000);
        digitalWrite(RELAY_PIN_1, HIGH);
        Serial.println("Watering Done");
        module_1_count_water_cicle_counted = 0;
      }
    }
  }

  module_2_connected = (module_presence_pinState2 != 0);
  if (module_2_connected) {
    Serial.println("Detected module at port [2]");
  }

  module_3_connected = (module_presence_pinState3 != 0);
  if (module_3_connected) {
    Serial.println("Detected module at port [3]");
  }

  module_4_connected = (module_presence_pinState4 != 0);
  if (module_4_connected) {
    Serial.println("Detected module at port [4]");
  }

  any_module_connected = (module_1_connected || module_2_connected || module_3_connected || module_4_connected);

  if (any_module_connected) {


    float h = dht.readHumidity();
    float t = dht.readTemperature();
 Serial.println();
  Serial.println("General:");

    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT");
      temp_global=0;
       hum_global=0;
    } else {
      Serial.print("  -- Humidity: ");
      Serial.print(h);
      hum_global = h;
      Serial.println();
      Serial.print("  -- Temperature: ");
      Serial.print(t);
      temp_global = t;
      Serial.println(" *C");
      



    }


      float lux = lightMeter.readLightLevel();
   Serial.print("  -- Light: ");
  Serial.print(lux);
  lux_global = (int)lux;
  Serial.println(" lx");


  air_sensorValue = analogRead(0); // Read analog input pin 0
  Serial.print("  -- Air Quality : ");
   air_global = (int)air_sensorValue;
  Serial.print(air_sensorValue, DEC); // Prints the value read
  Serial.println(" PPM");



  }




send_mess_temp_var++;
      if (send_mess_temp_var == send_mess_temp_var_cicle) {
       // Construct the JSON data string
    //String jsonData = "{\"system\":\"GreenSystemsHome\",\"hardwareID\":\"000000001\",\"postID\":\"0000000000001\",\"data\":{\"temp\":10,\"humidity\":200,\"light\":200,\"water\":true,\"air\":180,\"UV\":20},\"portsActive\":1,\"port1\":{\"soilHumidity\":150,\"plant\":\"orchid\",\"lastWatered\":\"16.04.2024 6:00\",\"salt\":20,\"ph\":12,\"error\":\"none\"}}";

     // Check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // don't continue
  }

  // Attempt to connect to Wi-Fi network
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(10000); // wait 10 seconds for connection
  }

  Serial.println("Connected to Wi-Fi");
  printWifiStatus();

  // Send JSON payloads to servers
    sendJsonPayload(server, port, route, createJsonData());
   


       
        send_mess_temp_var = 0;
      }



      



  delay(2000);
}



void printWifiStatus() {
  // Print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your Wi-Fi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

String createJsonData() {
  StaticJsonDocument<512> jsonDoc;
  jsonDoc["system"] = "GreenSystemsHome";
  jsonDoc["hardwareID"] = "000000001";
  jsonDoc["postID"] = "0000000000001";
  JsonObject data = jsonDoc.createNestedObject("data");
  data["temp"] = temp_global;
  data["humidity"] = hum_global;
  data["light"] = lux_global;
  data["water"] = true;
  data["air"] = air_global;
  data["UV"] = 20;
  jsonDoc["portsActive"] = 1;
  JsonObject port1 = jsonDoc.createNestedObject("port1");
  port1["soilHumidity"] = 150;
  port1["plant"] = "orchid";
  port1["lastWatered"] = "16.04.2024 6:00";
  port1["salt"] = 20;
  port1["ph"] = 12;
  port1["error"] = "none";

  // Serialize JSON to string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return jsonString;
}

String createTelegramJsonPayload(String tg_mesage) {
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["tg_message"] = tg_mesage;

  // Serialize JSON to string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return jsonString;
}

void sendJsonPayload(const char* server, int port, const char* route, String jsonString) {
  WiFiEspClient client;
  if (client.connect(server, port)) {
    Serial.println("Connected to server");

    // Send HTTP POST request
    client.print("POST /");
    client.print(route);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.println(jsonString);

    // Wait for server response
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
    }

    // Disconnect from server
    client.stop();
    Serial.println("\nDisconnected from server");
  } else {
    Serial.println("Connection to server failed");
  }
}