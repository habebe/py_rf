#include "py_context.h"
#include <structmember.h>
#include "../hrf.h"
#include "py_device_info.h"

static PyObject *g_context_singleton = NULL;

int py_context_init(Py_Context *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

void py_context_dealloc(Py_Context *self)
{
    self->object = NULL;
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *py_context_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    if (g_context_singleton)
    {
        Py_INCREF(g_context_singleton);
        return g_context_singleton;
    }

    Py_Context *self = (Py_Context *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;
    self->object = hrf_context_get();

    g_context_singleton = (PyObject *)self;
    Py_INCREF(g_context_singleton);
    return (PyObject *)self;
}

static PyObject *py_context_repr(Py_Context *self)
{
    if (self->object)
    {
        struct Context *context = (struct Context *)self->object;
        return PyUnicode_FromFormat(
            "{\"usb_device_count\":%d,\"device_count\":%d}",
            context->usb_device_count,
            context->device_count);
    }
    return PyUnicode_FromFormat("{}");
}

static PyObject *py_context_get_native(Py_Context *self, void *closure)
{
    if (!self->object)
        Py_RETURN_NONE;
    return PyCapsule_New(self->object, "Context", NULL);
}

static PyObject *py_context_usb_device_count(Py_Context *self, void *closure)
{
    if (!self->object)
    {
        PyErr_SetString(PyExc_RuntimeError, "Context is not initialized");
        return NULL;
    }
    struct Context *context = (struct Context *)self->object;
    return PyLong_FromLong(context->usb_device_count);
}

static PyObject *py_context_device_count(Py_Context *self, void *closure)
{
    if (!self->object)
    {
        PyErr_SetString(PyExc_RuntimeError, "Context is not initialized");
        return NULL;
    }
    struct Context *context = (struct Context *)self->object;
    return PyLong_FromLong(context->device_count);
}

static PyObject *py_context_device_infos(Py_Context *self, PyObject *Py_UNUSED(args))
{
    if (!self->object)
    {
        PyErr_SetString(PyExc_RuntimeError, "Context is not initialized");
        return NULL;
    }
    struct Context *context = (struct Context *)self->object;
    PyObject *device_infos = PyList_New(0);
    for (int i = 0; i < context->device_count; i++)
    {
        PyObject *obj = py_device_info_create(&(context->device_infos[i]));
        if (obj)
        {
            PyList_Append(device_infos, obj);
            Py_DECREF(obj);
        }
    }
    return device_infos;
}

static PyGetSetDef py_context_getset[] = {
    {"_native", (getter)py_context_get_native, NULL, "Opaque native pointer", NULL},
    {"usb_device_count", (getter)py_context_usb_device_count, NULL, "number of usb devices", NULL},
    {"device_count", (getter)py_context_device_count, NULL, "number of detected devices", NULL},
    {NULL}};

static PyMethodDef py_context_methods[] = {
    {"device_info", (PyCFunction)py_context_device_infos, METH_NOARGS, "List of device infos"},
    {NULL}};

static PyTypeObject py_context_type = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "Context",
    .tp_basicsize = sizeof(Py_Context),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = py_context_new,
    .tp_init = (initproc)py_context_init,
    .tp_dealloc = (destructor)py_context_dealloc,
    .tp_getset = py_context_getset,
    .tp_repr = (reprfunc)py_context_repr,
    .tp_methods = py_context_methods,
};

int py_context_add_type(PyObject *module)
{
    if (PyType_Ready(&py_context_type) < 0)
        return -1;

    Py_INCREF(&py_context_type);
    if (PyModule_AddObject(module, "Context", (PyObject *)&py_context_type) < 0)
    {
        Py_DECREF(&py_context_type);
        return -1;
    }
    return 0;
}