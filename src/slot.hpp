/*
 * slot.hpp
 *
 *  Created on: 12 авг. 2015 г.
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

#ifndef _EVENT_SLOT_HPP_
#define _EVENT_SLOT_HPP_

#include "slot.h"
#include "counted.h"

#define slot_templ template<typename R, typename ...Args>
#define _Self event::Slot<R(Args...)>

slot_templ
inline _Self::~Slot()
{
    this->clear();
}

slot_templ
inline _Self::Slot(_Self&& src)
{
    this->swap(src);
}

slot_templ
inline _Self::Slot(_Self const& src)
{
    this->assign(src);
}

slot_templ
inline _Self& _Self::operator=(_Self&& src)
{
    return this->swap(src);
}

slot_templ
inline _Self& _Self::operator=(_Self const& src)
{
    return this->assign(src);
}


slot_templ
template<typename T>
inline _Self::Slot(T* receiver, R(T::*member)(Args...) const)
{
    this->clear();
    using InType = R(T::*)(Args...) const;
    union { InType input; struct { CMemType result; int offset; } s; } u = {0,};
    u.input = member;
    this->m_call.cmem = u.s.result;

    union { T* input; char* inter; Dummy* result; } u1 = {0,};
    u1.input = receiver;
    u1.inter += u.s.offset;

    this->m_dummy = u1.result;
    this->m_this = receiver;

    memory::counted::ref(this->m_this);
    this->m_is_member = true;
}

slot_templ
template<typename T>
inline _Self::Slot(T* receiver, R(T::*member)(Args...))
{
    this->clear();
    using InType = R(T::*)(Args...);
    union { InType input; struct { MemType result; int offset; } s; } u = {0,};
    u.input = member;
    this->m_call.mem = u.s.result;

    union { T* input; char* inter; Dummy* result; } u1 = {0,};
    u1.input = receiver;
    u1.inter += u.s.offset;

    this->m_dummy = u1.result;
    this->m_this = receiver;

    memory::counted::ref(this->m_this);
    this->m_is_member = true;
}

slot_templ
inline _Self::Slot(R(*function)(Args...))
{
    this->m_call.func = function;
    this->m_is_member = false;
}

slot_templ
inline _Self& _Self::assign(_Self const& src)
{
    if (__builtin_expect(!!(this != &src), 1))
    {
        if (this->m_is_member)
            memory::counted::unref(this->m_this);

        this->m_call.ptr = src.m_call.ptr;
        this->m_dummy = src.m_dummy;
        this->m_this = src.m_this;
        this->m_is_member = src.m_is_member;

        if (this->m_is_member)
            memory::counted::ref(this->m_this);
    }

    return *this;
}

slot_templ
inline _Self& _Self::swap(_Self& src)
{
    if (__builtin_expect(!!(this != &src), 1))
    {
        std::swap(this->m_call.ptr, src.m_call.ptr);
        std::swap(this->m_dummy, src.m_dummy);
        std::swap(this->m_this, src.m_this);
        std::swap(this->m_is_member, src.m_is_member);
    }

    return *this;
}

slot_templ
inline void _Self::clear()
{
    if (this->m_is_member)
        memory::counted::unref(this->m_this);

    this->m_this = null;
    this->m_dummy = null;
    this->m_call.ptr = null;
    this->m_is_member = false;
}

slot_templ
inline R _Self::invoke(Args... args)
{
    return this->operator()(args...);
}

slot_templ
inline R _Self::operator()(Args... args)
{
    return (this->m_is_member)
            ? (this->m_dummy->*this->m_call.cmem)(args...)
            : (*this->m_call.func)(args...);
}

slot_templ
inline void const* _Self::operator*() const
{
    return this->m_this;
}

slot_templ
inline bool _Self::equal(_Self const& src) const
{
    return (&src == this
            || (this->m_is_member
             ? (src.m_this == this->m_this && src.m_call.ptr == this->m_call.ptr)
             : (src.m_call.ptr == this->m_call.ptr)));
}

slot_templ
inline bool _Self::empty() const
{
    return (this->m_is_member
            ? (this->m_this == null || this->m_dummy == null || this->m_call.ptr == null)
            : (this->m_call.ptr == null));
}

slot_templ
inline bool _Self::operator ==(_Self const& src) const
{
    return this->equal(src);
}

slot_templ
inline bool _Self::operator !=(_Self const& src) const
{
    return (not this->equal(src));
}

slot_templ
inline bool _Self::operator !() const
{
    return (not this->empty());
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#undef slot_templ
#undef _Self


#endif /* _EVENT_SLOT_HPP_ */
