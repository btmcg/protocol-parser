#￼Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


GIT := /usr/bin/git
VERSION := $(shell $(GIT) describe --abbrev=4 --always --dirty --tags)
VERSION_FILE := src/version.hpp

$(VERSION_FILE): .git/HEAD .git/index
	@echo "#pragma once" > $@
	@echo "constexpr char const* VERSION = \"$(VERSION)\";" >> $@

# make sure the file is a dependency of all targets, so it is always
# built and there aren't any include errors
all: $(VERSION_FILE)
$(foreach module,$(call get-all-modules), \
  $(eval $(module): $(VERSION_FILE)) \
)
