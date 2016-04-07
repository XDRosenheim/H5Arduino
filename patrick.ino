#include <FreeRTOS_AVR.h>
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

long time = 0;
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

SemaphoreHandle_t sem;

static void Thread1(void* arg) {
  while (1) {
    // Wait for signal from 2.
    xSemaphoreTake(sem, portMAX_DELAY);
    // If 2 is still running, wait.
    while ( is2going ) {
      Serial.println("1: Waiting for 2.");
    }

    is1going = true;
  	//Serial.println("1: Start");
    // Turn LED off.
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
    is1going = false;
    //Serial.println("1: End");
  	if ( ! is1going ) {
      // Send signal.
      //Serial.println("1: Ping.");
      xSemaphoreGive(sem);
    }
    // Wait for signal from 2.
    xSemaphoreTake(sem, maxWaitTime);
  }
}
static void Thread2(void* arg) {
  while (1) {
    // If 2 is still running, wait.
    while ( is1going ) {
      Serial.println("2: Waiting for 1.");
    }

    is2going = true;
    Serial.println("2: Start");
    // Turn LED off.
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
    is2going = false;
    Serial.println("2: End");
    if ( ! is2going ) {
      // Send signal.
      // Serial.println("2: Ping.");
      xSemaphoreGive(sem);
    }
    // Wait for signal from 2.
    xSemaphoreTake(sem, maxWaitTime);
  }
}

void setup() {
  time = millis();
  Serial.begin(9600);
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
  // create task at priority two
  s1 = xTaskCreate(Thread1, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  // create task at priority one
  s2 = xTaskCreate(Thread2, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  // check for creation errors
  if (sem== NULL || s1 != pdPASS || s2 != pdPASS ) {
    Serial.println(F("Creation problem"));
    while(1);
  }
  // start scheduler
  vTaskStartScheduler();
  Serial.println(F("Insufficient RAM"));
  while(1);
}

void loop() {
  // time = millis();
  // if ( time % 1000 == 0 && time > 0) {
  //   Serial.println(millis());
  // }
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
