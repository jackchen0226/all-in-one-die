const int BUZZER_PIN = 10;
const int SENSOR_PIN = A0;

void play_sound() {
    // Megalovania
  // Composed by Toby Fox
  // D3 eigth note (beamed)
  tone(BUZZER_PIN, 147);
  delay(63);
  noTone(BUZZER_PIN);
  delay(63);
  
  // D3 eigth note (beamed)
  tone(BUZZER_PIN, 147);
  delay(63);
  noTone(BUZZER_PIN);
  delay(63);

  // D4 eigth note, dotted rest (beamed)
  tone(BUZZER_PIN, 294);
  delay(125);
  noTone(BUZZER_PIN);
  delay(125);

  // A4 quarter note, dotted rest/end
  tone(BUZZER_PIN, 220);
  delay(250);
  noTone(BUZZER_PIN);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  play_sound();

  Serial.println(analogRead(SENSOR_PIN));
  delay(10000);
}
