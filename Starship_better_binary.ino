// This program displays a binary number on the Left Power Pod
// The user then enters a decimal number into the serial monitor
// The computer then displays whether the user converted the
// number correctly or not.

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

  randomSeed(analogRead(light_sensor)); // Randomize our number generator

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

  Serial.println("Ready to play ...");
}

void loop() {
  // Turn all lights off
  dispOff();

  int binaryNum = random(256);

  // Display it on the Left Power Pod
  dispNum(lpod_latch, binaryNum);

  // Prompt the user for the guess
  Serial.println("Enter your guess.");
  byte myGuess = getSerialData();

  // display the guess on the Right Pod
  dispNum(rpod_latch, myGuess);
  delay(2000);

  // Did we get it right?
  if (myGuess == binaryNum) {
    // We were right!
    diskSweep(50);
    dispNum(disk_right_latch, B00000111); // Display Green Lights
    dispNum(disk_left_latch, B00000111);
    Serial.println("Correct!!");
  } else {
    dispNum(disk_right_latch, B11000000); // Display Red Lights
    dispNum(disk_left_latch, B11000000);
    tone(disk_speaker, 500, 1000); // sound sad tone
    Serial.print("Sorry... the correct number was ");
    Serial.println(binaryNum);
  }
  delay(3000); // Allow the user to see the results for 3 seconds
}

byte getSerialData() {
  char receivedChars[4];   // an array to store the received data
  char charNum = 0; //Track which digit has been received
  boolean notDone = true; // Allow us to exit when the data is done
  char rc; // This will hold the character received

  while (Serial.available() == 0) {} // Wait until Serial data appears

  while (Serial.available() > 0 && notDone) { // while there is Serial data ...
    rc = Serial.read();

    delay(100); // This makes sure we don't miss data

    if (rc != '\n') {
      receivedChars[charNum] = rc;
      charNum++;
    }
    else {
      receivedChars[charNum] = '\0'; // terminate the string
      notDone = false;
    }
  }
  return atoi(receivedChars);
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

void diskSweep(long swpSpeed) {
  int num = B10000000;

  for (int i = 0; i < 9; i++) {
    dispNum(disk_left_latch, num);
    dispNum(disk_right_latch, num);
    num = num / 2;
    delay(swpSpeed);
  }
}
