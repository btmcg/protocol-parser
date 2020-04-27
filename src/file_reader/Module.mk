LOCAL_PATH := $(call my-dir)
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LDLIBS := -lz

$(call use-fmt)

$(call add-executable-module)
