#define FLOW_PIN 18 
#define FLOW_FACTOR (1.0 / 7.5) 

uint32_t pulse_count = 0;
void on_trigger_handle() {
  pulse_count++;
}

void setup() {
  Serial.begin(115200);

  pinMode(FLOW_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), on_trigger_handle, RISING); // Trigger on rising edge
}

void loop() {
  pulse_count = 0;
  delay(200);
  uint32_t end_pulse_count = pulse_count;
  float flow = end_pulse_count * FLOW_FACTOR; // 7.5 Hz = 1 L/min
  Serial.printf("Flow = %.02f L/min\n", flow);
}