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

*/

fn <T> GenFunc <T, T2>(T one, T2 two) {
    return one + two;
}