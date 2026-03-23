int func(int i, char c) {
    return i += c;
}

int main() {
    int c = func("str", 'c');
}

// Fail