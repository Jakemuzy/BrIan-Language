struct Inner {
    int a;
};

struct Outer {
    Inner i;
};

int main() {
    Outer o;
    o.i.a = 5;
    return o.i.a;
}
//PASS