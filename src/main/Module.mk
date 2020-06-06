LOCAL_MODULE := parser
LOCAL_LIBRARIES := file_reader itch util

$(call add-executable-module,$(call get-path))
