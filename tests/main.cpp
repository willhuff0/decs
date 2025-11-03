#include <decs/Decs.hpp>

#include <iostream>
#include "moodycamel/MoodycamelQueue.hpp"

struct CompA {
    std::vector<float> test;
};

struct CompB {
    int a;
    int b;
    float c;
    CompA nestedType;

    CompB(int _b, CompA nested) {
        a = 2;
        b = _b;
        c = 2.5f;
        nestedType = std::move(nested);
    }
};

int main() {
    auto decs = Decs::Create(std::make_unique<MoodycamelQueue<std::function<void()>>>());

    for (int i = 0; i < 1000; ++i) {
        decs->CreateEntity()
                .AddComponent<CompA>()
                .AddComponent<CompB>(i, CompA{})
                .Build();
    }

    decs->ExecuteDeferredFunctions();

    decs->RegisterSystem<CompA, CompB>([](const Mut<CompA>& a, const Mut<CompB>& b) {
        a.SetValue([](CompA& mutA) {
            mutA.test.push_back(4);
        });
    });

    decs->RegisterSystem<CompB>([](ComponentIndex i, const CompB& b) {
        std::cout << i << " " << b.a << std::endl;
    });

    decs->RegisterSystem<CompB>([](const DeferredMutator<CompB>& b) {
        b.SetValue([](CompB& mutB) {
            mutB.a++;
        });
    });
    decs->RegisterSystem<CompB>([](const DeferredMutator<CompB>& b) {
        b.SetValue([](CompB& mutB) {
            mutB.a--;
        });
    });

    for (int i = 0; i < 10000; ++i) {
        decs->IterateSystems();
    }

//    auto compB = decs->GetComponent<CompB>(entity);
//    std::cout << compB.a << std::endl;

    return 0;
}