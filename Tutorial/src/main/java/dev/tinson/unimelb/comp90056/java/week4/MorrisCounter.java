package dev.tinson.unimelb.comp90056.java.week4;

import java.util.Arrays;
import java.util.Random;
import java.util.stream.DoubleStream;

public class MorrisCounter {
    private final int ng, gc;
    private long[][] estimates;

    public MorrisCounter(double epsilon, double delta) {
        ng = (int)Math.ceil(12 * Math.log(1 / delta));
        gc = (int)Math.ceil(2 / epsilon / epsilon);
        estimates = new long[ng][gc];
        for (long[] group : estimates)
            Arrays.fill(group, 1);
    }

    public void increment() {
        for (long[] group : estimates)
            for (int i = 0; i < gc; ++i)
                if (new Random().nextDouble() < 1.0 / group[i])
                    group[i] <<= 1;
    }

    public double count() {
        DoubleStream stream = Arrays.stream(estimates).mapToDouble(group -> {
            double sum = 0;
            for (long i : group)
                sum += i;
            return sum / gc;
        }).sorted();
        if (ng % 2 == 0)
            return stream.skip(ng / 2 - 1).limit(2).sum() / 2 - 1;
        return stream.skip(ng / 2).findFirst().getAsDouble() - 1;
    }
}
