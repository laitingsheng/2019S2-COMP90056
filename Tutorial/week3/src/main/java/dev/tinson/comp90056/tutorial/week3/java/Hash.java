package dev.tinson.comp90056.tutorial.week3.java;

import java.util.Random;

public class Hash {
    private static final int p = 24593; //smaller than 2^15
    private final int a, b; // only use for hash tables < 24593 in size

    public static int basic(Object key) {
        return key.hashCode() & 0x0000ffff; // only lower 16 bits
    }

    public Hash() {
        Random random = new Random();
        a = random.nextInt(p - 1) + 1;
        b = random.nextInt(p);
    }

    public int hash(int x, int r) {
        return (a * x + b) % p % r;
    }
}
