// Embedded Programming 2 — Session 1 warm-up
// An LED flashing morse code using Wokwi
// The design uses two LED's: the blue (LED_PIN1) displays the message and the red (LED_PIN2) displays when the message ends

#define LED_PIN1 40
#define LED_PIN2 38

void morse_s(int ledpin){
  for(int i = 0; i<3; i++){
    digitalWrite(ledpin, HIGH);
    delay(250);

    digitalWrite(ledpin, LOW);
    delay(250);
  }
}

void morse_o(int ledpin){
  for(int i = 0; i<3; i++){
    digitalWrite(ledpin, HIGH);
    delay(400);

    digitalWrite(ledpin, LOW);
    delay(400);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  Serial.println("Ready");
}

void loop() {
  int msgPin = LED_PIN1;
  int overPin = LED_PIN2;
  morse_s(msgPin);
  delay(500);
  morse_o(msgPin);
  delay(500);
  morse_s(msgPin);
  digitalWrite(overPin, HIGH);
  delay(500);
  digitalWrite(overPin, LOW);
  delay(1000);
}