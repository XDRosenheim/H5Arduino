// Licensed under 
//   __________________.____            ________  
//  /  _____/\______   \    |     ___  _\_____  \ 
// /   \  ___ |     ___/    |     \  \/ / _(__  < 
// \    \_\  \|    |   |    |___   \   / /       \
//  \______  /|____|   |_______ \   \_/ /______  /
//         \/                  \/              \/ 
// More info: http://www.gnu.org/licenses/gpl-3.0.en.html

#include <FreeRTOS_AVR.h>
#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>

// First set
const uint8_t RED_PIN = 9;
const uint8_t YEL_PIN = 8;
const uint8_t GRN_PIN = 7;
// Second set
const uint8_t RED2_PIN = 5;
const uint8_t YEL2_PIN = 3;
const uint8_t GRN2_PIN = 6;
// delays
const uint32_t orangeWait = 500;
const uint32_t greenWait = 2000;
const uint32_t yellowWait = 1000;

bool is1going = false;
bool is2going = false;
int redToOrange = 3000;
int orangeToGreen = 1000;
int greenToYellow = 3000;
int yellowToRed = 500;

int maxWaitTime = 
    redToOrange +
    orangeToGreen +
    greenToYellow +
    yellowToRed;

byte mac[] = { 0x90, 0xA2, 0xda, 0x00, 0x68, 0xA3 };
IPAddress ip(192, 168, 1, 2);
EthernetServer server(80);

SemaphoreHandle_t sem;

static void Thread1(void* arg) {
  while (1) {
    // Wait for signal from 2.
    xSemaphoreTake(sem, portMAX_DELAY);
  	red(1);
  	vTaskDelay(redToOrange);
  	orange(1);
    vTaskDelay(orangeToGreen);
    green(1);
    vTaskDelay(greenToYellow);
    yellow(1);
    vTaskDelay(yellowToRed);
    red(1);
    vTaskDelay(20);
    // Send signal.
    xSemaphoreGive(sem);
    // Wait for signal.
    xSemaphoreTake(sem, maxWaitTime);
  }
}

static void Thread2(void* arg) {
  while (1) {
    red(2);
    vTaskDelay(redToOrange);
    orange(2);
    vTaskDelay(orangeToGreen);
    green(2);
    vTaskDelay(greenToYellow);
    yellow(2);
    vTaskDelay(yellowToRed);
    red(2);
    vTaskDelay(20);
    xSemaphoreGive(sem);
    xSemaphoreTake(sem, maxWaitTime);
  }
}

static void Thread3(void* arg) {
  while (1) {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
      Serial.println("new client");
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            // output the value of each analog input pin
            for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
              int sensorReading = analogRead(analogChannel);
              client.print("analog input ");
              client.print(analogChannel);
              client.print(" is ");
              client.print(sensorReading);
              client.println("<br />");
            }
            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YEL_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(RED2_PIN, OUTPUT);
  pinMode(YEL2_PIN, OUTPUT);
  pinMode(GRN2_PIN, OUTPUT);
  Ethernet.begin(mac, ip);
  W5100.setRetransmissionTime(0x07D0);
  W5100.setRetransmissionCount(1);
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  red(1);
  red(2);
  portBASE_TYPE s1, s2, s3;
  // initialize semaphore
  sem = xSemaphoreCreateCounting(1, 0);
  s1 = xTaskCreate(Thread1, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  s2 = xTaskCreate(Thread2, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  s3 = xTaskCreate(Thread3, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);

  // check for creation errors
  if (sem== NULL || s1 != pdPASS || s2 != pdPASS || s3 != pdPASS) {
    Serial.println(F("Creation problem"));
    while(1);
  }
  // start scheduler
  vTaskStartScheduler();
  Serial.println(F("Insufficient RAM"));
  while(1);
}

void loop() {
}

void red(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YEL_PIN, LOW);
    digitalWrite(GRN_PIN, LOW);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, HIGH);
    digitalWrite(YEL2_PIN, LOW);
    digitalWrite(GRN2_PIN, LOW);
  }
}

void yellow(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YEL_PIN, HIGH);
    digitalWrite(GRN_PIN, LOW);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, LOW);
    digitalWrite(YEL2_PIN, HIGH);
    digitalWrite(GRN2_PIN, LOW);
  }
}

void green(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YEL_PIN, LOW);
    digitalWrite(GRN_PIN, HIGH);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, LOW);
    digitalWrite(YEL2_PIN, LOW);
    digitalWrite(GRN2_PIN, HIGH);
  }
}

void orange(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YEL_PIN, HIGH);
    digitalWrite(GRN_PIN, LOW);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, HIGH);
    digitalWrite(YEL2_PIN, HIGH);
    digitalWrite(GRN2_PIN, LOW);
  }
}
