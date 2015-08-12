/*
 * counted.h
 *
 *  Created on: 14 июня 2015 г.
 *      Author: Voldemar Khramtsov <harestomper@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _COUNTED_H_
#define _COUNTED_H_

#include <atomic>   // std::atomic_int
#include <cstdlib>  // ::malloc and ::free
#include <memory>   // std::_Destroy and std::_Construct
#include <utility>  // std::forward

#include "core.h"

namespace memory {

const int MARKER = static_cast<int>(0xaaaaaa);

struct Head
{
    int marker;
    std::atomic_int count;
};

struct counted
{
    template<typename vT, typename ...Args>
    static vT* alloc(Args&&... args) throw (std::bad_alloc)
    {
        union { Head* ad; char* ch; vT* out; void* ptr; } u = {0,};

        u.ptr = ::malloc(sizeof(vT) + sizeof(Head));

        if (unlikely(u.ptr == null))
            throw std::bad_alloc();

        u.ad->count = 1;
        u.ad->marker = MARKER;
        u.ch += sizeof(Head);
        std::_Construct(u.ptr, std::forward<Args>(args)...);

        return u.out;
    }

    template<typename vT>
    static void free(vT* ptr)
    {
        union { Head* ad; char* ch; vT* in; void* ptr; } u = {0,};
        u.in = ptr;
        u.ch -= sizeof(Head);

        if (ptr && u.ad->marker == MARKER)
        {
            u.ad->marker = 0;
            u.ad->count = 0;
            std::_Destroy(ptr);
            ::free(u.ptr);
        }
    }

    static int ref(void* ptr)
    {
        union { void* ptr; char* ch; Head* ad; } u = {0,};
        u.ptr = ptr;
        u.ch -= sizeof(Head);
        return ((ptr && u.ad->marker == MARKER) ? ++u.ad->count : -1);
    }

    template<typename vT>
    static int unref(vT* ptr)
    {
        union { vT* in; Head* ad; char* ch; } u = {0,};
        u.in = ptr;
        u.ch -= sizeof(Head);

        if (ptr && u.ad->marker == MARKER && u.ad->count > 0)
        {
            if (--u.ad->count == 0)
                counted::free(ptr);

            return u.ad->count;
        }

        return -1;
    }

    static bool is_counted(void const* mem)
    {
        union { void const* ptr; Head const* head; char const* ch; } u = {0,};
        u.ptr = mem;
        u.ch -= sizeof(Head);
        return (mem && u.head->marker == MARKER && u.head->count > 0);
    }

    static int ref_count(void const* mem)
    {
        union { void const* ptr; Head const* head; char const* ch; } u = {0,};
        u.ptr = mem;
        u.ch -= sizeof(Head);
        return ((mem && u.head->marker == MARKER) ? u.head->count : -1);
    }
};


}  // namespace memory

#endif /* _COUNTED_H_ */
