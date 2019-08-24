package dev.tinson.comp90056.tutorial;

public interface DistinctCounter<T> {
    void add(T item);

    double distinct();
}
