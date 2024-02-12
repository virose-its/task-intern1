#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <ESP32Servo.h>
#include <RTClib.h>
#include <Wire.h>

int countdown;
int i = 0;
int degree;

/* OLED Display */
#define SCREEN_WIDTH 128                                        // OLED width,  in pixels
#define SCREEN_HEIGHT 64                                        // 0OLED height, in pixels
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  // create an OLED display object connected to I2C

/* Servo */
const int servoPin = 4;  // Servo Pin
Servo servo;             // Servo Object
int pos = 0;             // Servo Position

/* Push Button */
#define GREEN_BUTTON_PIN 19
#define RED_BUTTON_PIN 18
#define BLUE_BUTTON_PIN 5

int green_button_state;
int red_button_state;
int blue_button_state;

/* LED */
#define GREEN_LED_PIN 16
#define RED_LED_PIN 17

int green_led_state = LOW;
int red_led_state = LOW;

/* RTC Clock */
RTC_DS1307 rtc;
char namaHari[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

/* Speaker */
#define SPEAKER_PIN 2

/* DHT Input (Suhu-Humidity) */
const int DHT_PIN = 15;
DHTesp dhtSensor;

/* LDR Input (Kecerahan) */
#define LDR_PIN 13
const float GAMMA = 0.7;
const float RL10 = 50;

void setup() {
    Serial.begin(9600);  // Serial Monitor

    /* Setup OLED */
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {          // initialize OLED display with I2C address 0x3C
        Serial.println(F("failed to start SSD1306 OLED"));  // Handler
    }

    oled.clearDisplay();  // clear display

    oled.setTextSize(1);       // set text size
    oled.setTextColor(WHITE);  // set text color
    oled.setCursor(0, 2);      // set position to display (x,y)
    oled.println("VI-ROSE");   // set text
    oled.display();            // display on OLED

    /* Setup Servo */
    servo.attach(servoPin, 500, 2400);

    /* Setup Button */
    pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);  // set ESP32 pin to input pull-up mode
    pinMode(RED_BUTTON_PIN, INPUT_PULLUP);    // set ESP32 pin to input pull-up mode
    pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);   // set ESP32 pin to input pull-up mode

    green_button_state = digitalRead(GREEN_BUTTON_PIN);
    red_button_state = digitalRead(RED_BUTTON_PIN);
    blue_button_state = digitalRead(BLUE_BUTTON_PIN);

    /* Setup LED */
    pinMode(GREEN_LED_PIN, OUTPUT);  // set ESP32 pin to output mode
    pinMode(RED_LED_PIN, OUTPUT);    // set ESP32 pin to output mode

    /* Setup RTC CLock */
    if (!rtc.begin()) {
        Serial.println("RTC TIDAK TERBACA");
    }
    if (!rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // update rtc dari waktu komputer
    }

    /* Setup Speaker */
    pinMode(SPEAKER_PIN, OUTPUT);

    /* Setup DHT Sensor */
    dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

    /* Setup LDR */
    pinMode(LDR_PIN, INPUT);
}

void loop() {
    /* Example Loop Servo */
    if (millis() / 3000 == i) {
        degree = random(1, 180);
        servo.write(degree);
        i++;
    }
    String servo = "Servo " + String(degree) + " derajat\n\n";

    /* Example control button */
    green_button_state = digitalRead(GREEN_BUTTON_PIN);  // read new state
    red_button_state = digitalRead(RED_BUTTON_PIN);      // read new state
    blue_button_state = digitalRead(BLUE_BUTTON_PIN);    // read new state

    /* Example control LED and Speaker */
    if (green_button_state == LOW) {
        green_led_state = !green_led_state;
        digitalWrite(GREEN_LED_PIN, green_led_state);  // control LED arccoding to the toggled state
        tone(SPEAKER_PIN, 500);
    } else if (red_button_state == LOW) {
        red_led_state = !red_led_state;
        digitalWrite(RED_LED_PIN, red_led_state);  // control LED arccoding to the toggled state
        noTone(SPEAKER_PIN);
    }

    /* Example RTC Clock */
    DateTime now = rtc.now();
    String waktu = namaHari[now.dayOfTheWeek()] + ',' + String(now.day()) + '/' + String(now.month()) + '/' + String(now.year()) + ' ' + String(now.hour()) + ':' + String(now.minute()) + ':' + String(now.second()) + "\n\n";

    /* Example DHT Sensor */
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    String temperatureString = "Temp: " + String(data.temperature, 2) + "°C\n";
    String humidityString = "Humidity: " + String(data.humidity, 1) + "%\n";

    /* Example LDR */
    int analogValue = analogRead(LDR_PIN);
    float voltage = analogValue / 1024. * 5;
    float resistance = 2000 * voltage / (1 - voltage / 5);
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
    String ldrString = "LDR: " + String(lux, 2) + " lux\n";

    /* Example OLED */
    String oledDisplayString = servo + waktu + temperatureString + humidityString + ldrString;
    oled.setTextSize(1);       // set text size
    oled.setTextColor(WHITE);  // set text color
    oled.setCursor(0, 2);      // set position to display (x,y)

    oled.clearDisplay();              // clear previous OLED
    oled.println(oledDisplayString);  // Display the string on OLED
    oled.display();                   // display on OLED
}