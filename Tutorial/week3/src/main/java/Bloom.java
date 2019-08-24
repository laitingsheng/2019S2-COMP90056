// Bloom.java
// Bloom filter class
// awirth for COMP90056
// Aug 2017,8,9

public class Bloom<Key> {
    private int n; // size of the array
    private int k; // number of hash functions
    private Hash[] hash;

    private boolean[] A;

    public static void main(String args[]) { //for testing
        if (args.length != 2) {
            System.err.println("should be two arguments: n k");
            System.exit(0);
        }
        int n = Integer.parseInt(args[0]);
        int k = Integer.parseInt(args[1]);

        String ss[] = {"Tony", "Matthias", "Yadeesha", "Shuo"};
        int m = 100;
        Bloom<String> sb = new Bloom(n, k);
        Bloom<Integer> si = new Bloom(n, k);
        for (String s : ss) {
            sb.insert(s);
        }
        for (String s : ss) {
            if (sb.query(s)) {
                System.out.println("Yes: " + s);
            }
        }
        for (int i = 0; i < m; i += 3) {
            si.insert(i);
        }
        for (int i = 0; i < m; i++) {
            if (si.query(i) && i % 3 != 0) {
                System.out.println("Yes: " + i);
            }
        }
    }

    public Bloom(int n, int k) {
        this.n = n; // initialize n & k
        this.k = k;
        A = new boolean[n];//initializes to false
        hash = new Hash[k]; // build an array of k hash functions
        for (int i = 0; i < k; i++) {
            hash[i] = new Hash();
        }
    }

    public void insert(Key key) {
        int h = Hash.h_basic(key);
        for (int i = 0; i < k; i++) {
            int hu = hash[i].h2u(h, n); // hash the key with each
            // hash function
            A[hu % n] = true;        // set every hashed position bit
            // to true
        }
    }

    public boolean query(Key key) {
        int h = Hash.h_basic(key);
        for (int i = 0; i < k; i++) {
            int hu = hash[i].h2u(h, n); // hash the key
            if (!A[hu % n]) {            // if at least one of the
                return false;        // bits is false, return
            }                // false
        }
        return true;
    }
}
