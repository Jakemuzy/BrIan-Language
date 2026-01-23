#include <>

int main() {
    // Etc
    ;

    // Decl Stmts
    char a = 'a';
    char b, c = 'z', d;
    a = 1;
    d = 3 - c + 12 * 3 + (1 + 2);

    int x = 4;
    x = 4 - 4;
    x = a;
    x += 31;
    x -= 1;
    x *= 2;
    x /= 3;
    x %= 4;
    x &= 5;
    x |= 6;
    x &&= 7;
    x ||= 8;
    x ~= 9;
    x ^= 10;
    x >>= 11;
    x <<= 12;
    x++;
    x--;

    float f = 4.25;
    float g = 81.321;
    float k = 0.471;
    g = f + k;
    g = g - k;

    double d = 4.1656183;
    double c = 6471819264.162624812;
    d -= c;
    c += 647193.17264;
    d = c + d;

    string t1 = "test";
    string t2 = "longest string known to man";
    string t3 = "tab    ";
    string t4 = "space   ";
    string t5 = "line 
                ";

    IfStmts(1.23, a);
    SwitchStmts('a', "badha");
    WhileStmts();
    DoWhileStmts();
    ForStmts();
    ArrIndexing(1, 3);

    return 0;
}

void IfStmts()
{

    if (1) {
    }
    if(x < y) { }
    if(x > y) {     }
    if(x <= y) { ; }
    if  (y >= x) {}
    if ( t1 != t2 ) { } 
    if(!5) {}
    if(4 && 5) { }
    if(5 || x) { }
    if (x == y)
    {
    }

    if (2) { } elif (1) {}
    if (x < y)
    {
    } elif (y > x) {
        int f = 10 + 10;
    }
    if (9+10) {
        9 + 10;
    }
    else {
    10 + 9;
    }
    if (10 < 11) {} elif ( 11 > 10 ) {} else {
        int x = y;
    } 
}

void SwitchStmts()
{
    switch(x) {
    }

    switch (x) {
        case 1 {

        }
    }

    switch(x + y) {
        case 1 {
            x = 10;
        }
    }

    switch (5) {
        case 1 {
            ; 
        }
        case 2 {
            x = y;
        }
    }

    switch(x) {
        default {

        }
    }

    switch(x) {
        case 1 {
            x = 1;
        }
        case 2 {
            y = 2;
        }
        default {
            z = 3;
        }
    }

    switch(x) {
        case (1 + 2) {
            int a = 10;
            a = a + 1;
        }
    }

    switch(x) {
        case 1 {
            switch(y) {
                case 2 {
                    z = 3;
                }
            }
        }
    }
}

void WhileStmts()
{
    while(x) {}

    while (x < y) {
    }

    while(1) {
        x = x + 1;
    }

    while(0) { }

    while (x && y) {
        ;
    }

    while (x || y) {
        x = y;
    }

    while (!x) {
        while(y) {
            y = y - 1;
        }
    }
}

void DoWhileStmts()
{
    do {} while(x);

    do {
    } while (x < y);

    do {
        x = x + 1;
    } while(1);

    do {
        ;
    } while(0);

    do {
        do {
            y = y - 1;
        } while(y);
    } while(x);
}

void ForStmts()
{
    for (;;) { }
    for (; x < y; ) {
        x = x + 1;
    }
    for (x = 0; x < y; x = x + 1) {
        y = y - 1;
    }
    for (x = 0, y = 10; x < y; x = x + 1, y = y - 1) {
        ;
    }
    for (x = 0; ; x = x + 1) {
        ;
    }
    for (; x; x = x - 1) {
        ;
    }
    for (x = 10; x > 0; ) {
        x = x - 1;
    }
    for (x = 0; x < 5; x = x + 1) {
        for (y = 10; y; y = y - 1) {
            ;
        }
    }
}
