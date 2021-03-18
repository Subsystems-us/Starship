// Laser battle is a game of intergalactic battle.
// The enemy ship is flying back and forth across our disk and
// our detection sensors show you where the enemy is located.
// When he is dead ahead (Both 7 LEDs lit), press the button to fire the lasers.
// If you hit him, you will advance to the next enemy ship, but
// be aware, he will be faster. If you miss, the red disk lights will light,
// the level reached will be displayed on the Left Pod, and the game will halt.

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

boolean endRound = false;
long tgtTime = 400;
int tgtPos = 1;
boolean up = true;
int tgtLevel = 1;

void setup() {

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

  endRound = false;

  while (!endRound) {

    boolean fireLaser = false;

    dispDisk(tgtPos);

    long startTime = millis();

    while (millis() < startTime + tgtTime) {
      if (!digitalRead(button_input)) fireLaser = true;
    }

    if (fireLaser) {
      if (tgtPos == 8) {
        hitAnnim();
        endRound = true;
        tgtLevel += 1;
        tgtTime -= 50;
        up = true;
        tgtPos = random(15);
      }  else {
        missAnnim();
        dispNum(lpod_latch, tgtLevel);
        while (1) {} // Die here
      }
    }

    if (up) {
      tgtPos++;
      if (tgtPos > 15) {
        up = false;
        tgtPos = 14;
      }
    } else {
      tgtPos--;
      if (tgtPos < 1) {
        up = true;
        tgtPos = 2;
      }
    }
  }
}

void missAnnim() {
  dispOff();
  fireLaser();
  soundMiss();
}

void hitAnnim() {
  dispOff();
  fireLaser();
  soundHit();
  dispOff();
}

void soundHit() {
  dispNum(lpod_latch, 255);
  dispNum(rpod_latch, 255);
  for (int i = 300; i < 800; i = i + 50) {
    tone(disk_speaker, i, 25);
    delay(25);
  }
}

void soundMiss() {
  dispNum(disk_left_latch, 192);
  dispNum(disk_right_latch, 192);
  for (int i = 800; i > 200; i = i - 50) {
    tone(disk_speaker, i, 25);
    delay(25);
  }
}

void fireLaser() {
  byte vals[9] = {0, 1, 3, 7, 15 , 31, 63, 127, 255};
  byte vals2[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};

  for (int i = 1; i < 9; i++) {
    dispNum(lpod_latch, vals[i]);
    dispNum(rpod_latch, vals[i]);
    delay(150);
  }

  for (int i = 1; i < 8; i++) {
    dispNum(disk_left_latch, vals2[i] );
    dispNum(disk_right_latch, vals2[i]);
    delay(75);
  }

  for (int i = 1; i < 8; i++) {
    dispNum(lpod_latch, 192);
    dispNum(rpod_latch, 192);
    tone(disk_speaker, 700, 40);
    delay(40);
  }
  dispOff();
}

void dispNum(char ledSet, char ledValue) {
  // set the desired latch low
  digitalWrite(ledSet, LOW);
  // shift out the desired number
  shiftOut(shift_data, shift_clk, MSBFIRST, ledValue);
  // set the desired latch high
  digitalWrite(ledSet, HIGH);
}

void dispDisk(int pos) {
  byte vals[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};

  if (pos < 8) {
    dispNum(disk_left_latch, vals[pos]);
    dispNum(disk_right_latch, 0);
  }
  if (pos > 8) {
    dispNum(disk_left_latch, 0);
    dispNum(disk_right_latch, vals[16 - pos]);
  }
  if (pos == 8) {
    dispNum(disk_left_latch, 128);
    dispNum(disk_right_latch, 128);
  }
}

void waitForButton() {
  while (digitalRead(button_input)) {}
  delay(30);
  while (!digitalRead(button_input)) {}
}

void dispOff() {
  // Turn all lights off
  dispNum(rpod_latch, 0);
  dispNum(lpod_latch, 0);
  dispNum(disk_left_latch, 0);
  dispNum(disk_right_latch, 0);
}
