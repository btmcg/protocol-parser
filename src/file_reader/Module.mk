MODULE_LDLIBS := -lz
MODULE_LIBRARIES := util

$(use-fmt)

$(call add-static-library-module,$(get-path))
