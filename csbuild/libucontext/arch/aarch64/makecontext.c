/*
 * Copyright (c) 2018 Ariadne Conill <ariadne@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "defs.h"
#include <libucontext/libucontext.h>


extern void libucontext_trampoline(void);

_Static_assert(offsetof(libucontext_ucontext_t, uc_mcontext.regs[0]) == R0_OFFSET, "R0_OFFSET is invalid");
_Static_assert(offsetof(libucontext_ucontext_t, uc_mcontext.sp) == SP_OFFSET, "SP_OFFSET is invalid");
_Static_assert(offsetof(libucontext_ucontext_t, uc_mcontext.pc) == PC_OFFSET, "PC_OFFSET is invalid");
_Static_assert(offsetof(libucontext_ucontext_t, uc_mcontext.pstate) == PSTATE_OFFSET, "PSTATE_OFFSET is invalid");

void
libucontext_makecontext(libucontext_ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	unsigned long *sp;
	unsigned long *regp;
	va_list va;
	int i;

	sp = (unsigned long *) ((uintptr_t) ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size);
	sp -= argc < 8 ? 0 : argc - 8;
	sp = (unsigned long *) (((uintptr_t) sp & -16L));

	ucp->uc_mcontext.sp = (uintptr_t) sp;
	ucp->uc_mcontext.pc = (uintptr_t) func;
	ucp->uc_mcontext.regs[19] = (uintptr_t) ucp->uc_link;
	ucp->uc_mcontext.regs[30] = (uintptr_t) &libucontext_trampoline;

	va_start(va, argc);

	regp = &(ucp->uc_mcontext.regs[0]);

	for (i = 0; (i < argc && i < 8); i++)
		*regp++ = va_arg (va, unsigned long);

	for (; i < argc; i++)
		*sp++ = va_arg (va, unsigned long);

	va_end(va);
}

#ifdef EXPORT_UNPREFIXED
extern __typeof(libucontext_makecontext) makecontext __attribute__((weak, __alias__("libucontext_makecontext")));
extern __typeof(libucontext_makecontext) __makecontext __attribute__((weak, __alias__("libucontext_makecontext")));
#endif
