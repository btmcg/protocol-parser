LOCAL_PATH := $(call my-dir)
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := allocator common file_reader itch_parser

$(call use-fmt)

$(call add-executable-module)
