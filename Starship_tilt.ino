// This program calibrates the accelerometer and then
// Acts as a tilt sensor to show the relative angle
// in the X and Y axis directions.

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

int maxX = 0;
int maxY = 0;
int maxZ = 0;
int minX = 1024;
int minY = 1024;
int minZ = 1024;

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

  Serial.begin(9600);
  Serial.println("Move the Starship through 360 degrees");
  Serial.println("Press the button when done");

  int tempAcc;
  while (digitalRead(button_input)) {
    tempAcc = analogRead(accl_X);
    if (tempAcc > maxX) maxX = tempAcc;
    if (tempAcc < minX) minX = tempAcc;
    tempAcc = analogRead(accl_Y);
    if (tempAcc > maxY) maxY = tempAcc;
    if (tempAcc < minY) minY = tempAcc;
    tempAcc = analogRead(accl_Z);
    if (tempAcc > maxZ) maxZ = tempAcc;
    if (tempAcc < minZ) minZ = tempAcc;
  }

  Serial.println("Calibration Complete.");
}

void loop() {
  int curVal = analogRead(accl_X);
  acclLevelX(curVal);
  curVal = analogRead(accl_Y);
  acclLevelY(curVal);
  delay(100);
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

void acclLevelX(int curLevel) {
  byte vals[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
  if (curLevel > maxX) curLevel = maxX;
  if (curLevel < minX) curLevel = minX;
  int diffX = (curLevel - minX) / ((maxX - minX) / 8);
  dispNum(lpod_latch, vals[diffX]);
  dispNum(rpod_latch, vals[diffX]);
}

void acclLevelY(int curLevel) {
  byte vals[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
  if (curLevel > maxX) curLevel = maxX;
  if (curLevel < minX) curLevel = minX;
  int diffY = (curLevel - minX) / ((maxX - minX) / 16);
  dispNum(disk_left_latch, 0);
  dispNum(disk_right_latch, 0);
  if (diffY < 9)  dispNum(disk_right_latch, vals[diffY]);
  else  dispNum(disk_left_latch, vals[17 - diffY]);
}
