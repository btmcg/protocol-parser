LOCAL_PATH := $(call my-dir)
LOCAL_MODULE := parser
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := file_reader itch_parser util

$(call add-executable-module)
