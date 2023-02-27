#include <cstdint>
#include <string>

#include "wiic_macros.h"

/* nix */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <mutex>
#include <vector>

using namespace std;

#define MAX_PAYLOAD 32

#define WM_OUTPUT_CHANNEL 0x11
#define WM_INPUT_CHANNEL 0x13

#define WM_SET_REPORT 0x50

/* commands */
#define WM_CMD_LED 0x11
#define WM_CMD_REPORT_TYPE 0x12
#define WM_CMD_RUMBLE 0x13
#define WM_CMD_IR 0x13
#define WM_CMD_SPEAKER_ENABLE 0x14
#define WM_CMD_CTRL_STATUS 0x15
#define WM_CMD_WRITE_DATA 0x16
#define WM_CMD_READ_DATA 0x17
#define WM_CMD_SPEAKER_DATA 0x18
#define WM_CMD_SPEAKER_MUTE 0x19
#define WM_CMD_IR_2 0x1A

/* input report ids */
#define WM_RPT_CTRL_STATUS 0x20
#define WM_RPT_READ 0x21
#define WM_RPT_WRITE 0x22
#define WM_RPT_BTN 0x30
#define WM_RPT_BTN_ACC 0x31
#define WM_RPT_BTN_ACC_IR 0x33
#define WM_RPT_BTN_EXP 0x34
#define WM_RPT_BTN_ACC_EXP 0x35
#define WM_RPT_BTN_IR_EXP 0x36
#define WM_RPT_BTN_ACC_IR_EXP 0x37

#define WM_BT_INPUT 0x01
#define WM_BT_OUTPUT 0x02

typedef struct wiimote_state_t {
  /* wiimote_t */
  unsigned short btns;
} wiimote_state_t;

#define WIIMOTE_PI 3.14159265f

/* Error output macros */
#define WIIC_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

/* Warning output macros */
#define WIIC_WARNING(fmt, ...) \
  fprintf(stderr, "[WARNING] " fmt "\n", ##__VA_ARGS__)

/* Information output macros */
#define WIIC_INFO(fmt, ...) fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__)

#ifdef WITH_WIIC_DEBUG
#define WIIC_DEBUG(fmt, ...) \
  fprintf(stderr, "[DEBUG] " __FILE__ ":%i: " fmt "\n", __LINE__, ##__VA_ARGS__)
#else
#define WIIC_DEBUG(fmt, ...)
#endif

/* Convert between radians and degrees */
#define RAD_TO_DEGREE(r) ((r * 180.0f) / WIIMOTE_PI)
#define DEGREE_TO_RAD(d) (d * (WIIMOTE_PI / 180.0f))

/* Convert to big endian */
#define BIG_ENDIAN_LONG(i) (htonl(i))
#define BIG_ENDIAN_SHORT(i) (htons(i))

#define absf(x) ((x >= 0) ? (x) : (x * -1.0f))
#define diff_f(x, y) ((x >= y) ? (absf(x - y)) : (absf(y - x)))

typedef struct {
  std::string name;
  std::string addr;
} discovered_dev_t;

class Wii {
 public:
  enum ButtonDefs {
    BUTTON_TWO = 0x0001,
    BUTTON_ONE = 0x0002,
    BUTTON_B = 0x0004,
    BUTTON_A = 0x0008,
    BUTTON_MINUS = 0x0010,
    BUTTON_ZACCEL_BIT6 = 0x0020,
    BUTTON_ZACCEL_BIT7 = 0x0040,
    BUTTON_HOME = 0x0080,
    BUTTON_LEFT = 0x0100,
    BUTTON_RIGHT = 0x0200,
    BUTTON_DOWN = 0x0400,
    BUTTON_UP = 0x0800,
    BUTTON_PLUS = 0x1000,
    BUTTON_ZACCEL_BIT4 = 0x2000,
    BUTTON_ZACCEL_BIT5 = 0x4000,
    BUTTON_UNKNOWN = 0x8000,
    BUTTON_ALL = 0x1F9F
  };

  enum EventTypes {
    NONE = 0,
    EVENT,
    STATUS,
    CONNECT,
    DISCONNECT,
    UNEXPECTED_DISCONNECT,
    WIIC_READ_DATA,
    WIIC_NUNCHUK_INSERTED,
    WIIC_NUNCHUK_REMOVED,
    WIIC_CLASSIC_CTRL_INSERTED,
    WIIC_CLASSIC_CTRL_REMOVED,
    WIIC_GUITAR_HERO_3_CTRL_INSERTED,
    WIIC_GUITAR_HERO_3_CTRL_REMOVED,
    WIIC_MOTION_PLUS_INSERTED,
    WIIC_MOTION_PLUS_REMOVED,
    WIIC_BALANCE_BOARD_INSERTED,
    WIIC_BALANCE_BOARD_REMOVED
  };

  Wii();
  ~Wii();

  std::list<discovered_dev_t> DeviceList();
  std::string name(bdaddr_t const &dev) const;

  int Connect(string address);
  void Disconnect();
  int Send(uint8_t report_type, uint8_t *msg, int len);
  int Set_Report_Type();

  int Pool();
  void Propagate_Event(uint8_t event, uint8_t *msg);
  void pressed_buttons(uint8_t *msg);
  void event_status(uint8_t *msg);

  EventTypes GetEvent();

  bool Connected();

  void Set_Leds(int _leds);
  int Get_Leds();

  void Status();
  float GetBatteryLevel();
  int GetHandshakeState();

  bool RumbleEnabled();
  void Rumble(int status);
  void ToggleRumble();

  virtual short Cast(void *Ptr) { return *((short *)(Ptr)); }  // Inlined.

  int isPressed(int Button) { return (Cast(&btns) & Button) == Button; }

  int isHeld(int Button) { return (Cast(&btns_held) & Button) == Button; }

  int isReleased(int Button) {
    return (Cast(&btns_released) & Button) == Button;
  }

  int isJustPressed(int Button) {
    return ((Cast(&btns) & Button) == Button) &&
           ((Cast(&btns_held) & Button) != Button);
  }

  unsigned short btns;          /**< what buttons have just been pressed	*/
  unsigned short btns_held;     /**< what buttons are being held down
                                 */
  unsigned short btns_released; /**< what buttons were just released this
                                 */
 private:
  bdaddr_t bdaddr; /**< bt address
                    */
  int out_sock;    /**< output socket
                    */
  int in_sock;     /**< input socket
                    */

  char bdaddr_str[18]; /**< readable bt address */
  struct wiimote_state_t
      lstate;              /**< last saved state              */
  int state;               /**< various state flags					*/
  int flags;               /**< options flag               */
  int autoreconnect;       /**< auto-reconnect the device in case of unexpected
                              disconnection */
  uint8_t handshake_state; /**< the state of the connection handshake	*/

  uint8_t leds;        /**< currently lit leds        */
  float battery_level; /**< battery level
                        */

  // struct read_req_t* read_req;		/**< list of data read requests
  // */ struct expansion_t exp;			/**< wiimote expansion device
  // */

  // struct accel_t accel_calib;		/**< wiimote accelerometer calibration
  // */ struct vec3b_t accel;			/**< current raw acceleration
  // data			*/
  // struct orient_t orient;			/**< current orientation on each axis
  // (smoothed and unsmoothed)		*/ struct gforce_t gforce;
  // /**< current gravity forces on each axis (smoothed and unsmoothed)	*/ float
  // orient_threshold;			/**< threshold for orient to generate an event
  // */ int accel_threshold;				/**< threshold for accel
  // to generate an event */

  // struct ir_t ir;					/**< IR data
  // */

  EventTypes event;               /**< type of event that occured               */
  uint8_t event_buf[MAX_PAYLOAD]; /**< event buffer
                                   */

  struct timeval
      timestamp; /**< Absolute timestamp (relative to the most recent data) */
  std::mutex(m_);
  int device_id;
  int socket_id;
};