/**
 * CS 2110 Fall 2015 HW2
 * Part 1 - Coding with bases
 * 
 * @author Laura Cox Section B5
 *
 * Global rules for this file:
 * - You may not use more than 2 conditionals per method. Conditionals are
 *   if-statements, if-else statements, or ternary expressions. The else block
 *   associated with an if-statement does not count toward this sum.
 * - You may not use more than 2 looping constructs per method. Looping
 *   constructs include for loops, while loops and do-while loops.
 * - You may not use nested loops.
 * - You may not declare any file-level variables.
 * - You may not declare any objects, other than String in select methods.
 * - You may not use switch statements.
 * - You may not use the unsigned right shift operator (>>>)
 * - You may not write any helper methods, or call any other method from this or
 *   another file to implement any method.
 * - The only Java API methods you are allowed to invoke are:
 *     String.length()
 *     String.charAt()
 *     String.equals()
 * - You may not invoke the above methods from string literals.
 *     Example: "12345".length()
 * - When concatenating numbers with Strings, you may only do so if the number
 *   is a single digit.
 *
 * Method-specific rules for this file:
 * - You may not use multiplication, division or modulus in any method, EXCEPT
 *   strdtoi.
 * - You may declare exactly one String variable each in itostrb, and itostrx.
 */
public class HW2Bases
{
	/**
	 * strdtoi - Decimal String to int
	 *
	 * Convert a string containing ASCII characters (in decimal) to an int.
	 * You do not have to handle negative numbers. The Strings we will pass in will be
	 * valid decimal numbers, and able to fit in a 32-bit signed integer.
	 * 
	 * Example: strdtoi("123"); // => 123
	 */
	public static int strdtoi(String decimal)
	{
		// multiply by the place that this is at; need while loop that terminates after finishing string
		int output = 0;
		int power = 1;
		for (int num = decimal.length()-1; num >= 0; num--)
		{
			
			output = output +  power*(decimal.charAt(num)-'0');
			power = 10*power;
		}

		return output;
	}

	/**
	 * strbtoi - Binary String to int
	 *
	 * Convert a string containing ASCII characters (in binary) to an int.
	 * You do not have to handle negative numbers. The Strings we will pass in will be
	 * valid binary numbers, and able to fit in a 32-bit signed integer.
	 * 
	 * Example: strbtoi("1
	 */
	public static int strbtoi(String binary)
	{
		int bit = 0;
	
		for (int num = 0; num < binary.length(); num++) {
			bit = bit << 1;
			bit = bit + (binary.charAt(num) - '0');
		}
		return bit;
	}

	/**
	 * strxtoi - Hexadecimal String to int
	 *
	 * Convert a string containing ASCII characters (in hex) to an int.
	 * The input string will only contain numbers and uppercase letters A-F.
	 * You do not have to handle negative numbers. The Strings we will pass in will be
	 * valid hexadecimal numbers, and able to fit in a 32-bit signed integer.
	 * 
	 * Example: strxtoi("A6"); // => 166
	 */
	public static int strxtoi(String hex)
	{
		int bit = 0;
		for (int num = 0; num < hex.length(); num++) {
            bit = bit << 4;
            if ((hex.charAt(num)-'0') > 9) {
				bit = bit + hex.charAt(num)-'A' + 10;
            } else {
                bit = hex.charAt(num)-'0' + bit;
            }
		}
		return bit;
	}

	/**
	 * itostrb - int to Binary String
	 *
	 * Convert a int into a String containing ASCII characters (in binary).
	 * You do not have to handle negative numbers.
	 * The String returned should contain the minimum number of characters necessary to
	 * represent the number that was passed in.
	 * 
	 * Example: itostrb(7); // => "111"
	 */
	public static String itostrb(int binary)
	{
		String bit = "";
		int base10 = binary;
		int mod = 0;
		if (binary == 0) {
			return "0";
		}
		while (base10 > 0) {
			mod = (base10 & 0x1);
			bit = (char)(mod + '0') + bit;
			base10 = base10 >> 1;
		}
	
		return bit;
	}

	/**
	 * itostrx - int to Hexadecimal String
	 *
	 * Convert a int into a String containing ASCII characters (in hexadecimal).
	 * The output string should only contain numbers and uppercase letters A-F.
	 * You do not have to handle negative numbers.
	 * The String returned should contain the minimum number of characters necessary to
	 * represent the number that was passed in.
	 * 
	 * Example: itostrx(166); // => "A6"
	 */
	public static String itostrx(int hex)
	{
		String bit = "";
		int base16 = hex;
		int mod = 0;
		if (hex == 0) {
			return "0";
		}
		while (base16 > 0) {
			mod = (base16 & 0xF);
            if (mod < 10) {
				bit = (char)(mod + '0') + bit;
            } else {
                bit = (char)(mod - 10 + 'A') + bit;
            }
            base16 = base16 >> 4;
		}
		return bit;

	}
}
