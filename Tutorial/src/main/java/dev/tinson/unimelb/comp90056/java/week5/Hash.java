// Hash.java
// Hash class
// awirth for COMP90056
// Aug 2017,8,9

public class Hash{
	private int p = 1073741789; //smaller than 2^30
	private int a,b;		// only use for hash tables < 24593 in size
	
	public Hash(){
		a=StdRandom.uniform(p-1)+1;
		b=StdRandom.uniform(p); // changed from p-1
		System.out.format("a %16d b %12d p %12d %n", a,b,p);
	}
	public int h2u(int x,int range){
		long prod = (long)a*(long)x;
		prod += (long)b;
		long y = prod % (long) p;
		int r = (int) y % range;
		//System.out.format("x %12d y %12d r %12d %n", x,y,r);
		return r;
	}
	
	public static int h_basic(Object key,int domain){
		// domain should be something like 0x0fffffff
		return (key.hashCode() & domain);
	}
	
	public static void main(String args[]){
		Hash h = new Hash();
		
		int h2 = h.h2u(100,20);
	}
}
