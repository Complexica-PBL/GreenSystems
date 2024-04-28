#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int RELAY_PIN_1 = 16;
int module_1_count_water_cicle = 3;
int module_1_count_water_cicle_counted = 0;
int module_1_soil;




const int buzzer = 14;  //buzzer to arduino pin 9

#define sensorPower 18
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

  pinMode(buzzer, OUTPUT);  // Set buzzer - pin 9 as an output
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
    Serial.print("  -- Module [1] soil= ");
    Serial.println(module_1_soil);



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
  }

  delay(2000);
}
