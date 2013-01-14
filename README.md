checked_cast
============

C++ checked_cast implementation for a faster safe dynamic_cast

Synopsis
============
dynamic_cast in C++ requires RTTI lookups and is therefore very slow. It ensures that the cast is correct and this takes time. Time that not necessarily needs to be spend. If you know that the cast is OK, there is no need to do all the RTTI lookups and a static_cast will work just fine. But during debug it might be usefull to use dynamic_cast to ensure that the code is correct. Hence: checked_cast.

Use
============
checked_cast is basically just a dynamic_cast which turns to a static_cast in debug situations.

Sample
============
```C++
Derived* p=checked_cast<Derived*>(pBase);
```
