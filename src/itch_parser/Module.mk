LOCAL_PATH := $(call my-dir)
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := allocator common

$(call use-fmt)

$(call add-static-library-module)
