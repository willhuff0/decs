#include <System.hpp>

struct TestComp1 {
    int a, b;
};

struct TestComp2 {
    std::vector<float> c;
};

int main() {
    System<TestComp1, TestComp2> system(Query({}, {}), [](const TestComp1& testComp1, const TestComp2& testComp2) {

    });
}