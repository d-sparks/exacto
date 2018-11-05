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

* Data members in classes do not end with an underscore, but are still snake case.

Unlike 0.e and before, classes are no longer named `CGame`, `CBoard`, etc, but rather simply `Game` and `Board`, etc.

