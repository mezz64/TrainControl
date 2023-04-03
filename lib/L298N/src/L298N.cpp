/*
  L298N.cpp - Libreria per gestire i movimenti di un motore DC con il modulo
  L298N Autore:   Andrea Lombardo Sito web: http://www.lombardoandrea.com
*/

#include "L298N.h"

typedef void (*CallBackFunction)();

// L298N::L298N(uint8_t pinEnable, uint8_t pinIN1, uint8_t pinIN2) {
//   _pinEnable = pinEnable;
//   _pinIN1 = pinIN1;
//   _pinIN2 = pinIN2;
//   _pwmVal = 100;
//   _isMoving = false;
//   _canMove = true;
//   _lastMs = 0;
//   _direction = STOP;

//   pinMode(_pinEnable, OUTPUT);
//   pinMode(_pinIN1, OUTPUT);
//   pinMode(_pinIN2, OUTPUT);
// }

L298N::L298N(uint8_t pinIN1, uint8_t pinIN2, int pwmHz) {
  _pinEnable = -1;
  _pinIN1 = pinIN1;
  _pinIN2 = pinIN2;
  _pwmVal = 0;  // It's always at the max speed due to jumper on module
  _isMoving = false;
  _canMove = true;
  _lastMs = 0;
  _direction = STOP;
  _pwmHz = pwmHz;

  // PWM id's are absolute so we do a pin check to make sure A & B are different
  if (pinIN1 == 26) {
    _pwm0 = 0;
    _pwm1 = 1;
  } else {
    _pwm0 = 2;
    _pwm1 = 3;
  }
  

  // pinMode(_pinIN1, OUTPUT);
  // pinMode(_pinIN2, OUTPUT);

  ledcSetup(_pwm0, _pwmHz, 8); // pwm channel, freq, resolution 1000
  ledcSetup(_pwm1, _pwmHz, 8);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(_pinIN1, _pwm0);
  ledcAttachPin(_pinIN2, _pwm1);
}

void L298N::setSpeed(unsigned short pwmVal) {
  _pwmVal = pwmVal;
}

unsigned short L298N::getSpeed() {
  return this->isMoving() ? _pwmVal : 0;
}

void L298N::setPWM(int pwmHz) {
  _pwmHz = pwmHz;
  ledcSetup(_pwm0, _pwmHz, 8); // pwm channel, freq, resolution
  ledcSetup(_pwm1, _pwmHz, 8);
}

unsigned long L298N::getPWM() {
  return _pwmHz;
}

void L298N::forward() {
  // digitalWrite(_pinIN1, HIGH);
  // digitalWrite(_pinIN2, LOW);

  // analogWrite(_pinIN1, _pwmVal);

  ledcWrite(_pwm0, _pwmVal);
  ledcWrite(_pwm1, 0);

  _direction = FORWARD;
  _isMoving = true;
}

void L298N::backward() {
  // digitalWrite(_pinIN1, LOW);
  // // digitalWrite(_pinIN2, HIGH);
  
  // analogWrite(_pinIN2, _pwmVal);

  ledcWrite(_pwm0, 0);
  ledcWrite(_pwm1, _pwmVal);

  _direction = BACKWARD;
  _isMoving = true;
}

void L298N::run(L298N::Direction direction) {
  switch (direction) {
    case BACKWARD:
      this->backward();
      break;
    case FORWARD:
      this->forward();
      break;
    case STOP:
      this->stop();
      break;
  }
}

// Timing and callback

void L298N::runFor(unsigned long delay,
                   L298N::Direction direction,
                   CallBackFunction callback) {
  if ((_lastMs == 0) && _canMove) {
    _lastMs = millis();

    switch (direction) {
      case FORWARD:
        this->forward();
        break;
      case BACKWARD:
        this->backward();
        break;
      case STOP:
      default:
        this->stop();
        break;
    }
  }

  if (((millis() - _lastMs) > delay) && _canMove) {
    this->stop();
    _lastMs = 0;
    _canMove = false;

    callback();
  }
}

void L298N::runFor(unsigned long delay, L298N::Direction direction) {
  this->runFor(delay, direction, fakeCallback);
}

void L298N::forwardFor(unsigned long delay, CallBackFunction callback) {
  this->runFor(delay, FORWARD, callback);
}

void L298N::forwardFor(unsigned long delay) {
  this->runFor(delay, FORWARD);
}

void L298N::backwardFor(unsigned long delay, CallBackFunction callback) {
  this->runFor(delay, BACKWARD, callback);
}

void L298N::backwardFor(unsigned long delay) {
  this->runFor(delay, BACKWARD);
}

void L298N::stop() {
  // digitalWrite(_pinIN1, LOW);
  // digitalWrite(_pinIN2, LOW);

  ledcWrite(_pwm0, 0);
  ledcWrite(_pwm1, 0);

  // ledcWrite(_pinEnable, 255);

  _direction = STOP;
  _isMoving = false;
}

void L298N::reset() {
  _canMove = true;
}

boolean L298N::isMoving() {
  return _isMoving;
}

L298N::Direction L298N::getDirection() {
  return _direction;
}

void L298N::fakeCallback() {}