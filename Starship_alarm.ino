// Sound the alarm if the Ship is hit

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

int sens = 5;

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
  while (digitalRead(button_input)) {}
  for (int i = 0; i < 7; i++) {
    dispNum(lpod_latch, B00111111 >> i);
    delay(800);
  }
  int curX = analogRead(accl_X);
  int curY = analogRead(accl_Y);
  int curZ = analogRead(accl_Z);

  boolean alarm = false;

  while (!alarm) {
    if (analogRead(accl_X) > curX + sens) alarm = true;
    if (analogRead(accl_Y) > curY + sens) alarm = true;
    if (analogRead(accl_Z) > curZ + sens) alarm = true;
    if (analogRead(accl_X) < curX - sens) alarm = true;
    if (analogRead(accl_Y) < curY - sens) alarm = true;
    if (analogRead(accl_Z) < curZ - sens) alarm = true;
  }
  alarmAnim();
}

void alarmAnim() {
  for (int t = 0; t < 3; t++) {
    dispNum(disk_left_latch, 192);
    dispNum(disk_right_latch, 192);
    for (int i = 8; i >= 0; i--) {
      tone(disk_speaker, 600 - i * 20, 25);
      delay(25);
    }
    dispNum(disk_left_latch, 0);
    dispNum(disk_right_latch, 0);
    delay(200);
  }
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
