/*
int main() {

    int p, l, k = 4, zf = 174;
    int p42[4] = {1, 2, 3, 4};
    int zxg[];
    int kb[] = {1, 2};
    int x = "Str";
    x = x + 5;

    struct Employee {
        string name;
        UI32 salary;
        UI8 weekHours;
    };

    Employee emp;

    Employee.b;
}
*/

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

// FAIL