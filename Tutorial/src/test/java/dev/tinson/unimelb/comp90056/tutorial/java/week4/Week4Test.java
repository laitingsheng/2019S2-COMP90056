package dev.tinson.unimelb.comp90056.tutorial.java.week4;

import dev.tinson.unimelb.comp90056.tutorial.java.common.DistinctCounter;
import org.junit.Test;

import java.util.Random;

import static org.junit.Assert.assertTrue;

public final class Week4Test {
    @Test
    public void testMorrisCounter() {
        int count = 0;
        for (int i = 0; i < 100; ++i) {
            MorrisCounter counter = new MorrisCounter(0.1, 0.1);
            for (int m = 0; m < 100; ++m)
                counter.increment();
            double c = counter.count();
            if (c >= 90 && c <= 110) // epsilon = 0.1
                ++count;
        }
        assertTrue(count >= 90 && count <= 100); // delta = 0.1
    }

    @Test
    public void testBaselineDistinctCounter() {
        // trivial, just for completeness
        DistinctCounter<Integer> counter = new BaselineDistinctCounter<>();
        assertTrue(new Random().ints(100, 0, 100).peek(counter::add).distinct().count() == counter.distinct());
    }
}
