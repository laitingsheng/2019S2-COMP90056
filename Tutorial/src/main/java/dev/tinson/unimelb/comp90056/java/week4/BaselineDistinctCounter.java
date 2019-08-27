package dev.tinson.unimelb.comp90056.java.week4;

import dev.tinson.unimelb.comp90056.java.common.DistinctCounter;

import java.util.HashSet;

public final class BaselineDistinctCounter<K> implements DistinctCounter<K> {
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
