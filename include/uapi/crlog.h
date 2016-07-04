#ifndef UAPI_CRLOG_H__
#define UAPI_CRLOG_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/*
 * We work with up to 32 arguments in macros here. If more
 * provided -- behaviour is undefined.
 *
 * Moreover, make sure the argumens are evaluated before they
 * hit the macros, otherwise counters get screwed.
 */

/*
 * By Laurent Deniau at https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
 */
#define CRLOG_PP_NARG_(...)			CRLOG_PP_ARG_N(__VA_ARGS__)
#define CRLOG_PP_NARG(...)			CRLOG_PP_NARG_(1, ##__VA_ARGS__, CRLOG_PP_RSEQ_N())

#define CRLOG_PP_ARG_N( _0, _1, _2, _3, _4,	\
		       _5, _6, _7, _8, _9,	\
		      _10,_11,_12,_13,_14,	\
		      _15,_16,_17,_18,_19,	\
		      _20,_21,_22,_23,_24,	\
		      _25,_26,_27,_28,_29,	\
		      _30,_31,  N, ...)		N

#define CRLOG_PP_RSEQ_N()			\
		       31, 30, 29, 28, 27,	\
		       26, 25, 24, 23, 22,	\
		       21, 20, 19, 18, 17,	\
		       16, 15, 14, 13, 12,	\
		       11, 10,  9,  8,  7,	\
		        6,  5,  4,  3,  2,	\
		        1,  0

#define CRLOG_GENMASK_0(N, x)		0
#define CRLOG_GENMASK_1(N,  op, x, ...)	 (op(N,  0, x))
#define CRLOG_GENMASK_2(N,  op, x, ...)	((op(N,  1, x)) | CRLOG_GENMASK_1(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_3(N,  op, x, ...)	((op(N,  2, x)) | CRLOG_GENMASK_2(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_4(N,  op, x, ...)	((op(N,  3, x)) | CRLOG_GENMASK_3(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_5(N,  op, x, ...)	((op(N,  4, x)) | CRLOG_GENMASK_4(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_6(N,  op, x, ...)	((op(N,  5, x)) | CRLOG_GENMASK_5(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_7(N,  op, x, ...)	((op(N,  6, x)) | CRLOG_GENMASK_6(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_8(N,  op, x, ...)	((op(N,  7, x)) | CRLOG_GENMASK_7(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_9(N,  op, x, ...)	((op(N,  8, x)) | CRLOG_GENMASK_8(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_10(N, op, x, ...)	((op(N,  9, x)) | CRLOG_GENMASK_9(N,  op,  __VA_ARGS__))
#define CRLOG_GENMASK_11(N, op, x, ...)	((op(N, 10, x)) | CRLOG_GENMASK_10(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_12(N, op, x, ...)	((op(N, 11, x)) | CRLOG_GENMASK_11(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_13(N, op, x, ...)	((op(N, 12, x)) | CRLOG_GENMASK_12(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_14(N, op, x, ...)	((op(N, 13, x)) | CRLOG_GENMASK_13(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_15(N, op, x, ...)	((op(N, 14, x)) | CRLOG_GENMASK_14(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_16(N, op, x, ...)	((op(N, 15, x)) | CRLOG_GENMASK_15(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_17(N, op, x, ...)	((op(N, 16, x)) | CRLOG_GENMASK_16(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_18(N, op, x, ...)	((op(N, 17, x)) | CRLOG_GENMASK_17(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_19(N, op, x, ...)	((op(N, 18, x)) | CRLOG_GENMASK_18(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_20(N, op, x, ...)	((op(N, 19, x)) | CRLOG_GENMASK_19(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_21(N, op, x, ...)	((op(N, 20, x)) | CRLOG_GENMASK_20(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_22(N, op, x, ...)	((op(N, 21, x)) | CRLOG_GENMASK_21(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_23(N, op, x, ...)	((op(N, 22, x)) | CRLOG_GENMASK_22(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_24(N, op, x, ...)	((op(N, 23, x)) | CRLOG_GENMASK_23(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_25(N, op, x, ...)	((op(N, 24, x)) | CRLOG_GENMASK_24(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_26(N, op, x, ...)	((op(N, 25, x)) | CRLOG_GENMASK_25(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_27(N, op, x, ...)	((op(N, 26, x)) | CRLOG_GENMASK_26(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_28(N, op, x, ...)	((op(N, 27, x)) | CRLOG_GENMASK_27(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_29(N, op, x, ...)	((op(N, 28, x)) | CRLOG_GENMASK_28(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_30(N, op, x, ...)	((op(N, 29, x)) | CRLOG_GENMASK_29(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_31(N, op, x, ...)	((op(N, 30, x)) | CRLOG_GENMASK_30(N, op,  __VA_ARGS__))
#define CRLOG_GENMASK_32(N, op, x, ...)	((op(N, 31, x)) | CRLOG_GENMASK_31(N, op,  __VA_ARGS__))

#define CRLOG_CONCAT(arg1, arg2)	CRLOG_CONCAT1(arg1, arg2)
#define CRLOG_CONCAT1(arg1, arg2)	CRLOG_CONCAT2(arg1, arg2)
#define CRLOG_CONCAT2(arg1, arg2)	arg1##arg2

#define CRLOG_GENMASK_(N, op, ...)	CRLOG_CONCAT(CRLOG_GENMASK_, N)(N, op, ##__VA_ARGS__)
#define CRLOG_GENMASK(op, ...)		CRLOG_GENMASK_(CRLOG_PP_NARG(__VA_ARGS__), op, ##__VA_ARGS__)

#define crlog_genbit(ord, n, v, ...)					\
	_Generic((v),							\
									\
		 /* Basic types */					\
		 char:				0,			\
		 signed char:			0,			\
		 unsigned char:			0,			\
		 signed short int:		0,			\
		 unsigned short int:		0,			\
		 signed int:			0,			\
		 unsigned int:			0,			\
		 signed long:			0,			\
		 unsigned long:			0,			\
		 signed long long:		0,			\
		 unsigned long long:		0,			\
									\
		 /* Not used for a while */				\
		 /* float:			12, */			\
		 /* double:			13, */			\
		 /* long double:		14, */			\
									\
		 /* Basic poniters */					\
		 char *:			(1u << (ord - n - 1)),	\
		 signed char *:			(1u << (ord - n - 1)),	\
		 unsigned char *:		(1u << (ord - n - 1)),	\
		 signed short int *:		0,			\
		 unsigned short int *:		0,			\
		 signed int *:			0,			\
		 unsigned int *:		0,			\
		 signed long *:			0,			\
		 unsigned long *:		0,			\
		 signed long long *:		0,			\
		 unsigned long long *:		0,			\
		 void *:			0,			\
									\
		 /* Const basic pointers */				\
		 const char *:			(1u << (ord - n - 1)),	\
		 const signed char *:		(1u << (ord - n - 1)),	\
		 const unsigned char *:		(1u << (ord - n - 1)),	\
		 const signed short int *:	0,			\
		 const unsigned short int *:	0,			\
		 const signed int *:		0,			\
		 const unsigned int *:		0,			\
		 const signed long *:		0,			\
		 const unsigned long *:		0,			\
		 const signed long long *:	0,			\
		 const unsigned long long *:	0,			\
		 const void *:			0,			\
									\
		 /* Systypes and pointers */				\
		 default:			-1)

#define CRLOG_MAGIC			0x3130676c

typedef struct {
	uint32_t	magic;
	uint32_t	size;
	uint32_t	nargs;
	uint32_t	mask;
	uint64_t	fmt_off;	/* offset to format from msg start */
	uint64_t	args_off[0];	/* offsets to string args or direct values */
} crlog_msg_t;

typedef struct {
	int		msg_fd;
	size_t		msg_buf_size;
	bool		buf_may_grow;
} crlog_opts_t;

extern int crlog_init(void **crlog_ctl, crlog_opts_t *opts);
extern void crlog_fini(void *crlog_ctl);

extern int crlog_process_stream(int fdin, int fdout);
extern int crlog_process_msg(crlog_msg_t *msg, int fdout);
extern int crlog_process_all_msg(void *crlog_ctl, int fdout);
extern int crlog_encode_msg(void *crlog_ctl, unsigned int nargs, unsigned int mask, const char *format, ...);

#define crlog_encode(crlog_ctl, fmt, ...)						\
	crlog_encode_msg(crlog_ctl, CRLOG_PP_NARG(__VA_ARGS__),				\
			 CRLOG_GENMASK(crlog_genbit, ##__VA_ARGS__), fmt, ##__VA_ARGS__)

#endif /* UAPI_CRLOG_H__ */
