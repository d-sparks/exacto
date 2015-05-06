#include <algorithm>
#include <iostream>
#include <vector>
#include "assert.h"
#include "../src/inlines.h"
#include "../src/magics.cpp"

using namespace std;

// Tests generateSubsets with a basic example.
int testGenerateSubsets() {
    cout << "Testing generateSubsets" << endl;

    BB b = exp_2(5) | exp_2(45) | exp_2(63);
    vector<BB> subsets;
    vector<BB> expectedSubsets = {
        0,
        exp_2(5),
        exp_2(45),
        exp_2(63),
        exp_2(5) | exp_2(45),
        exp_2(5) | exp_2(63),
        exp_2(45) | exp_2(63),
        b,
    };
    magics::generateSubsets(b, &subsets);
    sort(begin(subsets), end(subsets));
    sort(begin(expectedSubsets), end(expectedSubsets));
    ASSERT(subsets == expectedSubsets, "Wrong subsets");

    return 1;
}

int main() {
    int t = 0;

    t += testGenerateSubsets();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}

