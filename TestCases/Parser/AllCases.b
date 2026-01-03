#include <>

main() {
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

    // Empty
    if () {
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

}


