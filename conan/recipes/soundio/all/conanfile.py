import os
import textwrap

from conan.errors import ConanInvalidConfiguration
from conan.tools.apple import fix_apple_shared_install_name
from conan.tools.build import cross_building
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get, replace_in_file, rmdir
from conan.tools.microsoft import is_msvc, is_msvc_static_runtime
from conan.tools.scm import Version

from conan import ConanFile

required_conan_version = ">=2.1"


class SoundIoConan(ConanFile):
    name = "soundio"
    description = "A sound output library"
    license = "MIT"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "http://libsound.io/"
    topics = ("sound")
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }
    options_description = {
    }
    short_paths = True


    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")


    def validate(self):
        pass

    def validate_build(self):
        pass

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def layout(self):
        cmake_layout(self, src_folder="src")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_STATIC_LIBS"] = not self.options.shared
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["BUILD_TESTING"] = False


        tc.variables["MSVC_STATIC_CRT"] = is_msvc_static_runtime(self)

        tc.cache_variables["CMAKE_POLICY_DEFAULT_CMP0077"] = "NEW"
        tc.generate()

    def _patch_sources(self):
        pass

    def build(self):
        self._patch_sources()
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(
            self,
            "LICENSE",
            self.source_folder,
            os.path.join(self.package_folder, "licenses"),
        )
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))
        rmdir(self, os.path.join(self.package_folder, "share"))
        fix_apple_shared_install_name(self)

    @property
    def _lib_name(self):
        if (
            self.options.shared
            and self.settings.build_type == "Debug"
            and not is_msvc(self)
        ):
            return "soundio_d"
        return "soundio"

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "SoundIo")
        self.cpp_info.set_property("cmake_target_name", "SoundIo::SoundIo")
        self.cpp_info.set_property("pkg_config_name", "soundio")
        self.cpp_info.components["soundio_component"].set_property(
            "pkg_config_name", "soundio"
        )
        self.cpp_info.components["soundio_component"].includedirs.append(
            os.path.join("include", "soundio")
        )
        self.cpp_info.components["soundio_component"].libs = [self._lib_name]
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.components["soundio_component"].system_libs.append("m")

        self.buildenv_info.define_path("SoundIo_HOME", self.package_folder)
        self.runenv_info.define_path("SoundIo_HOME", self.package_folder)
