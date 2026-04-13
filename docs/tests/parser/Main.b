/* 
interface IOperation {
    let int a, b;

    fn int operation(int one, int two);
}

fn int main() {
    let float k = 3 + 3;
    let int g = 3 * 3 + 3 ** 2;
}

let u8 grind = 3;
let u16 rewind = 4.2;   // Okay since not type checked yet
let string var = "string here";
let char boo = 'b';

fn <T> GenFunc <T, T2>(T one, T2 two) {
    return one + two;
}

fn int funcPointerTest() {
    let fp void(int, int) funcPointer = main;
}

struct Employee : IEmployee {
    let int hourly_salary, hours;
    let Employee* boss;
}


struct Manager : IEmployee, IManager {
    operator +(Employee i, Employee f) {
         
        i->?boss != f->boss;    // Make this a condition statement

        
        return ( i.?hours + f.hours ) as float;
    }
}

fn fp void(int, int) fpReturn(int i) {
    for (i = 0, b += 23; i < 4; i++) {
        ;
    }
}

fn fp fp void(int, int)(int) funcTest(int x) {

    let closure int(int, int) funcPointer = lambda fp int(int, char) (float x, int y) captures x {
        return x + y;
    };

    return fpReturn;
}

enum BARK {
      IDENTFIIER, ONER,  
};


fn void ForTest() {
    if (i < 4) {

    } elif (b++) {
        x += 2;
    } elif (c --) {
        y == 2;
    }

    else {
        k += a**b;
    }
}
*/

fn void SwitchTest() {
    switch (x->b) {     // This should be allowed
        case b {

        }
        case 3 {
            
        } default {
            x++;
        }
    }
}