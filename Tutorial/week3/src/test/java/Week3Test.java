import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.junit.rules.TestRule;

import static org.junit.Assert.assertTrue;

public class Week3Test {
    @Rule
    public TestRule rule = new TestName();

    @Test
    public void testBloomString() {
        String[] ss = {"Tony", "Matthias", "Yadeesha", "Shuo"};
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
}
