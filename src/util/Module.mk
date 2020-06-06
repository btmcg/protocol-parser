LOCAL_SOURCE_FILES := $(wildcard *.cpp)
LOCAL_LIBRARIES :=

$(call use-fmt)

$(call add-static-library-module,$(call get-path))
