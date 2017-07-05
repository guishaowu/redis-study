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

//双向链表节点（元素）
typedef struct listNode {
    struct listNode *prev; // 前一个节点
    struct listNode *next; // 后一个节点
    void *value;           // 节点的值
} listNode;
/**
 * 双向链表迭代器
 */
typedef struct listIter {
    listNode *next; // 下一个元素
    int direction; // 迭代方向，从前往后或者从后往前
} listIter;

/**
 * 双向链表
 */
typedef struct list {
    listNode *head;  // 头节点
    listNode *tail;  //尾节点
    void *(*dup)(void *ptr); // 复制方法
    void (*free)(void *ptr); // 释放方法
    int (*match)(void *ptr, void *key); // 匹配方法
    unsigned long len;            //长度
} list;

/* Functions implemented as macros */
/**
 * 双向链表长度
 */
#define listLength(l) ((l)->len)
/**
 * 双向链表第一个元素
 */
#define listFirst(l) ((l)->head)
/**
 * 双向链表最后一个元素
 */
#define listLast(l) ((l)->tail)
/**
 * 双向链表指定元素的前一个元素
 */
#define listPrevNode(n) ((n)->prev)
/**
 * 双向链表指定元素的后一个元素
 */
#define listNextNode(n) ((n)->next)
/**
 * 双向链表指定元素的值
 */
#define listNodeValue(n) ((n)->value)
/**
 * 设置双向链表的元素复制方法
 */
#define listSetDupMethod(l,m) ((l)->dup = (m))
/**
 * 设置双向链表的元素释放方法
 */
#define listSetFreeMethod(l,m) ((l)->free = (m))
/**
 * 设置双向链表的元素匹配方法
 */
#define listSetMatchMethod(l,m) ((l)->match = (m))
/**
 * 返回双向链表复制元素方法
 */
#define listGetDupMethod(l) ((l)->dup)
/**
 * 返回双向链表释放元素方法
 */
#define listGetFree(l) ((l)->free)
/**
 * 返回双向链表元素匹配方法
 */
#define listGetMatchMethod(l) ((l)->match)

/* Prototypes */
/**
 * 创建一个空的双向链表
 */
list *listCreate(void);
/**
 * 释放双向链表：释放所有元素，释放链表指针
 * @param list 被释放的双向链表
 * @return
 */
void listRelease(list *list);
/**
 * 清空双向链表： 释放所有元素
 * @param list
 * @return
 */
void listEmpty(list *list);
/**
 * 添加一个元素到双向链表头部
 * @param list 双向链表
 * @param value 元素的值
 * @return 双向链表
 */
list *listAddNodeHead(list *list, void *value);
/**
 *  添加一个元素到双向链表的尾部
 * @param list  双向链表
 * @param value  元素的值
 * @return 双向链表
 */
list *listAddNodeTail(list *list, void *value);
/**
 *  插入元素到双向链表中指定元素的前面或者后面
 * @param list  双向链表
 * @param old_node  指定的元素
 * @param value 元素的值
 * @param after 1 后面 0 前面
 * @return 双向链表
 */
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
/**
 *  删除双向链表中的一个指定元素
 * @param list 双向链表
 * @param node 双向链表中的元素
 */
void listDelNode(list *list, listNode *node);
/**
 * 获取双向链表指定方向的迭代器
 * @param list 双向链表
 * @param direction  方向 向前或向后（请看宏定义 AL_START_HEAD AL_START_TAIL）
 * @return 双向链表迭代器
 */
listIter *listGetIterator(list *list, int direction);
/**
 * 返回双向链表迭代器指向元素
 * @param iter 双向链表迭代器
 * @return 双向链表元素
 */
listNode *listNext(listIter *iter);
/**
 * 释放双向链表迭代器
 * @param iter 双向链表迭代器
 */
void listReleaseIterator(listIter *iter);
// 复制链表
/**
 * 复制双向链表
 * @param orig 原双向链表
 * @return 复制后的双向链表
 */
list *listDup(list *orig);
/**
 *  查找双向链表中指定key的元素， 使用match方法，没有match方法，比较指针
 * @param list 双向链表
 * @param key 指定的值
 * @return 找到的链表元素 ，没有则为null
 */
listNode *listSearchKey(list *list, void *key);
// 返回指定索引的元素，负数从尾部开始-1，-2
/**
 *  返回双向链表中指定索引的元素， 索引为负表示从尾部开始
 * @param list 双向链表
 * @param index 索引值
 * @return 链表元素，超出范围则为null
 */
listNode *listIndex(list *list, long index);
/**
 * 创建向后方向的双向链表迭代器
 * @param list 双向链表
 * @param li 双向链表迭代器
 */
void listRewind(list *list, listIter *li);
/**
 * 创建向前方向的双向链表迭代器
 * @param list 双向链表
 * @param li 双向链表迭代器
 */
void listRewindTail(list *list, listIter *li);
/**
 * 将移除双向链表尾部元素插入到头部
 * @param list 双向链表
 * @return
 */
void listRotate(list *list);
/**
 * 将一个双向链表o的元素加入另一个链表l的尾部,清空o的元素
 * @param l 双向链表
 * @param o 双向链表
 */
void listJoin(list *l, list *o);

/* Directions for iterators */
#define AL_START_HEAD 0  // 从头 开始
#define AL_START_TAIL 1  // 从尾 开始

#endif /* __ADLIST_H__ */
