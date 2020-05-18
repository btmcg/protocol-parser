LOCAL_PATH := $(call my-dir)
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := common file_reader itch_parser

$(call add-executable-module)
