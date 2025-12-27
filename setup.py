from setuptools import setup, Extension, find_packages
import numpy as np
import sys
import os

# ---- libusb configuration ----
# You may need to tweak these paths depending on platform

include_dirs = [np.get_include()]
library_dirs = []
libraries = []
extra_compile_args = []
extra_link_args = []

if sys.platform.startswith("linux"):
    libraries.append("usb-1.0")

elif sys.platform == "darwin":
    libraries.append("usb-1.0")
    # Homebrew default paths
    include_dirs.append("/opt/homebrew/include")
    library_dirs.append("/opt/homebrew/lib")

elif sys.platform == "win32":
    libraries.append("libusb-1.0")
    # Example: user provides LIBUSB_ROOT
    libusb_root = os.environ.get("LIBUSB_ROOT")
    if not libusb_root:
        raise RuntimeError("Set LIBUSB_ROOT to libusb install directory")
    include_dirs.append(os.path.join(libusb_root, "include"))
    library_dirs.append(os.path.join(libusb_root, "lib"))

debug = True

if debug:
    extra_compile_args += ["-O0", "-g3", "-fno-omit-frame-pointer"]
else:
    extra_compile_args += ["-O3"]
        
ext_modules = [
    Extension(
        name="py_rf._core",
        sources=[
            "src/py_rf/_core.c",
            "src/py_rf/hrf.c",
            "src/py_rf/hrf_data.c",
            "src/py_rf/py_types.c"
            ],
        include_dirs=include_dirs,
        library_dirs=library_dirs,
        libraries=libraries,
        extra_compile_args=extra_compile_args if sys.platform != "win32" else ["/O2"],
        extra_link_args=extra_link_args,
    )
]

setup(
    name="py_rf",
    version="0.2.0",
    description="NumPy C extension with libusb dependency",
    package_dir={"": "src"},
    packages=find_packages(where="src"),
    ext_modules=ext_modules,
    install_requires=["numpy>=1.21"],
)
