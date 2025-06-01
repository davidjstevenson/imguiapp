from conan import ConanFile
from conan.api.conan_api import ConanAPI
from conan.tools.cmake import CMakeToolchain
from pathlib import Path

DIR = Path(__file__).resolve().parent


class ImGuiApp(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    # pass in the preset so we can put things in a place the cmake preset expects
    options = {
        "preset": ["ANY"]
    }

    default_options = {k: v[0] for k, v in options.items() if k != "preset"}

    def requirements(self):
        self.requires("fmt/11.1.3")
        self.requires("boost/1.84.0", force=True)
        self.requires("glew/2.2.0")
        self.requires("glfw/3.3.8")
        self.requires("imgui/1.91.8-docking", force=True)
        self.requires("implot/0.16")
        self.requires("nlohmann_json/3.12.0")
        self.requires("tracy/0.11.1")

    def build_requirements(self):
        pass

    def configure_boost(self):
        self.options["boost/*"].shared = False
        self.options["boost/*"].with_stacktrace_backtrace = False
        self.options["boost/*"].without_stacktrace = True

    def configure_tracy(self):
        self.options["tracy/*"].on_demand = True
        self.options["tracy/*"].no_broadcast = True
        self.options["tracy/*"].callstack = True
        self.options["tracy/*"].shared = False

    def configure(self):
        self.configure_boost()
        self.configure_tracy()

    def layout(self):
        # set the layout to our preset files can reliably reference the toolchain
        dir = DIR / "builds" / (self.options.preset.value or "default")
        self.folders.generators = dir / "generators"
        self.folders.build = dir

    def generate(self):
        # this generates the "conan_toolchain.cmake" that if required by the cmake presets
        tc = CMakeToolchain(self)
        # set to None to disable creating a presets file - we bypass this and go straight to the
        # toolchain. docs suggest inheriting from the generated presets, however anything you inherit
        # from must ALWAYS exist even if not being used
        tc.user_presets_path = None
        tc.presets_prefix = "conan"
        tc.generate()
