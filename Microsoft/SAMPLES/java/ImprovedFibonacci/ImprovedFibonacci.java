/*
 *
 * ImprovedFibonacci
 *
 */
class ImprovedFibonacci 
{
	static final int MAX_INDEX = 10;

	public static void main(String[] args) 
	{
		int lo = 1;
		int hi = 1;
		String mark;

		System.out.println("1: " + lo);
		for (int i = 2; i < MAX_INDEX ; i++)
		{
			if (hi % 2 == 0)
				mark = " *";
			else
				mark = "";
			System.out.println(i + ": " + hi + mark);
			hi = lo + hi; // new hi
			/* new lo is (sum - old lo) i.e., the old hi */
			lo = hi - lo;
		}
	}
}

