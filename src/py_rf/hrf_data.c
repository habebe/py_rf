#include "hrf.h"
#include "hrf_data.h"

uint16_t HRF_VID = 0x1d50;
const char HRF_VERSION[] = "1.0";

int hrf_max2837_read(
    HRF *hrf,
    libusb_device_handle *device,
    uint8_t register_number,
    uint16_t *value)
{
   int result;

   if (register_number >= 32)
   {
      return -1;
   }

   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_MAX2837_READ,
       0,
       register_number,
       (unsigned char *)value,
       2,
       0);

   return result;
}

int hrf_si5351c_read(
    HRF *hrf,
    libusb_device_handle *device,
    uint16_t register_number,
    uint16_t *value)
{
   uint8_t temp_value;
   int result;

   if (register_number >= 256)
   {
      return -1;
   }

   temp_value = 0;
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_SI5351C_READ,
       0,
       register_number,
       (unsigned char *)&temp_value,
       1,
       0);

   if (result < 1)
   {
      return result;
   }
   else
   {
      *value = temp_value;
      return result;
   }
}

int hrf_board_id_read(HRF *hrf, libusb_device_handle *device, uint8_t *value)
{
   return libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_BOARD_ID_READ,
       0,
       0,
       value,
       1,
       0);
}

int hrf_rffc5071_read(HRF *hrf, libusb_device_handle *device, uint16_t register_number, uint16_t *value)
{
   int result;

   if (register_number >= 31)
   {
      return -1;
   }
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_RFFC5071_READ,
       0,
       register_number,
       (unsigned char *)value,
       2,
       0);
   return result;
}

int hrf_spiflash_read(
    HRF *hrf, libusb_device_handle *device,
    const uint32_t address,
    const uint16_t length,
    unsigned char *data)
{
   int result;
   if (address > 0x0FFFFF)
   {
      return -1;
   }
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_SPIFLASH_READ,
       address >> 16,
       address & 0xFFFF,
       data,
       length,
       0);
   return result;
}

int hrf_version_string_read(
    HRF *hrf, libusb_device_handle *device,
    char *version,
    uint8_t length)
{
   int result;
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_VERSION_STRING_READ,
       0,
       0,
       (unsigned char *)version,
       length,
       0);
   if (result >= 0)
      version[result] = '\0';
   return result;
}

int hrf_board_partid_serialno_read(
    HRF *hrf, libusb_device_handle *device,
    HRF_SerialNumber *serial_number)
{
   uint8_t length;
   int result;

   length = sizeof(serial_number);
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_BOARD_PARTID_SERIALNO_READ,
       0,
       0,
       (unsigned char *)serial_number,
       length,
       0);

   if (result < length)
   {
      return result;
   }
   else
   {
      serial_number->part_id[0] =
          TO_LE(serial_number->part_id[0]);
      serial_number->part_id[1] =
          TO_LE(serial_number->part_id[1]);
      serial_number->serial_no[0] =
          TO_LE(serial_number->serial_no[0]);
      serial_number->serial_no[1] =
          TO_LE(serial_number->serial_no[1]);
      serial_number->serial_no[2] =
          TO_LE(serial_number->serial_no[2]);
      serial_number->serial_no[3] =
          TO_LE(serial_number->serial_no[3]);
      return result;
   }
}

int hrf_board_rev_read(HRF *hrf, libusb_device_handle *device, uint8_t *value)
{
   // USB_API_REQUIRED(device, 0x0106)
   int result;
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_BOARD_REV_READ,
       0,
       0,
       value,
       1,
       0);

   return result;
}

int hrf_supported_platform_read(HRF *hrf, libusb_device_handle *device, uint32_t *value)
{
   unsigned char data[4];
   // USB_API_REQUIRED(device, 0x0106)
   int result;
   result = libusb_control_transfer(
       device,
       LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
       HRF_VENDOR_REQUEST_SUPPORTED_PLATFORM_READ,
       0,
       0,
       &data[0],
       4,
       0);

   if (result < 1)
   {
      return result;
   }
   else
   {
      *value = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
      return result;
   }
}