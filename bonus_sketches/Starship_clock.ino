// Turn Your Starship into a clock
// The left disk lights display the hour (in binary)
// The right disk lights display the minute (in binary)
// The Left Power Pod displays the seconds
// The time is automatically inserted at compile time
// using the compiler directive __TIME__

#define shift_data 3
#define shift_clk 4
#define lpod_latch 5
#define rpod_latch 6
#define disk_left_latch 7
#define disk_right_latch 8
#define led_red 9
#define led_blue 10
#define led_yellow 11
#define disk_speaker 12
#define button_input 13
#define temperature_sensor A0
#define light_sensor A1
#define color_sensor A2
#define accl_X A3
#define accl_Y A4
#define accl_Z A5

String timeNow = __TIME__;
int h = timeNow.substring(0, 2).toInt();
int m = timeNow.substring(3, 5).toInt();
int s = timeNow.substring(6).toInt();
long curMils = millis();

void setup() {
  Serial.begin(9600);

  pinMode(led_red, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(shift_data, OUTPUT);
  pinMode(shift_clk, OUTPUT);
  pinMode(lpod_latch , OUTPUT);
  pinMode(rpod_latch , OUTPUT);
  pinMode(disk_left_latch, OUTPUT);
  pinMode(disk_right_latch, OUTPUT);
  pinMode(color_sensor, INPUT);
  pinMode(light_sensor, INPUT);
  pinMode(disk_speaker, OUTPUT);
  pinMode(button_input, INPUT);
  pinMode(temperature_sensor, INPUT);

  // Turn all lights off
  dispOff();

}

void loop() {

  // Display Time
  dispNum(disk_left_latch, h);
  dispNum(disk_right_latch, m);
  dispNum(lpod_latch, s);

  // Wait for 1 second
  while (millis() < curMils) {}
  curMils = millis() + 1000;
  // Add 1 to the seconds
  s++;
  // Adjust time as needed
  if (s == 60) {
    s = 0;
    m++;
  }
  if (m == 60) {
    m = 0;
    h++;
  }
  if (h == 13) h = 1; // non-24 hour time
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
