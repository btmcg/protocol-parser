MODULE_LIBRARIES := util
MODULE_SOURCE_FILES := $(call rwildcard,$(get-path),*.cpp)
MODULE_GENERATED_FILES := itch/itch.cppgen.hpp

$(use-fmt)

$(call add-static-library-module,$(get-path))
