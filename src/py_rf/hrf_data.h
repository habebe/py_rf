#ifndef __HRF_DATA_INCLUDE_FILE__
#define __HRF_DATA_INCLUDE_FILE__

#include <libusb-1.0/libusb.h>

extern const char HRF_VERSION[];
extern uint16_t HRF_VID;

typedef enum HRF_PID
{
   HRF_ID_JAWBREAKER = 0x604B,
   HRF_ID_HACKRF_ONE = 0x6089,
   HRF_ID_RAD1O = 0xCC15,
   HRF_ID_INVALID = 0xFFFF,
} HRF_PID;

typedef enum HRF_Vendor_Request
{
   HRF_VENDOR_REQUEST_SET_TRANSCEIVER_MODE = 1,
   HRF_VENDOR_REQUEST_MAX2837_WRITE = 2,
   HRF_VENDOR_REQUEST_MAX2837_READ = 3,
   HRF_VENDOR_REQUEST_SI5351C_WRITE = 4,
   HRF_VENDOR_REQUEST_SI5351C_READ = 5,
   HRF_VENDOR_REQUEST_SAMPLE_RATE_SET = 6,
   HRF_VENDOR_REQUEST_BASEBAND_FILTER_BANDWIDTH_SET = 7,
   HRF_VENDOR_REQUEST_RFFC5071_WRITE = 8,
   HRF_VENDOR_REQUEST_RFFC5071_READ = 9,
   HRF_VENDOR_REQUEST_SPIFLASH_ERASE = 10,
   HRF_VENDOR_REQUEST_SPIFLASH_WRITE = 11,
   HRF_VENDOR_REQUEST_SPIFLASH_READ = 12,
   HRF_VENDOR_REQUEST_BOARD_ID_READ = 14,
   HRF_VENDOR_REQUEST_VERSION_STRING_READ = 15,
   HRF_VENDOR_REQUEST_SET_FREQ = 16,
   HRF_VENDOR_REQUEST_AMP_ENABLE = 17,
   HRF_VENDOR_REQUEST_BOARD_PARTID_SERIALNO_READ = 18,
   HRF_VENDOR_REQUEST_SET_LNA_GAIN = 19,
   HRF_VENDOR_REQUEST_SET_VGA_GAIN = 20,
   HRF_VENDOR_REQUEST_SET_TXVGA_GAIN = 21,
   HRF_VENDOR_REQUEST_ANTENNA_ENABLE = 23,
   HRF_VENDOR_REQUEST_SET_FREQ_EXPLICIT = 24,
   HRF_VENDOR_REQUEST_USB_WCID_VENDOR_REQ = 25,
   HRF_VENDOR_REQUEST_INIT_SWEEP = 26,
   HRF_VENDOR_REQUEST_OPERACAKE_GET_BOARDS = 27,
   HRF_VENDOR_REQUEST_OPERACAKE_SET_PORTS = 28,
   HRF_VENDOR_REQUEST_SET_HW_SYNC_MODE = 29,
   HRF_VENDOR_REQUEST_RESET = 30,
   HRF_VENDOR_REQUEST_OPERACAKE_SET_RANGES = 31,
   HRF_VENDOR_REQUEST_CLKOUT_ENABLE = 32,
   HRF_VENDOR_REQUEST_SPIFLASH_STATUS = 33,
   HRF_VENDOR_REQUEST_SPIFLASH_CLEAR_STATUS = 34,
   HRF_VENDOR_REQUEST_OPERACAKE_GPIO_TEST = 35,
   HRF_VENDOR_REQUEST_CPLD_CHECKSUM = 36,
   HRF_VENDOR_REQUEST_UI_ENABLE = 37,
   HRF_VENDOR_REQUEST_OPERACAKE_SET_MODE = 38,
   HRF_VENDOR_REQUEST_OPERACAKE_GET_MODE = 39,
   HRF_VENDOR_REQUEST_OPERACAKE_SET_DWELL_TIMES = 40,
   HRF_VENDOR_REQUEST_GET_M0_STATE = 41,
   HRF_VENDOR_REQUEST_SET_TX_UNDERRUN_LIMIT = 42,
   HRF_VENDOR_REQUEST_SET_RX_OVERRUN_LIMIT = 43,
   HRF_VENDOR_REQUEST_GET_CLKIN_STATUS = 44,
   HRF_VENDOR_REQUEST_BOARD_REV_READ = 45,
   HRF_VENDOR_REQUEST_SUPPORTED_PLATFORM_READ = 46,
   HRF_VENDOR_REQUEST_SET_LEDS = 47,
   HRF_VENDOR_REQUEST_SET_USER_BIAS_T_OPTS = 48,
} HRF_VENDOR_REQUEST;

typedef enum HRF_Board_Id
{
   USB_BOARD_ID_JAWBREAKER = 0x604B,
   USB_BOARD_ID_HACKRF_ONE = 0x6089,
   USB_BOARD_ID_RAD1O = 0xCC15,
   USB_BOARD_ID_INVALID = 0xFFFF,
} HRF_Board_Id;

typedef struct HRF_DeviceInfo
{
   int index;
   HRF_Board_Id board_id;
} HRF_DeviceInfo;

typedef struct HRF
{
   char *version;
   libusb_context *context;

   int usb_device_count;
   struct libusb_device_descriptor *device_descriptors;
   libusb_device **usb_devices;
   char **serial_numbers;

   int device_count;
   HRF_DeviceInfo *device_info;
} HRF;

typedef struct HRF_SerialNumber
{
   uint32_t part_id[2];
   uint32_t serial_no[4];
} HRF_SerialNumber;

int hrf_board_id_read(HRF *hrf, libusb_device_handle *device, uint8_t *value);
int hrf_max2837_read(HRF *hrf, libusb_device_handle *device, uint8_t register_number, uint16_t *value);
int hrf_si5351c_read(HRF *hrf, libusb_device_handle *device, uint16_t register_number, uint16_t *value);
int hrf_rffc5071_read(HRF *hrf, libusb_device_handle *device, uint16_t register_number, uint16_t *value);
int hrf_spiflash_read(HRF *hrf, libusb_device_handle *device, const uint32_t address, const uint16_t length, unsigned char *data);
int hrf_board_rev_read(HRF *hrf, libusb_device_handle *device, uint8_t *value);
int hrf_supported_platform_read(HRF *hrf, libusb_device_handle *device, uint32_t *value);

#endif
