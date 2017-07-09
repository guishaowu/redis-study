/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __INTSET_H
#define __INTSET_H
#include <stdint.h>
/**
 * 有序整型集合
 */
typedef struct intset {
    uint32_t encoding; // 数据类型
    uint32_t length;  // 长度
    int8_t contents[];
} intset;

/**
 * 创建空的整型集合
 * @return
 */
intset *intsetNew(void);
/**
 * 插入一个整数到有序整型集合中 ，使用二分查找，找到值对应的位置,必要情况会升级（所有元素由16位升到32位）
 * @param is 整型集合
 * @param value 插入的值
 * @param success 成功或失败后设置改指针的值
 * @return
 */
intset *intsetAdd(intset *is, int64_t value, uint8_t *success);
/**
 * 移除一个整数， 使用二分查找，找位置
 * @param is
 * @param value
 * @param success
 * @return
 */
intset *intsetRemove(intset *is, int64_t value, int *success);
/**
 * 查找整数在集合中的位置
 * @param is
 * @param value
 * @return
 */
uint8_t intsetFind(intset *is, int64_t value);
/**
 * 返回集合中一个随机元素
 * @param is
 * @return
 */
int64_t intsetRandom(intset *is);
/**
 * 返回集合中指定位置的整数值
 * @param is
 * @param pos
 * @param value
 * @return
 */
uint8_t intsetGet(intset *is, uint32_t pos, int64_t *value);
/**
 * 返回集合的长度
 * @param is
 * @return
 */
uint32_t intsetLen(const intset *is);
/**
 * 返回集合使用的内存大小
 * @param is
 * @return
 */
size_t intsetBlobLen(intset *is);

#ifdef REDIS_TEST
int intsetTest(int argc, char *argv[]);
#endif

#endif // __INTSET_H
