LOCAL_MODULE    := bookstress-data-gen
LOCAL_LIBRARIES := file_reader protocol util

$(call add-executable-module,$(call get-path))
