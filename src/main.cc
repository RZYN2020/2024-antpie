#include <iostream>
#include "main.hh"
#include "opt/example.hh"


int main() {
    Foo f = Foo { fun() };
    std::cout << f.a << std::endl;
}