ifndef ____nmk_defined__utils

#
# Generate versions.
define gen-versions
$(1)_VERSION_MAJOR	:= $(2)
$(1)_VERSION_MINOR	:= $(3)
$(1)_VERSION_SUBLEVEL	:= $(4)
$(1)_VERSION_EXTRA	:= $(5)
$(1)_VERSION_NAME	:= $(6)
$(1)_VERSION		:= $($(1)_VERSION_MAJOR)$(if $($(1)_VERSION_MINOR),.$($(1)_VERSION_MINOR))$(if $($(1)_VERSION_SUBLEVEL),.$($(1)_VERSION_SUBLEVEL))$(if $($(1)_VERSION_EXTRA),.$($(1)_VERSION_EXTRA))

export $(1)_VERSION_MAJOR $(1)_VERSION_MINOR $(1)_VERSION_SUBLEVEL
export $(1)_VERSION_EXTRA $(1)_VERSION_NAME $(1)_VERSION
endef

#
# Usage: option = $(call try-cc,source-to-build,cc-options,cc-defines)
try-cc = $(shell sh -c                                                          \
                'TMP="$(OUTPUT)$(TMPOUT).$$$$";                                 \
                 echo "$(1)" |                                                  \
                 $(CC) $(3) -x c - $(2) -o "$$TMP" > /dev/null 2>&1 && echo y;  \
                 rm -f "$$TMP"')

# pkg-config-check
# Usage: ifeq ($(call pkg-config-check, library),y)
pkg-config-check = $(shell sh -c 'pkg-config $(1) && echo y')

#
# Remove duplicates.
uniq = $(strip $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1))))

#
# Add $(obj)/ for paths that are not relative
objectify = $(foreach o,$(sort $(call uniq,$(1))),$(if $(filter /% ./% ../%,$(o)),$(o),$(obj)/$(o)))

# To cleanup entries.
cleanify = $(foreach o,$(sort $(call uniq,$(1))),$(o) $(o:.o=.d) $(o:.o=.i) $(o:.o=.s))

#
# Footer.
$(__nmk_dir)scripts/utils.mk:
	@true
____nmk_defined__utils = y
endif
