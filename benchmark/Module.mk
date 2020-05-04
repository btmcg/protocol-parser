LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := benchmark-runner
LOCAL_CPPFLAGS := -I.
LOCAL_SOURCE_FILES := $(call rwildcard,$(LOCAL_PATH),*.cpp)
LOCAL_LIBRARIES :=

$(call use-fmt)
$(call use-google-benchmark)

$(call add-executable-module)
