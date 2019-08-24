// Hash.java
// Hash class
// awirth for COMP90056
// Aug 2017,8,9

public class Hash {
    private int p = 24593; //smaller than 2^15
    private int a, b;        // only use for hash tables < 24593 in size

    public static int h_basic(Object key) {        // if you only want the
        return (key.hashCode() & 0x0000ffff);   // lower 16 bits
    }

    public static void main(String args[]) {
        Hash h = new Hash();

        int h2 = h.h2u(100, 20);
    }

    public Hash() {
        a = StdRandom.uniform(p - 1) + 1;    // choose random parameters
        b = StdRandom.uniform(p);
    }

    public int h2u(int x, int r) {

        int y = (a * x + b) % p;
        return y % r;
    }
}
