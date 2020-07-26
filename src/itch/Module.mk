MODULE_LIBRARIES := allocator util

$(use-fmt)

$(call add-static-library-module,$(get-path))
