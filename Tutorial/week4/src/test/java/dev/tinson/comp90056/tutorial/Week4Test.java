package dev.tinson.comp90056.tutorial;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.junit.rules.TestRule;

import java.util.Random;

import static org.junit.Assert.assertTrue;

public class Week4Test {
    @Rule
    public TestRule rule = new TestName();

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
        new Random().ints(100, 0, 10).forEach(counter::add);
        double d = counter.distinct();
        assertTrue(d > 0 && d <= 10);
    }
}
