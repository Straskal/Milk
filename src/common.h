/*
 *  MIT License
 *
 *  Copyright(c) 2018 - 2020 Stephen Traskal
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef int16_t     i16;
typedef int32_t     i32;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;

#define UNUSED(v)               ((void)v)
#define MIN(x, y)               ((x) > (y) ? (y) : (x))
#define MAX(x, y)               ((x) > (y) ? (x) : (y))
#define CLAMP(v, low, up)       (MAX(low, MIN(v, up)))
#define SIGN(x)                 ((x > 0) - (x < 0))
#define IS_BIT_SET(val, bit)    (val & bit)

#endif
