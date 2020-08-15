MODULE_LIBRARIES := allocator protocol util

$(use-fmt)

$(call add-static-library-module,$(get-path))
