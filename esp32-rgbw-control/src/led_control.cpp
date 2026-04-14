##include "led_control.h"
#include "config#include "led_control.h"
#include "config.h"
#include <Arduino.h>
#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B,#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledc#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000,#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);
  
  ledcAttachPin(LED_PIN_R, 0);
  ledcAttachPin(LED_PIN_G#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);
  
  ledcAttachPin(LED_PIN_R, 0);
  ledcAttachPin(LED_PIN_G, 1);
  ledcAttach#include "led_control.h"
#include "config.h"
#include <Arduino.h>

Color currentColor = {0, 0, 0, 0, 255};
LedMode currentMode = MODE_SOLID;
unsigned long lastUpdate = 0;
int breatheValue = 0;
bool breatheDirection = true;

void initLED() {
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_W, OUTPUT);
  
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);
  
  ledcAttachPin(LED_PIN_R, 0);
  ledcAttachPin(LED_PIN_G, 1);
  ledcAttachPin(LED_PIN_B, 2);
  ledcAttachPin(LED_PIN_W