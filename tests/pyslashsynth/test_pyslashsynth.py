import pytest

from pyslashsynth import __author__ as author
from pyslashsynth import __version__ as version
from pyslashsynth.cxx import __version__ as cxxversion


def test_version():
    assert version == "0.1.0"


def test_author():
    assert author == "Jaap Versteegh"


def test_cxxversion():
    assert cxxversion == "0.1.0"
