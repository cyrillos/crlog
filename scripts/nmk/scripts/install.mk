ifndef ____nmk_defined__install

DESTDIR		?= /
PREFIX		?= /usr/local
BINDIR		?= $(PREFIX)/bin
SBINDIR		?= $(PREFIX)/sbin
MANDIR		?= $(PREFIX)/share/man
SYSTEMDUNITDIR	?= $(PREFIX)/lib/systemd/system/
LOGROTATEDIR	?= $(PREFIX)/etc/logrotate.d/
LIBDIR		?= $(PREFIX)/lib
INCLUDEDIR	?= $(PREFIX)/include/
LIBEXECDIR	?= $(PREFIX)/libexec

export DESTDIR PREFIX BINDIR SBINDIR MANDIR SYSTEMDUNITDIR
export LOGROTATEDIR LIBDIR INCLUDEDIR LIBEXECDIR

#
# Footer.
$(__nmk_dir)scripts/install.mk:
	@true
____nmk_defined__install = y
endif
