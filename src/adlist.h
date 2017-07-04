/* adlist.h - A generic doubly linked list implementation
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

#ifndef __ADLIST_H__
#define __ADLIST_H__

/* Node, List, and Iterator are the only data structures used currently. */

//链表节点
typedef struct listNode {
    struct listNode *prev; // 前一个节点
    struct listNode *next; // 后一个节点
    void *value;           // 节点的值
} listNode;

typedef struct listIter {
    listNode *next; // 下一个元素
    int direction; // 迭代方向，从前往后或者从后往前
} listIter;

//链表
typedef struct list {
    listNode *head;  // 头节点
    listNode *tail;  //尾节点
    void *(*dup)(void *ptr); // 复制方法
    void (*free)(void *ptr); // 释放方法
    int (*match)(void *ptr, void *key); // 比较方法
    unsigned long len;            //长度
} list;

/* Functions implemented as macros */
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l,m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

/* Prototypes */
// 创建一个空的list
list *listCreate(void);
// 释放list
void listRelease(list *list);
// 清空list
void listEmpty(list *list);
// 添加一个元素到头节点
list *listAddNodeHead(list *list, void *value);
// 添加一个元素到尾部
list *listAddNodeTail(list *list, void *value);
// 插入一个元素到指定节点前面或者后面
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
// 删除元素
void listDelNode(list *list, listNode *node);
// 返回迭代器
listIter *listGetIterator(list *list, int direction);
// 返回迭代元素
listNode *listNext(listIter *iter);
// 释放迭代器
void listReleaseIterator(listIter *iter);
// 复制链表
list *listDup(list *orig);
// 查找元素
listNode *listSearchKey(list *list, void *key);
// 返回指定索引的元素，负数从尾部开始-1，-2
listNode *listIndex(list *list, long index);
// 创建head方向迭代器
void listRewind(list *list, listIter *li);
// 创建tail方向迭代器
void listRewindTail(list *list, listIter *li);
// 将移除尾部元素插入到头部
void listRotate(list *list);
//将o对应的链表元素加入到l对应的尾部，结束后，o链表为空
void listJoin(list *l, list *o);

/* Directions for iterators */
#define AL_START_HEAD 0  // 从头 开始
#define AL_START_TAIL 1  // 从尾 开始

#endif /* __ADLIST_H__ */
