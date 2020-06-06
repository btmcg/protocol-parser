LOCAL_MODULE := benchmark-runner
LOCAL_CPPFLAGS := -I.
LOCAL_LIBRARIES := allocator

$(call use-fmt)
$(call use-google-benchmark)

$(call add-executable-module,$(call get-path))
