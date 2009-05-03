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

#include "slist.h"
#include "mem.h"

#include <assert.h>

void linked_list_append(LinkedList *list, void *data)
{
  ListNode *iter;
  ListNode *node;

  assert(list);

  node = MEM_ALLOC(ListNode);
  node->data = data;
  node->next = NULL;

  if (!list->head)
    list->head = node;
  else {
    iter = list->head;

    while (iter->next)
      iter = iter->next;

    iter->next = node;
  }
}

void linked_list_foreach(LinkedList *list, void (*func)(void*))
{
  ListNode *iter;

  if (list) {
    iter = list->head;

    while (iter) {
      func(iter->data);
      iter = iter->next;
    }
  }
}

void linked_list_free(LinkedList *list)
{
  ListNode *item;
  ListNode *next;

  if (list) {
    item = list->head;

    while (item) {
      next = item->next;

      list->node_destructor(item->data);
      free(item);

      item = next;
    }

    free(list);
  }
}

int linked_list_is_empty(LinkedList *list)
{
  return linked_list_size(list) == 0;
}

LinkedList *linked_list_new(void (*node_destructor)(void*))
{
  LinkedList *ret = MEM_ALLOC(LinkedList);

  ret->head = NULL;
  ret->node_destructor = node_destructor;
  ret->size = 0;

  return ret;
}

size_t linked_list_size(LinkedList *list)
{
  if (!list)
    return 0;

  return list->size;
}
