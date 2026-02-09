typedef int MyInt;
typedef double Real;
typedef MyInt AnotherInt;

struct Point {
    int x;
    int y;
};
typedef Point Point2D;

struct Rectangle {
    Point2D topLeft;
    Point2D bottomRight;
};
typedef Rectangle Rect;

enum Color {
    RED,
    GREEN,
    BLUE
};
typedef Color ColorType;

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};
typedef Direction Dir;

typedef Point2D Position;
typedef Rect Area;
typedef ColorType Shade;
typedef Dir Heading;

struct Node {
    int value;
    Node next;
};
typedef Node NodeType;
typedef NodeType* NodePtr;

struct Complex {
    double real;
    double imag;
};
typedef Complex ComplexNum;
typedef ComplexNum ComplexAlias;
