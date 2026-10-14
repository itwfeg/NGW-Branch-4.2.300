import Softek.* ;

class sample {
	public static void main(String args[]) {
		int i ;
		int n ;
		int p;
		String s ;
		java.awt.Rectangle rect;

		/* Create Barcode class */
		Softek.Barcode barcode = new Softek.Barcode();

		/* Set some properties */
		barcode.ReadCode39 = 1 ;
		barcode.MultipleRead = 1 ;
		barcode.LicenseKey = "YOUR LICENSE KEY" ;


		//java.awt.Rectangle r = new java.awt.Rectangle(1000, 500, 500, 300);
		//barcode.SetScanRect(r, 0);
		/* Scan Image */
		n = barcode.ScanBarCode("image.tif") ;

		System.out.print("Found ") ;
		System.out.print(n) ;
		System.out.print(" barcode(s)\n") ;

		for (i = 0; i < n; i++)
		{
			s = barcode.GetBarString(i + 1) ;
			p = barcode.GetBarStringPage(i + 1);
			rect = barcode.GetBarStringRect(i + 1);
			System.out.print(s) ;
			System.out.print("\t\t(") ;
			s = barcode.GetBarStringType(i + 1) ;
			System.out.print(s) ;
			System.out.print(", Page ");
			System.out.print(p);
			System.out.print(", Pos ");
			System.out.print(rect.x);
			System.out.print(" ");
			System.out.print(rect.y);
			System.out.print(" ");
			System.out.print(rect.width);
			System.out.print(" ");
			System.out.print(rect.height);
			System.out.print(")\n");
		}
  }
}
