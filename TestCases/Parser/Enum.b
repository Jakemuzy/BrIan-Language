enum simple {
    A, B, C
};

enum explicit {
    X = 1, Y = 2, Z = 10
};

enum mixed {
    P, Q = 5, R
};

enum empty {
};

struct s1 {
    enum inner1 { I1, I2 };
};

struct s2 {
    enum inner2 { J1 = 10, J2, J3 = 20 };
};

struct s3 {
    enum colors { RED, GREEN, BLUE };
};

