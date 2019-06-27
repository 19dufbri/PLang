function main() void {
    int i;
    i = 5;
    int m = min(i, 6);
    return m;
}

function min(int a, int b) int {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}