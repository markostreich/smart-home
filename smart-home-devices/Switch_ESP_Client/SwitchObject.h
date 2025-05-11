#ifndef SWITCH_OBJECT
#define SWITCH_OBJECT

#include <string>

/**
 * @brief Struc representing a switch object to transport an on or off command.
 * 
 * The duration represents the time period after which the switch should return
 * from true to false.
 *
 * Example JSON representation:
 * {
 *   "name": "Switch One",
 *   "state": false,
 *   "duration": 0;
 *   "deviceName": "Device One"
 * }
 */
struct SwitchObjectTO {

  std::string name;

  bool state;

  unsigned int duration;

  std::string deviceName;
};

struct SwitchObject {
  SwitchObjectTO object;

  unsigned long stopTime;
  
  unsigned int pin;
};

#endif //SWITCH_OBJECT