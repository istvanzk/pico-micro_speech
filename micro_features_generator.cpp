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

NOTES: 
    * The older version at https://github.com/henriwoodcock/pico-wake-word/lib/
      pico-tflmicro @ 6ff6387/examples/micro_speech/micro_feature_generator.cpp
      used tensorflow/lite/experimental/microfrontend/lib/ implementation.
    * The newer (2019+) tensorflow lite for micro implementation uses the audio_preprocessor_int8_model_data
    See https://github.com/espressif/esp-tflite-micro/blob/master/examples/micro_speech/main/micro_features_generator.cc
*/


#include "micro_features_generator.h"

#include <cmath>
#include <cstring>

#include "tensorflow/lite/core/c/common.h"
#include "micro_model_settings.h"
#include "models/audio_preprocessor_int8_model_data.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"


namespace {

bool g_is_first_time = true;

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

constexpr size_t kArenaSize = 16 * 1024;
alignas(16) uint8_t g_arena[kArenaSize];

constexpr int kAudioSampleDurationCount =
    kFeatureSliceDurationMs * kAudioSampleFrequency / 1000;
constexpr int kAudioSampleStrideCount =
    kFeatureSliceStrideMs * kAudioSampleFrequency / 1000;
using AudioPreprocessorOpResolver = tflite::MicroMutableOpResolver<18>;
}  // namespace

//TF_LITE_ENSURE_STATUS defined in tflite-micro/tensorflow/lite/core/c/common.h
TfLiteStatus RegisterOps(AudioPreprocessorOpResolver& op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
  TF_LITE_ENSURE_STATUS(op_resolver.AddCast());
  TF_LITE_ENSURE_STATUS(op_resolver.AddStridedSlice());
  TF_LITE_ENSURE_STATUS(op_resolver.AddConcatenation());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMul());
  TF_LITE_ENSURE_STATUS(op_resolver.AddAdd());
  TF_LITE_ENSURE_STATUS(op_resolver.AddDiv());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMinimum());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMaximum());
  TF_LITE_ENSURE_STATUS(op_resolver.AddWindow());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFftAutoScale());
  TF_LITE_ENSURE_STATUS(op_resolver.AddRfft());
  TF_LITE_ENSURE_STATUS(op_resolver.AddEnergy());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBank());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBankSquareRoot());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBankSpectralSubtraction());
  TF_LITE_ENSURE_STATUS(op_resolver.AddPCAN());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFilterBankLog());
  return kTfLiteOk;
}

TfLiteStatus InitializeMicroFeatures() {

  g_is_first_time = true;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_audio_preprocessor_int8_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf("Audio Preprocessor Model provided for Micro Feature generator is schema version %d "
                "not equal to supported version %d.", model->version(), TFLITE_SCHEMA_VERSION);
    return kTfLiteError;
  }

  static AudioPreprocessorOpResolver op_resolver;
  RegisterOps(op_resolver);

  static tflite::MicroInterpreter static_interpreter(model, op_resolver, g_arena, kArenaSize);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    MicroPrintf("AllocateTensors failed for Micro Feature provider model. Line %d", __LINE__);
    return kTfLiteError;
  }

  return kTfLiteOk;  
}

TfLiteStatus GenerateSingleFeature(const int16_t* audio_data,
                                   const int audio_data_size,
                                   int8_t* feature_output,
                                   tflite::MicroInterpreter* interpreter) {
  TfLiteTensor* input = interpreter->input(0);
  TfLiteTensor* output = interpreter->output(0);
  std::copy_n(audio_data, audio_data_size,
              tflite::GetTensorData<int16_t>(input));
  if (interpreter->Invoke() != kTfLiteOk) {
    MicroPrintf("Single Feature generator model invocation failed");
  }

  std::copy_n(tflite::GetTensorData<int8_t>(output), kFeatureSliceSize,
              feature_output);

  return kTfLiteOk;
}

TfLiteStatus GenerateMicroFeatures(const int16_t* input, 
                                   int input_size,
                                   Features* features_output) {
  size_t remaining_samples = input_size;
  size_t feature_index = 0;
  while (remaining_samples >= kAudioSampleDurationCount &&
         feature_index < kFeatureSliceCount) {
    TF_LITE_ENSURE_STATUS(
        GenerateSingleFeature(input, kAudioSampleDurationCount,
                              (*features_output)[feature_index], interpreter));
    feature_index++;
    input += kAudioSampleStrideCount;
    remaining_samples -= kAudioSampleStrideCount;
  }

  return kTfLiteOk;

}