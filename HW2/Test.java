public class Test {
	public static void main(String[] args) {
		int bits = 0;
		int index = 3;

		bits |= (1 << index);

		System.out.println("" + bits);
	}
}