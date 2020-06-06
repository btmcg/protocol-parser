LOCAL_MODULE := benchmark-runner
LOCAL_CPPFLAGS := -I.
LOCAL_SOURCE_FILES := $(call rwildcard,$(call get-path),*.cpp)
LOCAL_LIBRARIES := allocator

$(call use-fmt)
$(call use-google-benchmark)

$(call add-executable-module,$(call get-path))
