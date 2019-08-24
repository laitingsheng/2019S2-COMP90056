package dev.tinson.comp90056.tutorial.week4.java;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.junit.rules.TestRule;

import static org.junit.Assert.assertTrue;

public class Week4Test {
    @Rule
    public TestRule rule = new TestName();

    @Test
    public void testMorrisCounter() {
        for (int i = 0; i < 100; ++i) {
            MorrisCounter counter = new MorrisCounter(0.1, 0.1);
            for (int m = 0; m < 100; ++m)
                counter.increment();
            assertTrue(counter.count() > 32);
        }
    }
}
