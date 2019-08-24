// Bloom.java
// Bloom filter class
// awirth for COMP90056
// Aug 2017,8,9

public class Bloom<Key> {
    private final int n;
    private final Hash[] hashes;
    private final boolean[] A;

    public Bloom(int n, int k) {
        this.n = n; // initialize n & k
        // number of hash functions
        A = new boolean[n];//initializes to false
        hashes = new Hash[k]; // build an array of k hash functions
        for (int i = 0; i < k; i++)
            hashes[i] = new Hash();
    }

    public void insert(Key key) {
        int h = Hash.h_basic(key);
        // hash function
        for (Hash hash: hashes)
            A[hash.h2u(h, n) % n] = true;        // set every hashed position bit to true
    }

    public boolean query(Key key) {
        int h = Hash.h_basic(key);
        // if at least one of the false
        for (Hash hash: hashes)
            if (!A[hash.h2u(h, n) % n])
                return false;        // bits is false, return
        return true;
    }
}
