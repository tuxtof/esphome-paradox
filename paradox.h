#include "esphome.h"

class ParadoxUnit : public Component, public UARTDevice, public CustomAPIDevice {
 public:
  ParadoxUnit(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
    // nothing to do here
    register_service(&ParadoxUnit::arm_alarm, "arm_alarm", {"code"});
    register_service(&ParadoxUnit::disarm_alarm, "disarm_alarm", {"code"});
    register_service(&ParadoxUnit::status_alarm, "status_alarm", {"zone"});
    id(paradox_state_alarm).publish_state("pending");
    write_str("RA001\r");

  }

  int readline(int readch, char *buffer, int len)
  {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
      switch (readch) {
        case '\n': // Ignore new-lines
          break;
        case '\r': // Return on CR
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        default:
          if (pos < len-1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  void loop() override {
    const int max_line_length = 80;
    static char buffer[max_line_length];

    while (available()) {
      if(readline(read(), buffer, max_line_length) > 0) {

        ESP_LOGD("paradox buffer", buffer);

        if (strcmp(buffer,"G001N003A001") == 0) id(paradox_motion_cuisine).publish_state(true);
        else if (strcmp(buffer,"G000N003A001") == 0) id(paradox_motion_salon).publish_state(false);
        else if (strcmp(buffer,"G001N004A001") == 0) id(paradox_motion_salon).publish_state(true);
        else if (strcmp(buffer,"G000N004A001") == 0) id(paradox_motion_salon).publish_state(false);
        else if (strcmp(buffer,"G001N005A001") == 0) id(paradox_motion_palier).publish_state(true);
        else if (strcmp(buffer,"G000N005A001") == 0) id(paradox_motion_palier).publish_state(false);
        else if (strcmp(buffer,"G001N006A001") == 0) id(paradox_motion_ssol).publish_state(true);
        else if (strcmp(buffer,"G000N006A001") == 0) id(paradox_motion_ssol).publish_state(false);

        else if (strcmp(buffer,"G001N020A001") == 0) id(paradox_eau_buanderie).publish_state(true);
        else if (strcmp(buffer,"G000N020A001") == 0) id(paradox_eau_buanderie).publish_state(false);
        else if (strcmp(buffer,"G001N021A001") == 0) id(paradox_eau_garage).publish_state(true);
        else if (strcmp(buffer,"G000N021A001") == 0) id(paradox_eau_garage).publish_state(false);
        else if (strcmp(buffer,"G001N022A001") == 0) id(paradox_eau_chauffe_eau).publish_state(true);
        else if (strcmp(buffer,"G000N022A001") == 0) id(paradox_eau_chauffe_eau).publish_state(false);

        else if (strcmp(buffer,"G001N030A001") == 0) id(paradox_porte_garage).publish_state(true);
        else if (strcmp(buffer,"G000N030A001") == 0) id(paradox_porte_garage).publish_state(false);
        else if (strcmp(buffer,"G001N031A001") == 0) id(paradox_portail_garage).publish_state(true);
        else if (strcmp(buffer,"G000N031A001") == 0) id(paradox_portail_garage).publish_state(false);
        else if (strcmp(buffer,"G001N032A001") == 0) id(paradox_porte_maison).publish_state(true);
        else if (strcmp(buffer,"G000N032A001") == 0) id(paradox_porte_maison).publish_state(false);

        else if (strncmp(buffer,"G064N000",8) == 0) id(paradox_state_alarm).publish_state("armed_away");
        else if (strncmp(buffer,"RA001A",6) == 0) id(paradox_state_alarm).publish_state("armed_away");

        else if (strncmp(buffer,"G065N001",8) == 0) id(paradox_state_alarm).publish_state("arming");
        
        else if (strncmp(buffer,"G013N",5) == 0) id(paradox_state_alarm).publish_state("disarmed");
        else if (strncmp(buffer,"G014N",5) == 0) id(paradox_state_alarm).publish_state("disarmed");
        else if (strncmp(buffer,"RA001D",6) == 0) id(paradox_state_alarm).publish_state("disarmed");

        else if (strncmp(buffer,"G064N005",8) == 0) id(paradox_state_alarm).publish_state("triggered");
        else if (strncmp(buffer,"G064N006",8) == 0) id(paradox_state_alarm).publish_state("triggered");
        else if (strncmp(buffer,"G064N007",8) == 0) id(paradox_state_alarm).publish_state("triggered");
      }
    }
  }

  void arm_alarm(int code) {
    char maString[20];
    sprintf(maString,"AA001A%i\r",code);
    write_str(maString);
  }

  void disarm_alarm(int code) {
    char maString[20];
    sprintf(maString,"AD001A%i\r",code);
    write_str(maString);
  }

//   void status_alarm(std::string zone) {
//     write_str("RA001\r");
//   }

};
