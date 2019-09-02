package dev.tinson.unimelb.comp90056.tutorial.java.week5;

import dev.tinson.unimelb.comp90056.tutorial.java.common.DistinctCounter;
import org.junit.Test;

import java.util.Random;

import static org.junit.Assert.assertTrue;

public final class Week5Test {
    @Test
    public void testAMSCounter() {
        DistinctCounter<Integer> counter = new AMSDistinctCounter<>(100, 0.1);
        double d = new Random().ints(100, 0, 100).peek(counter::add).distinct().count(), cd = counter.distinct();
        System.out.println(d);
        System.out.println(cd);
        assertTrue(cd >= d * 0.9 && cd <= d * 1.1);
    }
}
