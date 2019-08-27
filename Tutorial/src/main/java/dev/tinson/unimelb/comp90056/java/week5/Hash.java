package dev.tinson.unimelb.comp90056.java.week5;

import java.util.Random;

public final class Hash {
    private int p = 1073741789; //smaller than 2^30
    private int a, b;

    public static int basic(Object key, int domain) {
        // domain should be something like 0x0fffffff
        return key.hashCode() & domain;
    }

    public Hash() {
        Random random = new Random();
        a = random.nextInt(p - 1) + 1;
        b = random.nextInt(p);
    }

    public int hash(int x, int r) {
        return (int)((((long)a * x) + b) % p) % r;
    }
}
