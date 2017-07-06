/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__

#include <time.h>

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

/**
 * 事件处理Loop
 */
struct aeEventLoop;

/* Types and data structures */
/**
 * 文件事件处理方法
 */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
/**
 * 时间事件处理方法
 */
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* File event structure */
/**
 * 文件事件数据结构
 */
typedef struct aeFileEvent {
    int mask; /* 事件类型， 可读事件或者可写事件 one of AE_(READABLE|WRITABLE) */
    aeFileProc *rfileProc;  // 文件事件处理函数
    aeFileProc *wfileProc;  // 文件事件处理函数
    void *clientData;       // 事件数据
} aeFileEvent;

/* Time event structure */
/**
 * 时间事件数据结构
 */
typedef struct aeTimeEvent {
    long long id; /* time event identifier.  AE_DELETED_EVENT_ID 删除标识*/
    long when_sec; /* seconds 事件应该被处理的时间 秒*/
    long when_ms; /* milliseconds* 事件应该被处理的时间 毫秒 */
    aeTimeProc *timeProc;  //时间事件处理函数
    aeEventFinalizerProc *finalizerProc;
    void *clientData;    //事件数据
    struct aeTimeEvent *next;  // 下一个时间事件
} aeTimeEvent;

/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
/**
 * 事件处理Loop的数据结构
 */
typedef struct aeEventLoop {
    int maxfd;   /* highest file descriptor currently registered */
    int setsize; /* max number of file descriptors tracked */
    long long timeEventNextId; // 时间事件ID，创建时间事件是递增
    time_t lastTime;     /* Used to detect system clock skew */
    aeFileEvent *events; /* Registered events */  // 文件事件列表
    aeFiredEvent *fired; /* Fired events */
    aeTimeEvent *timeEventHead; // 时间事件列表
    int stop;      // 停止标识
    void *apidata; /*使用的事件注册，事件触发的api This is used for polling API specific data */
    aeBeforeSleepProc *beforesleep;
    aeBeforeSleepProc *aftersleep;
} aeEventLoop;

/* Prototypes */
// 创建一个大小 setsize的EventLoop
/**
 * 创建事件处理Loop
 * @param setsize  文件事件的最大数量
 * @return
 */
aeEventLoop *aeCreateEventLoop(int setsize);
/**
 * 删除事件处理Loop
 * @param eventLoop
 */
void aeDeleteEventLoop(aeEventLoop *eventLoop);
/**
 * 设置事件处理Loop停止标识
 * @param eventLoop 事件处理Loop
 * @return
 */
void aeStop(aeEventLoop *eventLoop);
//
/**
 * 创建文件事件
 * @param eventLoop 事件处理Loop
 * @param fd 文件描述符
 * @param mask 事件类型
 * @param proc 事件处理函数
 * @param clientData 数据
 * @return
 */
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
/**
 * 删除文件事件
 * @param eventLoop 事件处理Loop
 * @param fd 文件描述符
 * @param mask 事件类型（可读或者可写）
 */
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);

int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
// 创建时间事件，插入列表头部，时间为milliseconds之后
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
//删除时间事件
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
//出来事件
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
int aeWait(int fd, int mask, long long milliseconds);
/**
 *  事件循环处理，直到设置stop标识
 * @param eventLoop 事件处理Loop
 */
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
void aeSetAfterSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *aftersleep);
int aeGetSetSize(aeEventLoop *eventLoop);
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
