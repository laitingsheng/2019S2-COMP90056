// Reservoir.java
// Reservoir sampler class
// Scaffolding code, for students to enter details into
// awirth for COMP90056
// Aug 2019

public class Reservoir {
    // Stores k items from the "stream"
    private int[] A; // sample
    private int m = 0; // stream length
    private int k; // sample size

    public static void main(String args[]) {
        int i;
        Reservoir r;
        int f;
        int k = 10; // for example -- feel free to change this!
        int S[] = new int[k];

        r = new Reservoir(k);

        // How do you test your code?
    }

    public Reservoir(int k) {
        // constructor
    }

    public void considerItem(int x) {
        // to make things a bit simpler, focus on the case of sampling k integers from a stream
    }

    public int report(int S[]) {
        // tell us what's inside, by copying into S
        // use the returned int to say how many integers are in the sample! (what if n<k?!)

        // actually, this is a very ugly C-style way of doing this.
    }
}
