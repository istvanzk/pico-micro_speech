
cmake_minimum_required(VERSION 3.12)

project(micro_speech C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)


add_executable(command_responder_test "")

target_include_directories(command_responder_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/.
)

set_target_properties(
  command_responder_test
  PROPERTIES
  COMPILE_FLAGS -fno-rtti
  COMPILE_FLAGS -fno-exceptions
  COMPILE_FLAGS -fno-threadsafe-statics
  COMPILE_FLAGS -nostdlib
)

target_sources(command_responder_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/command_responder.cpp
  ${CMAKE_CURRENT_LIST_DIR}/command_responder_test.cpp
  ${CMAKE_CURRENT_LIST_DIR}/command_responder.h
)

target_link_libraries(
  command_responder_test
  pico-tflmicro
  hardware_pwm
)

# enable usb output, disable uart output
pico_enable_stdio_usb(command_responder_test 1)
pico_enable_stdio_uart(command_responder_test 0)
# create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(command_responder_test)


add_executable(micro_speech_test "")

target_include_directories(micro_speech_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/.
)

set_target_properties(
  micro_speech_test
  PROPERTIES
  COMPILE_FLAGS -fno-rtti
  COMPILE_FLAGS -fno-exceptions
  COMPILE_FLAGS -fno-threadsafe-statics
  COMPILE_FLAGS -nostdlib
)

target_sources(micro_speech_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/models/micro_speech_quantized_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/models/audio_preprocessor_int8_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/testdata/no_1000ms_audio_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/testdata/yes_1000ms_audio_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/testdata/no_30ms_audio_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/testdata/yes_30ms_audio_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/testdata/noise_1000ms_audio_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/testdata/silence_1000ms_audio_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/micro_speech_test.cpp
  ${CMAKE_CURRENT_LIST_DIR}/models/micro_speech_quantized_model_data.h
  ${CMAKE_CURRENT_LIST_DIR}/models/audio_preprocessor_int8_model_data.h
  ${CMAKE_CURRENT_LIST_DIR}/testdata/no_1000ms_audio_data.h
  ${CMAKE_CURRENT_LIST_DIR}/testdata/yes_1000ms_audio_data.h
  ${CMAKE_CURRENT_LIST_DIR}/testdata/no_30ms_audio_data.h
  ${CMAKE_CURRENT_LIST_DIR}/testdata/yes_30ms_audio_data.h
  ${CMAKE_CURRENT_LIST_DIR}/testdata/noise_1000ms_audio_data.h
  ${CMAKE_CURRENT_LIST_DIR}/testdata/silence_1000ms_audio_data.h
)

target_link_libraries(
  micro_speech_test
  pico-tflmicro
  hardware_pwm
  pico-tflmicro_test
)

# enable usb output, disable uart output
pico_enable_stdio_usb(micro_speech_test 1)
pico_enable_stdio_uart(micro_speech_test 0)
# create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(micro_speech_test)


add_executable(micro_speech "")

target_include_directories(micro_speech
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/.
)

set_target_properties(
  micro_speech
  PROPERTIES
  COMPILE_FLAGS -fno-rtti
  COMPILE_FLAGS -fno-exceptions
  COMPILE_FLAGS -fno-threadsafe-statics
  COMPILE_FLAGS -nostdlib
)

target_sources(micro_speech
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/audio_provider.cpp
  ${CMAKE_CURRENT_LIST_DIR}/command_responder.cpp
  ${CMAKE_CURRENT_LIST_DIR}/feature_provider.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main_functions.cpp
  ${CMAKE_CURRENT_LIST_DIR}/micro_features_generator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/models/micro_speech_quantized_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/models/audio_preprocessor_int8_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/recognize_commands.cpp
  ${CMAKE_CURRENT_LIST_DIR}/audio_provider.h
  ${CMAKE_CURRENT_LIST_DIR}/command_responder.h
  ${CMAKE_CURRENT_LIST_DIR}/feature_provider.h
  ${CMAKE_CURRENT_LIST_DIR}/main_functions.h
  ${CMAKE_CURRENT_LIST_DIR}/micro_features_generator.h
  ${CMAKE_CURRENT_LIST_DIR}/micro_model_settings.h
  ${CMAKE_CURRENT_LIST_DIR}/models/micro_speech_quantized_model_data.h
  ${CMAKE_CURRENT_LIST_DIR}/models/audio_preprocessor_int8_model_data.h
  ${CMAKE_CURRENT_LIST_DIR}/recognize_commands.h
)

target_link_libraries(
  micro_speech
  pico-tflmicro
  pico_analog_microphone
  hardware_adc
  hardware_dma
  hardware_pwm
  # For PDM microphone add: hardware_pio
)

# enable usb output, disable uart output
pico_enable_stdio_usb(micro_speech 1)
pico_enable_stdio_uart(micro_speech 0)
# create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(micro_speech)

add_subdirectory("pico-microphone")