"""
slashsynth C++ module loader

Import object from the slashsynth C++ module
"""

__author__ = "Jaap Versteegh"
__contact__ = "j.r.versteegh@gmail.com"

import importlib
import logging

_log = logging.getLogger("pyslashsynth.cxx")
_module_name = ".pycxxslashsynth"

try:
    _log.info(f"Importing {_module_name}")
    pycxx = importlib.import_module(_module_name, package="pyslashsynth")
except ImportError:
    _log.error(f"Failed to import {_module_name}")
    raise ImportError(
        f'Failed to import "{_module_name}". Did the C++ package not compile or install properly?',
        name=_module_name,
    )

__version__ = pycxx.__version__
globals().update(
    {
        symbol: getattr(pycxx, symbol)
        for symbol in dir(pycxx)
        if not symbol.startswith("_")
    }
)
