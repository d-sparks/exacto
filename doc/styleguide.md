## Exacto Styleguide

Versions up to and including 0.e had no explicit styleguide.

The styleguide for Exacto is the Google C++ styleguide with a few changes:

* If a function signature (definition or declaration) does not fit on one line, always put one argument per line like so:

```
void myFunc(type1 arg1,
            type2 arg2,
            type3 arg3);
```

* This does not apply to function _calls_, which still try to minimize the number of lines used for the call, e.g.

```
void myFunc(type1 arg1, type2 arg2,
            type3 arg3);
```

* Global constants defined with #define are all uppercase.

* Global array variables are all uppercase.

* Data members in classes do not end with an underscore, but are still snake case. Similarly methods that can be used logically as a data member (and take no arguments) can be snake case. It's not great, but far too much of Exacto's logic uses the bitboards of the Board class and it would be unwieldy for these to end with underscores. Setter and constructor arguments that would shadow a data member's name will start with an underscore.

* Test files live in a separate directory and may use namespace std. I am willing to revist the decision of tests using std.

* Some classes or namespaces that are large are separated into multiple cpp files when it is logical. In that case the name of the cpp file would be `class_function.cpp`. For example `exacto_evaluate.cpp`, `exacto.cpp`, and `exacto_search.cpp`.

Unlike 0.e and before, classes are no longer named `CGame`, `CBoard`, etc, but rather simply `Game` and `Board`, etc.

