/*
 * event.h
 *
 *  Created on: 2 авг. 2015 г.
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

#ifndef _EVENT_SIGNAL_H_
#define _EVENT_SIGNAL_H_

#include <type_traits>
#include <utility>
#include <vector>

#include "slot.h"

namespace event {

template<typename R, typename ...Args> struct Signal;


template<typename R, typename ...Args>
struct Signal<R(Args...)>
{
private:
    struct Place
    {
        int              stamp;
        Slot<R(Args...)> slot;
        Place(int st, Slot<R(Args...)>& sl) : stamp(st), slot(std::move(sl)) { }
    };

    std::vector<Place> m_slots;

public:
    Signal();
    ~Signal();
    Signal(Signal&& src);
    Signal(Signal const& src);
    Signal& operator=(Signal&& src);
    Signal& operator=(Signal const& src);

    Signal& swap(Signal& src);
    Signal& assign(Signal const& src);

    template<typename T, typename Member>
    int connect(T*, Member);
    int connect(Slot<R(Args...)> slot); // --> id or timestamp

    void disconnect(int id);
    void disconnect(void const* pthis);
    void disconnect(Slot<R(Args...)> const& slot);

    R operator()(Args... args);
    R emit(Args... args);
    R async_emit(Args... args);

};
} /* namespace event */

#include "signal.hpp"

#endif /* _EVENT_SIGNAL_H_ */
