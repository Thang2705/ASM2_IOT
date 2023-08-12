#define KHIALCOHOL V1
#define RUNMODE V3
#define TRANGTHAICB V4
#define DISPLAY_SENSOR_DATA V1 // New virtual pin for Display Value widget
#define MUCCANHBAO V2

#define BLYNK_TEMPLATE_ID "TMPL6zMcq3J9V"
#define BLYNK_TEMPLATE_NAME "MQ3"

#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG

#include <BlynkSimpleEsp8266.h>
#include <Wire.h>                 //Thư viện giao tiếp I2C
#include <LiquidCrystal_I2C.h>    //Thư viện LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); //Thiết lập địa chỉ và loại LCD

BlynkTimer timer;
int timerID1;
int mq3_value;
int alcoholThreshold = 420; // Change this to adjust the threshold
int ledMode = 12; // D6 led to display operating mode
boolean runMode = 1; // Enable/disable alert mode
boolean doorState = 0;
int buzzer = 5; // D1 buzzer connection

char auth[] = "Z6STspn4lKdbXYeAEb9HnDSVjel4HhJ6";
char ssid[] = "T&B 305";
char pass[] = "8888a8888";

WidgetLED led(KHIALCOHOL);

void setup()
{
  Serial.begin(115200);
  delay(100);
  pinMode(ledMode, OUTPUT);
  digitalWrite(ledMode, LOW); // Turn off the led mode
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW); // Turn off the buzzer
  Blynk.begin(auth, ssid, pass);
  timerID1 = timer.setInterval(1000L, handleTimerID1);

  Blynk.virtualWrite(RUNMODE, runMode); // Initialize the virtual pin value
  Blynk.virtualWrite(TRANGTHAICB, doorState); // Initialize the virtual pin value
  Blynk.virtualWrite(MUCCANHBAO, alcoholThreshold); // Initialize the virtual pin value
  Blynk.virtualWrite(DISPLAY_SENSOR_DATA, mq3_value); // Initialize the Display Value widget with sensor data

  Wire.begin(D4, D3);          //Thiết lập chân kết nối I2C (SDA,SCL);
  lcd.init();                  //Khởi tạo LCD
  lcd.clear();                 //Xóa màn hình
  lcd.backlight();             //Bật đèn nền
  mq3_value = analogRead(A0);

  lcd.setCursor(0, 0);         //Đặt vị trí muốn hiển thị ô thứ 1 trên dòng 1
  lcd.print("Alcohol: ");      //Ghi đoạn text "Alcohol: "
  lcd.print(mq3_value);        //Display the initial alcohol content value
  lcd.setCursor(0, 1);         //Đặt vị trí ở ô thứ 1 trên dòng 2
  lcd.print("Threshold: ");    //Ghi đoạn text "Threshold: "
  lcd.print(alcoholThreshold); 
}

void loop()
{
  Blynk.run();
  timer.run();
  lcd.display();
  delay(1000);
  lcd.noDisplay();
  delay(500);
}

void handleTimerID1()
{
  mq3_value = analogRead(A0);
  if (led.getValue())
  {
    led.off();
  }
  else
  {
    led.on();
  }

  if (runMode == 1)
  {
    if (mq3_value > alcoholThreshold)
    {
      if (doorState == 0)
      {
        doorState = 1;
        digitalWrite(ledMode, HIGH);
        digitalWrite(buzzer, HIGH); // Turn on the buzzer
        Blynk.logEvent("canhbao", "Alert! Alcohol level exceeds threshold"); // Send canhbao event to Blynk app
      }
    }
    else
    {
      if (doorState == 1)
      {
        doorState = 0;
        digitalWrite(ledMode, LOW);
        digitalWrite(buzzer, LOW); // Turn off the buzzer
      }
    }
  }
  else
  {
    doorState = 0;
    digitalWrite(ledMode, LOW);
    digitalWrite(buzzer, LOW); // Turn off the buzzer
  }

  Blynk.virtualWrite(TRANGTHAICB, doorState);
  lcd.setCursor(8, 0); // Set the cursor position to overwrite the previous value
  lcd.print("    "); // Clear the previous value
  lcd.setCursor(8, 0); // Set the cursor position again
  lcd.print(mq3_value); // Display the latest alcohol content value

  Blynk.virtualWrite(DISPLAY_SENSOR_DATA, mq3_value); // Update the Display Value widget with the latest sensor data
}

BLYNK_WRITE(RUNMODE) // This function will be called when the virtual pin V3 is updated
{
  int newValue = param.asInt();
  if (newValue == 1)
  {
    runMode = 1;
    digitalWrite(ledMode, HIGH);
  }
  else
  {
    runMode = 0;
    digitalWrite(ledMode, LOW);
    digitalWrite(buzzer, LOW); // Turn off the buzzer
  }
}

BLYNK_WRITE(MUCCANHBAO) // This function will be called when the virtual pin V2 is updated
{
  alcoholThreshold = param.asInt();
  lcd.setCursor(10, 1); // Set the cursor position to overwrite the previous value
  lcd.print("     "); // Clear the previous value
  lcd.setCursor(10, 1); // Set the cursor position again
  lcd.print(alcoholThreshold); // Display the updated alcohol threshold value on the LCD
}