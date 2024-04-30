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



#include <SoftwareSerial.h>

// RX and TX pins connected to the DT-06 module
SoftwareSerial espSerial(2, 3); // RX, TX

String host = "192.168.0.1"; // IP of the localhost
int port = 3001; // Port number

String wifi_name= "wifi123";
String wifi_pass= "123456789";

// Forward declaration of functions
void sendCommand(String command, const int timeout);
void sendJsonData(String jsonData);



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
  espSerial.begin(9600);
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


  // Setup WiFi connection
  sendCommand("AT+RST\r\n", 5000); // Reset module
  sendCommand("AT+CWMODE=1\r\n", 2000); // Set mode to STA
  sendCommand("AT+CWJAP=\"" + wifi_name + "\",\"" + wifi_pass + "\"\r\n", 5000); // Connect to WiFi using variables
  sendCommand("AT+CIPMUX=1\r\n", 2000); // Enable multiple connections
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + host + "\"," + String(port) + "\r\n", 5000); // Start TCP connection

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
    } else {
      Serial.print("  -- Humidity: ");
      Serial.print(h);
      Serial.println();
      Serial.print("  -- Temperature: ");
      Serial.print(t);
      Serial.println(" *C");
      



    }


      float lux = lightMeter.readLightLevel();
   Serial.print("  -- Light: ");
  Serial.print(lux);
  Serial.println(" lx");


  air_sensorValue = analogRead(0); // Read analog input pin 0
  Serial.print("  -- Air Quality : ");
  Serial.print(air_sensorValue, DEC); // Prints the value read
  Serial.println(" PPM");



  }




send_mess_temp_var++;
      if (send_mess_temp_var == send_mess_temp_var_cicle) {
       // Construct the JSON data string
    String jsonData = "{\"system\":\"GreenSystemsHome\",\"hardwareID\":\"000000001\",\"postID\":\"0000000000001\",\"data\":{\"temp\":10,\"humidity\":200,\"light\":200,\"water\":true,\"air\":180,\"UV\":20},\"portsActive\":1,\"port1\":{\"soilHumidity\":150,\"plant\":\"orchid\",\"lastWatered\":\"16.04.2024 6:00\",\"salt\":20,\"ph\":12,\"error\":\"none\"}}";

    // Send the JSON data
    sendJsonData(jsonData);


     // Check if the module sends any messages
  if (espSerial.available()) {
    Serial.write(espSerial.read());
  }

       
        send_mess_temp_var = 0;
      }



      



  delay(2000);
}



void sendCommand(String command, const int timeout) {
  Serial.print("Sending: ");
  Serial.print(command);
  espSerial.print(command); // Send the command to the ESP module
  
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (espSerial.available()) {
      char c = espSerial.read(); // read the next character.
      Serial.write(c);
    }
  }
  Serial.println();
}

void sendJsonData(String jsonData) {
  String cipSend = "AT+CIPSEND=0," + String(jsonData.length()) + "\r\n";
  sendCommand(cipSend, 2000); // Prepare to send data

  delay(100); // Wait a little for the '>' prompt
  sendCommand(jsonData, 2000); // Send the actual JSON data
}