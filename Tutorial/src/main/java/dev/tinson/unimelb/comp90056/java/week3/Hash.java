package dev.tinson.unimelb.comp90056.java.week3;

import java.util.Random;

public final class Hash {
    private static final int p = 24593; //smaller than 2^15
    private final int a, b;

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
