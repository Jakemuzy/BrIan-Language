struct Rect {
    int x, y, w, h;
    void Flip() {
        int temp = w;
        w = h;
        h = temp;
    } 
};

// Pass