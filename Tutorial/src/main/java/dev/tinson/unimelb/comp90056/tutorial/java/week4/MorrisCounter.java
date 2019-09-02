package dev.tinson.unimelb.comp90056.tutorial.java.week4;

import java.util.Arrays;
import java.util.Random;
import java.util.stream.DoubleStream;

public final class MorrisCounter {
    private final int n, k;
    private final long[][] estimates;

    public MorrisCounter(double epsilon, double delta) {
        n = (int)Math.ceil(12 * Math.log(1 / delta));
        k = (int)Math.ceil(2 / epsilon / epsilon);
        estimates = new long[n][k];
        for (long[] group : estimates)
            Arrays.fill(group, 1);
    }

    public void increment() {
        for (long[] group : estimates)
            for (int i = 0; i < k; ++i)
                if (new Random().nextDouble() < 1.0 / group[i])
                    group[i] <<= 1;
    }

    public double count() {
        DoubleStream stream = Arrays.stream(estimates).mapToDouble(group -> {
            double sum = 0;
            for (long i : group)
                sum += i;
            return sum;
        }).sorted();
        if (n % 2 == 0)
            return stream.skip(n / 2 - 1).limit(2).sum() / 2 / k - 1;
        return stream.skip(n / 2).findFirst().getAsDouble() / k - 1;
    }
}
