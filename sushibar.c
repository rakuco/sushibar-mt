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

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sushibar_free(SushiBar *sushi)
{
  int err;

  if (sushi) {
    if ((err = pthread_mutex_destroy(sushi->block)))
      fprintf(stderr, "sushibar_free (block mutex): %s\n", strerror(err));

    if ((err = pthread_mutex_destroy(sushi->mutex)))
      fprintf(stderr, "sushibar_free (data mutex): %s\n", strerror(err));

    free(sushi->block);
    free(sushi->mutex);
    free(sushi);
  }
}

SushiBar *sushibar_new(void)
{
  int err;
  SushiBar *ret;

  ret = MEM_ALLOC(SushiBar);
  ret->eating = 0;
  ret->waiting = 0;
  ret->must_wait = 0;
  ret->mutex = MEM_ALLOC(pthread_mutex_t);
  ret->block = MEM_ALLOC(pthread_mutex_t);

  if ((err = pthread_mutex_init(ret->mutex, NULL))) {
    fprintf(stderr, "sushibar_new (data mutex): %s\n", strerror(err));

    free(ret);

    exit(1);
  }

  if ((err = pthread_mutex_init(ret->block, NULL))) {
    fprintf(stderr, "sushibar_new (block mutex): %s\n", strerror(err));

    pthread_mutex_destroy(ret->mutex);
    free(ret);

    exit(1);
  }

  return ret;
}

void *sushibar_run(void *data)
{
  SushiBar *sushi = (SushiBar*)data;

  assert(sushi);
  assert(sushi->block);
  assert(sushi->mutex);

  pthread_mutex_lock(sushi->mutex);

  if (sushi->must_wait) {
    sushi->waiting++;
    pthread_mutex_unlock(sushi->mutex);
    pthread_mutex_lock(sushi->block);
    sushi->waiting--;
  }

  sushi->eating++;
  sushi->must_wait = (sushi->eating == 5);

  if ((sushi->waiting) && (!sushi->must_wait))
    pthread_mutex_unlock(sushi->block);
  else
    pthread_mutex_unlock(sushi->mutex);

  /* eat sushi */
  printf("Ich esse sushi!\n");

  pthread_mutex_lock(sushi->mutex);
  sushi->eating--;

  if (sushi->eating == 0)
    sushi->must_wait = 0;

  if ((sushi->waiting) && (!sushi->must_wait))
    pthread_mutex_unlock(sushi->block);
  else
    pthread_mutex_unlock(sushi->mutex);

  return NULL;
}
