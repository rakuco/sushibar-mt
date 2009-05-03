/*
 * Copyright 2009  Raphael Kubo da Costa <kubito@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sushibar.h"
#include "mem.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void sushibar_free(SushiBar *sushi)
{
  if (sushi) {
    if (pthread_mutex_destroy(sushi->block))
      perror("sushibar_free (block mutex)");
    if (pthread_mutex_destroy(sushi->mutex))
      perror("sushibar_free (data mutex)");

    free(sushi);
    sushi->block = 0;
    sushi->mutex = 0;
  }
}

SushiBar *sushibar_new(void)
{
  SushiBar *ret;

  ret = MEM_ALLOC(SushiBar);
  ret->eating = 0;
  ret->waiting = 0;
  ret->must_wait = 0;

  if (pthread_mutex_init(ret->mutex, NULL)) {
    perror("sushibar_new (data mutex)");

    free(ret);

    exit(1);
  }

  if (pthread_mutex_init(ret->block, NULL)) {
    perror("sushibar_new (block mutex)");

    pthread_mutex_destroy(ret->mutex);
    free(ret);

    exit(1);
  }

  return ret;
}
