import java.util.Arrays;

// AMSm.java
// AMS distinct elements counter
// awirth for COMP90056
// Aug 2017,8,9

public class AMSm implements Distinct{
	private int n;	//domain and range
	private int zs[]; // array of z values
	private Hash hs[]; // array of hash functions
	private int range = 536870912-3;//2^29-3
	//private int c=1835; //from the formula!
	private int c=18; //NOT from the formula!
	private int k; // size of the array of hash functions
		
	public AMSm(int n,int del1){
	    // set the values of n and k (latter from del1)
	    // set up zs and hs
		
		
	}
	public void add(Object o){
		for(int i=0;i<k;i++){
			int val = Hash.h_basic(o,n);
			// use the h2u from hs[i]

			// for each hash function, store the maximum number of
			// zeros seen
		}
	}
	public static double getMedian(int[] data) {
	    int[] copy = Arrays.copyOf(data, data.length);
	    Arrays.sort(copy);
	    // if the length is odd, return the middle item, else return the average of the two middle items
	    return (copy.length % 2 != 0) ? copy[copy.length / 2] : (copy[copy.length / 2] + copy[(copy.length / 2) - 1]) / 2;
	}
	public double distinct(){
		// use the fact that 2^(z+0.5) is monotonic to return median estimate
		return 0; //placeholder
	}
	
}
