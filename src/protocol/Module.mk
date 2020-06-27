LOCAL_LIBRARIES := util
LOCAL_SOURCE_FILES := $(call rwildcard,$(call get-path),*.cpp)

$(call use-fmt)

$(call add-static-library-module,$(call get-path))
