#ifndef __HRF_DEVICE_INCLUDE_FILE__
#define __HRF_DEVICE_INCLUDE_FILE__
#include "hrf.h"
#include "hrf_data.h"
#include <pthread.h>

#define TRANSFER_COUNT 4
#define TRANSFER_BUFFER_SIZE 262144
#define DEVICE_BUFFER_SIZE 32768
#define USB_MAX_SERIAL_LENGTH 32

struct Device;

typedef struct Transfer
{
    Device *device;
    uint8_t *buffer;
    int buffer_length;
    int valid_length;
    void *rx_ctx;
    void *tx_ctx;
} Transfer;

typedef int (*hrf_sample_block_cb_fn)(Transfer *transfer);
typedef void (*hrf_tx_block_complete_cb_fn)(Transfer *transfer, int);
typedef void (*hrf_flush_cb_fn)(void *flush_ctx, int);

struct Device
{
    libusb_device_handle *usb_device;
    struct libusb_transfer **transfers;
    hrf_sample_block_cb_fn callback;
    bool transfer_thread_started; /* volatile shared between threads (read only) */
    pthread_t transfer_thread;
    bool streaming; /* volatile shared between threads (read only) */
    void *rx_ctx;
    void *tx_ctx;
    bool do_exit;
    unsigned char buffer[TRANSFER_COUNT * TRANSFER_BUFFER_SIZE];
    bool transfers_setup;           /* true if the USB transfers have been setup */
    pthread_mutex_t transfer_lock;  /* must be held to cancel or restart transfers */
    volatile int active_transfers;  /* number of active transfers */
    pthread_cond_t all_finished_cv; /* signalled when all transfers have finished */
    bool flush;
    struct libusb_transfer *flush_transfer;
    hrf_tx_block_complete_cb_fn flush_callback;
    hrf_flush_cb_fn tx_completion_callback;
    void *flush_ctx;
};

#endif
