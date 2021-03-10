// This program simulates the landing of our Starship
// on a planet. The Disk lights show the current velocity, the
// left disk lights are the negative velocity while the right
// lights are positive. The left and right power pods show the
// remaining fuel. Pressing the button fires the decent control
// rockets. Open the Serial Plotter to see the descent in real time.

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

float curFuel = 16.0; // Current Fuel Remaining
float yPos = 10000.0; // Current Altitude
float Accl; // Acceleration terms
float Vel1 = 0.0; // Velocity terms
float Vel2 = 0.0;
const float deltaT = 0.250; // Update time in seconds

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

  Serial.println(0); // Set the lower end of the Plotter
}

void loop() {
  // Output current yPos
  Serial.println(yPos);

  // Check for landing
  if (yPos < 1.0) {
    if (Vel1 > -15.0) {
      // Safe landing
      safeLand();
    } else {
      // Crash
      crashLand();
    }
    while (1) {} // Stop here
  }

  // Calculate next position
  boolean butPush = !digitalRead(button_input) && (curFuel > 0.0);
  if (butPush) curFuel = curFuel - 0.1;
  Accl = -9.8 + butPush * 30;
  Vel2 = Accl * deltaT + Vel1;
  yPos = Accl * (deltaT * deltaT) / 2 + Vel1 * deltaT + yPos;
  Vel1 = Vel2;
  // Display velocity on Disk Lights
  diskVelocity(Vel1);
  fuelLevel(byte(curFuel));

  delay(deltaT * 1000);
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

void fuelLevel(byte curLevel) {
  byte lpod = 0;
  byte rpod = 0;
  byte vals[9] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
  if (curLevel < 9) {
    lpod = 0;
    rpod = vals[curLevel];
  } else {
    rpod = B11111111;
    lpod = vals[curLevel - 8];
  }
  dispNum(lpod_latch, lpod);
  dispNum(rpod_latch, rpod);
}

void diskVelocity(double curVel) {
  byte vals[9] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
  byte dispV = 0;
  boolean neg = false;
  if (curVel < 0) {
    neg = true;
    curVel = curVel * -1.0;
  }
  for (int i = 1; i < 9; i++) {
    if (curVel >= i * 10.0) dispV = vals[i];
  }
  if (neg) {
    dispNum(disk_left_latch, dispV);
    dispNum(disk_right_latch, 0);
  } else {
    dispNum(disk_right_latch, dispV);
    dispNum(disk_left_latch, 0);
  }
}

void diskSweep(long swpSpeed) {
  int num = B10000000;

  for (int i = 0; i < 9; i++) {
    dispNum(disk_left_latch, num);
    dispNum(disk_right_latch, num);
    num = num / 2;
    delay(swpSpeed);
  }
}

void safeLand() {
  // Turn all lights off
  dispOff();
  delay(500);
  for (int i = 0; i < 17; i++) {
    fuelLevel(i);
    delay(100);
  }
  delay(500);
  diskSweep(50);
  tone(disk_speaker, 500, 200);
  diskSweep(50);
  tone(disk_speaker, 700, 200);
  dispNum(disk_right_latch, 7);
  dispNum(disk_left_latch, 7);
}

void crashLand() {
  // Sad light display
  dispOff();
  delay(500);
  for (int i = 16; i >= 0; i--) {
    fuelLevel(i);
    delay(200);
  }
  delay(500);
  tone(disk_speaker, 400, 500);
  dispNum(disk_left_latch, 192);
  dispNum(disk_right_latch, 192);
}
