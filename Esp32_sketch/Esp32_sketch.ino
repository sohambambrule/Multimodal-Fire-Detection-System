
#define BLYNK_TEMPLATE_ID "TMPL3n7S4-RSQ"
#define BLYNK_TEMPLATE_NAME "firealarm"
#define BLYNK_AUTH_TOKEN "NEgfuymAAWassdhi0aiitv-2D06Xggr6"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

char ssid[] = "";
char pass[] = "";

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Pin Definitions
#define MQ2_PIN       34
#define FLAME_PIN     27
#define BUZZER_PIN    26
#define RED_LED       25
#define GREEN_LED     33

// Thresholds
float tempThreshold = 40.0;
int gasThreshold = 1500;

// State Tracking
bool previousFireState = false;

void setup()
{
  Serial.begin(115200);

  pinMode(FLAME_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  // Initialize SHT31
  if (!sht31.begin(0x44))
  {
    Serial.println("SHT31 not detected!");

    // Try 0x45 if needed
    // if (!sht31.begin(0x45))

    while (1)
    {
      delay(100);
    }
  }

  Serial.println("Connecting to WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println(" FIRE DETECTION SYSTEM READY ");
}

void loop()
{
  Blynk.run();

  // Read Sensors
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  int gasValue = analogRead(MQ2_PIN);
  int flameValue = digitalRead(FLAME_PIN);

  bool fireDetected = false;

  // Flame Detection
  // Most flame sensor modules output LOW when flame is detected
  if (flameValue == LOW)
  {
    fireDetected = true;
  }

  // Gas + Temperature Detection
  if (temperature > tempThreshold &&
      gasValue > gasThreshold)
  {
    fireDetected = true;
  }

  // Alarm Logic
  if (fireDetected)
  {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    // Active Buzzer ON
    digitalWrite(BUZZER_PIN, HIGH);

    if (!previousFireState)
    {
      Blynk.logEvent(
        "fire_alert",
        "FIRE DETECTED! Check immediately."
      );

      Serial.println("FIRE_DETECTED");
      Serial.println("FIRE ALERT SENT");
    }
  }
  else
  {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    // Active Buzzer OFF
    digitalWrite(BUZZER_PIN, LOW);

    if (previousFireState)
    {
      Serial.println("SAFE");
    }
  }

  previousFireState = fireDetected;

  // Send Data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, gasValue);
  Blynk.virtualWrite(V3, (flameValue == LOW) ? 1 : 0);
  Blynk.virtualWrite(V4, fireDetected ? 1 : 0);

  // Serial Monitor
  Serial.println("--------------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas Level: ");
  Serial.println(gasValue);

  Serial.print("Flame Value: ");
  Serial.println(flameValue);

  Serial.print("Fire Status: ");
  Serial.println(fireDetected ? "YES" : "NO");

  delay(1000);
}
