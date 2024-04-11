// DST Checker to check if any given day of the year is in Daylight Savings Time or not. :)
#define march 3
#define november 11
#define hour 12
int wday;
int month;
int day;
int previousSunday;
int firstSunday;
void setup() {
  Serial.begin(115200);
}
void loop() {
  month = xxx;  // <-- Enter month here (1-12)
  day = xxx;    // <-- Enter day here (1-31)
  wday = xxx;   // <-- Enter weekday here (0-6, where 0 = Sunday)
  previousSunday = day - wday;
  firstSunday = wday - day;
  if (((month > march) && (month < november)) || 
  ((month == march) && (previousSunday >= 8)) || 
  ((month == march) && (day > 14)) || 
  ((month == november) && (firstSunday >= 0) && (day <= 7))) {
    Serial.println("Is DST");
  } else {
    Serial.println("Not DST");
  }
  delay(1000);
}
