LOCAL_PATH := $(call my-dir)
LOCAL_SOURCE_FILES := $(wildcard *.cpp)

$(call use-fmt)

$(call add-executable-module)
