/*
 * slot.h
 *
 *  Created on: 1 авг. 2015 г.
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

#ifndef _EVENT_SLOT_H_
#define _EVENT_SLOT_H_

#include <algorithm>

#include "core.h"

namespace event {
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<typename R, typename ...Args> struct Slot;

template<typename R, typename ...Args>
struct Slot<R(Args...)>
{
private:
    struct Dummy;
    using MemType = R(Dummy::*)(Args...);
    using CMemType = R(Dummy::*)(Args...) const;
    using FnType = R(*)(Args...);
    using CallType = union { MemType mem; CMemType cmem; FnType func; void* ptr; };

    void*      m_this = null;
    Dummy*     m_dummy = null;
    CallType   m_call = {0,};
    bool       m_is_member = false;

public:
    Slot() = delete;

    ~Slot();
    Slot(Slot&& src);
    Slot(Slot const& src);
    Slot& operator=(Slot&& src);
    Slot& operator=(Slot const& src);

    template<typename T, typename Base>
    Slot(T* receiver, R(Base::*member)(Args...) const);

    template<typename T, typename Base>
    Slot(T* receiver, R(Base::*member)(Args...));

    Slot(R(*function)(Args...));

    Slot& assign(Slot const& src);
    Slot& swap(Slot& src);
    void clear();

    R invoke(Args... args);
    R operator()(Args... args);

    void const* operator*() const;
    bool equal(Slot const& src) const;
    bool empty() const;

    bool operator ==(Slot const& src) const;
    bool operator !=(Slot const& src) const;
    bool operator !() const;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
} /* namespace event */

#include "slot.hpp"

#endif /* _EVENT_SLOT_H_ */
