#include "py_device_info.h"
#include <structmember.h>
#include "../hrf.h"

int py_device_info_init(Py_DeviceInfo *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

void py_device_info_dealloc(Py_DeviceInfo *self)
{
    // Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *py_device_info_repr(Py_DeviceInfo *self)
{
    if (self)
    {
        return PyUnicode_FromFormat(
            "{\"index\":%d,\"vid\":%d,\"pid\":%d,\"bid\":%d}",
            self->index,
            self->vid,
            self->pid,
            self->bid);
    }
    return PyUnicode_FromFormat("{}");
}

static PyMemberDef py_device_info_members[] = {
    {"index", T_INT, offsetof(Py_DeviceInfo, index), 0, "Device index"},
    {"vid", T_INT, offsetof(Py_DeviceInfo, vid), 0, "Vendor ID"},
    {"pid", T_INT, offsetof(Py_DeviceInfo, pid), 0, "Product ID"},
    {"bid", T_INT, offsetof(Py_DeviceInfo, bid), 0, "Board ID"},
    {NULL}};

static PyTypeObject py_device_info_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "DeviceInfo",
    .tp_basicsize = sizeof(Py_DeviceInfo),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)py_device_info_init,
    .tp_dealloc = (destructor)py_device_info_dealloc,
    .tp_repr = (reprfunc)py_device_info_repr,
    .tp_members = py_device_info_members,
};

int py_device_info_add_type(PyObject *module)
{
    if (PyType_Ready(&py_device_info_type) < 0)
        return -1;

    Py_INCREF(&py_device_info_type);
    if (PyModule_AddObject(module, "DeviceInfo", (PyObject *)&py_device_info_type) < 0)
    {
        Py_DECREF(&py_device_info_type);
        return -1;
    }
    return 0;
}

PyObject *py_device_info_create(const struct DeviceInfo *src)
{
    Py_DeviceInfo *obj = PyObject_New(Py_DeviceInfo, &py_device_info_type);

    if (!obj)
        return NULL;

    obj->index = src->index;
    obj->vid = src->vid;
    obj->pid = src->pid;
    obj->bid = src->board_id;

    return (PyObject *)obj;
}
