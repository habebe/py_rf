#ifndef __PY_DEVICE_INCLUDE_FILE__
#define __PY_DEVICE_INCLUDE_FILE__

#include <Python.h>

struct DeviceInfo;

typedef struct
{
    PyObject_HEAD int index;
    int vid;
    int pid;
    int bid;
    void *object;
} Py_Device;

int py_device_init(Py_Device *self, PyObject *args, PyObject *kwds);
void py_device_dealloc(Py_DeviceInfo *self);
int py_device_add_type(PyObject *m);

PyObject *py_device_create(const struct Device *src);

#endif
