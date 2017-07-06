/* Hash Tables Implementation.
 *
 * This file implements in-memory hash tables with insert/del/replace/find/
 * get-random-element operations. Hash tables will auto-resize if needed
 * tables of power of two in size are used, collisions are handled by
 * chaining. See the source code for more information... :)
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

#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)

// key-value  next解决hash值冲突
/**
 * 键值对
 */
typedef struct dictEntry {
    void *key;
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next;
} dictEntry;
// key hash,复制,比较,释放方法  value 复制，释放方法
/**
 * 键值对类型， 键的hash方法，复制，比较，释放方法
 * 值的复制，释放方法
 */
typedef struct dictType {
    uint64_t (*hashFunction)(const void *key);
    void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, void *key);
    void (*valDestructor)(void *privdata, void *obj);
} dictType;

/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
/**
 * 字典表 HashTable
 */
typedef struct dictht {
    dictEntry **table;  // 键值对数组
    unsigned long size; // 大小
    unsigned long sizemask; // size -1
    unsigned long used; // 已使用
} dictht;

/**
 * 字典，包含两个字典表，方便做rehash, 由于字典表大小的限制
 */
typedef struct dict {
    dictType *type;
    void *privdata;
    dictht ht[2];
    long rehashidx; /* rehashing not in progress if rehashidx == -1 */
    unsigned long iterators; /* number of iterators currently running */
} dict;

/* If safe is set to 1 this is a safe iterator, that means, you can call
 * dictAdd, dictFind, and other functions against the dictionary even while
 * iterating. Otherwise it is a non safe iterator, and only dictNext()
 * should be called while iterating. */
/**
 * 字典迭代器
 */
typedef struct dictIterator {
    dict *d;
    long index;
    int table, safe;
    dictEntry *entry, *nextEntry;
    /* unsafe iterator fingerprint for misuse detection. */
    long long fingerprint;
} dictIterator;
/**
 *
 */
typedef void (dictScanFunction)(void *privdata, const dictEntry *de);
typedef void (dictScanBucketFunction)(void *privdata, dictEntry **bucketref);

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
/**
 * 释放键值对的值
 */
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)
/**
 * 设置键值对的值
 */
#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        (entry)->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        (entry)->v.val = (_val_); \
} while(0)

/**
 * 设置键值对有符号整型值
 */
#define dictSetSignedIntegerVal(entry, _val_) \
    do { (entry)->v.s64 = _val_; } while(0)

/**
 * 设置键值对的无符号整型值
 */
#define dictSetUnsignedIntegerVal(entry, _val_) \
    do { (entry)->v.u64 = _val_; } while(0)
/**
 * 设置键值对的double值
 */
#define dictSetDoubleVal(entry, _val_) \
    do { (entry)->v.d = _val_; } while(0)

/**
 * 释放键值对的键
 */
#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

/**
 * 设置键值对的键
 */
#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        (entry)->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        (entry)->key = (_key_); \
} while(0)

/**
 * 比较两个key
 */
#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))
/**
 * key 在字典中的hash值
 */
#define dictHashKey(d, key) (d)->type->hashFunction(key)
/**
 * 获取键值对的键值
 */
#define dictGetKey(he) ((he)->key)
/**
 * 获取键值对的值
 */
#define dictGetVal(he) ((he)->v.val)
/**
 * 获取键值对的有符号整型值
 */
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
/**
 * 获取键值对无符号整型值
 */
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
/**
 *  获取键值对的double值
 */
#define dictGetDoubleVal(he) ((he)->v.d)
/**
 * 字典的大小
 */
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
/**
 * 字典已使用的大小
 */
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
/**
 * 字典是否在rehash中
 */
#define dictIsRehashing(d) ((d)->rehashidx != -1)

/* API */
/**
 * 创建字典，初始化
 */
dict *dictCreate(dictType *type, void *privDataPtr);
//创建字典的hashTable
int dictExpand(dict *d, unsigned long size);
/**
*/
/**
 *  * 添加一个键值对到字典
调用dictAddRaw，如果键已经存在，返回error
 * @param d 字典
 * @param key  键
 * @param val  值
 * @return
 */
int dictAdd(dict *d, void *key, void *val);
/**
 * 添加一个键到字典，返回对应的键值对（没有设置值）
如果正在rehash, 执行一次移动元素操作
如果键已经存在，返回NULL
 * @param d  字典
 * @param key  键
 * @param existing 保存key存在时的键值对
 * @return
 */
dictEntry *dictAddRaw(dict *d, void *key, dictEntry **existing);
/**
 * 返回对应键的键值对，如果没有，就讲键添加到字典
 * @param d  字典
 * @param key 键
 * @return
 */
dictEntry *dictAddOrFind(dict *d, void *key);
/**
 * 键不存在，添加键值对，键存在，更新值
 * @param d 字典
 * @param key  键
 * @param val 值
 * @return
 */
int dictReplace(dict *d, void *key, void *val);
/**
 * 删除元素，如果rehash,移动一次
* @param d 字典
 * @param key 键
 * @return 成功或者失败
 */
int dictDelete(dict *d, const void *key);
/**
 * 删除元素，不释放，如果key不存在，返回Null
 * @param ht 字典
 * @param key 键
 * @return 删除的键值对
 */
dictEntry *dictUnlink(dict *ht, const void *key);
/**
 * 释放上一函数的删除的元素
 * @param d 字典
 * @param he 兼职对
 * @return
 */
void dictFreeUnlinkedEntry(dict *d, dictEntry *he);
/**
 * 释放字典
 * @param d 字典
 * @return
 */
void dictRelease(dict *d);
/**
 *  字典中查找key，对应的键值对，没有返回null,如果rehash，移动一次
 * @param d 字典
 * @param key  键
 * @return 查找结果
 */
dictEntry * dictFind(dict *d, const void *key);
/**
 * 返回字典中对应键的值
 * @param d 字典
 * @param key  键
 * @return
 */
void *dictFetchValue(dict *d, const void *key);
/**
 * 回收空间，根据已经使用的，使用最小的size
 * @param d
 * @return
 */
int dictResize(dict *d);
//迭代器
dictIterator *dictGetIterator(dict *d);
dictIterator *dictGetSafeIterator(dict *d);
/**
 * 迭代器 迭代元素
 * @param iter
 * @return
 */
dictEntry *dictNext(dictIterator *iter);
/**
 * 释放迭代器
 * @param iter
 */
void dictReleaseIterator(dictIterator *iter);
/**
 * 获取一个随机元素
 * @param d
 * @return
 */
dictEntry *dictGetRandomKey(dict *d);
/**
 * 获取count个随机元素，不重复，不保证一定有count个结果
 * @param d
 * @param des
 * @param count
 * @return
 */
unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count);
/**
 * 返回字典的统计信息，元素数量等信息
 * @param buf 存储统计信息
 * @param bufsize
 * @param d
 */
void dictGetStats(char *buf, size_t bufsize, dict *d);
/**
 * 生成hash 值
 * @param key
 * @param len
 * @return
 */
uint64_t dictGenHashFunction(const void *key, int len);
uint64_t dictGenCaseHashFunction(const unsigned char *buf, int len);
/**
 * 清空字典
 * @param d
 * @param callback 当hashtable的索引与65535为0时调用callback参数d.privatedata
 */
void dictEmpty(dict *d, void(callback)(void*));
/**
 * 打开resize标识
 */
void dictEnableResize(void);
/**
 * 关闭resize标识
 */
void dictDisableResize(void);
/**
 * 执行n次rehash的移动元素操作
 * @param d
 * @param n
 * @return
 */
int dictRehash(dict *d, int n);
/**
 * 执行ms毫秒rehash的移动元素操作
 * @param d
 * @param ms
 * @return
 */
int dictRehashMilliseconds(dict *d, int ms);
/**
 * 设置hash 函数的种子
 * @param seed
 */
void dictSetHashFunctionSeed(uint8_t *seed);
/**
 * 获取hash函数的种子
 * @return
 */
uint8_t *dictGetHashFunctionSeed(void);
/**
 * scan字典
 * @param d
 * @param v
 * @param fn
 * @param bucketfn
 * @param privdata
 * @return
 */
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, dictScanBucketFunction *bucketfn, void *privdata);
/**
 * key对应的hash值
 * @param d 字典
 * @param key 键
 * @return
 */
unsigned int dictGetHash(dict *d, const void *key);
/**
 * 根据key指针和hash值，找到对应的键值对
 * @param d
 * @param oldptr key指针
 * @param hash 
 * @return
 */
dictEntry **dictFindEntryRefByPtrAndHash(dict *d, const void *oldptr, unsigned int hash);

/* Hash table types */
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
