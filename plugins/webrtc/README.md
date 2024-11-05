# flutter-webrtc plugin

## ivi-homescreen build config

Required CMake build flags

    BUILD_PLUGIN_WEBRTC
    LIBWEBRTC_INC_DIR
    LIBWEBRTC_LIB

Example Usage

    -DBUILD_PLUGIN_WEBRTC=ON
    -DLIBWEBRTC_INC_DIR=/mnt/raid10/workspace-automation/app/libwebrtc_build/src/libwebrtc/include
    -DLIBWEBRTC_LIB=/mnt/raid10/workspace-automation/app/libwebrtc_build/src/out/Linux-x64/libwebrtc.so

## Building libwebrtc

Follow instructions in source repo:
https://github.com/webrtc-sdk/libwebrtc

## Flutter Functional Test case

https://github.com/flutter-webrtc/flutter-webrtc/tree/main/example
