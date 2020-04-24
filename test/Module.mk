LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := test-runner
LOCAL_CPPFLAGS := -I.
LOCAL_SOURCE_FILES := $(call rwildcard,$(LOCAL_PATH),*.cpp)

$(call use-catch)

$(call add-executable-module)
