struct beef {
    int x;
    string y;
    char z;
    int qr = 174;
};

struct cheese {
    int x;
    string y;
    struct beez {
        char f;
        string k;
    };
};

struct point {
    int x;
    int y;
};

struct matrix {
    int data[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
};

int main() {
    beef.x = 4;
    beef.qr++;
    beef.z += 'a';

    cheese.beez.f = 'z';
    cheese.beez.k = "hello";

    point points[5];
    points[2].x = 10;
    points[2].y = 20;

    matrix m;
    m.data[1][2] = 42;

    beef.x = beef.x + points[2].y;
    cheese.beez.k = cheese.beez.k + " world";
}
