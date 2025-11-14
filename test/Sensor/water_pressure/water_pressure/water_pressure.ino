#define data_pin 13

void setup() {
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:

void loop() {

  int sensorValue = analogRead(data_pin);          // read the input on analog data_pin
  float voltage = sensorValue * (5.0 / 4095.0);    // Convert the analog reading (which goes from 0 - 4095) to a voltage (0 - 5V):
  float pressure = (voltage - 0.5) * (16.0 / (4.5 - 0.5));  //ELWPs16   0-16 bar

  // print the results to the Serial Monitor:
  Serial.print("ADC = ");
  Serial.print(sensorValue);
  Serial.print("\t | Vout = ");
  Serial.print(voltage);
  Serial.print("\t | Pressure = ");
  Serial.println(pressure);

  // wait 2 milliseconds before the next loop
  delay(2);
}