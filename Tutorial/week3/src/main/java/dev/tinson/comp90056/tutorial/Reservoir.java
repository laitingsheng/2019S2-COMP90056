package dev.tinson.comp90056.tutorial;

import java.util.Arrays;
import java.util.Random;
import java.util.stream.IntStream;

public class Reservoir {
    private final int[] A;
    private int m = 0;

    public Reservoir(int k) {
        A = new int[k];
    }

    public int size() {
        return Math.min(m, A.length);
    }

    public void sample(IntStream stream) {
        stream.forEach(item -> {
            if (m < A.length)
                A[m++] = item;
            else {
                int r = new Random().nextInt(++m);
                if (r < A.length)
                    A[r] = item;
            }
        });
    }

    public int[] report() {
        return Arrays.copyOf(A, size());
    }
}
