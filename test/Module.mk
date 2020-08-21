MODULE_NAME := test-runner
MODULE_CPPFLAGS := -I.
MODULE_SOURCE_FILES := $(call rwildcard,$(get-path),*.cpp)
MODULE_LIBRARIES := allocator itch protocol util

$(use-catch)
$(use-fmt)

$(call add-executable-module,$(get-path))
