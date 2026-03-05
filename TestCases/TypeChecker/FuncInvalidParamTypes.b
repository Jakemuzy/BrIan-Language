int func(int i, char c) {
    return i += c;
}

int main() {
    int c = func(1, 'c');
}

// FAIL