# webview_flutter [WIP]

Implementation is targeting the Chromium Embedded Framework (CEF)

## Generate messages.g.h and messages.g.cc

    flutter pub run pigeon --input pigeons/android_webview.dart --cpp_header_out messages.g.h --cpp_source_out messages.g.cc --cpp_namespace plugin_webview_flutter

## Steps to test plugin

1. Follow guide at https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md to build chromium/cef
    a. Use the following GN_DEFINES:

    `export GN_DEFINES="use_sysroot=true use_allocator=none is_cfi=false use_thin_lto=false use_ozone=true ozone_auto_platforms=false ozone_platform_headless=true ozone_platform_wayland=true ozone_platform_x11=false use_xkbcommon=true use_gtk=false angle_use_wayland=false angle_enable_gl=false"`

    b. For debugging, may also add `symbol_level=2 is_debug=true`

    TODO: Add tips/patches for building on Ubuntu 20

    c. Apply the following patch from webiew_flutter_view/patches/chromium_cef directory:

        - 0001_enable_gl_without_angle.patch

    Note: During the build step, only cefsimple and chrome_sandbox are necessary targets

2. Once built, there is a built-in python script to generate a binary distribution

    `python3 {chromium src dir}/cef/tools/make_distrib.py --output-dir=../binary_distrib/ --no-docs --ninja-build --x64-build --ozone`

3. Copy this binary distribution folder to {ivi-homescreen-plugins dir}/plugins/webview_flutter_plugin/third_party
4. Within the copied binary distribution, delete the tests/ directory, as well as the packaged Angle libraries in the debug and release directories: libEGL.so, libGLESv2.so, libvk_swiftshader.so, libvulkan.so.1

5. [Temporary] Copy all files in {binary_distrib}/Resources to {binary_distrib}/Debug and {binary_distrib}/Release

6. Initialize git repository in the binary distribution folder:
    `git init`
    `git add .`
    `git commit -s -m "Initial Commit"`

7. Apply the following patches from workspace_automation:
    `{workspace_automation dir}/patches/cef-prebuilt/0001-clang-build.patch`
    `{workspace_automation dir}/patches/cef-prebuilt/0002-cef-subprocess.patch`
    `{workspace_automation dir}/patches/cef-prebuilt/0003-add-dylib.patch`
    `{workspace_automation dir}/patches/cef-prebuilt/0004-add-dylib-header.patch`
    `{workspace_automation dir}/patches/cef-prebuilt/0005-include_dylib.patch`
    `TODO: more patches`

8. If libwayland-client version is less than 1.23.0, download/compile/install libwayland_client 1.23.0

9. Set environment variable for root directory of CEF binary distribution:
    `export CEF_ROOT={ivi-homescreen-plugins dir}/plugins/webview_flutter_plugin/third_party/{cef_binary_distrib}`

10. Configure ivi-homescreen CMake with the following additional arguments: 
    `-DBUILD_PLUGIN_WEBVIEW_FLUTTER_VIEW=ON -DCEF_ROOT=${CEF_ROOT}`

11. Build and install all targets (includes webview subprocess):
    `ninja all install`

12. Source workspace_automation setup script, and run test package:
    `. {workspace_automation root}/setup_env.sh`
    `cd {workspace_automation root}/app/tcna-packages/packages/webview/webview_flutter_linux/example`
    `LD_PRELOAD={CEF binary distribution dir}/{build type}/libcef.so LD_PRELOAD="/usr/lib/x86_64-linux-gnu/;{path to libwayland-client.so.0.23.0} flutter run -d desktop-homescreen`