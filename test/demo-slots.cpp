

#include <cstdio>
#include "slot.h"

struct Base 
{
    bool base_method(int a, int b) { return a > b; }
};

struct Test : virtual public Base
{
    bool compare_method(int a, int b) { return a > b; }
    static bool static_compare(int a, int b);
    
    bool nonconst_method(int a, int b)
    {
        printf("%s:%s:%i\n", __FILE__, __func__, __LINE__);
        return a > b;
    }

    bool const_method(int a, int b) const
    {
        printf("%s:%s:%i\n", __FILE__, __func__, __LINE__);
        return a > b;
    }

    void no_return_method(int a, int b)
    {
        printf("%s:%s:%i\n", __FILE__, __func__, __LINE__);
    }

    void noreturn_noarguments_method()
    {
        printf("%s:%s:%i\n", __FILE__, __func__, __LINE__);
    }

    static bool static_method(int a, int b)
    {
        printf("%s:%s:%i\n", __FILE__, __func__, __LINE__);
        return a > b;
    }
};

bool static_function0(int a, int b)
{
    printf("%s:%s:%i\n", __FILE__, __func__, __LINE__);
    return a > b;
}

bool static_function1(int a, int b)
{
    return a > b;
}

void make_test0(Test* ts)
{
    auto base_method = event::Slot<bool(int,int)>(ts, &Test::base_method);
    auto nonconst_method = event::Slot<bool(int,int)>(ts, &Test::nonconst_method);
    auto const_method = event::Slot<bool(int,int)>(ts, &Test::const_method);
    auto noreturn = event::Slot<void(int,int)>(ts, &Test::no_return_method);
    auto noreturn_noargs = event::Slot<void()>(ts, &Test::noreturn_noarguments_method);

    base_method(1, 2);
    nonconst_method(2, 3);
    const_method(3, 4);
    noreturn(4, 5);
    noreturn_noargs();
}

int main(int argc, char** argv)
{
    Test stack_test_obj;
    Test* heap_test_obj = new Test;

    printf("Demo for stacked object:\n");
    make_test0(&stack_test_obj);

    printf("Demo for counted object:\n");
    make_test0(heap_test_obj);

    auto static_method = event::Slot<bool(int,int)>(&Test::static_method);
    auto slot_static_func = event::Slot<bool(int,int)>(static_function0);

    printf("Demo for static method and function:\n");
    static_method(1, 3);
    slot_static_func(3, 5);
    
    printf("Start benchmark:\n");
    const int max_count = 40000000;
    int i, start, stop;
    int *original, *experiment;

    original = new int[max_count];
    experiment = new int[max_count];

    ::srand(::clock());
    for (i = 0; i < max_count; ++i)
        original[i] = ::rand();

    auto slot1 = event::Slot<bool(int,int)>(&stack_test_obj, &Test::compare_method);
    std::copy(original, original + max_count, experiment);
    start = ::clock();
    std::sort(experiment, experiment + max_count, slot1);
    stop = ::clock();
    printf("slot + method : %f\n", (stop - start) / static_cast<double>(CLOCKS_PER_SEC));

    auto slot2 = event::Slot<bool(int,int)>(static_function1);
    std::copy(original, original + max_count, experiment);
    start = ::clock();
    std::sort(experiment, experiment + max_count, slot2);
    stop = ::clock();
    printf("slot + static : %f\n", (stop - start) / static_cast<double>(CLOCKS_PER_SEC));

    std::copy(original, original + max_count, experiment);
    start = ::clock();
    std::sort(experiment, experiment + max_count, static_function1);
    stop = ::clock();
    printf("static func   : %f\n", (stop - start) / static_cast<double>(CLOCKS_PER_SEC));

    delete[] original;
    delete[] experiment;
    delete heap_test_obj;

    return 0;
}
