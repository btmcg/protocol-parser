LOCAL_MODULE    := book-stressor
LOCAL_LIBRARIES := file_reader itch protocol util

$(call use-fmt)

$(call add-executable-module,$(call get-path))
