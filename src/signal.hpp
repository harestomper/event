/*
 * signal.hpp
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

#ifndef _EVENT_SIGNAL_HPP_
#define _EVENT_SIGNAL_HPP_

#include "signal.h"

#include <ctime>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define signal_templ template<typename R, typename ...Args>
#define _Self event::Signal<R(Args...)>

signal_templ
inline _Self::Signal()
{
    this->m_slots.reserve(10);
}
//------------------------------------------------------------------------------
signal_templ
inline _Self::~Signal()
{
    this->m_slots.clear();
}
//------------------------------------------------------------------------------
signal_templ
inline _Self::Signal(_Self&& src)
{
    this->swap(src);
}
//------------------------------------------------------------------------------
signal_templ
inline _Self::Signal(_Self const& src)
{
    this->assign(src);
}
//------------------------------------------------------------------------------
signal_templ
inline _Self& _Self::operator=(_Self&& src)
{
    return this->swap(src);
}
//------------------------------------------------------------------------------
signal_templ
inline _Self& _Self::operator=(_Self const& src)
{
    return this->assign(src);
}
//------------------------------------------------------------------------------
signal_templ
inline _Self& _Self::swap(_Self& src)
{
    std::swap(this->m_slots, src.m_slots);
    return *this;
}
//------------------------------------------------------------------------------
signal_templ
inline _Self& _Self::assign(_Self const& src)
{
    this->m_slots.clear();
    this->m_slots.assign(src.m_slots);
    return *this;
}
//------------------------------------------------------------------------------
signal_templ
inline int _Self::connect(Slot<R(Args...)> slot)
{
    auto it = this->m_slots.begin();
    auto end = this->m_slots.end();

    while (it < end)
    {
        if (it->slot == slot)
            return it->stamp;
        ++it;
    }

    it = this->m_slots.emplace(end, ::clock(), slot);

    return it->stamp;
}
//------------------------------------------------------------------------------
signal_templ
template<typename T, typename Member>
inline int _Self::connect(T* pth, Member mem)
{
    return this->connect(event::Slot<R(Args...)>(pth, mem));
};
//------------------------------------------------------------------------------
signal_templ
inline void _Self::disconnect(int id)
{
    auto it = this->m_slots.begin();
    auto end = this->m_slots.end();

    while (it < end)
    {
        if (it->stamp == id)
        {
            it = this->m_slots.erase(it);
            break;
        }

        ++it;
    }
}
//------------------------------------------------------------------------------
signal_templ
inline void _Self::disconnect(void const* pthis)
{
    auto it = this->m_slots.begin();
    auto end = this->m_slots.end();

    while (it < end)
    {
        if (*(it->slot) == pthis)
            it = this->m_slots.erase(it);
        else
            ++it;
    }
}
//------------------------------------------------------------------------------
signal_templ
inline void _Self::disconnect(Slot<R(Args...)> const& slot)
{
    auto it = this->m_slots.begin();
    auto end = this->m_slots.end();

    while (it < end)
    {
        if (it->slot == slot)
            it = this->m_slots.erase(it);
        else
            ++it;
    }
}
//------------------------------------------------------------------------------
signal_templ
inline R _Self::operator()(Args... args)
{
    return this->emit(args...);
}
//------------------------------------------------------------------------------
signal_templ
inline R _Self::emit(Args... args)
{
    auto it = this->m_slots.begin();
    auto end = this->m_slots.end();

    while (it < (end - 1))
    {
        it->slot(args...);
        ++it;
    }

    if (it < end)
        return it->slot(args...);

    return R();
}
//------------------------------------------------------------------------------
signal_templ
inline R _Self::async_emit(Args... args)
{
    /*TODO: make async calls */
    return this->emit(args...);
}

#undef signal_templ
#undef _Self
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif /* _EVENT_SIGNAL_HPP_ */
