# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

TARGET_ARCH_ABI := arm64-v8a

include $(CLEAR_VARS)
LOCAL_MODULE := hook

include $(CLEAR_VARS)
LOCAL_LDLIBS     := -llog
LOCAL_CFLAGS     := -DMOD_ID='"{{ mod.id }}"' -DVERSION='"0.1.0"'
LOCAL_MODULE     := {{ mod.out }}
LOCAL_CPPFLAGS   := -std=c++2a
LOCAL_C_INCLUDES := ./include ./src
LOCAL_SRC_FILES  := ./src/main.cpp ./extern/beatsaber-hook/shared/inline-hook/And64InlineHook.cpp ./extern/beatsaber-hook/shared/utils/il2cpp-utils.cpp ./extern/beatsaber-hook/shared/utils/utils.cpp ./extern/beatsaber-hook/shared/inline-hook/inlineHook.c ./extern/beatsaber-hook/shared/inline-hook/relocate.c
include $(BUILD_SHARED_LIBRARY)