// Distinct.java
// Interface for distinct elements counter
// awirth for COMP90056
// Aug 2017,8,9

public interface Distinct{
	public static int zeros(int v){
		return Integer.numberOfTrailingZeros(v); // builtin function
	}
	
	void add(Object o); // add a new element to the collection
	double distinct(); // return a double representing the number of
			    // distinct elements
}
