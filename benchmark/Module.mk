MODULE_NAME := benchmark-runner
MODULE_CPPFLAGS := -I.
MODULE_LIBRARIES := allocator

$(use-fmt)
$(use-google-benchmark)

$(call add-executable-module,$(get-path))
