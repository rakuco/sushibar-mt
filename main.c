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

#include "mem.h"
#include "slist.h"
#include "sushibar.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5

static void join_threads(void *data)
{
  pthread_t *thread = (pthread_t*)data;

  /* TODO: some error checking would be nice */
  pthread_join(*thread, NULL);
}

static void linked_list_node_destructor(void *data)
{
  free(data);
}

int main(int argc, char *argv[])
{
  size_t i;
  pthread_t *tid;
  LinkedList *thread_list;
  SushiBar *sushi;
  ThreadInformation thr[N];

  sushi = sushibar_new();
  thread_list = linked_list_new(linked_list_node_destructor);

  for (i = 0; i < N; ++i) {
    tid = MEM_ALLOC(pthread_t);
    linked_list_append(thread_list, tid);

    thr[i].bar = sushi;
    thr[i].thread_id = i;

    /* TODO: error checking? */
    pthread_create(tid, NULL, sushibar_run, &thr[i]);
  }

  linked_list_foreach(thread_list, join_threads);

  linked_list_free(thread_list);
  sushibar_free(sushi);

  return 0;
}
