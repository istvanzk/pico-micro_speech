/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================

From: https://github.com/henriwoodcock/pico-wake-word/blob/master/src/command_responser.cc
and: https://github.com/espressif/esp-tflite-micro/blob/master/examples/micro_speech/main/command_responder.cc
*/

#include "command_responder.h"
#include "tensorflow/lite/micro/micro_log.h"

#include <stdio.h>
#include "pico/stdlib.h"

#define LED_PIN 25

// The default implementation writes out the name of the recognized command
// to the error console. Real applications will want to take some custom
// action instead, and should implement their own versions of this function.
/*
void RespondToCommand(tflite::ErrorReporter* error_reporter,
                      int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command) {
  if (is_new_command) {
    TF_LITE_REPORT_ERROR(error_reporter, "Heard %s (%d) @%dms", found_command,
                         score, current_time);
  }
}
*/
void RespondToCommand(int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command) {

  // LED settings on Pico board
  static bool is_initialized = false;
  // If not initialized, setup
  if(!is_initialized) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    is_initialized = true;
  }

  if (is_new_command) {
    MicroPrintf("Heard %s (%d) @%dms", found_command, score, current_time);

    if (found_command == "yes"){
      //turn led on
      gpio_put(LED_PIN, 1);
    }
    else if (found_command == "no") {
      //turn led off
      gpio_put(LED_PIN, 0);
    }
  }
}

