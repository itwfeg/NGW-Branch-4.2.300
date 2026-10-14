/// <copyright file="Product.cs" company="Hobart">
/// Copyright (c) Hobart, a division of ITW Food Equipment Group LLC 2014
/// </copyright>
/// <author>Randy Blankley</author>
/// <date>September 2014</date>
/// <summary>Product class</summary>

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace HobartRemoteAPI
{
    /// <summary>
    /// Product object class containing product record information.
    /// </summary>
    public class Product
    {
        private int PrID_ = 0;
        public int Id
        {
            get { return PrID_; }
            set { PrID_ = value; }
        }

        private int ClNum_ = 0;
        public int ReportingClassNum
        {
            get { return ClNum_; }
            set { ClNum_ = value; }
        }

        private int TxNum_ = 0;
        public int TextNum
        {
            get { return TxNum_; }
            set { TxNum_ = value; }
        }

        private int SMNum_ = 0;
        public int SpecialMessageNum
        {
            get { return SMNum_; }
            set { SMNum_ = value; }
        }

        private int NFNum_ = 0;
        public int NutritionNum
        {
            get { return NFNum_; }
            set { NFNum_ = value; }
        }

        private int NtNum_ = 0;
        public int NoteNum
        {
            get { return NtNum_; }
            set { NtNum_ = value; }
        }

        private int LbNum1_ = 0;
        public int Label1Num
        {
            get { return LbNum1_; }
            set { LbNum1_ = value; }
        }

        private int LbNum2_ = 0;
        public int Label2Num
        {
            get { return LbNum2_; }
            set { LbNum2_ = value; }
        }

        private int LbNum3_ = 0;
        public int Label3Num
        {
            get { return LbNum3_; }
            set { LbNum3_ = value; }
        }

        private int CaNum_ = 0;
        public int CategoryNum
        {
            get { return CaNum_; }
            set { CaNum_ = value; }
        }

        private int SPNum_ = 0;
        public int SpecialNum
        {
            get { return SPNum_; }
            set { SPNum_ = value; }
        }

        private int LCNum_ = 0;
        public int COOLCategoryNum
        {
            get { return LCNum_; }
            set { LCNum_ = value; }
        }

        private int LPNum_ = 0;
        public int COOLPreTextNum
        {
            get { return LPNum_; }
            set { LPNum_ = value; }
        }

        private int LXNum_ = 0;
        public int CurrentCOOLTextNum
        {
            get { return LXNum_; }
            set { LXNum_ = value; }
        }

        private int DLXNum_ = 0;
        public int DefaultCOOLTextNum
        {
            get { return DLXNum_; }
            set { DLXNum_ = value; }
        }

        private int LLID_ = 0;
        public int COOLListId
        {
            get { return LLID_; }
            set { LLID_ = value; }
        }

        private int MaNum_ = 0;
        public int MarqueeNum
        {
            get { return MaNum_; }
            set { MaNum_ = value; }
        }

        private int ProdTxt5Num_ = 0;
        public int ProductText5Num
        {
            get { return ProdTxt5Num_; }
            set { ProdTxt5Num_ = value; }
        }

        private int ProdTxt6Num_ = 0;
        public int ProductText6Num
        {
            get { return ProdTxt6Num_; }
            set { ProdTxt6Num_ = value; }
        }

        private int ProdTxt7Num_ = 0;
        public int ProductText7Num
        {
            get { return ProdTxt7Num_; }
            set { ProdTxt7Num_ = value; }
        }

        private int ProdTxt8Num_ = 0;
        public int ProductText8Num
        {
            get { return ProdTxt8Num_; }
            set { ProdTxt8Num_ = value; }
        }

        private int ProdTxt9Num_ = 0;
        public int ProductText9Num
        {
            get { return ProdTxt9Num_; }
            set { ProdTxt9Num_ = value; }
        }

        private int PrPrice_ = 0;
        public int Price
        {
            get { return PrPrice_; }
            set { PrPrice_ = value; }
        }

        private int PrPrMod_ = 0;
        public int PriceFactor
        {
            get { return PrPrMod_; }
            set { PrPrMod_ = value; }
        }

        private bool PrIsFrcPr_ = false;
        public bool ForcePrice
        {
            get { return PrIsFrcPr_; }
            set { PrIsFrcPr_ = value; }
        }

        private int PrExcPr_ = 0;
        public int ExceptionPrice
        {
            get { return PrExcPr_; }
            set { PrExcPr_ = value; }
        }

        private int PrDscMe_ = 0;
        public int DiscountMethod
        {
            get { return PrDscMe_; }
            set { PrDscMe_ = value; }
        }

        private int PrDscPr1_ = 0;
        public int DiscountPrice1
        {
            get { return PrDscPr1_; }
            set { PrDscPr1_ = value; }
        }

        private int PrDscPr2_ = 0;
        public int DiscountPrice2
        {
            get { return PrDscPr2_; }
            set { PrDscPr2_ = value; }
        }

        private int PrDscPr3_ = 0;
        public int DiscountPrice3
        {
            get { return PrDscPr3_; }
            set { PrDscPr3_ = value; }
        }

        private int PrTare_ = 0;
        public int Tare
        {
            get { return PrTare_; }
            set { PrTare_ = value; }
        }

        private bool PrIsFrcTr_ = false;
        public bool ForceTare
        {
            get { return PrIsFrcTr_; }
            set { PrIsFrcTr_ = value; }
        }

        private int PrPrpTare_ = 0;
        public int ProportionalTare
        {
            get { return PrPrpTare_; }
            set { PrPrpTare_ = value; }
        }

        private int PrNetWt_ = 0;
        public int NetWeight
        {
            get { return PrNetWt_; }
            set { PrNetWt_ = value; }
        }

        private int PrByCnt_ = 0;
        public int ByCount
        {
            get { return PrByCnt_; }
            set { PrByCnt_ = value; }
        }

        private bool PrIsFrcBC_ = false;
        public bool ForceByCount
        {
            get { return PrIsFrcBC_; }
            set { PrIsFrcBC_ = value; }
        }

        private int PrShlfLfD_ = 0;
        public int ShelfLifeDays
        {
            get { return PrShlfLfD_; }
            set { PrShlfLfD_ = value; }
        }

        private int PrShlfLfH_ = 0;
        public int ShelfLifeHours
        {
            get { return PrShlfLfH_; }
            set { PrShlfLfH_ = value; }
        }

        private int PrProdLfD_ = 0;
        public int ProductLifeDays
        {
            get { return PrProdLfD_; }
            set { PrProdLfD_ = value; }
        }

        private int PrProdLfH_ = 0;
        public int ProductLifeHours
        {
            get { return PrProdLfH_; }
            set { PrProdLfH_ = value; }
        }

        private bool PrIsPrnSL_ = false;
        public bool DoNotPrintShelfLife
        {
            get { return PrIsPrnSL_; }
            set { PrIsPrnSL_ = value; }
        }

        private bool PrIsPrnPL_ = false;
        public bool DoNotPrintProductLife
        {
            get { return PrIsPrnPL_; }
            set { PrIsPrnPL_ = value; }
        }

        private bool PrIsPrnPO_ = false;
        public bool DoNotPrintPackedOn
        {
            get { return PrIsPrnPO_; }
            set { PrIsPrnPO_ = value; }
        }

        private int PrBnsPts_ = 0;
        public int BonusPoints
        {
            get { return PrBnsPts_; }
            set { PrBnsPts_ = value; }
        }

        private int PrPrtns_ = 0;
        public int Portions
        {
            get { return PrPrtns_; }
            set { PrPrtns_ = value; }
        }

        private bool PrIsUsSL_ = false;
        public bool ApplySecurityLabel
        {
            get { return PrIsUsSL_; }
            set { PrIsUsSL_ = value; }
        }

        private int PrInMode_ = 0;
        public int IntakeMode
        {
            get { return PrInMode_; }
            set { PrInMode_ = value; }
        }

        private int PrLblRot1_ = 0;
        public int Label1Rotation
        {
            get { return PrLblRot1_; }
            set { PrLblRot1_ = value; }
        }

        private int PrLblPlc1_ = 0;
        public int Label1Placement
        {
            get { return PrLblPlc1_; }
            set { PrLblPlc1_ = value; }
        }

        private int PrLblRot2_ = 0;
        public int Label2Rotation
        {
            get { return PrLblRot2_; }
            set { PrLblRot2_ = value; }
        }

        private int PrLblPlc2_ = 0;
        public int Label2Placement
        {
            get { return PrLblPlc2_; }
            set { PrLblPlc2_ = value; }
        }

        private int PrLblRot3_ = 0;
        public int Label3Rotation
        {
            get { return PrLblRot3_; }
            set { PrLblRot3_ = value; }
        }

        private int PrLblPlc3_ = 0;
        public int Label3Placement
        {
            get { return PrLblPlc3_; }
            set { PrLblPlc3_ = value; }
        }
        
        private string PrName1_ = "";
        public string Name1
        {
            get { return PrName1_; }
            set { PrName1_ = value; }
        }

        private string PrName2_ = "";
        public string Name2
        {
            get { return PrName2_; }
            set { PrName2_ = value; }
        }

        private bool PrVCOLTxt_ = false;
        public bool ForceCOOLReview
        {
            get { return PrVCOLTxt_; }
            set { PrVCOLTxt_ = value; }
        }

        private bool PrCOLReq_ = false;
        public bool COOLRequired
        {
            get { return PrCOLReq_; }
            set { PrCOLReq_ = value; }
        }

        private int PrPrPlT_ = 0;
        public int ProductionPlanningType
        {
            get { return PrPrPlT_; }
            set { PrPrPlT_ = value; }
        }

        private int PrNum_ = 0;
        public int PLU
        {
            get { return PrNum_; }
            set { PrNum_ = value; }
        }

        private int PrType_ = 0;
        public int Type
        {
            get { return PrType_; }
            set { PrType_ = value; }
        }

        private int PrBarPref_ = 0;
        public int BarcodePrefix
        {
            get { return PrBarPref_; }
            set { PrBarPref_ = value; }
        }

        private int PrBarProN_ = 0;
        public int BarcodeProductNum
        {
            get { return PrBarProN_; }
            set { PrBarProN_ = value; }
        }

        private int PrBarVenN_ = 0;
        public int BarcodeVendorNum
        {
            get { return PrBarVenN_; }
            set { PrBarVenN_ = value; }
        }

        private int PrEANCd_ = 0;
        public int BarcodeEANCode
        {
            get { return PrEANCd_; }
            set { PrEANCd_ = value; }
        }

        private string PrRawBarC_ = "";
        public string RawBarcodeNum
        {
            get { return PrRawBarC_; }
            set { PrRawBarC_ = value; }
        }

        private string PrCdNm1_ = "";
        public string CodedName1
        {
            get { return PrCdNm1_; }
            set { PrCdNm1_ = value; }
        }

        private string PrCdNm2_ = "";
        public string CodedName2
        {
            get { return PrCdNm2_; }
            set { PrCdNm2_ = value; }
        }

        private DateTime PrSpcSD_;
        public DateTime SpecialStartDate
        {
            get { return PrSpcSD_; }
            set { PrSpcSD_ = value; }
        }

        private string PrDesc_ = "";
        public string Description
        {
            get { return PrDesc_; }
            set { PrDesc_ = value; }
        }
        
        private string PrGrNm1_ = "";
        public string Graphic1
        {
            get { return PrGrNm1_; }
            set { PrGrNm1_ = value; }
        }

        private string PrGrNm2_ = "";
        public string Graphic2
        {
            get { return PrGrNm2_; }
            set { PrGrNm2_ = value; }
        }

        private string PrGrNm3_ = "";
        public string Graphic3
        {
            get { return PrGrNm3_; }
            set { PrGrNm3_ = value; }
        }

        private string PrGrNm4_ = "";
        public string Graphic4
        {
            get { return PrGrNm4_; }
            set { PrGrNm4_ = value; }
        }

        private string PrImage_ = "";
        public string Image
        {
            get { return PrImage_; }
            set { PrImage_ = value; }
        }

        private string PrCOLTN_ = "";
        public string COOLTrackingText
        {
            get { return PrCOLTN_; }
            set { PrCOLTN_ = value; }
        }

        private static bool ParseRecord(string record, ref Product product)
        {
            // split data into elements
            string[] data = record.Split(',');
            int index = 0;

            // validate data
            if (data.Length < 75)
                return false;

            // parse data into product record
            product = new Product();
            product.PrID_ = Util.ConvertToInt(data[index++]);
            product.ClNum_ = Util.ConvertToInt(data[index++]);
            product.TxNum_ = Util.ConvertToInt(data[index++]);
            product.ProdTxt5Num_ = Util.ConvertToInt(data[index++]);
            product.ProdTxt6Num_ = Util.ConvertToInt(data[index++]);
            product.ProdTxt7Num_ = Util.ConvertToInt(data[index++]);
            product.ProdTxt8Num_ = Util.ConvertToInt(data[index++]);
            product.ProdTxt9Num_ = Util.ConvertToInt(data[index++]);
            product.SMNum_ = Util.ConvertToInt(data[index++]);
            product.NFNum_ = Util.ConvertToInt(data[index++]);
            product.NtNum_ = Util.ConvertToInt(data[index++]);
            product.LbNum1_ = Util.ConvertToInt(data[index++]);
            product.LbNum2_ = Util.ConvertToInt(data[index++]);
            product.LbNum3_ = Util.ConvertToInt(data[index++]);
            product.CaNum_ = Util.ConvertToInt(data[index++]);
            product.SPNum_ = Util.ConvertToInt(data[index++]);
            product.LCNum_ = Util.ConvertToInt(data[index++]);
            product.LPNum_ = Util.ConvertToInt(data[index++]);
            product.LXNum_ = Util.ConvertToInt(data[index++]);
            product.DLXNum_ = Util.ConvertToInt(data[index++]);
            product.LLID_ = Util.ConvertToInt(data[index++]);
            product.MaNum_ = Util.ConvertToInt(data[index++]);
            product.PrPrice_ = Util.ConvertToInt(data[index++]);
            product.PrPrMod_ = Util.ConvertToInt(data[index++]);
            product.PrIsFrcPr_ = Util.ConvertToBool(data[index++]);
            product.PrExcPr_ = Util.ConvertToInt(data[index++]);
            product.PrDscMe_ = Util.ConvertToInt(data[index++]);
            product.PrDscPr1_ = Util.ConvertToInt(data[index++]);
            product.PrDscPr2_ = Util.ConvertToInt(data[index++]);
            product.PrDscPr3_ = Util.ConvertToInt(data[index++]);
            product.PrTare_ = Util.ConvertToInt(data[index++]);
            product.PrIsFrcTr_ = Util.ConvertToBool(data[index++]);
            product.PrPrpTare_ = Util.ConvertToInt(data[index++]);
            product.PrNetWt_ = Util.ConvertToInt(data[index++]);
            product.PrByCnt_ = Util.ConvertToInt(data[index++]);
            product.PrIsFrcBC_ = Util.ConvertToBool(data[index++]);
            product.PrShlfLfD_ = Util.ConvertToInt(data[index++]);
            product.PrShlfLfH_ = Util.ConvertToInt(data[index++]);
            product.PrProdLfD_ = Util.ConvertToInt(data[index++]);
            product.PrProdLfH_ = Util.ConvertToInt(data[index++]);
            product.PrIsPrnSL_ = Util.ConvertToBool(data[index++]);
            product.PrIsPrnPL_ = Util.ConvertToBool(data[index++]);
            product.PrIsPrnPO_ = Util.ConvertToBool(data[index++]);
            product.PrBnsPts_ = Util.ConvertToInt(data[index++]);
            product.PrPrtns_ = Util.ConvertToInt(data[index++]);
            product.PrIsUsSL_ = Util.ConvertToBool(data[index++]);
            product.PrInMode_ = Util.ConvertToInt(data[index++]);
            product.PrLblRot1_ = Util.ConvertToInt(data[index++]);
            product.PrLblPlc1_ = Util.ConvertToInt(data[index++]);
            product.PrLblRot2_ = Util.ConvertToInt(data[index++]);
            product.PrLblPlc2_ = Util.ConvertToInt(data[index++]);
            product.PrLblRot3_ = Util.ConvertToInt(data[index++]);
            product.PrLblPlc3_ = Util.ConvertToInt(data[index++]);
            product.PrName1_ = Util.ConvertToString(data[index++]);
            product.PrName2_ = Util.ConvertToString(data[index++]);
            product.PrVCOLTxt_ = Util.ConvertToBool(data[index++]);
            product.PrCOLReq_ = Util.ConvertToBool(data[index++]);
            product.PrPrPlT_ = Util.ConvertToInt(data[index++]);
            product.PrNum_ = Util.ConvertToInt(data[index++]);
            product.PrType_ = Util.ConvertToInt(data[index++]);
            product.PrBarPref_ = Util.ConvertToInt(data[index++]);
            product.PrBarProN_ = Util.ConvertToInt(data[index++]);
            product.PrBarVenN_ = Util.ConvertToInt(data[index++]);
            product.PrEANCd_ = Util.ConvertToInt(data[index++]);
            product.PrRawBarC_ = Util.ConvertToString(data[index++]);
            product.PrCdNm1_ = Util.ConvertToString(data[index++]);
            product.PrCdNm2_ = Util.ConvertToString(data[index++]);
            product.PrSpcSD_ = Util.ConvertToDateTime(data[index++]);
            product.PrDesc_ = Util.ConvertToString(data[index++]);
            product.PrGrNm1_ = Util.ConvertToString(data[index++]);
            product.PrGrNm2_ = Util.ConvertToString(data[index++]);
            product.PrGrNm3_ = Util.ConvertToString(data[index++]);
            product.PrGrNm4_ = Util.ConvertToString(data[index++]);
            product.PrImage_ = Util.ConvertToString(data[index++]);
            product.PrCOLTN_ = Util.ConvertToString(data[index++]);

            // fixup product name fields
            product.PrName1_ = product.PrDesc_.Replace((char)0x1E, ',');
            product.PrName2_ = product.PrDesc_.Replace((char)0x1E, ',');

            // fixup description field
            product.PrDesc_ = product.PrDesc_.Replace((char)0x1E, ',');
            product.PrDesc_ = product.PrDesc_.Replace((char)0xDE, '\n');

            return true;
        }

        /// <summary>
        /// Retrieve products using where clause.
        /// </summary>
        /// <param name="client">scale</param>
        /// <param name="whereClause">select statement where clause</param>
        /// <param name="products">products</param>
        /// <returns>true on success, false otherwise</returns>
        public static bool ExecuteSelectStatement(Comm client, string whereClause, ref List<Product> products)
        {
            List<string> records = null;

            // perform the select statement
            if (!client.ExecuteSelectStatement("Prods", whereClause, ref records))
                return false;

            // create container for results
            products = new List<Product>();

            // parse each record
            foreach (string record in records)
            {
                Product product = null;

                if (!ParseRecord(record, ref product))
                    return false;

                products.Add(product);
            }

            return true;
        }

        /// <summary>
        /// Retrieve one or more products from the HT scale.
        /// </summary>
        /// <param name="client">scale</param>
        /// <param name="productIds">list of products to retrieve</param>
        /// <param name="products">dictionary of product information</param>
        /// <returns>true on success, false otherwise</returns>
        public static bool RetrieveProductFromScale(Comm client, List<int> productIds, ref Dictionary<int, Product> products)
        {
            try
            {
                // create a new dictionary to hold results
                products = new Dictionary<int, Product>();

                // process list
                foreach (int productId in productIds)
                {
                    List<Product> result = null;

                    // retrieve product
                    if (!ExecuteSelectStatement(client, string.Format("\"PrNum\"=\"{0}\"", productId), ref result))
                        return false;

                    products[productId] = (result.Count <= 0) ? null : result[0];
                }
            }
            catch (Exception e)
            {
                return false;
            }

            return true;
        }

        /// <summary>
        /// Retrieve one or more products from the HT scale.
        /// </summary>
        /// <param name="ip">scale IP address</param>
        /// <param name="productIds">list of products to retrieve</param>
        /// <param name="products">dictionary of product information</param>
        /// <returns>true on success, false otherwise</returns>
        public static bool RetrieveProductFromScale(IPAddress ip, List<int> productIds, ref Dictionary<int, Product> products)
        {
            // connect to ip address
            Comm client = new Comm(ip);

            // process query
            return RetrieveProductFromScale(client, productIds, ref products);
        }

        /// <summary>
        /// Retrieve a single product from the HT scale.
        /// </summary>
        /// <param name="client">scale</param>
        /// <param name="productId">product to retrieve</param>
        /// <param name="product">product information, or null if product does not exist</param>
        /// <returns>true on success, false otherwise</returns>
        public static bool RetrieveProductFromScale(Comm client, int productId, ref Product product)
        {
            List<int> productIds = new List<int>();
            productIds.Add(productId);

            Dictionary<int, Product> products = null;

            // retrieve our product
            if (!RetrieveProductFromScale(client, productIds, ref products))
                return false;
            else if (!products.ContainsKey(productId))
                return false;

            product = products[productId];
            return true;
        }

        /// <summary>
        /// Retrieve a single product from the HT scale.
        /// </summary>
        /// <param name="ip">scale IP address</param>
        /// <param name="productId">product to retrieve</param>
        /// <param name="product">product information, or null if product does not exist</param>
        /// <returns>true on success, false otherwise</returns>
        public static bool RetrieveProductFromScale(IPAddress ip, int productId, ref Product product)
        {
            // connect to ip address
            Comm client = new Comm(ip);

            // process query
            return RetrieveProductFromScale(client, productId, ref product);
        }

    }
}
