MODULE_LIBRARIES := util
MODULE_SOURCE_FILES := $(call rwildcard,$(get-path),*.cpp)

$(use-fmt)

$(call add-static-library-module,$(get-path))
