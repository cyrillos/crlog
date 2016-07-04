#ifndef CRLOG_H__
#define CRLOG_H__

#include <stdbool.h>

#include "uapi/crlog.h"

/* This controls the crlog engine instance */
typedef struct {
	char	*msg_buf;
	char	*msg_buf_head;
	char	*msg_buf_end;
	size_t	msg_buf_size;
	int	msg_buf_fd;
	bool	buf_may_grow;
} crlog_ctl_t;

#endif /* CRLOG_H__ */
