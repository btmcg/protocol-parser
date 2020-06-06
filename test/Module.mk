LOCAL_MODULE := test-runner
LOCAL_CPPFLAGS := -I.
LOCAL_SOURCE_FILES := $(call rwildcard,$(call get-path),*.cpp)
LOCAL_LIBRARIES := allocator itch util

$(call use-catch)
$(call use-fmt)

$(call add-executable-module,$(call get-path))
