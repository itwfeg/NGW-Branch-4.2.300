/// <copyright file="Program.cs" company="Hobart">
/// Copyright (c) Hobart, a division of ITW Food Equipment Group LLC 2014
/// </copyright>
/// <author>Randy Blankley</author>
/// <date>September 2014</date>
/// <summary>Example illustrating retrieval of a product record</summary>

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;

using HobartRemoteAPI;

namespace RetrieveProductExample
{
    class Program
    {
        /// <summary>
        /// What scale to connect to.
        /// </summary>
        static string scaleIpAddress = "10.3.177.169";

        /// <summary>
        /// Pull a single product from an HT scale.
        /// </summary>
        static void Example1()
        {
            // scale ip address and product to retrieve
            IPAddress scale = IPAddress.Parse(scaleIpAddress);
            int prodId = 1;

            // result product goes here
            Product prod = null;

            // do the lookup!
            if (!Product.RetrieveProductFromScale(scale, prodId, ref prod))
                Console.WriteLine("Error retrieving product!");
            else
            {
                Console.WriteLine("Success!");

                // null product indicates the PLU is NOF otherwise we have valid product
                if (null == prod)
                    Console.WriteLine("Product not on file!");
                else
                    Console.WriteLine(String.Format("Unit Price: {0}", prod.Price));
            }
        }

        /// <summary>
        /// Pull multiple products from an HT scale.
        /// </summary>
        static void Example2()
        {
            // scale ip address and products to retrieve
            IPAddress scale = IPAddress.Parse(scaleIpAddress);

            List<int> prodIds = new List<int>();
            prodIds.Add(1);
            prodIds.Add(2);
            prodIds.Add(3);

            // result products goes here
            Dictionary<int,Product> prods = null;

            // do the lookup!
            if (!Product.RetrieveProductFromScale(scale, prodIds, ref prods))
                Console.WriteLine("Error retrieving products!");
            else
            {
                Console.WriteLine("Success!");

                // null product indicates the PLU is NOF otherwise we have valid product
                foreach (KeyValuePair<int, Product> i in prods)
                {
                    Product prod = i.Value;

                    if (null == prod)
                        Console.WriteLine("Product not on file!");
                    else
                        Console.WriteLine(String.Format("Unit Price: {0}", prod.Price));
                }
            }
        }

        /// <summary>
        /// Pull a range of products from an HT scale.
        /// </summary>
        static void Example3()
        {
            // scale ip address and products to retrieve
            Comm scale = new Comm(IPAddress.Parse(scaleIpAddress));

            // result products goes here
            List<Product> prods = null;

            // do the lookup! all products where 0 < product id <= 100
            if (!Product.ExecuteSelectStatement(scale, "\"0\"<\"PrNum\" AND \"PrNum\"<=\"100\"", ref prods))
                Console.WriteLine("Error retrieving products!");
            else
            {
                Console.WriteLine("Success!");

                // null product indicates the PLU is NOF otherwise we have valid product
                foreach (Product prod in prods)
                    Console.WriteLine(String.Format("Product {0} Unit Price: {1}", prod.PLU, prod.Price));
            }
        }

        /// <summary>
        /// Pull all products in category 1 from an HT scale.
        /// </summary>
        static void Example4()
        {
            // scale ip address and products to retrieve
            Comm scale = new Comm(IPAddress.Parse(scaleIpAddress));

            // result products goes here
            List<Product> prods = null;

            // do the lookup! all products where category is 1
            if (!Product.ExecuteSelectStatement(scale, "\"CaNum\"=\"1\"", ref prods))
                Console.WriteLine("Error retrieving products!");
            else
            {
                Console.WriteLine("Success!");

                // null product indicates the PLU is NOF otherwise we have valid product
                foreach (Product prod in prods)
                    Console.WriteLine(String.Format("Product {0} Unit Price: {1}", prod.PLU, prod.Price));
            }
        }

        /// <summary>
        /// Raw select statement from scale database.
        /// </summary>
        static void Example9()
        {
            // scale ip address
            Comm scale = new Comm(IPAddress.Parse(scaleIpAddress));

            List<string> records = null;

            // do the lookup! in this case all softkeys for the main run screen
            // WARNING: this is raw column data from the database, you will need to setup a parser to handle the data coming back
            if (!scale.ExecuteSelectStatement("SftKeys", "\"SgNum\"=\"999003\"", ref records))
                Console.WriteLine("Error performing select!");
            else
            {
                Console.WriteLine("Success!");

                // print each record
                foreach (string record in records)
                    Console.WriteLine(record);
            }
        }

        static void Main(string[] args)
        {
            Example1(); // single product lookup
            Example2(); // multiple product lookup
            Example3(); // product lookup by range
            Example4(); // product lookup by category

            Example9(); // raw select statement
        }
    }
}
