LOCAL_MODULE    := bookstress-data-read
LOCAL_LIBRARIES := file_reader protocol util

$(call use-fmt)

$(call add-executable-module,$(call get-path))
