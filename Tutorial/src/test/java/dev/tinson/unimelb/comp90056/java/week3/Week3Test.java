package dev.tinson.unimelb.comp90056.java.week3;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.junit.rules.TestRule;

import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class Week3Test {
    @Rule
    public TestRule rule = new TestName();

    @Test
    public void testBloomString() {
        String[] ss = {"abc", "bcd", "cde", "def"};
        Bloom<String> b = new Bloom<>(40, 3);
        for (String s : ss)
            b.insert(s);
        for (String s : ss)
            assertTrue(b.query(s));
    }

    @Test
    public void testBloomInteger() {
        Bloom<Integer> b = new Bloom<>(40, 3);
        for (int i = 0; i < 100; i += 3)
            b.insert(i);
        int c = 0;
        for (int i = 0; i < 100; ++i)
            if (i % 3 == 0)
                assertTrue(b.query(i));
            else
                c += b.query(i) ? 1 : 0;
        assertTrue(c >= 0 && c <= 67);
    }

    @Test
    public void testReservoir() {
        Reservoir reservoir = new Reservoir(10);
        HashMap<Integer, Integer> counter = new HashMap<>();
        reservoir.sample(new Random(System.currentTimeMillis()).ints(5, 0, 20)
                                        .peek(item -> counter.merge(item, 1, Integer::sum)));
        assertEquals(reservoir.size(), 5);
        Map<Integer, Integer> copy = (Map<Integer, Integer>)counter.clone();
        for (int item : reservoir.report())
            copy.merge(item, -1, Integer::sum);
        for (int v : copy.values())
            assertEquals(v, 0);
        reservoir.sample(new Random(System.currentTimeMillis()).ints(15, 0, 20)
                                        .peek(item -> counter.merge(item, 1, Integer::sum)));
        assertEquals(reservoir.size(), 10);
        copy = (Map<Integer, Integer>)counter.clone();
        for (int item : reservoir.report())
            copy.merge(item, -1, Integer::sum);
        for (int v : copy.values())
            assertTrue(v >= 0 && v <= 20);
    }
}
