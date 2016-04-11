#include <Arduino_FreeRTOS.h>
#include <croutine.h>
#include <event_groups.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <StackMacros.h>
#include <task.h>
#include <timers.h>
// Licensed under 
//   __________________.____            ________  
//  /  _____/\______   \    |     ___  _\_____  \ 
// /   \  ___ |     ___/    |     \  \/ / _(__  < 
// \    \_\  \|    |   |    |___   \   / /       \
//  \______  /|____|   |_______ \   \_/ /______  /
//         \/                  \/              \/ 
// More info: http://www.gnu.org/licenses/gpl-3.0.en.html

// #include <FreeRTOS_AVR.h>


int runMe = 0;

// First set
const uint8_t RED_PIN = 9;
const uint8_t YEL_PIN = 8;
const uint8_t GRN_PIN = 7;
// Second set
const uint8_t RED2_PIN = 5;
const uint8_t YEL2_PIN = 3;
const uint8_t GRN2_PIN = 6;

const uint8_t READ_PIN = 2;
// delays
const uint32_t orangeWait = 500;
const uint32_t greenWait = 2000;
const uint32_t yellowWait = 1000;

bool is1going = false;
bool is2going = false;
int redToOrange = 460 / portTICK_PERIOD_MS;
int orangeToGreen = 960 / portTICK_PERIOD_MS;
int greenToYellow = 2960 / portTICK_PERIOD_MS;
int yellowToRed = 960 / portTICK_PERIOD_MS;

int i = 0;

int maxWaitTime = 
    redToOrange +
    orangeToGreen +
    greenToYellow +
    yellowToRed;

SemaphoreHandle_t sem;

static void Thread1(void* arg) {
  while (1) {
    i = digitalRead(READ_PIN);
    Serial.print("1: ");
    Serial.println(i);
    if (i == LOW) {
      Serial.println("1. LOW.");
      red(1);
      vTaskDelay(redToOrange);
      orange(1);
      vTaskDelay(orangeToGreen);
      green(1);
      vTaskDelay(greenToYellow);
      yellow(1);
      vTaskDelay(yellowToRed);
      red(1);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      Serial.println("1. End.");
      // Send signal.
      xSemaphoreGive(sem);
      // Wait for signal.
      xSemaphoreTake(sem, maxWaitTime);
    }
  }
}

static void Thread2(void* arg) {
  while (1) {
    i = digitalRead(READ_PIN);
    Serial.print("2: ");
    Serial.println(i);
    if (i == HIGH) {
      Serial.println("2. HIGH.");
      red(2);
      vTaskDelay(redToOrange);
      orange(2);
      vTaskDelay(orangeToGreen);
      green(2);
      vTaskDelay(greenToYellow);
      yellow(2);
      vTaskDelay(yellowToRed);
      red(2);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      Serial.println("2. End.");
      xSemaphoreGive(sem);
      xSemaphoreTake(sem, maxWaitTime);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(READ_PIN, INPUT);
  pinMode(A0, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YEL_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(RED2_PIN, OUTPUT);
  pinMode(YEL2_PIN, OUTPUT);
  pinMode(GRN2_PIN, OUTPUT);
  red(1);
  red(2);
  portBASE_TYPE s1, s2;
  // initialize semaphore
  sem = xSemaphoreCreateCounting(1, 0);
  s1 = xTaskCreate(Thread1, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  s2 = xTaskCreate(Thread2, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  // check for creation errors
  if (sem== NULL || s1 != pdPASS || s2 != pdPASS) {
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
