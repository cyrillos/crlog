#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include <sys/param.h>
#include <sys/mman.h>

#include <ffi.h>

#include "compiler.h"
#include "crlog.h"
#include "err.h"

#ifdef DEBUG
#define pr_debug(fmt, ...)	fprintf(stderr, "crlog: " fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif
#define pr_err(fmt, ...)	fprintf(stderr, "crlog: %s:%d: " fmt, __FILE__, __LINE__,  ##__VA_ARGS__)
#define pr_perror(fmt, ...)	fprintf(stderr, "crlog: %s:%d: " fmt ": %m\n", __FILE__, __LINE__,  ##__VA_ARGS__)

static void *get_buf_end(crlog_ctl_t *ctl)
{
	return ctl->msg_buf_end;
}

static void *get_buf_head(crlog_ctl_t *ctl)
{
	return ctl->msg_buf_head;
}

static void set_buf_head(crlog_ctl_t *ctl, void *head)
{
	ctl->msg_buf_head = head;
}

static void *get_buf(crlog_ctl_t *ctl)
{
	return ctl->msg_buf;
}

/*
 * This is destructive over @m, make a copy
 * of the message if need to reuse.
 */
int crlog_process_msg(crlog_msg_t *m, int fdout)
{
	ffi_type *args[34] = {
		[0]		= &ffi_type_sint,
		[1]		= &ffi_type_pointer,
		[2 ... 33]	= &ffi_type_slong
	};
	void *values[34];
	long margs[32];
	char *fmt;
	ffi_cif cif;
	ffi_arg rc;
	size_t i;

	fmt = &((char *)m)[(long)(void *)m->fmt_off];

	values[0] = (void *)&fdout;
	values[1] = (void *)&fmt;

	for (i = 0; i < m->nargs; i++) {
		if (m->mask & (1u << i)) {
			/*
			 * Adjust string argument address
			 * from offset kept.
			 */
			margs[i] = (long)m + m->args_off[i];
			values[i + 2] = &margs[i];
		} else
			values[i + 2] = (void *)&m->args_off[i];
	}

	if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, m->nargs + 2,
			 &ffi_type_sint, args) == FFI_OK)
		ffi_call(&cif, FFI_FN(dprintf), &rc, values);
	return 0;
}

int crlog_process_all_msg(void *crlog_ctl, int fdout)
{
	crlog_msg_t *m;
	int ret = 0;

	for (m = get_buf((crlog_ctl_t *)crlog_ctl);
	     (void *)m < get_buf_end((crlog_ctl_t *)crlog_ctl) &&
	     m->magic == CRLOG_MAGIC; m = (void *)m + m->size) {
		ret = crlog_process_msg(m, fdout);
		if (ret)
			break;
	}

	return ret;
}

int crlog_process_stream(int fdin, int fdout)
{
	crlog_msg_t *m;
	size_t size;
	ssize_t ret;

	m = malloc(sizeof(*m));
	if (!m)
		return -ENOMEM;
	size = sizeof(*m);

	for (;;) {
		ret = read(fdin, m, sizeof(*m));
		if (ret <= 0) {
			if (ret < 0)
				pr_perror("Can't read data from stream");
			free(m);
			return ret;
		}

		if (m->size > size) {
			crlog_msg_t *new = realloc(m, m->size);
			if (!new) {
				pr_err("No memory for %zu bytes\n", (size_t)m->size);
				free(m);
				return -ENOMEM;
			}
			size = new->size;
			m = new;
		}

		ret = read(fdin, &m->args_off[0], m->size - sizeof(*m));
		if (ret < (m->size - sizeof(*m))) {
			pr_err("Read %zu bytes but expected %zu\n",
			       ret, (m->size - sizeof(*m)));
			free(m);
			return -EIO;
		}

		crlog_process_msg(m, fdout);
	}

	return 0;
}

static int alloc_room(crlog_ctl_t *ctl, size_t size)
{
	char *new = ctl->msg_buf_head + size;
	char *old_msg_buf_head;
	char *old_msg_buf;
	size_t new_size;

	if (ctl->msg_buf_end > new)
		return 0;
	else if (!ctl->buf_may_grow)
		return -ENOSPC;

	new_size = (size_t)(ctl->msg_buf_size * 2);
	old_msg_buf_head = ctl->msg_buf_head;
	old_msg_buf = ctl->msg_buf;

	pr_debug("Growing from %zu to %zu\n", (size_t)ctl->msg_buf_size, new_size);

	new = mremap(ctl->msg_buf, ctl->msg_buf_size, new_size, MREMAP_MAYMOVE, ctl->msg_buf);
	if (new == MAP_FAILED) {
		pr_perror("Can't remap %zu bytes to %zu",
			  (size_t)ctl->msg_buf_size, new_size);
		return -ENOMEM;
	}
	ctl->msg_buf = new;
	ctl->msg_buf_size = new_size;
	ctl->msg_buf_end = new + new_size;
	ctl->msg_buf_head = new + (old_msg_buf_head - old_msg_buf);

	if (ctl->msg_buf_fd != -1) {
		if (ftruncate(ctl->msg_buf_fd, ctl->msg_buf_size)) {
			int ret = -errno;
			pr_perror("Can't truncate to %zu bytes",
				  ctl->msg_buf_size);
			return ret;
		}
	}

	return 0;
}

static void *queue_data(crlog_ctl_t *ctl, size_t size)
{
	char *new = ctl->msg_buf_head + size;

	if (ctl->msg_buf_end > new) {
		void *old = ctl->msg_buf_head;
		ctl->msg_buf_head = new;
		return old;
	}

	return ERR_PTR(-ENOMEM);
}

static void *queue_string(crlog_ctl_t *ctl, const char *s)
{
	void *prev_head = get_buf_head(ctl);
	char *str;

	str = memccpy(ctl->msg_buf_head, s, 0,
		      ctl->msg_buf_end - ctl->msg_buf_head);
	if (!str)
		return ERR_PTR(-ENOMEM);
	ctl->msg_buf_head = (void *)str;
	return prev_head;
}

int crlog_encode_msg(void *crlog_ctl, unsigned int nargs,
		     unsigned int mask, const char *format, ...)
{
	crlog_ctl_t *ctl = crlog_ctl;
	void *prev_head;

	size_t i, m_size;
	crlog_msg_t *m;
	va_list argptr;
	void *p;

	if (alloc_room(ctl, 4096))
		return -ENOMEM;

	prev_head = get_buf_head(crlog_ctl);
	m = queue_data(ctl, sizeof(*m) + nargs * sizeof(m->args_off[0]));
	if (IS_ERR(m))
		return PTR_ERR(m);

	m->magic = CRLOG_MAGIC;
	m->nargs = nargs;
	m->mask = mask;

	p = queue_string(ctl, format);
	if (IS_ERR(p)) {
		set_buf_head(ctl, prev_head);
		return PTR_ERR(p);
	}
	m->fmt_off = (uint64_t)p - (uint64_t)(void *)m;

	va_start(argptr, format);
	for (i = 0; i < nargs; i++) {
		m->args_off[i] = (long)va_arg(argptr, uint64_t);
		if (mask & (1u << i)) {
			p = queue_string(ctl, (void *)m->args_off[i]);
			if (IS_ERR(p)) {
				set_buf_head(ctl, prev_head);
				return PTR_ERR(p);
			}
			m->args_off[i] = (uint64_t)p - (uint64_t)(void *)m;
		}
	}
	va_end(argptr);

	m_size = get_buf_head(crlog_ctl) - prev_head;
	m->size = roundup(m_size, 8);
	if (m->size != m_size) {
		p = queue_data(ctl, m->size - m_size);
		if (IS_ERR(p)) {
			set_buf_head(ctl, prev_head);
			return PTR_ERR(p);
		}
	}

	return 0;
}

void crlog_fini(void *crlog_ctl)
{
	crlog_ctl_t *ctl = crlog_ctl;

	if (ctl->msg_buf_fd >= 0) {
		ftruncate(ctl->msg_buf_fd, ctl->msg_buf_head - ctl->msg_buf);
		close(ctl->msg_buf_fd);
	}
	munmap(ctl->msg_buf, ctl->msg_buf_size);
	free(ctl);
}

int crlog_init(void **crlog_ctl, crlog_opts_t *opts)
{
	bool has_file = false;
	crlog_ctl_t *ctl;

	if (!opts || !crlog_ctl)
		return -EINVAL;

	opts->msg_buf_size = round_up(max((size_t)opts->msg_buf_size,
					  (size_t)512), 8);

	ctl = malloc(sizeof(*ctl));
	if (!ctl)
		return -ENOMEM;
	memset(ctl, 0, sizeof(*ctl));

	if (opts->msg_fd >= 0)
		has_file = true;

	ctl->msg_buf_size = opts->msg_buf_size;
	ctl->msg_buf = mmap(NULL, ctl->msg_buf_size,
			     PROT_READ | PROT_WRITE,
			     (has_file ?  (MAP_SHARED) :
			      (MAP_ANONYMOUS | MAP_PRIVATE)) |
			     MAP_POPULATE,
			     has_file ? opts->msg_fd : -1, 0);
	if (ctl->msg_buf == MAP_FAILED) {
		pr_perror("Can't map for %zu bytes", ctl->msg_buf_size);
		free(ctl);
		return -ENOMEM;
	}

	if (has_file) {
		if (ftruncate(opts->msg_fd, ctl->msg_buf_size)) {
			int ret = -errno;
			pr_perror("Can't trucate for %zu bytes",
				  ctl->msg_buf_size);
			munmap(ctl->msg_buf, ctl->msg_buf_size);
			free(ctl);
			return ret;
		}
	}

	ctl->msg_buf_head = ctl->msg_buf;
	ctl->msg_buf_end = ctl->msg_buf + ctl->msg_buf_size;
	ctl->msg_buf_fd = has_file ? opts->msg_fd : -1;
	ctl->buf_may_grow = opts->buf_may_grow;

	*crlog_ctl = ctl;
	return 0;
}
