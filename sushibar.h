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

#ifndef SUSHIBAR_H
#define SUSHIBAR_H

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

/**
 * Structure representing the sushi bar.
 *
 * This should actually be a singleton.
 *
 * @author Raphael Kubo da Costa <kubito@gmail.com>
 */
typedef struct {
  size_t eating;    /**< How many people are currently eating. */
  size_t waiting;   /**< How many people are currently waiting. */

  char   must_wait; /**< Flag to indicate if all seats are taken. */

  sem_t *mutex;     /**< Data protection semaphore. */
  sem_t *block;     /**< Waiting semaphore used when all seats are taken. */
} SushiBar;

/**
 * Structure with information on each running thread.
 *
 * @author Raphael Kubo da Costa <kubito@gmail.com>
 */
typedef struct {
  size_t id;          /**< Thread ID. */
  SushiBar *sushibar; /**< The SushiBar instance. */
  pthread_t thread;   /**< A pthread_t instance. */
} ThreadInformation;

/**
 * Frees memory allocated for a SushiBar.
 *
 * @param sushi The SushiBar instance to free.
 */
void sushibar_free(SushiBar *sushi);

/**
 * Creates a new SushiBar.
 */
SushiBar *sushibar_new(void);

/**
 * Implementation of the algorithm.
 *
 * @param data A @p ThreadInformation instance.
 */
void *sushibar_run(void *data);

#endif
