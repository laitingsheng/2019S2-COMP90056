// Morris.java
// Morris counter class
// awirth for COMP90056
// Aug 2017,9

public class Morris {
    private int counter;
    private int power2; // this is still an int, so doesn't technically pack into
    // log log n bits, but illustrates the idea

    public static void main(String[] args) {    // to test this out
        int n = Integer.parseInt(args[0]);
        int k = Integer.parseInt(args[1]);

        Morris[] m = new Morris[k];

        for (int j = 0; j < k; j++) {
            m[j] = new Morris();
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < k; j++) {
                m[j].increment();
                System.out.println("actual " + i + " Morris[" + j + "]" + m[j].mycount());
            }
        }
    }

    public Morris() {
        counter = 0;
        power2 = 1;
    }

    public void increment() {
        double r = StdRandom.uniform();
        double p = Math.pow(2, counter);
        // double p = power2;
        if (r < 1 / p) {
            counter++;
            power2 *= 2;
        }
    }

    public int mycount() {
        return power2 - 1;
    }
}
