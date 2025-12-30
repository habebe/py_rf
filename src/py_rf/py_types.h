#ifndef __PY_TYPES_INCLUDE_FILE__
#define __PY_TYPES_INCLUDE_FILE__

#include <Python.h>
#include "hrf_data.h"
#include "hrf_device_info.h"

typedef struct
{
    PyObject_HEAD int index;
    int board_id;
} PyHRF_DeviceInfo;

int PyHRF_DeviceInfo_init(PyHRF_DeviceInfo *self, PyObject *args, PyObject *kwds);
void PyHRF_DeviceInfo_dealloc(PyHRF_DeviceInfo *self);
int add_deviceinfo_type(PyObject *m);
PyObject *py_create_DeviceInfo(const struct HRF_DeviceInfo *src);

#endif
