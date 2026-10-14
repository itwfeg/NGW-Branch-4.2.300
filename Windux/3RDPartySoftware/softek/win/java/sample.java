import Softek.* ;

class sample {
	public static void main(String args[]) {
		int i ;
		int n ;
		int p;
		int errno ;
		int winerrno ;
		String s ;
		java.awt.Rectangle rect;

		/* Create Barcode class */
		Softek.Barcode barcode = new Softek.Barcode();

		//barcode.LicenseKey = "MY LICENSE KEY";

		/* Set some properties */
		barcode.ReadCode39 = 1 ;
		barcode.MultipleRead = 1 ;


		//java.awt.Rectangle r = new java.awt.Rectangle(1000, 500, 500, 300);
		//barcode.SetScanRect(r, 0);
		/* Scan Image */
		n = barcode.ScanBarCode("image.tif") ;

		if (n < 0)
		{
			errno = barcode.GetLastError() ;
			winerrno = barcode.GetLastWinError();
			System.out.print("ScanBarcode returned " + n + "\n");
			System.out.print("Last Softek Error Number = " + errno + "\n");
			System.out.print("Last Windows Error Number = " + winerrno + "\n");
			return ;
		}
			
		System.out.print("Found " + n + " barcodes\n") ;

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
