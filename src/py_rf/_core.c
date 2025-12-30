#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <numpy/arrayobject.h>
#include "hrf.h"
#include "py_types.h"

#include "types/py_context.h"

static PyObject *py_hrf_get_device_info(PyObject *self, PyObject *args)
{
    PyObject *device_infos = PyList_New(0);
    HRF hrf;
    int status = hrf_init(&hrf);
    if (status < 0)
    {
        return PyErr_Format(PyExc_RuntimeError, "hrf_init failed: %d", status);
    }

    int i;
    for (i = 0; i < hrf.device_count; i++)
    {
        PyObject *obj = py_create_DeviceInfo(&(hrf.device_info[i]));
        if (obj)
        {
            PyList_Append(device_infos, obj);
            Py_DECREF(obj);
        }
    }

    return device_infos;
}

static PyObject *
device_descriptor_to_pydict(const struct libusb_device_descriptor *desc)
{
    PyObject *d = PyDict_New();
    if (!d)
        return NULL;

#define ADD_UINT8(key, value)                         \
    do                                                \
    {                                                 \
        PyObject *o = PyLong_FromUnsignedLong(value); \
        PyDict_SetItemString(d, key, o);              \
        Py_DECREF(o);                                 \
    } while (0)

#define ADD_UINT16(key, value)                        \
    do                                                \
    {                                                 \
        PyObject *o = PyLong_FromUnsignedLong(value); \
        PyDict_SetItemString(d, key, o);              \
        Py_DECREF(o);                                 \
    } while (0)

    ADD_UINT8("length", desc->bLength);
    ADD_UINT8("descriptorType", desc->bDescriptorType);
    ADD_UINT16("bcdUSB", desc->bcdUSB);

    ADD_UINT8("deviceClass", desc->bDeviceClass);
    ADD_UINT8("deviceSubClass", desc->bDeviceSubClass);
    ADD_UINT8("deviceProtocol", desc->bDeviceProtocol);
    ADD_UINT8("maxPacketSize0", desc->bMaxPacketSize0);

    ADD_UINT16("vendorId", desc->idVendor);
    ADD_UINT16("productId", desc->idProduct);
    ADD_UINT16("bcdDevice", desc->bcdDevice);

    ADD_UINT8("manufacturerIndex", desc->iManufacturer);
    ADD_UINT8("productIndex", desc->iProduct);
    ADD_UINT8("serialNumberIndex", desc->iSerialNumber);

    ADD_UINT8("numConfigurations", desc->bNumConfigurations);

#undef ADD_UINT8
#undef ADD_UINT16

    return d;
}

static PyObject *hrf_info(PyObject *self, PyObject *args)
{
    HRF hrf;
    PyObject *dict = PyDict_New();
    if (!dict)
    {
        return NULL;
    }
    int status = hrf_init(&hrf);
    if (status < 0)
    {
        return PyErr_Format(PyExc_RuntimeError, "hackrf_init failed: %d", status);
    }

    if (hrf.version)
    {
        PyObject *version = PyUnicode_FromString(hrf.version);
        PyDict_SetItemString(dict, "version", version);
        Py_DECREF(version);
    }

    PyObject *usb_device_count = PyLong_FromLong(hrf.usb_device_count);
    PyDict_SetItemString(dict, "usb_device_count", usb_device_count);
    Py_DECREF(usb_device_count);

    PyObject *device_count = PyLong_FromLong(hrf.device_count);
    PyDict_SetItemString(dict, "device_count", device_count);
    Py_DECREF(device_count);

    int i = 0;
    PyObject *device_indices = PyList_New(0);
    if (!device_indices)
    {
        Py_DECREF(dict);
        return NULL;
    }
    for (i = 0; i < hrf.device_count; i++)
    {
        PyList_Append(device_indices, PyLong_FromLong(hrf.device_info[i].index));
    }
    PyDict_SetItemString(dict, "device_indices", device_indices);
    Py_DECREF(device_indices);

    PyObject *device_descriptors = PyList_New(0);
    if (!device_descriptors)
    {
        Py_DECREF(dict);
        return NULL;
    }
    if (hrf.device_descriptors)
    {
        for (i = 0; i < hrf.usb_device_count; i++)
        {
            PyObject *device_descriptor = device_descriptor_to_pydict(&hrf.device_descriptors[i]);
            PyList_Append(device_descriptors, device_descriptor);
            Py_DECREF(device_descriptor);
        }
    }
    PyDict_SetItemString(dict, "device_descriptors", device_descriptors);
    Py_DECREF(device_descriptors);

    PyObject *serial_numbers = PyList_New(0);
    if (hrf.serial_numbers)
    {
        for (i = 0; i < hrf.usb_device_count; i++)
        {
            if (hrf.serial_numbers[i])
            {
                PyObject *serial_number = PyUnicode_FromString(hrf.serial_numbers[i]);
                PyList_Append(serial_numbers, serial_number);
                Py_DECREF(serial_number);
            }
            else
            {
                PyObject *serial_number = PyUnicode_FromString("");
                PyList_Append(serial_numbers, serial_number);
                Py_DECREF(serial_number);
            }
        }
    }
    PyDict_SetItemString(dict, "serial_numbers", serial_numbers);
    Py_DECREF(serial_numbers);

    // name: str
    PyObject *name = PyUnicode_FromString("demo");
    PyDict_SetItemString(dict, "name", name);
    Py_DECREF(name);

    // enabled: bool
    PyObject *enabled = PyBool_FromLong(1);
    PyDict_SetItemString(dict, "enabled", enabled);
    Py_DECREF(enabled);

    // value: float
    PyObject *value = PyFloat_FromDouble(3.14);
    PyDict_SetItemString(dict, "value", value);
    Py_DECREF(value);

    return dict;
}

static PyObject *sum_double(PyObject *self, PyObject *args)
{
    PyObject *input_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &input_obj))
    {
        return NULL;
    }

    // Convert to a NumPy array of type float64 (NPY_DOUBLE), contiguous.
    PyArrayObject *arr = (PyArrayObject *)PyArray_FROM_OTF(
        input_obj,
        NPY_DOUBLE,
        NPY_ARRAY_IN_ARRAY);

    if (arr == NULL)
    {
        return NULL; // NumPy already set an exception
    }

    npy_intp size = PyArray_SIZE(arr);
    double *data = (double *)PyArray_DATA(arr);

    double s = 0.0;
    for (npy_intp i = 0; i < size; i++)
    {
        s += data[i];
    }

    Py_DECREF(arr);
    return PyFloat_FromDouble(s);
}

static PyObject *usb_device_count(PyObject *self, PyObject *args)
{
    libusb_context *ctx = NULL;
    libusb_device **list = NULL;

    int r = libusb_init(&ctx);
    if (r < 0)
    {
        return PyErr_Format(PyExc_RuntimeError,
                            "libusb_init failed: %d", r);
    }

    ssize_t count = libusb_get_device_list(ctx, &list);

    libusb_free_device_list(list, 1);
    libusb_exit(ctx);

    if (count < 0)
    {
        return PyErr_Format(PyExc_RuntimeError,
                            "libusb_get_device_list failed");
    }

    return PyLong_FromSsize_t(count);
}

static PyMethodDef Methods[] = {
    {"sum_double", sum_double, METH_VARARGS, "Sum all elements of an array as float64 in C."},
    {"usb_device_count", usb_device_count, METH_VARARGS, "Return number of USB devices using libusb"},
    {"info", hrf_info, METH_VARARGS, "Returns hackrf_info."},
    {"get_device_info", py_hrf_get_device_info, METH_VARARGS, "Returns hrf device info."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_core",
    "NumPy C-API demo module",
    -1,
    Methods};

PyMODINIT_FUNC PyInit__core(void)
{
    PyObject *m = PyModule_Create(&moduledef);
    if (m == NULL)
        return NULL;
    add_deviceinfo_type(m);
    py_context_add_type(m);

    // Initialize NumPy C-API
    import_array(); // This is a macro that returns NULL on failure
    return m;
}
