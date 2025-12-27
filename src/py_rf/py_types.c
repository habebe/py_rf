#include "py_types.h"
#include <structmember.h>

int PyHRF_DeviceInfo_init(PyHRF_DeviceInfo *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {
        "index",
        "board_id",
        //"board_name",
        NULL};

    int index;
    int board_id;
    // const char *board_name = NULL;

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "ii", //"ii|s",
            kwlist,
            &index,
            &board_id
            //,&board_name
            ))
    {
        return -1;
    }

    self->index = index;
    self->board_id = board_id;

    /*
    if (board_name)
    {
        self->board_name = PyUnicode_FromString(board_name);
        if (!self->board_name)
        {
            return -1;
        }
    }
    else
    {
        self->board_name = PyUnicode_FromString("");
    }
*/
    return 0;
}

void PyHRF_DeviceInfo_dealloc(PyHRF_DeviceInfo *self)
{
    // Py_XDECREF(self->board_name);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyMemberDef PyHRF_DeviceInfo_members[] = {
    {"index", T_INT, offsetof(PyHRF_DeviceInfo, index), 0,
     "Device index"},
    {"board_id", T_INT, offsetof(PyHRF_DeviceInfo, board_id), 0,
     "Board ID"},
    //  {"board_name", T_OBJECT_EX, offsetof(PyHRF_DeviceInfo, board_name), 0,"Device name"},
    {NULL}};

static PyTypeObject PyHRF_DeviceInfoType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "hrf.HRF_DeviceInfo",
    .tp_basicsize = sizeof(PyHRF_DeviceInfo),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)PyHRF_DeviceInfo_init,
    .tp_dealloc = (destructor)PyHRF_DeviceInfo_dealloc,
    .tp_members = PyHRF_DeviceInfo_members,
};

PyObject *py_create_DeviceInfo(
    const struct HRF_DeviceInfo *src)
{
    PyHRF_DeviceInfo *obj =
        PyObject_New(PyHRF_DeviceInfo, &PyHRF_DeviceInfoType);

    if (!obj)
        return NULL;

    obj->index = src->index;
    obj->board_id = src->board_id;
    return (PyObject *)obj;
}

int add_deviceinfo_type(PyObject *m)
{
    if (PyType_Ready(&PyHRF_DeviceInfoType) < 0)
        return -1;

    Py_INCREF(&PyHRF_DeviceInfoType);
    if (PyModule_AddObject(
            m,
            "HRF_DeviceInfo",
            (PyObject *)&PyHRF_DeviceInfoType) < 0)
    {
        Py_DECREF(&PyHRF_DeviceInfoType);
        return -1;
    }

    return 0;
}
