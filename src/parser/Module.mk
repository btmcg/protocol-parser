LOCAL_PATH := $(call my-dir)
LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := common file_reader

$(call use-fmt)

$(call add-executable-module)
