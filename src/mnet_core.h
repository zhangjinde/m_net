/* 
 * Copyright (c) 2015 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef MNET_H
#define MNET_H

#if defined(_WIN32) || defined(_WIN64)
   #define MNET_OS_WIN 1
#elif defined(__APPLE__)
   #define MNET_OS_MACOX 1
#else
   #define MNET_OS_LINUX 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   CHANN_TYPE_STREAM = 1,
   CHANN_TYPE_DGRAM,
   CHANN_TYPE_BROADCAST,
} chann_type_t;

typedef enum {
   CHANN_STATE_CLOSED = 0,
   CHANN_STATE_DISCONNECT,
   CHANN_STATE_CONNECTING,
   CHANN_STATE_CONNECTED,
   CHANN_STATE_LISTENING,
} chann_state_t;

typedef enum {
   CHANN_EVENT_RECV = 1,     /* socket has data to read */
   CHANN_EVENT_SEND,         /* socket send buf empty, inactive default */
   CHANN_EVENT_ACCEPT,       /* socket accept */
   CHANN_EVENT_CONNECTED,    /* socket connected */
   CHANN_EVENT_DISCONNECT,   /* socket disconnect when EOF or error */
} chann_event_t;

typedef struct s_mchann chann_t;

typedef struct {
   chann_event_t event;         /* event type */
   int err;                     /* errno */
   chann_t *n;                  /* chann to emit event */
   chann_t *r;                  /* chann accept from remote */
   void *opaque;                /* opaque in set_cb */
} chann_msg_t;

typedef struct {
   char ip[16];
   int port;
} chann_addr_t;

typedef void (*chann_msg_cb)(chann_msg_t*);
typedef void (*mnet_log_cb)(chann_t*, int, const char *log_string);


/* set allocator/log before init */
void mnet_allocator(void* (*new_malloc)(int),
                    void* (*new_realloc)(void*, int),
                    void  (*new_free)(void*));
void mnet_setlog(int level, mnet_log_cb); /* 0:disable, 1:error, 2:info, 3:verbose */


/* init before use chann */
int mnet_init(void);
void mnet_fini(void);
int mnet_report(int level);     /* 0:chann_count 1:chann_detail */

#define MNET_ONE_SECOND_BIT 20   /* 1 seconds == (1<<20) microseconds, not for Linux */
int mnet_poll(int microseconds); /* dispatch chann event,  */



/* channel */
chann_t* mnet_chann_open(chann_type_t type);
void mnet_chann_close(chann_t *n);

int mnet_chann_listen(chann_t *n, const char *host, int port, int backlog);

int mnet_chann_connect(chann_t *n, const char *host, int port);
void mnet_chann_disconnect(chann_t *n);

void mnet_chann_set_cb(chann_t *n, chann_msg_cb cb, void *opaque);
void mnet_chann_active_event(chann_t *n, chann_event_t et, int active);

int mnet_chann_recv(chann_t *n, void *buf, int len);
int mnet_chann_send(chann_t *n, void *buf, int len);

int mnet_chann_set_bufsize(chann_t *n, int bufsize);
int mnet_chann_cached(chann_t *n);

int mnet_chann_addr(chann_t *n, chann_addr_t*);

int mnet_chann_state(chann_t *n);
long long mnet_chann_bytes(chann_t *n, int be_send);



/* tools without init */
int mnet_resolve(char *host, int port, chann_type_t ctype, chann_addr_t*);
int mnet_parse_ipport(const char *ipport, chann_addr_t *addr);


#ifdef __cplusplus
}
#endif

#endif
