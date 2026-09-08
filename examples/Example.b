
// Interfaces
interface Printable {
    fn void print();
}


// Generic Struct

struct Object <T> {
    // Generic disallow variable definitions and operator overloading
    let T one, two;

    // Member functions are still allowed
    fn T Add() {
        return one + two;
    }

    // Generic structs cannot implement interfaces
}

// Normal Struct implementing interface

struct Pair : Printable {
    // Normal structs allow predefined member variables
    let int one = 1;
    let double two = 2;

    fn void print() {
        // Pretend printf exists 
        printf("Val 1: %d \tVal 2: %d\n", one, two);
    }

    // Operator overloading only works with custom types
    // and non-asignment operators. 

    operator +(Pair x, Pair y) {
        let Pair new;
        new.one = x.one + new.one;
        new.two = x.two + new.two;
        return new;
    }
}

// Main function 

fn int main() {
    let Pair a = {1, 2};
    let Pair b = {3, 4};

    // Lambda that adds two Pair values using operator+
    let closure Pair(Pair, Pair) addPairs =
        lambda Pair(Pair x, Pair y) captures a, b  {
            return x + y;
        };

    let Pair result = {0, 0};
    let mutex m;

    // Concurrency with tasks 
    // One of the many concurrency methods in BrIan
    let task t1 = spawn lambda void() captures a, b, result, m {
        let Pair temp = addPairs(b, a);

        lock(m) {
            result = result + temp;
        }
    };

    await t1;
    result.print();

    return 0;
}

