package dev.tinson.unimelb.comp90056.java.week5;

import dev.tinson.unimelb.comp90056.java.common.DistinctCounter;

import java.util.Arrays;
import java.util.stream.DoubleStream;

public final class AMSDistinctCounter<T> implements DistinctCounter<T> {
    private static final int c = 1835; // from the formula!
    // private static final int c = 18; // NOT from the formula!
    private static final int range = (1 << 29) - 3; // 2^29-3
    private static final double sqrt2 = Math.sqrt(2);

    private final int n, k;
    private final Hash[] hashes;
    private int[] zs;

    public AMSDistinctCounter(int N, double delta) {
        // 2^ceil(log2(N))-1
        int n = 1;
        while (N > 0) {
            N >>= 1;
            n <<= 1;
        }
        this.n = n - 1;

        k = c * (int)Math.ceil(Math.log(1 / delta));
        hashes = new Hash[k];
        for (int i = 0; i < k; i++)
            hashes[i] = new Hash();
        zs = new int[k];
    }

    public void add(T item) {
        int h = Hash.basic(item, n);
        for (int i = 0; i < k; i++)
            zs[i] = Math.max(zs[i], Integer.numberOfTrailingZeros(hashes[i].hash(h, range)));
    }

    public double distinct() {
        DoubleStream stream = Arrays.stream(zs).mapToDouble(z -> (1 << z) * sqrt2).sorted();
        if (k % 2 == 0)
            return stream.skip(k / 2 - 1).limit(2).sum() / 2;
        return stream.skip(k / 2).findFirst().getAsDouble();
    }
}
