float X, Y, Z;

float roll_threshold = 5;

int maxRoll = 20;
 
void setup() {
  //Serial.begin(9600);
}

void loop() {

  if (getTotalAcceleration() > roll_threshold) {
    rollDice();
}

float getTotalAcceleration(){
    // Compute total acceleration
  X = 0;
  Y = 0;
  Z = 0;
  for (int i=0; i<10; i++) {
    X += CircuitPlayground.motionX();
    Y += CircuitPlayground.motionY();
    Z += CircuitPlayground.motionZ();
    delay(1);
  }
  X /= 10;
  Y /= 10;
  Z /= 10;
 
  return sqrt(X*X + Y*Y + Z*Z);
}

void updateDisplay(int num){
  
}

void rollDice(){
  int newRoll = Random.Range(0, maxRoll);

  updateDisplay(newRoll);
}

void increaseMaxRoll(){
  maxRoll++;
  if(maxRoll > 20){
    maxRoll = 20;
  }
}

void decreaseMaxRoll(){
  maxRoll--;
  if(maxRoll < 4){
    maxRoll = 4;
  }
}