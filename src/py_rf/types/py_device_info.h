#ifndef __PY_DEVICE_INFO_INCLUDE_FILE__
#define __PY_DEVICE_INFO_INCLUDE_FILE__

#include <Python.h>

struct DeviceInfo;

typedef struct
{
    PyObject_HEAD int index;
    int vid;
    int pid;
    int bid;
} Py_DeviceInfo;

int py_device_info_init(Py_DeviceInfo *self, PyObject *args, PyObject *kwds);
void py_device_info_dealloc(Py_DeviceInfo *self);
int py_device_info_add_type(PyObject *m);

PyObject *py_device_info_create(const struct DeviceInfo *src);

#endif
