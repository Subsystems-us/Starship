// This program samples the color of an object under
// the Starship and displays the probability of
// the detected color.

// Map the Arduino pins to the Starship functions
#define shift_data 3  // Data for the LED shift registers
#define shift_clk 4  // Clock for the LED shift registers
#define lpod_latch 5  // Register latch for the left Power Pod
#define rpod_latch 6  // Register latch for the right Power Pod
#define disk_left_latch 7  // Latch for the left disk LEDs
#define disk_right_latch 8  // Latch for the right disk LEDs
#define led_red 9  // Red LED for the color sensor
#define led_blue 10  // Blue LED for the color sensor
#define led_green 11  // Green LED for the color sensor
#define disk_speaker 12  // Speaker connection
#define button_input 13  // User button
#define temperature_sensor A0  // Temperature sensor input
#define light_sensor A1  // Light sensor input
#define color_sensor A2  // Color sensor input
#define accl_X A3  // Accelerometer analog X-axis signal
#define accl_Y A4  // Accelerometer analog Y-axis signal
#define accl_Z A5  // Accelerometer analog Z-axis signal

long baseRedBlack = 0; // calibration value for Red LED on Black target
long baseRedWhite = 0; // calibration value for Red LED on White target
long baseGreenBlack = 0;
long baseGreenWhite = 0;
long baseBlueBlack = 0;
long baseBlueWhite = 0;

void setup() {
  // Start the Serial connection at 9600 baud
  Serial.begin(9600);

  // Set the pin Modes
  pinMode(led_red, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(shift_data, OUTPUT);
  pinMode(shift_clk, OUTPUT);
  pinMode(lpod_latch , OUTPUT);
  pinMode(rpod_latch , OUTPUT);
  pinMode(disk_left_latch, OUTPUT);
  pinMode(disk_right_latch, OUTPUT);
  pinMode(disk_speaker, OUTPUT);
  pinMode(button_input, INPUT);

  // Turn off color sensor LEDs
  digitalWrite(led_red, LOW); // Turn off the red LED
  digitalWrite(led_blue, LOW); // Turn off the blue LED
  digitalWrite(led_green, LOW); // Turn off the green LED

  dispOff(); // Turn off all register LEDs

  // Calibrate the sensors one time
  Serial.println("Ready..place BLACK target and press button");
  Serial.println();

  while (digitalRead(button_input)) {}

  // Get Red value
  digitalWrite(led_red, HIGH); // Turn on the red LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    baseRedBlack = baseRedBlack + analogRead(color_sensor);
  }
  baseRedBlack = baseRedBlack / 20;

  digitalWrite(led_red, LOW); // Turn off the red LED
  digitalWrite(led_green, HIGH); // Turn on the green LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    baseGreenBlack = baseGreenBlack + analogRead(color_sensor);
  }
  baseGreenBlack = baseGreenBlack / 20;

  digitalWrite(led_green, LOW); // Turn on the red LED
  digitalWrite(led_blue, HIGH); // Turn on the red LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    baseBlueBlack = baseBlueBlack + analogRead(color_sensor);
  }
  baseBlueBlack = baseBlueBlack / 20;

  digitalWrite(led_blue, LOW); // Turn on the red LED

  Serial.println("Ready..place WHITE target and press button");
  Serial.println();

  while (digitalRead(button_input)) {}

  // Get Red value
  digitalWrite(led_red, HIGH); // Turn on the red LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    baseRedWhite = baseRedWhite + analogRead(color_sensor);
  }
  baseRedWhite = baseRedWhite / 20;

  digitalWrite(led_red, LOW); // Turn off the red LED
  digitalWrite(led_green, HIGH); // Turn on the green LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    baseGreenWhite = baseGreenWhite + analogRead(color_sensor);
  }
  baseGreenWhite = baseGreenWhite / 20;

  digitalWrite(led_green, LOW); // Turn on the red LED
  digitalWrite(led_blue, HIGH); // Turn on the red LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    baseBlueWhite = baseBlueWhite + analogRead(color_sensor);
  }
  baseBlueWhite = baseBlueWhite / 20;

  digitalWrite(led_blue, LOW); // Turn on the red LED
}

void loop() {
  long redVal = 0;
  long greenVal = 0;
  long blueVal = 0;

  Serial.println("Ready..press button to test color.");
  Serial.println();

  while (digitalRead(button_input)) {}

  // Get Red value
  digitalWrite(led_red, HIGH); // Turn on the red LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    redVal = redVal + analogRead(color_sensor);
  }
  redVal = redVal / 20;

  digitalWrite(led_red, LOW); // Turn off the red LED
  digitalWrite(led_green, HIGH); // Turn on the green LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    greenVal = greenVal + analogRead(color_sensor);
  }
  greenVal = greenVal / 20;

  digitalWrite(led_green, LOW); // Turn on the red LED
  digitalWrite(led_blue, HIGH); // Turn on the red LED

  for (int i = 0; i < 20; i++) {
    delay(100);
    blueVal = blueVal + analogRead(color_sensor);
  }
  blueVal = blueVal / 20;

  digitalWrite(led_blue, LOW); // Turn on the red LED

  // Calculate the percentages
  redVal = 100 * (redVal - baseRedBlack) / (baseRedWhite - baseRedBlack);
  greenVal = 100 * (greenVal - baseGreenBlack) / (baseGreenWhite - baseGreenBlack);
  blueVal = 100 * (blueVal - baseBlueBlack) / (baseBlueWhite - baseBlueBlack);

  Serial.print("Red = ");
  Serial.print(redVal);
  Serial.println(" %");
  Serial.print("Green = ");
  Serial.print(greenVal);
  Serial.println(" %");
  Serial.print("Blue = ");
  Serial.print(blueVal);
  Serial.println(" %");
  Serial.println();

  if (redVal > greenVal & redVal > blueVal) Serial.println("*** Red...Desert World Found ***");
  else if (greenVal > redVal & greenVal > blueVal) Serial.println("*** Green...Forest World Found ***");
  else Serial.println("*** Blue...Water World Found ***");
  Serial.println();
}

void dispNum(char ledSet, char ledValue) {
  // set the desired latch low
  digitalWrite(ledSet, LOW);
  // shift out the desired number
  shiftOut(shift_data, shift_clk, MSBFIRST, ledValue);
  // set the desired latch high
  digitalWrite(ledSet, HIGH);
}

void dispOff() {
  // Turn all lights off
  dispNum(rpod_latch, 0);
  dispNum(lpod_latch, 0);
  dispNum(disk_left_latch, 0);
  dispNum(disk_right_latch, 0);
}
