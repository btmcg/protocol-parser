MODULE_NAME := benchmark-runner
MODULE_CPPFLAGS := -I.
MODULE_LIBRARIES := allocator

$(call use-fmt)
$(call use-google-benchmark)

$(call add-executable-module,$(call get-path))
