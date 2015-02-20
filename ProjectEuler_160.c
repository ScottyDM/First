/**
 * @author ScottyDM
 * Title   Project Euler -- Problem 160
 * Version 0.2
 * Date    2012-01-30
 * 
 * 
 * -------------------------------------------------------------------------------------------------
 * Problem 160: http://projecteuler.net/problem=160
 * 
 * For any N, let f(N) be the last five digits before the trailing zeroes in N!.
 * For example,
 * 
 *   9! = 362880 so f(9)=36288
 *   10! = 3628800 so f(10)=36288
 *   20! = 2432902008176640000 so f(20)=17664
 * 
 * Find f(1,000,000,000,000)
 * -------------------------------------------------------------------------------------------------
 * 
 * A fairly simple way to calculate a factorial is with a for loop, where the count limit is N. The 
 * loop would start with 1 (or 2), then count up to N, multiplying an accumulated answer against the 
 * loop variable. And so:
 *    factorial = 1;
 *    for (count = 1; count <= N; count++) {
 *      factorial = factorial * count;
 *    }
 * Of course the value of factorial will be a million-bajillion digits for any nontrivial value of 
 * N. It becomes time consuming to keep track of so many digits.
 * 
 * I see two key concepts that make this problem solvable in a reasonable amount of computer time:
 * 
 * 1. When doing any multiplication: If one factor has one or more LSDs (least significant digits) 
 *    which are zero, then the product will also be zero for those same digits. In other words the 
 *    number of LSD zeros in the product will grow, but never ever shrink. Since we don't care 
 *    about these zeros we can discard them as they appear.
 * 
 * 2. When doing any multiplication: If you only care about X LSDs, then you don't need to consider 
 *    any digits of either factor more significant than X LSDs. Consider...
 *      1234 * 56789 = 70077626
 *    And the same numbers, but truncated to three LSDs...
 *      234 * 789 = 184626
 *    Different answers, but look at the three LSDs of those answers. Both are 626.
 * 
 * Another key concept is that we don't want LSD zeros in either factor or the product. Examining 
 * this problem with a spreadsheet, and thinking of numbers in terms that they are products of a 
 * bunch of prime numbers (e.g. 50 is really 2 * 5 * 5), two more things become apparent:
 * 
 * 3. The accumulator (the accumulated answer at each step of the way) gets gobs and gobs of twos, 
 *    but no fives. The reason no fives is because any five that sneaks in immediately combines 
 *    with a two, becomes an LSD zero, and is discarded. Consider that 4! is 24, or 2 * 2 * 2 * 3. 
 *    Multiplying this by 5 (to get 5!) gives us 120, or 2 * 2 * 2 * 3 * 5. Getting rid of the LSD 
 *    zero gives us 12, or 2 * 2 * 3. Hence, many twos, no fives.
 * 
 * 4. Fives come from the for loop's counter, the gizmo that gives us N. Every fifth value of N 
 *    injects another five into the works. If a two also appears (such as N = 10, 20, ... 30000000, 
 *    etc.) We can easily get rid of it by stripping off the LSD zeros. This leaves a modified N 
 *    without both fives and twos. Now it could have more twos, or more fives. In fact given a 
 *    five-digit number we could have as many as seven fives: 5^7 = 78125. If there happens to be 
 *    seven twos in the accumulator then we end up with seven LSD zeros in their product.
 * 
 * If we can keep LSD zeros from showing up in the product, then we have some control over how many 
 * MSDs to keep in the factors before we multiply them together. The goal is for the number of 
 * digits in the two factors to total 18 or fewer, which will give us a product that fits within a 
 * long integer.
 * 
 * The key to doing this is to:
 * 
 * 1st. Simplify the value of count before we multiply it with factorial: remove all the paired 
 *      twos and fives (the tens, or LSD zeros). Let's call modified count, cnt (we need to preserve 
 *      count for next go round the loop).
 * 
 * 2nd. Simplify both cnt and factorial by removing fives in cnt and twos in factorial in equal 
 *      numbers. We don't need to preserve factorial, and it's not the factorial, so let's call it 
 *      fac. Anyway, when fac contains twos and cnt contains fives, then their product will have LSD 
 *      zeros, which we don't want. By removing these excess twos and fives we're less likely to 
 *      overflow the long integer data type.
 * 
 * 3rd. After simplification of cnt and fac, whack off unneeded MSDs. What is unneeded? 
 *      Experimentation suggests five digits is always appropriate for cnt, but the number needed 
 *      for fac depends on the value of N, with five as the minimum. This roughly translates to:
 *        6 digits when N = 10^6
 *        7 digits when N = 10^7
 *        7 digits when N = 10^8
 *        9 digits when N = 10^9   runtime = 3:15
 *        9 digits when N = 10^10  runtime = 32:00 (approximately)
 *      These are minimum values. Of course the goal is N = 10^12, but since we have thirteen we 
 *      shouldn't run out.
 * 
 * 4th. Even with the simplifications of the 1st and 2nd steps, twos and fives can sneak into the 
 *      product. This happens when there aren't enough LSDs preserved in fac before multiplication, 
 *      which makes it a good quick test of the digit count.
 * 
 * 5th. Even with the occasional glitch, such as a two/five pair in the product, the system seems 
 *      to be somewhat self-healing--as long as the error isn't too great. Still, it's hard to know 
 *      if the answer is to be trusted when glitches are detected. I guess submit the answer.
 * 
 * Implementation:
 * 
 * 1. At loop start assume fac will have no LSD zeros and will have enough significant digits to 
 *    get the job done.
 *    
 * 2. At loop start cnt can be any value and needs to be reduced: excess fives and twos removed.
 * 
 * 3. At loop start fac can be reduced and cnt further reduced: twos removed from fac and fives 
 *    removed from cnt in equal numbers.
 * 
 * 4. After the pre-multiplication reduction of fac and cnt, whack of excess MSDs. The goal is for 
 *    their product to fit into a long integer.
 * 
 * 5. There is a remote possibility that after whacking the factors, their product may have twos 
 *    and fives, so reduce: excess fives and twos removed from the product.
 * 
 * 6. As long as the product of fac and cnt is within range we don't need to whack off excess MSDs 
 *    before looping.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main () {
	long long int LIMIT = 100000000;
	long long int count;
	long long int cnt;
	long long int fac = 1;
	
	for (count = 1; count <= LIMIT; count++) {
		cnt = count;
		while ((cnt % 10) == 0) {
			cnt = cnt / 10;
		}
		while (((cnt % 5) == 0) && ((fac % 2) == 0)) {
			cnt = cnt / 5;
			fac = fac / 2;
		}
		cnt = cnt % 100000;
		fac = fac % 100000000L;	//10^6 min 6, 10^7 min 7, 10^8 min 7, 10^9 min 9, 10^10 min 9
		fac = (fac * cnt);
		//Check for fives and twos sneaking in and clobber them. This should rarely run.
		while (fac % 10 == 0) {
			printf("At %d, product is %d.\n", count, fac);
			fac = fac / 10;
		}
		//Print intermediate result every ten-millionth count
		if (count % 10000000 == 0) {
			printf("%d million is %d.\n", count / 1000000, fac);
		}
	}
	printf("The answer is %d.\n", fac % 100000);

	system("PAUSE");	
	return 0;
}
