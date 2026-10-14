import logging
import requests
import requests.exceptions
import os
import sys
import json

def handleRequest():

    if len(sys.argv) > 1 :

        # format of incoming data to script : countryCode/storeNbr/checkdigit/url/data
        # url = sys.argv[1].split(",")[3]
        timeoutT = int(sys.argv[1].split(",")[0])
        logging.debug("Timeout after: " + str(timeoutT))
        # OR (baseURL = url above ) then append the url to the endpoint (static?) /scale/v1/{countryCode}/{storeNbr}/mips/generate-barcode
        # So we would have:
        baseURL = sys.argv[1].split(",")[4]
        url = baseURL + "/scale/v1/" + sys.argv[1].split(",")[1] + "/" + sys.argv[1].split(",")[2] + "/mips/generate-barcode"
        requestData = sys.argv[1].split(",")[4:]
        data = sys.argv[1]

        logging.debug("The URL being used for the request: " + url)

        try:

            if len(requestData) > 0:
                
                jsonData = buildRequest(requestData, data)
                logging.debug("Sending this payload to the server: " + jsonData)
                headers = {'Content-Type': 'application/json'}
                response = requests.request("POST", url, headers=headers, data=jsonData, timeout = timeoutT)

                # parse and format the response into something easily readable in backend

                # Example response: {"barcode":"47900101400000000220","totalPrice":20.13} ---> 47900101400000000220 20.13
                responseText = response.text
                
                logging.debug("The response from the server: " + response.text)

                # delims
                delim = [i for i in range (len(responseText)) if responseText.startswith(":", i)]
                delimComma = responseText.find(",")

                barcodeNum = responseText[delim[0] + 2 : delimComma - 1]
                totalPrice = responseText[delim[1] + 1 : len(responseText) - 1]
                # backend already translates it to a decimal apparently...
                totalPrice = totalPrice.replace(totalPrice[totalPrice.find('.')], "", 1) 

                rString = barcodeNum + " " + totalPrice
                logging.debug("This data is heading back to the application: " + rString)
                print(rString)

        # problem reaching server
        except:
            logging.debug("Bad Request")
            print (2)  


# parses transaction info from application and builds json object for request based on specified format
def buildRequest( data, productsList ):
    
    # format of data
    dataFields = buildDefaultObject()
    dataToList = data

    # checkDigit
   # newCheckDigit = {'checkDigit': dataToList[0] }
   # dataFields.update(newCheckDigit)

    # date
    newDate = {'date': dataToList[1] }
    dataFields.update(newDate)

    # description
    newDescription = {'description' : dataToList[2] }
    dataFields.update(newDescription)

    # products - has the format of [partialPrice, pieces, productNbr, ....]
    productsStart = productsList.index("[")
    productsEnd = productsList.find("]")
    pList = productsList[productsStart + 1:productsEnd].split(",")
    productsListIter = 0

    while productsListIter < len(pList):

        partialPrice = float(float(pList[productsListIter]) / 100) # decimal format
        pieces = pList[productsListIter + 1]
        productNbr = pList[productsListIter + 2]

        newProduct = {'partialPrice' : partialPrice, 'pieces' : pieces, 'productNbr':productNbr}
        # insert the product and its info (dictionary) into the products section 
        dataFields['products'].append(newProduct)
        # since each product has 3 subfields, increment iterator by that
        productsListIter = productsListIter + 3
    
    # totalItems
    newTotalItems = {'totalItems': dataToList[len(dataToList) - 2] }
    dataFields.update(newTotalItems)

    # totalPrice
    newTotalPrice = {'totalPrice' : float(float(dataToList[len(dataToList) - 1]) / 100 ) } # decimal format
    dataFields.update(newTotalPrice)

    # build json obect
    return json.dumps(dataFields)

def buildDefaultObject():

    jObject = { }

    #jObject['checkDigit'] = 0 # int
    jObject['date'] = " "   # string
    jObject['description'] = " " # string (empty)
    jObject['products'] = []   # list of product dictionaries containing partialPrice, pieces and productNbr
    jObject['totalItems'] = 0   # int
    jObject['totalPrice'] = 0   # float

    return jObject

# used by the UI to see if the entered server URL is online
def handleInquiry():
    
    testURL = sys.argv[2]
    logging.debug("Handling connection status inquiry for this URL: " + testURL)
    try:
        r = requests.get(testURL)

    except (requests.exceptions.ConnectionError, requests.exceptions.Timeout):
        print(2)
        logging.debug("Error Reaching Server in connection status inquiry - timeout or connection error")
    
    # got a response from server, assume it's online
    else:
        print(1)

def main():

    logging.basicConfig(filename='/tmp/getBarcodeFromCloud.log', filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')

    if len(sys.argv) == 3 and sys.argv[1][0] == "1":
            handleInquiry()
    elif len(sys.argv) == 2:
            handleRequest()
    else:
        print(3)
        logging.debug("Problem with arguments")

if __name__ == "__main__":
    main()