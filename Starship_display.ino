// This program includes all of the needed display functions,
// #defines, and setup code.
// You can use this as the starting point for many programs.

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
}

void loop() {
}

void dispNum(char ledSet, char ledValue) {
  // set the desired latch low
  digitalWrite(ledSet, LOW);
  // shift out the desired number
  shiftOut(shift_data, shift_clk, MSBFIRST, ledValue);
  // set the desired latch high
  digitalWrite(ledSet, HIGH);
}

void dispPoint(char ledSet, char ledValue) {
  byte vals[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
  dispNum(ledSet, vals[ledValue]);
}

void dispBar(char ledSet, char ledValue) {
  byte vals[9] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
  dispNum(ledSet, vals[ledValue]);
}

void dispOff() {
  // Turn all lights off
  dispNum(rpod_latch, 0);
  dispNum(lpod_latch, 0);
  dispNum(disk_left_latch, 0);
  dispNum(disk_right_latch, 0);
}

// This will light the lights in sequence. ledSet is the set of LEDs to use,
// swpDir is true for forward, false for reverse. swpSpeed is the msec time
// between each LED.
void dispSweep(char ledSet, boolean swpDir, long swpSpeed) {
  int num = B10000000;
  if (swpDir) num = B00000001;

  for (int i = 0; i < 8; i++) {
    dispNum(ledSet, num);
    if (swpDir) num = num * 2;
    else num = num / 2;
    delay(swpSpeed);
  }
  dispNum(ledSet, 0);
}

// This will sweep the disk lights from forward to back.
void diskSweep(long swpSpeed) {
  int num = B10000000;

  for (int i = 0; i < 9; i++) {
    dispNum(disk_left_latch, num);
    dispNum(disk_right_latch, num);
    num = num / 2;
    delay(swpSpeed);
  }
}
