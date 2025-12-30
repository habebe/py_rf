#ifndef __PY_CONTEXT_INCLUDE_FILE__
#define __PY_CONTEXT_INCLUDE_FILE__

#include <Python.h>

typedef struct
{
    PyObject_HEAD void *object;
} Py_Context;

int py_context_init(Py_Context *self, PyObject *args, PyObject *kwds);
void py_context_dealloc(Py_Context *self);
int py_context_add_type(PyObject *m);

#endif
