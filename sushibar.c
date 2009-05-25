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
#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sushibar_free(SushiBar *sushi)
{
  if (sushi) {
    if (sem_destroy(sushi->block))
      perror("sushibar_free (block semaphore)");

    if (sem_destroy(sushi->mutex))
      perror("sushibar_free (mutex semaphore)");

    free(sushi->block);
    free(sushi->mutex);
    free(sushi);
  }
}

SushiBar *sushibar_new(void)
{
  SushiBar *ret;

  ret = MEM_ALLOC(SushiBar);
  ret->eating = 0;
  ret->waiting = 0;
  ret->must_wait = 0;
  ret->mutex = MEM_ALLOC(sem_t);
  ret->block = MEM_ALLOC(sem_t);

  if (sem_init(ret->mutex, 0, 1)) {
    perror("sushibar_new (mutex semaphore)");
    free(ret);
    exit(1);
  }

  if (sem_init(ret->block, 0, 0)) {
    perror("sushibar_new (block semaphore)");
    sem_destroy(ret->mutex);
    free(ret);
    exit(1);
  }

  return ret;
}

void *sushibar_run(void *data)
{
  SushiBar *sushi = (SushiBar*)((Foo*)data)->bar;
  size_t id = (size_t)((Foo*)data)->id;

  assert(sushi);
  assert(sushi->block);
  assert(sushi->mutex);

  sem_wait(sushi->mutex);

  /*printf("%02u: Creating thread...\n", id);*/

  if (sushi->must_wait) {
    sushi->waiting++;
    sem_post(sushi->mutex);
    sem_wait(sushi->block);
    sushi->waiting--;
  }

  sushi->eating++;
  sushi->must_wait = (sushi->eating == 5);

  if ((sushi->waiting) && (!sushi->must_wait))
    sem_post(sushi->block);
  else
    sem_post(sushi->mutex);

  /* eat sushi */
  printf("%02u: Eating sushi. Eaters: %u. Waiting: %u.\n", id, sushi->eating, sushi->waiting);
  sleep(1);
  /*printf("%02u: bla!\n", id);*/

  sem_wait(sushi->mutex);
  sushi->eating--;

  /*printf("%02u: Finished eating...\n", id);*/

  if (sushi->eating == 0)
    sushi->must_wait = 0;

  if ((sushi->waiting) && (!sushi->must_wait))
    sem_post(sushi->block);
  else
    sem_post(sushi->mutex);

  return NULL;
}
