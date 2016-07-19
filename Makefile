__nmk_dir=scripts/nmk/scripts/
export __nmk_dir

include $(__nmk_dir)include.mk
include $(__nmk_dir)install.mk
include $(__nmk_dir)macro.mk
include $(__nmk_dir)utils.mk

CFLAGS			+= $(USERCFLAGS)
export CFLAGS

SRC_DIR			:= $(CURDIR)
export SRC_DIR

DEFINES			+= -D_FILE_OFFSET_BITS=64
DEFINES			+= -D_GNU_SOURCE

WARNINGS		:= -Wall

ifneq ($(WERROR),0)
        WARNINGS	+= -Werror
endif

ifeq ($(DEBUG),1)
        DEFINES		+= -DDEBUG
        CFLAGS		+= -O0 -ggdb3
else
        CFLAGS		+= -O2 -g
endif

CFLAGS			+= -iquote include
CFLAGS			+= $(WARNINGS) $(DEFINES)

CRLOG_SO		:= libcrlog.so
CRLOG_UAPI_HEADERS	:= include/uapi/crlog.h

cflags-so		+= -rdynamic -Wl,-soname,$(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR)

$(eval $(call gen-versions,CRLOGSO,1,0,,,))
$(eval $(call gen-built-in,src))

$(CRLOG_SO): src/built-in.o
	$(call msg-link, $@)
	$(Q) $(CC) -shared $(cflags-so) -o $@ $^

CLI			:= crlog
CLI-LIBS		:= -lffi

$(eval $(call gen-built-in,cli))

$(CLI): cli/built-in.o src/built-in.o
	$(call msg-link, $@)
	$(Q) $(CC) -o $@ $(CLI-LIBS) $^

clean:
	$(Q) $(MAKE) $(build)=src $@
	$(Q) $(MAKE) $(build)=cli $@
	$(Q) $(RM) $(CRLOG_SO)
	$(Q) $(RM) $(CLI)
.PHONY: clean

UAPI_HEADERS := include/uapi/crlog.h

install: $(CRLOG_SO) $(CLI)
	$(E) "  INSTALL " $(CLI) $(CRLOG_SO)
	$(Q) mkdir -p $(DESTDIR)$(SBINDIR)
	$(Q) install -m 755 $(CLI) $(DESTDIR)$(SBINDIR)
	$(Q) mkdir -p $(DESTDIR)$(INCLUDEDIR)
	$(Q) install -m 644 $(UAPI_HEADERS) $(DESTDIR)$(INCLUDEDIR)
	$(Q) mkdir -p $(DESTDIR)$(LIBDIR)
	$(Q) install -m 755 $(CRLOG_SO) $(DESTDIR)$(LIBDIR)/$(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR).$(CRLOGSO_VERSION_MINOR)
	$(Q) ln -fns $(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR).$(CRLOGSO_VERSION_MINOR) $(DESTDIR)$(LIBDIR)/$(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR)
	$(Q) ln -fns $(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR).$(CRLOGSO_VERSION_MINOR) $(DESTDIR)$(LIBDIR)/$(CRLOG_SO)
.PHONY: install

uninstall:
	$(E) " UNINSTALL" $(CRLOG_SO) $(CLI)
	$(Q) $(RM) $(addprefix $(DESTDIR)$(SBINDIR)/,$(CLI))
	$(Q) $(RM) $(addprefix $(DESTDIR)$(INCLUDEDIR)/,$(notdir $(UAPI_HEADERS)))
	$(Q) $(RM) $(addprefix $(DESTDIR)$(LIBDIR)/,$(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR))
	$(Q) $(RM) $(addprefix $(DESTDIR)$(LIBDIR)/,$(CRLOG_SO))
	$(Q) $(RM) $(addprefix $(DESTDIR)$(LIBDIR)/,$(CRLOG_SO).$(CRLOGSO_VERSION_MAJOR).$(CRLOGSO_VERSION_MINOR))
.PHONY: uninstall

tags:
	$(call msg-gen, $@)
	$(Q) $(RM) tags
	$(Q) $(FIND) . -name '*.[hcS]' ! -path './.*' ! -path './test/*' -print | xargs $(CTAGS) -a
.PHONY: tags

etags:
	$(call msg-gen, $@)
	$(Q) $(RM) TAGS
	$(Q) $(FIND) . -name '*.[hcS]' ! -path './.*' ! -path './test/*' -print | xargs $(ETAGS) -a
.PHONY: etags

cscope:
	$(call msg-gen, $@)
	$(Q) $(FIND) . -name '*.[hcS]' ! -path './.*' ! -path './test/*' ! -type l -print > cscope.files
	$(Q) $(CSCOPE) -bkqu
.PHONY: cscope

all: $(CRLOG_SO) $(CLI)
	@true
.PHONY: all

test: $(CLI)
	$(Q) tests/test00
.PHONY: test

.DEFAULT_GOAL := all
