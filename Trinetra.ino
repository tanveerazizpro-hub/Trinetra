// --- Prahari Hardware Brain (Arduino Uno) ---
// Triggers: Piezo (Triple-Tap), IR (Strap Removal), Mic (Scream/Impact)

const int PIEZO_PIN = A0;   // Analog pin for Piezo sensor
const int IR_PIN = 3;       // Digital pin for IR sensor
const int MIC_PIN = 4;      // Digital pin for Red Mic Module (OUT/D0)

// Thresholds and Timers
const int PIEZO_THRESHOLD = 500; // Adjust based on your piezo sensitivity
unsigned long lastTapTime = 0;
int tapCount = 0;
const unsigned long TAP_TIMEOUT = 3000; // 3 seconds to complete 3 taps

void setup() {
  // Start serial communication for HC-05 Bluetooth module
  Serial.begin(9600); 
  
  pinMode(IR_PIN, INPUT);
  pinMode(MIC_PIN, INPUT);
  // Analog pins don't need pinMode declaration for analogRead
}

void loop() {
  unsigned long currentTime = millis();

  // 1. CHECK MIC SENSOR (Scream / Impact)
  // The LM393 module sends a HIGH signal when a loud noise exceeds the tuned threshold
  bool isScreaming = digitalRead(MIC_PIN) == HIGH;

  // 2. CHECK IR SENSOR (Strap Removal)
  // Adjust HIGH/LOW depending on your specific IR module's default state
  bool isStrapRemoved = digitalRead(IR_PIN) == HIGH; 

  // 3. CHECK PIEZO SENSOR (Triple-Tap)
  bool isTapped = false;
  if (analogRead(PIEZO_PIN) > PIEZO_THRESHOLD) {
    if (currentTime - lastTapTime > 200) { // Debounce delay so one tap isn't counted twice
      tapCount++;
      lastTapTime = currentTime;
    }
  }

  // Reset tap count if 3 seconds pass without completing the triple-tap
  if (tapCount > 0 && (currentTime - lastTapTime > TAP_TIMEOUT)) {
    tapCount = 0; 
  }

  if (tapCount >= 3) {
    isTapped = true;
    tapCount = 0; // Reset after successful trigger
  }

  // 4. MASTER ALARM LOGIC
  if (isScreaming || isStrapRemoved || isTapped) {
    // Send the trigger command over Bluetooth to the Android tablet
    Serial.println("ALERT_TRIGGERED"); 
    
    // 5-second delay to prevent spamming the tablet with hundreds of Bluetooth messages
    delay(5000); 
  }
}
