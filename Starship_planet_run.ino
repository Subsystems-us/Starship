// This program simulates flying through the solar system from the sun to Neptune.
// The Starship shields skip on the planets atmosphere when it passes and displays
// the name of the planet in the monitor.

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

  Serial.println("Ready for launch ...");

  while (digitalRead(button_input)) {}
  Serial.println("Start...");

  delay(796);
  diskSweep(25);
  dispNum(rpod_latch, 1);
  dispNum(lpod_latch, 1);
  Serial.println("Mercury");
  tone(disk_speaker, 500, 250);

  delay(454); //We subtract the time to diskSweep (654-200)ms
  diskSweep(25); // 25ms x 8 leds is 200ms
  dispNum(rpod_latch, 2);
  dispNum(lpod_latch, 2);
  Serial.println("Venus");
  tone(disk_speaker, 550, 250);

  delay(360);
  diskSweep(25);
  dispNum(rpod_latch, 4);
  dispNum(lpod_latch, 4);
  Serial.println("Earth");
  tone(disk_speaker, 600, 250);

  delay(850);
  diskSweep(25);
  dispNum(rpod_latch, 8);
  dispNum(lpod_latch, 8);
  Serial.println("Mars");
  tone(disk_speaker, 650, 250);

  delay(6940);
  diskSweep(25);
  dispNum(rpod_latch, 16);
  dispNum(lpod_latch, 16);
  Serial.println("Jupiter");
  tone(disk_speaker, 700, 250);

  delay(9600);
  diskSweep(25);
  dispNum(rpod_latch, 32);
  dispNum(lpod_latch, 32);
  Serial.println("Saturn");
  tone(disk_speaker, 750, 250);

  delay(19500);
  diskSweep(25);
  dispNum(rpod_latch, 64);
  dispNum(lpod_latch, 64);
  Serial.println("Uranus");
  tone(disk_speaker, 800, 250);

  delay(20100);
  diskSweep(25);
  dispNum(rpod_latch, 128);
  dispNum(lpod_latch, 128);
  Serial.println("Neptune");
  tone(disk_speaker, 850, 250);
}

void loop() {
}

void dispNum(char ledSet, char ledValue) {
  digitalWrite(ledSet, LOW);
  shiftOut(shift_data, shift_clk, MSBFIRST, ledValue);
  digitalWrite(ledSet, HIGH);
}

void dispOff() {
  // Turn all lights off
  dispNum(rpod_latch, 0);
  dispNum(lpod_latch, 0);
  dispNum(disk_left_latch, 0);
  dispNum(disk_right_latch, 0);
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
