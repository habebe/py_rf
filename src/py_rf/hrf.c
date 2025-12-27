#include "hrf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int hackrf_device_list(HRF *hrf);

int hrf_init(HRF *hrf)
{
    hrf->context = 0;
    int status = libusb_init(&hrf->context);

    if (status == 0)
    {
        status = hackrf_device_list(hrf);
        hrf->version = strdup(HRF_VERSION);
    }
    return status;
}

int hrf_get_device_infos(HRF *hrf, HRF_DeviceInfo **device_infos)
{
    int size = hrf->device_count;
    *device_infos = NULL;
    if (size > 0)
    {
        int i;
        *device_infos = (HRF_DeviceInfo *)calloc(size, sizeof(HRF_DeviceInfo));
        for (i = 0; i < size; i++)
        {
        }
    }
}

int hrf_exit(HRF *hrf)
{
    if (hrf->context)
    {
        libusb_exit(hrf->context);
        hrf->context = 0;
    }
    return 0;
}

int hrf_is_device(int vid, int pid)
{
    return (vid == HRF_VID) && ((pid == HRF_ID_JAWBREAKER) ||
                                (pid == HRF_ID_HACKRF_ONE) ||
                                (pid == HRF_ID_RAD1O));
}

#if 0


static int hrf_device_open(HRF *hrf, int index)
{
    libusb_device_handle *usb_device;
    int i, result;

    i = hrf->device_indices[index];

    result = libusb_open(hrf->usb_devices[i], &usb_device);
    if (result != 0)
    {
        usb_device = NULL;
        return result;
    }
    return result;
    // return hackrf_open_setup(usb_device, device);
}

static int hrf_detach_kernel_drivers(libusb_device_handle *usb_device_handle);

static int hackrf_open_setup(libusb_device_handle *usb_device, hackrf_device **device)
{
    int result;
    hackrf_device *lib_device;

    // int speed = libusb_get_device_speed(usb_device);
    //  TODO: Error or warning if not high speed USB?

    result = set_hackrf_configuration(usb_device, USB_CONFIG_STANDARD);
    if (result != LIBUSB_SUCCESS)
    {
        libusb_close(usb_device);
        return result;
    }

    result = libusb_claim_interface(usb_device, 0);
    if (result != LIBUSB_SUCCESS)
    {
        last_libusb_error = result;
        libusb_close(usb_device);
        return HACKRF_ERROR_LIBUSB;
    }

    lib_device = NULL;
    lib_device = (hackrf_device *)calloc(1, sizeof(*lib_device));
    if (lib_device == NULL)
    {
        libusb_release_interface(usb_device, 0);
        libusb_close(usb_device);
        return HACKRF_ERROR_NO_MEM;
    }

#if LIBUSB_API_VERSION >= 0x0100010C
    // WinUSB: Use RAW_IO to improve throughput on RX
    if (libusb_endpoint_supports_raw_io(usb_device, RX_ENDPOINT_ADDRESS) == 1)
    {
        libusb_endpoint_set_raw_io(usb_device, RX_ENDPOINT_ADDRESS, 1);
    }
#endif

    lib_device->usb_device = usb_device;
    lib_device->transfers = NULL;
    lib_device->callback = NULL;
    lib_device->transfer_thread_started = 0;
    lib_device->streaming = 0;
    lib_device->do_exit = 0;
    lib_device->active_transfers = 0;
    lib_device->flush = 0;
    lib_device->flush_transfer = NULL;
    lib_device->flush_callback = NULL;
    lib_device->flush_ctx = NULL;
    lib_device->tx_completion_callback = NULL;

    result = pthread_mutex_init(&lib_device->transfer_lock, NULL);
    if (result != 0)
    {
        free(lib_device);
        libusb_release_interface(usb_device, 0);
        libusb_close(usb_device);
        return HACKRF_ERROR_THREAD;
    }

    result = pthread_cond_init(&lib_device->all_finished_cv, NULL);
    if (result != 0)
    {
        free(lib_device);
        libusb_release_interface(usb_device, 0);
        libusb_close(usb_device);
        return HACKRF_ERROR_THREAD;
    }

    result = allocate_transfers(lib_device);
    if (result != 0)
    {
        free(lib_device);
        libusb_release_interface(usb_device, 0);
        libusb_close(usb_device);
        return HACKRF_ERROR_NO_MEM;
    }

    result = create_transfer_thread(lib_device);
    if (result != 0)
    {
        free(lib_device);
        libusb_release_interface(usb_device, 0);
        libusb_close(usb_device);
        return result;
    }

    *device = lib_device;
    open_devices++;

    return HACKRF_SUCCESS;
}

static int hrf_set_configuration(libusb_device_handle *usb_device, int config)
{
    int result, curr_config;

    result = libusb_get_configuration(usb_device, &curr_config);
    if (result != 0)
    {
        return result;
    }

    if (curr_config != config)
    {
        result = hrf_detach_kernel_drivers(usb_device);
        if (result != 0)
        {
            return result;
        }
        result = libusb_set_configuration(usb_device, config);
        if (result != 0)
        {
            return result;
        }
    }

    result = detach_kernel_drivers(usb_device);
    if (result != 0)
    {
        return result;
    }
    return LIBUSB_SUCCESS;
}

static int hrf_detach_kernel_drivers(libusb_device_handle *usb_device_handle)
{
    int i, num_interfaces, result;
    libusb_device *dev;
    struct libusb_config_descriptor *config;

    dev = libusb_get_device(usb_device_handle);
    result = libusb_get_active_config_descriptor(dev, &config);
    if (result < 0)
    {
        return result;
    }

    num_interfaces = config->bNumInterfaces;
    libusb_free_config_descriptor(config);
    for (i = 0; i < num_interfaces; i++)
    {
        result = libusb_kernel_driver_active(usb_device_handle, i);
        if (result < 0)
        {
            if (result == LIBUSB_ERROR_NOT_SUPPORTED)
            {
                return 0;
            }
            else
            {
                return result;
            }
        }
        else if (result == 1)
        {
            result = libusb_detach_kernel_driver(usb_device_handle, i);
            if (result != 0)
            {
                return result;
            }
        }
    }
    return 0;
}
#endif

static const uint8_t USB_MAX_SERIAL_LENGTH = (uint8_t)256;

static int
hackrf_device_list(HRF *hrf)
{
    int status = 0;
    int i;
    uint8_t idx, serial_number_length;
    char serial_number[USB_MAX_SERIAL_LENGTH + 1];
    uint8_t serial_descriptor_index;
    libusb_device_handle *usb_device = NULL;

    hrf->usb_devices = 0;
    hrf->device_descriptors = 0;
    hrf->serial_numbers = 0;

    hrf->device_count = 0;
    hrf->device_info = 0;

    hrf->usb_device_count = (int)libusb_get_device_list(
        hrf->context,
        &hrf->usb_devices);
    if (hrf->usb_device_count > 0)
    {
        hrf->device_descriptors = calloc(hrf->usb_device_count, sizeof(struct libusb_device_descriptor));
        hrf->serial_numbers = calloc(hrf->usb_device_count, sizeof(char *));
        hrf->device_info = calloc(hrf->usb_device_count, sizeof(HRF_DeviceInfo));

        for (i = 0; i < hrf->usb_device_count; i++)
        {
            libusb_get_device_descriptor(hrf->usb_devices[i], &(hrf->device_descriptors[i]));
            serial_descriptor_index = hrf->device_descriptors[i].iSerialNumber;
            if (serial_descriptor_index > 0)
            {
                int open_status = libusb_open(hrf->usb_devices[i], &usb_device);
                if (open_status != 0)
                {
                    usb_device = NULL;
                    continue;
                }
                serial_number_length = libusb_get_string_descriptor_ascii(
                    usb_device,
                    serial_descriptor_index,
                    (unsigned char *)serial_number,
                    sizeof(serial_number));

                if (serial_number_length >= USB_MAX_SERIAL_LENGTH)
                    serial_number_length = USB_MAX_SERIAL_LENGTH;

                serial_number[serial_number_length] = 0;
                hrf->serial_numbers[i] = strdup(serial_number);
                if (hrf_is_device(
                        hrf->device_descriptors[i].idVendor,
                        hrf->device_descriptors[i].idProduct))
                {
                    hrf->device_info[hrf->device_count].index = i;
                    hrf->device_info[hrf->device_count].board_id = 0;
                    hrf->device_count++;
                }
                libusb_close(usb_device);
                usb_device = NULL;
            }
        }
    }
    return status;
}