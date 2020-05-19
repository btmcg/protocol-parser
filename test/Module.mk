LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := test-runner
LOCAL_CPPFLAGS := -I.
LOCAL_SOURCE_FILES := $(call rwildcard,$(LOCAL_PATH),*.cpp)
LOCAL_LIBRARIES := allocator itch util

$(call use-catch)
$(call use-fmt)

$(call add-executable-module)
