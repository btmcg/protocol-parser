MODULE_NAME := parser
MODULE_LIBRARIES := file_reader itch util

$(call add-executable-module,$(call get-path))
