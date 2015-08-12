

#include "core.h"
#include "slot.h"
#include "signal.h"

struct Test
{
    event::Signal<void(int)> on_eval1;
    event::Signal<int(int)>  on_eval2;

    void func1(int x)
    {   on_eval1.emit(x); }

    void func2(int x)
    {
        printf("%s:%s:%i: result = %i\n", __FILE__, __func__, __LINE__, on_eval2(x));
    }
};

struct Observer
{
    void slot1(int x)
    {
        printf("%s:%s:%i:%p\n", __FILE__, __func__, __LINE__, this);
    }

    int slot2(int x)
    {
        printf("%s:%s:%i:%p\n", __FILE__, __func__, __LINE__, this);
        return x;
    }
};


int main(int args, char** argv)
{
    const int max_count = 10;
    Test notif;
    Observer *obs = new Observer[max_count];
    int  ids[max_count];
    int i;

    for (i = 0; i < max_count; ++i)
    {
        ids[i] = notif.on_eval1.connect(obs + i, &Observer::slot1);
        notif.on_eval2.connect(obs + i, &Observer::slot2);
    }

    notif.func1(1);
    notif.func2(2);

    for (i = 0; i < max_count; ++i)
    {
        notif.on_eval1.disconnect(ids[i]);
        notif.on_eval2.disconnect(obs + i);
    }
        
    delete[] obs;
    return 0;
}
