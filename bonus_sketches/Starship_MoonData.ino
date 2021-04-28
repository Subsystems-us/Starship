/* This program calculates some important values related to the moon phase
    and position. Use the Serial Monitor and ensure that 'NEWLINE' is selected
    in the dropdown box on the bottom of the window. Enter the Date and the
    program will calculate and display the moon data. It will also show
    the relative illumination of the moon on the LEDs. The moon moving from
    New moon to Full moon shows as the LEDs lighting from the Left Pos Lights
    to the center of the Disk. The LEDs will show from the center disk LEDs to
    the Right Pod LEDs when the moon moves from Full to NEW.
    Visit subsystems.us for more fun Arduino and science projets.
*/

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
void moonData(int tYear, int tMonth, float tDay) {
  int cYear, cMonth;
  if (tMonth < 3) {
    cYear = tYear - 1;
    cMonth = tMonth + 12;
  } else {
    cYear = tYear;
    cMonth = tMonth;
  }
  // Calculate the Julian Date
  int a = cYear / 100;
  int b = 2 - a + (int)(a / 4);
  long c = 365.25 * cYear;
  long d = 30.6001 * (cMonth + 1);
  float JD = b + c + d + tDay + 1720995; // This is actaully calculated as noon on the desired day

  // *** Calc moon data ***

  // Calculate illumination (synodic) phase
  float V = (JD - 2451550.1) / 29.530588853;
  V = V - (int)V;
  if (V < 0) V = V + 1;
  float    AG = V * 29.53; // Moon's age in days
  float    IP = V * 2 * PI; // Convert phase to radians

  // Calculate distance from anomalistic phase
  V = (JD - 2451562.2) / 27.55454988;
  V = V - (int)V;
  if (V < 0) V = V + 1;
  float    DP = V * 2 * PI; // convert to rads
  float    DI = 60.4 - 3.3 * cos(DP) - .6 * cos(2 * IP - DP) - .5 * cos(2 * IP);

  // Calculate latitude from nodal (draconic) phase
  V = (JD - 2451565.2) / 27.212220817;
  V = V - (int)V;
  if (V < 0) V = V + 1;
  float    NP = V * 2 * PI; // convert to rads
  float    LA = 5.1 * sin(NP);

  // Calculate longitude from sidereal motion
  V = (JD - 2451555.8) / 27.321582241;
  V = V - (int)V;
  if (V < 0) V = V + 1;
  float    LO = 360 * V + 6.3 * sin(DP) + 1.3 * sin(2 * IP - DP) + .7 * sin(2 * IP);

  dispOff(); // Turn off all register LEDs

  // Print out results
  Serial.println();
  Serial.print(F("On "));
  Serial.print((int)tMonth);
  Serial.print("/");
  Serial.print((int)tDay);
  Serial.print("/");
  Serial.println((int)tYear);
  Serial.println();
  Serial.print(F("  Moon's age from new (days): "));
  Serial.println(AG);
  Serial.print(F("      Distance (Earth radii): "));
  Serial.println(DI);
  Serial.print(F(" Ecliptic latitude (degrees): "));
  Serial.println(LA);
  Serial.print(F("Ecliptic longitude (degrees): "));
  Serial.println(LO);
  Serial.println();
  Serial.println();

  dispPoint(char(AG + 0.5));
}

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
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

  char buffer[] = {' ', ' ', ' ', ' ', ' '}; // 5 digits

  Serial.println(F("Enter the desired Year"));
  while (!Serial.available()) {} // Wait for characters
  // Serial data is available
  Serial.readBytesUntil('n', buffer, 5);
  int dYear = atoi(buffer);
  for (int i = 0; i < 5; i++) buffer[i] = ' '; // clear the array

  Serial.println(F("Enter the desired Month"));
  while (!Serial.available()) {} // Wait for characters
  // Serial data is available
  Serial.readBytesUntil('n', buffer, 5);
  int dMonth = atoi(buffer);
  for (int i = 0; i < 5; i++) buffer[i] = ' ';

  Serial.println(F("Enter the desired Day"));
  while (!Serial.available()) {} // Wait for characters
  // Serial data is available
  Serial.readBytesUntil('n', buffer, 5);
  float dDay = atof(buffer);

  moonData(dYear, dMonth, dDay);
  Serial.flush();
}

void dispNum(char ledSet, char ledValue) {
  // set the desired latch low
  digitalWrite(ledSet, LOW);
  // shift out the desired number
  shiftOut(shift_data, shift_clk, MSBFIRST, ledValue);
  // set the desired latch high
  digitalWrite(ledSet, HIGH);
}

void dispPoint(char ledValue) {
  byte vals[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};

  if (ledValue >= 1 & ledValue < 8 ) {
    dispNum(lpod_latch, vals[char(ledValue + 1)]);
  }
  if (ledValue > 7 & ledValue < 15 ) {
    dispNum(disk_left_latch, vals[char(ledValue - 7)]);
  }
  if (ledValue == 15 ) {
    dispNum(disk_left_latch, 128);
    dispNum(disk_right_latch, 128);
  }
  if (ledValue > 15 & ledValue < 23 ) {
    dispNum(disk_right_latch, vals[char(23 - ledValue)]);
  }
  if (ledValue > 22 & ledValue < 30 ) {
    dispNum(rpod_latch, vals[char(31 - ledValue)]);
  }
  if (ledValue < 1 | ledValue > 29 ) {
    dispNum(lpod_latch, 1);
    dispNum(rpod_latch, 1);
  }
}

void dispOff() {
  // Turn all lights off
  dispNum(rpod_latch, 0);
  dispNum(lpod_latch, 0);
  dispNum(disk_left_latch, 0);
  dispNum(disk_right_latch, 0);
}
