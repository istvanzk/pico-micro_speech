# MICRO_SPEECH implementation for PICO-TFLMICRO

![Exp](https://img.shields.io/badge/Fork&Copy-experimental-orange.svg)
[![Lic](https://img.shields.io/badge/License-Apache2.0-green)](http://www.apache.org/licenses/LICENSE-2.0)
![Py](https://img.shields.io/badge/Python-3.9+-green)
[![TensorFlow 2.15](https://img.shields.io/badge/TensorFlow-2.15-FF6F00?logo=tensorflow)](https://github.com/tensorflow/tensorflow/releases/tag/v2.15.0)
![Ver](https://img.shields.io/badge/Version-0.11-lightgrey)
![HWD](https://img.shields.io/badge/HWD_test-OK-green)

This repo implements the *micro_speech* example for deployment on Raspberry Pico RP2040 compatible micrcontroller boards. The code is meant to be installed and compiled as part of the [pico-tflmicro](https://github.com/raspberrypi/pico-tflmicro/) version of the [TensorFlow Lite Micro library](https://www.tensorflow.org/lite/microcontrollers) for the Raspberry Pi Pico microcontroller.

## Dependencies

The [pico-tflmicro](https://github.com/raspberrypi/pico-tflmicro/) version of the [TensorFlow Lite Micro library](https://www.tensorflow.org/lite/microcontrollers) for the Raspberry Pi Pico microcontroller.

The code re-uses most of the [esp-tflite-micro](https://github.com/espressif/esp-tflite-micro/) implementation of the micro_speech example, except the following code:

* The `analog_audio_provider.cc`, `pdm_audio_provider.cc`, `command_responser.cc` and `main.cc` code from [pico-wake-word](https://github.com/henriwoodcock/pico-wake-word) implementation is used.

* From the [Microphone Library for Pico @a837f63](https://github.com/ArmDeveloperEcosystem/microphone-library-for-pico/tree/a837f633a6ad2bac268349df35d57e46e551f416) the `microphone-library-for-pico/src` code has been copied with minor modification to this repo, under `pico-microphone/src` folder:

  * The `pico-microphone/CMakeLists.txt` file has been edited to remove these lines:
    ```
    include(pico_sdk_import.cmake)
    pico_sdk_init()
    ```

The outputs from the model generation process described for the [Micro Speech Example](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech) are expected to be placed in `micro_speech/models` folder:
  * micro_speech_quantized_model_data
  * audio_preprocessor_int8_model_data

## Installation

**Step 0**: Clone the [pico-sdk]() and follow the associated installation instructions. Set the `PICO_SDK_PATH` to the *full path* where the pico-sdk was installed.

**Step 1**: Clone the [pico-tflmicro](https://github.com/raspberrypi/pico-tflmicro/) repo.

**Step 2**: Clone this repo as `micro_speech` within the `pico-tflmicro/examples` folder of the [pico-tflmicro](https://github.com/raspberrypi/pico-tflmicro/) repo.

**Step 3**: Edit the [pico-tflmicro/CMakeLists_template.txt](https://github.com/raspberrypi/pico-tflmicro/blob/main/CMakeLists_template.txt) and insert
`add_subdirectory("examples/micro_speech")` to have this at the end of the file:
```
...

add_subdirectory("examples/hello_world")
add_subdirectory("examples/person_detection")
add_subdirectory("examples/micro_speech")

{{TEST_FOLDERS}}
```

**Step 4**: Follow the instructions in the [pico-tflmicro/README](https://github.com/raspberrypi/pico-tflmicro/blob/main/README.md) and generate an updated version of the pico-tflmicro project by running the command:
```
sync/sync_with_upstream.sh
```

**Step 5**: In the [pico-tflmicro](https://github.com/raspberrypi/pico-tflmicro/) directory run:
```
mkdir build
cd build
cmake ..
```

**Step 6**: To build the micro_speech example run:
```
cd examples/micro_speech
make
```
**Step 7**: Provided the build succeeds, the `*.bin`, `*.elf` and `*.uf2` compiled binary files are now available in `pico-tflmicro/build/examples/micro_speech` folder. These are RP2040 specific binaries and cannot be exectuted on the development machine!

## Notes

1. This implementation relies on the generation of two tflite model files: one for *audio_preprocessing*, and one for the *micro_speech* ML model itself. This is well described in the official
[Micro Speech Example](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech). The models in C++ format need to be placed under the `micro_speech/models`. 

2. To build the data files needed, use [Convert models and audio samples to C++](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech#converting-models-or-audio-samples-to-c). The test audio sample files in C++ format need to be placed under `micro_speech/testdata`.

3. There are other, 3+ years older, versions of the *micro_speech* example implementation which require the use of the [microfrontend](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/experimental/microfrontend) implementation e.g., [pico-wake-word](https://github.com/henriwoodcock/pico-wake-word) or [micro_speech @6ff638](https://github.com/raspberrypi/pico-tflmicro/tree/6ff6387ed1fb3b721b0996583c4af8872980833b/examples/micro_speech)
This current implementation does not use the *microfrontend* code and instead relies on *audio_preprocessor_int8_model_data* model.
 
## HWD tests with analog microphone

[SparkFun ICS-40180 analog MEMS microphone](https://learn.sparkfun.com/tutorials/mems-microphone-hookup-guide).

For an RP2040 based microcontroller, the ADC resolution is 12-bits, sampling frequency is 48Mhz, sample at a rate of 500kS/s and ADC max voltage is 3.3V. It takes 96 clock cycles for each sample i.e., (96/48Mhz)= 2μS is the sampling time required for each sample.
 * voltage_value  =3.3 * (digital_value/(2^resolution))
 * The AUD output DC offset is at one-half the power supply voltage i.e. about 1.65V

Code has been tested on SparkFun MicroMode RP20240 board.

## TODOs

 - [ ] Deploy and test on RP2040 based microcontroller board (microphone, etc.).
    * Write code which displays the audio amplitude/waveform or spectrogram on [SparkFun MicroMod Input and Display Carrier Board](https://learn.sparkfun.com/tutorials/sparkfun-micromod-input-and-display-carrier-board-hookup-guide) 
    * Modify `command_responser.cc` to provide display indication

 - [ ] Implement some more tests, as in the [pico-wake-word](https://github.com/henriwoodcock/pico-wake-word) or [micro_speech @6ff638](https://github.com/raspberrypi/pico-tflmicro/tree/6ff6387ed1fb3b721b0996583c4af8872980833b/examples/micro_speech) implementations. Can tests be implemented to be run on the development machine?
    * Check if the use of `tflite::MicroErrorReporter` and `TF_LITE_REPORT_ERROR` instead of `MicroPrintf` is still OK. The [logging](https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/docs/logging.md) guidelines indicate *do not use* and *not support/recommend*.