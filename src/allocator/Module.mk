LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES := util

$(call use-fmt)

$(call add-static-library-module,$(call get-path))
