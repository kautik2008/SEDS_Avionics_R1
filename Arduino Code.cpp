#include <Adafruit_LiquidCrystal.h>

// PIN CONNECTIONS 

// Ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// Light sensor
const int lightPin = A0;

// Push button
const int buttonPin = 7;

// LED and buzzer
const int ledPin = 2;
const int buzzerPin = 8;

// LCD
Adafruit_LiquidCrystal lcd(0);


// THRESHOLDS 

const int LIGHT_THRESHOLD = 950;
const int DISTANCE_THRESHOLD = 100;
const unsigned long WRECK_TIME = 5000;


// STATES 

typedef enum {
  OPEN_SEA,
  ANCHOR_DROPPED,
  STORM,
  CHARYBDIS,
  WRECKED
} State;
State currentState = OPEN_SEA;
void changeState(State newState);


// TIMER VARIABLES

unsigned long dangerStartTime = 0;
unsigned long previousBlinkTime = 0;



// SETUP 

void setup() {

	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	pinMode(buttonPin, INPUT_PULLUP);

    	pinMode(ledPin, OUTPUT);
    	pinMode(buzzerPin, OUTPUT);

    	lcd.begin(16, 2);

    	lcd.clear();
    	lcd.setCursor(0, 0);
  	lcd.print("OPEN SEA");

  	Serial.begin(9600);
}


// GET DISTANCE DUNCTION

float getDistance() {

  // Send ultrasonic pulse
 	digitalWrite(trigPin, LOW);
  	delayMicroseconds(2);

  	digitalWrite(trigPin, HIGH);
 	delayMicroseconds(10);

	digitalWrite(trigPin, LOW);

  // Measure echo time
  	long duration = pulseIn(echoPin, HIGH);

  // Convert time to distance in cm
  	float distance = duration * 0.0343 / 2;
  	return distance;
}


// DISPLAY STATE

void displayState() {

  	lcd.clear();
  	lcd.setCursor(0, 0);

  	switch (currentState) {
		case OPEN_SEA:
			lcd.print("OPEN SEA");
      		break;

    	case ANCHOR_DROPPED:
      		lcd.print("ANCHOR DROPPED");
      		break;

    	case STORM:
      		lcd.print("STORM");
      		break;

    	case CHARYBDIS:
      		lcd.print("CHARYBDIS");
      		break;

    	case WRECKED:
      		lcd.print("WRECKED");
      		break;
  	}
}


BUTTON DETECTION
bool bef = HIGH;

bool btn_prs() {
    bool cur = digitalRead(buttonPin);

    bool pressed = (cur == LOW && bef == HIGH);

    bef = cur;

    return pressed;
}
//True is returned when the button is pressed and released


// CHANGE STATE 

void changeState(State newState) {

  	currentState = newState;

  // Reset danger timer whenever we leave danger
  	if (newState == OPEN_SEA || newState == ANCHOR_DROPPED) {
    	dangerStartTime = 0;
  	}

  // Start the 5-second timer when entering danger
  	if (newState == STORM || newState == CHARYBDIS) {
    	dangerStartTime = millis();
  	}
  // Disply the current state in LCD
  // LCD Display changes only when the state changes. 
  // If it was placed inside the loop, it would have been erased and rewritten rapidly.
  	lcd.clear();
  	lcd.setCursor(0, 0);

  	switch (currentState) {
		case OPEN_SEA:
			lcd.print("OPEN SEA");
      			break;

    		case ANCHOR_DROPPED:
      			lcd.print("ANCHOR DROPPED");
      			break;

    		case STORM:
      			lcd.print("STORM");
      			break;

    		case CHARYBDIS:
      			lcd.print("CHARYBDIS");
      			break;

    		case WRECKED:
      			lcd.print("WRECKED");
      			break;
  	}

}


// MAIN LOOP 

void loop() {

  //  READ SENSORS 
  	int lightValue = analogRead(lightPin);
  	float distance = getDistance();
  	bool stormDetected = lightValue < LIGHT_THRESHOLD;
  	bool charybdisDetected = distance < DISTANCE_THRESHOLD;


  //  CHECK BUTTON 
  	bool button = btn_prs();


  //  SERIAL MONITOR 
  	Serial.print("Light = ");
  	Serial.print(lightValue);

  	Serial.print(" | Distance = ");
  	Serial.println(distance);
  
  // WRECKED
  	if (currentState == WRECKED) {
    // Wrecked is permanent
    		digitalWrite(ledPin, LOW);
    		digitalWrite(buzzerPin, LOW);

    		return;
  	}

  // ANCHOR DROPPED

  	if (currentState == ANCHOR_DROPPED) {

    // Ship is protected
    		digitalWrite(ledPin, LOW);
    		digitalWrite(buzzerPin, LOW);

    // Press button again to raise anchor
    		if (button) {
      			changeState(OPEN_SEA);
    		}

    		return;
  	}


  // OPEN SEA

  	if (currentState == OPEN_SEA) {

    		digitalWrite(ledPin, LOW);
    		digitalWrite(buzzerPin, LOW);

    // Button drops anchor
    		if (button) {
      			changeState(ANCHOR_DROPPED);
      			return;
    		}

    // Check for storm first
    		if (stormDetected) {
      			changeState(STORM);
        		return;
    		}

    // Check for Charybdis
    		if (charybdisDetected) {
      			changeState(CHARYBDIS);
      			return;
    		}
  	}
	

  // STORM

	if (currentState == STORM) {

    		digitalWrite(buzzerPin, LOW);

    // Button saves the ship
    		if (button) {
      			changeState(ANCHOR_DROPPED);
      			digitalWrite(ledPin, LOW);
      			return;
    		}
    // BLINK LED 
    		if (millis() - previousBlinkTime >= 300) {
      			previousBlinkTime = millis();
      			digitalWrite(ledPin, !digitalRead(ledPin));
    		}
    // 5 SECOND TIMER 
    		if (millis() - dangerStartTime >= WRECK_TIME) {
      			digitalWrite(ledPin, LOW);
      			changeState(WRECKED);
      			return;
    		}

    // STORM ENDED 
    		if (!stormDetected) {
      			digitalWrite(ledPin, LOW);
      			changeState(OPEN_SEA);
      			return;
    		}
  	}


  // CHARYBDIS
	if (currentState == CHARYBDIS) {
    // Buzzer ON
    		digitalWrite(buzzerPin, HIGH);
    // LED OFF
    		digitalWrite(ledPin, LOW);

    // Button saves the ship
    		if (button) {
      			digitalWrite(buzzerPin, LOW);
      			changeState(ANCHOR_DROPPED);
      			return;
    		}


    // CHECK 5 SECOND TIMER 

    		if (millis() - dangerStartTime >= WRECK_TIME) {
      			digitalWrite(buzzerPin, LOW);
      			changeState(WRECKED);
      			return;
    		}

    // ESCAPED FROM CHARYBDIS
    		if (!charybdisDetected) {
      			digitalWrite(buzzerPin, LOW);
      			changeState(OPEN_SEA);
      			return;
    		}
  	}
	delay(10);
}
