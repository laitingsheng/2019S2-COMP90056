package dev.tinson.unimelb.comp90056.tutorial.java.common;

public interface DistinctCounter<T> {
    void add(T item);

    double distinct();
}
