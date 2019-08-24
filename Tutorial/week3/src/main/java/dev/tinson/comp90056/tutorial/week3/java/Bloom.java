package dev.tinson.comp90056.tutorial.week3.java;

public class Bloom<K> {
    private final int n;
    private final Hash[] hashes;
    private final boolean[] filter;

    public Bloom(int n, int k) {
        this.n = n;
        hashes = new Hash[k];
        filter = new boolean[n];
        for (int i = 0; i < k; i++)
            hashes[i] = new Hash();
    }

    public void insert(K key) {
        int h = Hash.basic(key);
        for (Hash hash: hashes)
            filter[hash.hash(h, n) % n] = true;
    }

    public boolean query(K key) {
        int h = Hash.basic(key);
        for (Hash hash: hashes)
            if (!filter[hash.hash(h, n) % n])
                return false;
        return true;
    }
}
