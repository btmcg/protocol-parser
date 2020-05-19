LOCAL_PATH := $(call my-dir)
LOCAL_MODULE := parser
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := file_reader itch util

$(call add-executable-module)
