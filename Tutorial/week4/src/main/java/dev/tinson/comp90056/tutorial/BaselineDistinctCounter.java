package dev.tinson.comp90056.tutorial;

import java.util.HashSet;

public class BaselineDistinctCounter<K> implements DistinctCounter<K> {
    private HashSet<K> items = new HashSet<>();

    @Override
    public void add(K item) {
        items.add(item);
    }

    @Override
    public double distinct() {
        return items.size();
    }
}
