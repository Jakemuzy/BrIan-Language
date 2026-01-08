// Test all different types of functions work

void NoParam(){

}

void IntParam(int i){

}

void DoubleParam(double d){

}

void FloatParam(float f){

}

void CharParam(char c){

}

void StringParam(string s){

}

int main() {
    NoParam();

    int i = 5;
    float f1 = 4.16286531983265, f2 = 5;
    double d1 = 3.2118, d2 = 5;
    char c1 = 4, c2 = 'a';

    IntParam(3);
    IntParam(i);
    IntParam(f1);
    IntParam(f2);
    IntParam(d1);
    IntParam(d2);

    DoubleParam(7);
    DoubleParam(5.1);
    DoubleParam(d1);
    DoubleParam(d2);
    DoubleParam(f1);
    DoubleParam(f2);
    DoubleParam(i);

    FloatParam(751);
    FloatParam(841.917664);
    FloatParam(f1);
    FloatParam(f2);
    FloatParam(d1);
    FloatParam(d2);
    FloatParam(i);

    CharParam('c');
    CharParam(4);
    CharParam(c1);
    CharParam(c2);
}

