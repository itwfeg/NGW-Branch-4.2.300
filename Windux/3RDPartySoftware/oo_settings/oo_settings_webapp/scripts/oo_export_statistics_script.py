# @file oo_export_statistics_script.py
# @author Joe E Crew
# interacts with blackbox, retrieves online order data statistics, exports to CSV file

import csv
import datetime
import sys
import json
import requests
from requests.exceptions import ConnectionError, Timeout
from time import localtime, strftime


ORIGIN_ONLINE = 1
ORIGIN_KIOSK = 2
ORIGIN_NUMBER = 3
ORIGIN_MAX = 3

STATUS_WAITING = 1
STATUS_IN_PROGRESS = 2
STATUS_READY = 3     # Completed
STATUS_DELIVERED = 4
STATUS_CANCELLED = 5
STATUS_MAX = 5

PLU_TYPE_RANDOM = 1
PLU_TYPE_FIXED = 2
PLU_TYPE_BYCOUNT = 3
PLU_TYPE_FLUID = 4
PLU_MAX = 4

ORDER_STATUS_LIST = ['', 'Waiting', 'In-progress', 'Filled/Completed', 'Delivered', 'Cancelled']
ORDER_ORIGIN_LIST = ['', 'Online', 'Kiosk', 'Take-A-Number']

ORDER_ORDERNUMBER = 'OrderNumber'
ORDER_ORDERSTATUS = 'OrderStatus'
ORDER_CUSTOMERNAME = 'CustomerName'
ORDER_ORDERSMS = 'OrderSMS'
ORDER_ORDERORIGIN = 'OrderOrigin'
ORDER_STORENUMBER = 'StoreNumber'
ORDER_ORDERCREATION = 'OrderCreation'
ORDER_ORDERPICKUP = 'OrderPickup'
ORDER_ALLOWSUBSTITUTIONS = 'AllowSubstitutions'

ORDER_ITEMCOUNT = 'ItemCount'
ORDER_ORDERSTART = 'OrderStart'
ORDER_ORDERFINISH = 'OrderFinish'
ORDER_ORDERINSTRUCTIONS = 'OrderInstructions'
ORDER_OPERATORS = 'Operators'

xORDER_ORDERTOTAL = 'OrderTotal'
xORDER_BLOCKORDER = 'BlockOrder'
xORDER_REASONCANCELLED = 'ReasonCancelled'
xORDER_DENIEDCANCELREQ = 'DeniedCancelReq'
xORDER_NUMOFTIMESPRINTED = 'NumOfTimesPrinted'

xORDER_DEPTNUM = 'DeptNum'

ORDERASSIGNMENT_DEPTNUM = 'DeptNum'
ORDERASSIGNMENT_ORDERNUMBER = 'OrderNumber'
ORDERASSIGNMENT_ZONENUM = 'ZoneNum'
ORDERASSIGNMENT_PARTSTATUS = 'PartStatus'
ORDERASSIGNMENT_ITEMCOUNT = 'ItemCount'
ORDERASSIGNMENT_PARTSTART = 'PartStart'
ORDERASSIGNMENT_PARTFINISH = 'PartFinish'
ORDERASSIGNMENT_OPERATOR = 'Operator'

DETAIL_ORDERNUMBER = 'OrderNumber'
DETAIL_DEPTNUM = 'DeptNum'
DETAIL_ZONENUM = 'ZoneNum'
DETAIL_ORDERNUMID = 'OrderNumId'
DETAIL_PLUNUMBER = 'PrNum'
DETAIL_PRODUCTTYPE = 'ProductType'
DETAIL_PLUPRICE = 'PluPrice'
DETAIL_PLUBYCOUNT = 'PluByCount'
DETAIL_PLUWEIGHT = 'PluWeight'
DETAIL_PLUDESCRIPTION = 'PluDescription'
DETAIL_PLUINSTRUCTIONS = 'PluInstructions'
DETAIL_REASONCANCELLED = 'ReasonCancelled'
DETAIL_CANCELLED = 'Cancelled'
DETAIL_FILLED = 'Filled'
DETAIL_SUBSTITUTED = 'Substituted'
DETAIL_ORIGINALPLUNUMBER = 'OriginalPrNum'
DETAIL_FINALPRICE = 'FinalPrice'
DETAIL_FINALWEIGHT = 'FinalWeight'
DETAIL_FINALCOUNT = 'FinalCount'
DETAIL_ALLOWSUBSTITUTIONS = 'AllowSubstitutions'

PRODUCT_DEPTNUM = 'DeptNum'
PRODUCT_PRTYPE = 'PrType'
PRODUCT_PRPRICE = 'PrPrice'

bb_address = ''
dir_path = ''
datetime_format = '%Y-%m-%d %H:%M:%S'
start_date_time_str = "2022-01-01 00:00:00"
start_date_time = datetime.datetime.strptime(start_date_time_str, datetime_format)
end_date_time = datetime.datetime.now()
end_date_time_str = end_date_time.strftime(datetime_format)

PRICE_DIVISOR = 100.0
WEIGHT_DIVISOR = 1000.0

REPORT_BEGIN = "Report beginning date"
REPORT_END = "Report end date"

# debug_file = open(dir_path + '/oo_debug.dat', 'w', encoding="UTF8")
try:
    debug_file = open('oo_debug.dat', 'w')
    debug_file.write("Start Debug File %s\n" % (end_date_time_str))
except:
    print("Exception opening debug file")


def write_order_debug(order, used_or_not):
    # Write order to debug file
    debug_file.write("\nOrder %s used:\n" % (used_or_not))
    for col in order:
        try:
            debug_file.write(col + ": " + str(order[col]) + "\n")  # isinstance(o, int)
        except:
            debug_file.write("\nInvalid format in : " + col)  # did not like CustomerName Bobward 999

def write_detail_debug(detail):
    # Write order detail to debug file
    debug_file.write("\nOrder Detail:\n")
    for col in detail:
        try:
            debug_file.write(col + ": " + str(detail[col]) + "\n")  # isinstance(o, int)
        except:
            debug_file.write("\nInvalid format in : " + col)  # did not like CustomerName Bobward 999

def use_order(order):
    order_created = order[ORDER_ORDERCREATION]
    order_started = order[ORDER_ORDERSTART]
    order_finished = order[ORDER_ORDERFINISH]
    order_created_dt = None
    order_started_dt = None
    order_finished_dt = None

    if order_created != None and order_created[:4] != '0000':
        order_created_dt = datetime.datetime.strptime(order_created, datetime_format)
    if order_started != None and order_started[:4] != '0000':
        order_started_dt = datetime.datetime.strptime(order_started, datetime_format)
    if order_finished != None and order_finished[:4] != '0000':
        order_finished_dt = datetime.datetime.strptime(order_finished, datetime_format)

    if (order_created_dt != None and order_created_dt >= start_date_time and order_created_dt <= end_date_time) or \
        (order_started_dt != None and order_started_dt >= start_date_time and order_started_dt <= end_date_time) or \
        (order_finished_dt != None and order_finished_dt >= start_date_time and order_finished_dt <= end_date_time):
        return True

    return False

#///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
def run_summary():

    debug_file.write("\n------------- Summary Report -------------------\n")

    url = "http://%s/api/v1/version" % (bb_address)
    req = requests.get(url, timeout=(1, 30))

    if req.status_code != 200:
        print("Data query: %s" % url)
        print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
        sys.exit(-1)

    try:
        version = req.json()
    except:
        print("failure")
        sys.exit(-1)

    if not "deviceType" in version:
        print("failure")
        sys.exit(-1)

    stats_data = []

    debug_file.write("\n------------- PROCESS ORDERS -------------------\n")

    arr_status = [0, 0, 0, 0, 0, 0]
    arr_origin = [0, 0, 0, 0]
    arr_plutype = [0, 0, 0, 0, 0]
    order_number_list = []
    filled_order_list = []

    total_items = total_time_filling = total_orders_filled = total_price = total_cost = total_weight = filled_and_timed_order_count = 0
    start_order = '0000-00-00 00:00:00'
    end_order = '0000-00-00 00:00:00'

    # TAN = Take-A-Number
    TAN_wait_time_order_count = 0
    TAN_undetermined_wait_time_order_count = 0
    TAN_total_wait_time = 0

    # OK = Online or Kiosk
    OK_late_order_count = 0
    OK_total_late_time_seconds = 0
    OK_early_order_count = 0
    OK_total_early_time_seconds = 0

    #url = "http://%s/api/v1/orders/?%s.gte=%s&%s.lte=%s" % ( bb_address, ORDER_ORDERFINISH, start_date_time_str, ORDER_ORDERFINISH, end_date_time_str )
    url = "http://%s/api/v1/orders/" % ( bb_address )
    debug_file.write("%s\n" % (url))
    req = requests.get(url, timeout=(1, 30))

    if req.status_code != 200:
        print("Data query: %s" % url)
        print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
        return -1

    orders = req.json()

    debug_file.write("Number of orders in query: %i\n" % (len(orders)))

    for order in orders:
        if use_order(order) == False:
            # Write order to the debug file
            write_order_debug(order, 'NOT')
        else:
            try:
                start_order = None
                end_order = None
                filled_order = False

                order_num = order[ORDER_ORDERNUMBER]
                if order_num is None or order_num <= 0:
                    # Write order to the debug file
                    write_order_debug(order, 'NOT')
                    continue

                # Write order to the debug file
                write_order_debug(order, '')

                order_number_list.append(order_num)

                if order[ORDER_ITEMCOUNT] is not None:
                    total_items += order[ORDER_ITEMCOUNT]
                else:
                    debug_file.write("\nNone or blank in " + ORDER_ITEMCOUNT)

                offset = order[ORDER_ORDERSTATUS]
                if offset == STATUS_READY:
                    filled_order = True
                if offset is not None and offset <= STATUS_MAX:
                    arr_status[offset] += 1
                else:
                    debug_file.write("\nNone or invalid " + ORDER_ORDERSTATUS)

                origin = order[ORDER_ORDERORIGIN]
                if origin is not None and origin <= ORIGIN_MAX:
                    arr_origin[origin] += 1
                else:
                    debug_file.write("\nNone or invalid %s " %(ORDER_ORDERORIGIN))

                order_started = order[ORDER_ORDERSTART]
                order_finished = order[ORDER_ORDERFINISH]

                if filled_order:
                    filled_order_list.append(order_num)

                    if order_started != None and order_started[:4] != '0000' and order_finished != None and order_finished[:4] != '0000' and order_finished > order_started:
                        filled_and_timed_order_count += 1
                        try:
                            end = datetime.datetime.strptime(order_finished, datetime_format)
                            start = datetime.datetime.strptime(order_started, datetime_format)
                            time_diff = end - start
                            total_time_filling += time_diff.seconds
                        except:
                            debug_file.write("\nEND time check found invalid time for order %s" % str(order_num))
                    else:
                        debug_file.write("\nNone or invalid %s or %s" % (ORDER_ORDERSTART, ORDER_ORDERFINISH))

                if origin == 3:  # Take-A-Number
                    # Wait time for a Take-A-Number is the time from when the number is printed to when the customer
                    # is starting to be waited on, at which time the OrderStart will be set when the first item is filled.
                    # OrderCreation is automatically set when order is created in the database (as of 4/12/22-ish).
                    wait_time_seconds = -1
                    if order[ORDER_ORDERCREATION] is not None and order_started is not None:
                        created = datetime.datetime.strptime(order[ORDER_ORDERCREATION], datetime_format)
                        started = datetime.datetime.strptime(order_started, datetime_format)
                        if started > created:
                            TAN_wait_time_order_count += 1
                            TAN_total_wait_time += (started - created).total_seconds()
                        else:
                            TAN_undetermined_wait_time_order_count += 1
                    else:
                        TAN_undetermined_wait_time_order_count += 1

                elif origin == 1 or origin == 2:  # Online or Kiosk only
                    # There is no "wait time" for Online or Kiosk orders, since they can be created at any time,
                    # but not be filled until close to the time the customer wanted to pick them up.
                    # Late orders are Online or Kiosk orders that weren't available for the customer to pickup by their
                    # desired pickup time.  Early orders are available before their pickup time.  Both are determined by
                    # the difference in OrderFinish and OrderPickup; late when OrderFinish is later than OrderPickup.
                    if order[ORDER_ORDERPICKUP] is not None and order_finished is not None:
                        pickup = datetime.datetime.strptime(order[ORDER_ORDERPICKUP], datetime_format)
                        finished = datetime.datetime.strptime(order_finished, datetime_format)
                        if finished > pickup:
                            OK_late_order_count += 1
                            OK_total_late_time_seconds += (finished - pickup).total_seconds()
                        else:
                            OK_early_order_count += 1
                            OK_total_early_time_seconds += (pickup - finished).total_seconds()

            except:
                debug_file.write("\nInvalid JSON data for order: " + str(order_num))
                order_number_list.pop(-1)

    debug_file.write("\n-------------- END ORDERS ------------------\n")

    details_url = "http://%s/api/v1/orderdetails/" % (bb_address)
    details_req = requests.get(details_url, timeout=(1, 30))

    if details_req.status_code != 200:
        print("Data query: %s" % details_url)
        print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
        return -1

    details = details_req.json()

    # debug_file.write("\n-------------- PROCESS DETAILS ------------------\n")

    order_items_count = 0

    for detail in details:
        try:
            # debug_file.write("OrderDetail: order num:" + str(detail[DETAIL_ORDERNUMBER]) + "  plu:" + str(detail[DETAIL_PLUNUMBER]) + "\n")  # isinstance(o, int)

            o_num = detail[DETAIL_ORDERNUMBER]
            if o_num is None or o_num <= 0 or o_num not in order_number_list:
                continue

            # Write detail to the debug file
            write_detail_debug(detail)

            order_items_count += 1

            if o_num in filled_order_list:
                total_price += detail[DETAIL_FINALPRICE]

            if o_num in filled_order_list:
                total_weight += detail[DETAIL_FINALWEIGHT]

            offset = detail[DETAIL_PRODUCTTYPE]
            if offset is not None and offset <= PLU_MAX:
                arr_plutype[offset] += 1
            else:
                debug_file.write("\nNone or invalid %s " % (DETAIL_PRODUCTTYPE))

        except:
            debug_file.write("\nInvalid JSON orderDetail data for order: " + str(order_num))

    # start building up the list of statistical data we want to export

    # Start Date
    stats_data.append( start_date_time_str )

    # End Date
    stats_data.append( end_date_time_str )

    # Number of orders
    stats_data.append( len(order_number_list) )

    # Number of items
    stats_data.append(total_items)

    # Total filled/completed orders - note that this may be different than the value in filled_and_timed_order_count
    stats_data.append(arr_status[STATUS_READY])

    # Total time filling orders
    stats_data.append(total_time_filling)

    # Average time filling orders
    if filled_and_timed_order_count == 0:
        stats_data.append( 0 )
    else:
        stats_data.append( int(total_time_filling / filled_and_timed_order_count) )

    # Total Price - sum of FinalPrice for all items in filled orders
    stats_data.append( total_price )

    # Online orders
    stats_data.append(arr_origin[ORIGIN_ONLINE])

    # Kiosk orders
    stats_data.append(arr_origin[ORIGIN_KIOSK])

    # Take-A-Number orders
    stats_data.append(arr_origin[ORIGIN_NUMBER])

    # Status waiting
    stats_data.append(arr_status[STATUS_WAITING])

    # Status in progress
    stats_data.append(arr_status[STATUS_IN_PROGRESS])

    # Status ready (completed)
    stats_data.append(arr_status[STATUS_READY])

    # Status delivered
    stats_data.append(arr_status[STATUS_DELIVERED])

    # Total cancelled orders
    stats_data.append(arr_status[STATUS_CANCELLED])

    # Random Weight items
    stats_data.append(arr_plutype[PLU_TYPE_RANDOM])

    # By Count items
    stats_data.append(arr_plutype[PLU_TYPE_BYCOUNT])

    # Fixed Weight items
    stats_data.append(arr_plutype[PLU_TYPE_FIXED])

    if len(filled_order_list) == 0:
        # Average total price per order - order must be filled/complete
        stats_data.append( 0 )

        # Average weight per order - order must be filled/complete
        stats_data.append( 0 )
    else:
        # Average total price per order - order must be filled/complete
        stats_data.append( ( float(total_price) / len(filled_order_list) ) / PRICE_DIVISOR )

        # Average weight per order - order must be filled/complete
        stats_data.append( ( float(total_weight) / len(filled_order_list) ) / WEIGHT_DIVISOR )

    # Average items per order
    if len(order_number_list) == 0:
        stats_data.append( 0 )
    else:
        stats_data.append( float(order_items_count) / len(order_number_list) )

    # Take-A-Number 0rder wait time values ("Number of orders couldn't determine wait time", "Number of orders wait time determined", "Average wait time")
    stats_data.append( TAN_undetermined_wait_time_order_count )
    stats_data.append( TAN_wait_time_order_count )
    if (TAN_total_wait_time > 0):
        stats_data.append( "%6.2f" % ( TAN_total_wait_time / TAN_wait_time_order_count / 60.0 ) )
    else:
        stats_data.append( "No Take-A-Number orders" )

    # Late order values ("Number of late orders", "Average time late")
    stats_data.append( OK_late_order_count )
    if (OK_late_order_count > 0):
        stats_data.append( "%6.2f" % ( OK_total_late_time_seconds / OK_late_order_count / 60.0 ) )
    else:
        stats_data.append( "No late orders" )

    # Early order values ("Number of early orders", "Average time early")
    stats_data.append(OK_early_order_count)
    if (OK_early_order_count > 0):
        stats_data.append( "%6.2f" % ( OK_total_early_time_seconds / OK_early_order_count / 60.0 ) )
    else:
        stats_data.append("No early orders")

    curr_time = localtime()
    curr_date = datetime.date.today()
    #curr_str = datetime.strftime(datetime_format)
    output_file = dir_path + "oo_statistics_summary_%s_%s-%s-%s.csv" % ( curr_date, str(curr_time.tm_hour), str(curr_time.tm_min), str(curr_time.tm_sec) )
    # f = open(output_file, 'w')

    header = [REPORT_BEGIN, REPORT_END,
                    "Number of orders", "Number of items", "Number of filled/completed orders",
                    "Total time filling orders", "Average filling time per order",
                    "Total price of filled items", "Online orders", "Kiosk orders", "Take-A-Number orders",
                    "Number of orders - Waiting", "Number of orders - In-Progress", "Number of orders - Filled/Completed",
                    "Number of orders - Delivered", "Number of orders - Cancelled", "Random Weight items",
                    "By Count items", "Fixed Weight items",
                    "Average total price per order", "Average weight per order", "Average items per order",
                    "Number of orders couldn't determine wait time", "Number of orders wait time determined", "Average wait time (minutes)",
                    "Number of late orders", "Average time late (minutes)", "Number of early orders", "Average time early (minutes)"]

    try:
        # with open(output_file, 'w', encoding="UTF8") as f:
        with open(output_file, 'w') as f:
            writer = csv.writer(f, delimiter=',')
            row_num = 0
            for x in header:
                #utf8_text = str((x, stats_data[row_num])).encode("UTF8")
                writer.writerow( (x, stats_data[row_num]) )
                row_num += 1
    except Exception as e:
        eee = str(e)
        print(eee)

    debug_file.write("\n-------------- END STATISTICS ------------------\n")


#///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
def run_order_details():
    debug_file.write("\n------------- Order Details Report -------------------\n")

    header = ["Order Number", "Dept Number", "Zone Number", "Plu Number", "Description", "Product Type",
              "Instructions", "Cancelled?", "Reason Cancelled",
              "Allow Substitutions?", "Substituted?", "Original Plu Number", "Original Plu Price Variance", "Filled?",
              "By Count", "Final Count", "Requested Weight", "Final Weight", "Total Weight Variance",
              " Unit Price", "Final Price", "Total Price Variance"]

    prod_types = ['', 'Random Weight', 'Fixed Weight', 'By Count', 'Fluid Ounce']

    try:
        url = "http://%s/api/v1/orders/" % (bb_address)
        req = requests.get(url, timeout=(1, 30))

        if req.status_code != 200:
            print("Data query: %s" % url)
            print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
            return -1

        orders = req.json()
        order_number_list = []
        for order in orders:
            if use_order(order) == False:
                # Write order to the debug file
                write_order_debug(order, 'NOT')
            else:
                # Write order to the debug file
                write_order_debug(order, '')

                if order[ORDER_ORDERNUMBER] is not None and order[ORDER_ORDERNUMBER] > 0:
                    order_number_list.append( order[ORDER_ORDERNUMBER] )

        # now we will open a file for writing
        curr_time = localtime()
        curr_date = datetime.date.today()
        output_file = dir_path + "oo_orderdetails_%s_%s-%s-%s.csv" % ( curr_date, str(curr_time.tm_hour), str(curr_time.tm_min), str(curr_time.tm_sec) )
        with open(output_file, 'w') as data_file:

            # create the csv writer object
            csv_writer = csv.writer(data_file)

            csv_writer.writerow((REPORT_BEGIN, start_date_time_str))
            csv_writer.writerow((REPORT_END, end_date_time_str))

            # Writing headers of CSV file
            csv_writer.writerow(header)

            for order_num in order_number_list:

                details_url = "http://%s/api/v1/orderdetails/?%s.eq=%s" % (bb_address, DETAIL_ORDERNUMBER, str(order_num))
                details_req = requests.get(details_url, timeout=(1, 30))

                if req.status_code != 200:
                    print("Data query: %s" % details_url)
                    print("Could not retrieve order details data from Online Order server: status=%s" % req.status_code)
                    sys.exit(-1)

                details = details_req.json()
                for detail in details:
                    # Write detail to the debug file
                    write_detail_debug(detail)

                    # Writing data of CSV file
                    price_variance = 0.0
                    weight_variance = 0.0
                    ptype = prod_types[0]
                    original_item_price_variance = 0.0
                    requestedweight_str = ''
                    finalweight_str = ''
                    weightvariance_str = ''
                    unitprice_str = ''
                    finalprice_str = ''
                    pricevariance_str = ''
                    original_item_price_variance_str = ''

                    if detail[DETAIL_SUBSTITUTED] is not None and detail[DETAIL_SUBSTITUTED] != 0 and \
                        detail[DETAIL_DEPTNUM] is not None and \
                        detail[DETAIL_ORIGINALPLUNUMBER] is not None and detail[DETAIL_ORIGINALPLUNUMBER] != 0:
                        # Substituted, so determine difference in price if original item was used
                        original_item_url = "http://%s/api/v1/products/%s/%s" % (bb_address, str(detail[PRODUCT_DEPTNUM]), str(detail[DETAIL_ORIGINALPLUNUMBER]))
                        original_item_req = requests.get(original_item_url, timeout=(1, 30))

                        if original_item_req.status_code == 200:
                            items = original_item_req.json()
                            for item in items:
                                if item[PRODUCT_PRTYPE] == 1:  #random weight
                                    if detail[DETAIL_FINALPRICE] is not None and item[PRODUCT_PRPRICE] is not None and detail[DETAIL_PLUWEIGHT] is not None:
                                        original_item_price_variance = detail[DETAIL_FINALPRICE] - (item[PRODUCT_PRPRICE] * detail[DETAIL_PLUWEIGHT])
                                        original_item_price_variance_str = str("{0:1.3f}".format((original_item_price_variance/PRICE_DIVISOR)))
                                else:
                                    if detail[DETAIL_FINALPRICE] is not None and item[PRODUCT_PRPRICE] is not None:
                                        original_item_price_variance = detail[DETAIL_FINALPRICE] - item[PRODUCT_PRPRICE]
                                        original_item_price_variance_str = str("{0:1.3f}".format((original_item_price_variance/PRICE_DIVISOR)))
                                break  #just need to get the first product record, since there should only be one anyway

                    if detail[DETAIL_PRODUCTTYPE] is not None and detail[DETAIL_PRODUCTTYPE] < 5:
                        ptype = prod_types[detail[DETAIL_PRODUCTTYPE]]
                        if detail[DETAIL_PRODUCTTYPE] == 1:  #random weight
                            # need to check if either of these is None  (instance found where finalprice was none)
                            if detail[DETAIL_FINALPRICE] is not None and detail[DETAIL_PLUPRICE] is not None and detail[DETAIL_PLUWEIGHT] is not None:
                                price_variance = detail[DETAIL_FINALPRICE] - (detail[DETAIL_PLUPRICE] * detail[DETAIL_PLUWEIGHT])
                                pricevariance_str = str("{0:1.3f}".format((price_variance/PRICE_DIVISOR)))
                            if detail[DETAIL_FINALWEIGHT] is not None and detail[DETAIL_PLUWEIGHT] is not None:
                                weight_variance = detail[DETAIL_FINALWEIGHT] - detail[DETAIL_PLUWEIGHT]
                                weightvariance_str = str("{0:1.3f}".format((weight_variance/WEIGHT_DIVISOR)))
                        else:
                            # need to check if either of these is None  (instance found where finalprice was none)
                            if detail[DETAIL_FINALPRICE] is not None and detail[DETAIL_PLUPRICE] is not None:
                                price_variance = detail[DETAIL_FINALPRICE] - detail[DETAIL_PLUPRICE]
                                pricevariance_str = str("{0:1.3f}".format((price_variance/PRICE_DIVISOR)))

                    if detail[DETAIL_PLUWEIGHT] is not None:
                        requestedweight_str = str("{0:1.3f}".format((float(detail[DETAIL_PLUWEIGHT])/WEIGHT_DIVISOR)))
                    if detail[DETAIL_FINALWEIGHT] is not None:
                        finalweight_str = str("{0:1.3f}".format((detail[DETAIL_FINALWEIGHT]/WEIGHT_DIVISOR)))
                    if detail[DETAIL_PLUPRICE] is not None:
                        unitprice_str = str("{0:1.3f}".format((detail[DETAIL_PLUPRICE]/PRICE_DIVISOR)))
                    if detail[DETAIL_FINALPRICE] is not None:
                        finalprice_str = str("{0:1.3f}".format((detail[DETAIL_FINALPRICE]/PRICE_DIVISOR)))

                    csv_writer.writerow(
                        [detail[DETAIL_ORDERNUMBER], detail[DETAIL_DEPTNUM], detail[DETAIL_ZONENUM], detail[DETAIL_PLUNUMBER], detail[DETAIL_PLUDESCRIPTION][:20], ptype,
                         detail[DETAIL_PLUINSTRUCTIONS], detail[DETAIL_CANCELLED], detail[DETAIL_REASONCANCELLED],
                         detail[DETAIL_ALLOWSUBSTITUTIONS], detail[DETAIL_SUBSTITUTED], detail[DETAIL_ORIGINALPLUNUMBER], original_item_price_variance_str, detail[DETAIL_FILLED],
                         detail[DETAIL_PLUBYCOUNT], detail[DETAIL_FINALCOUNT], requestedweight_str, finalweight_str, weightvariance_str,
                         unitprice_str, finalprice_str, pricevariance_str])

    except Exception as e:
        eee = str(e)
        print(eee)

#///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
def run_orders():
    debug_file.write("\n------------- Orders Report -------------------\n")

    try:
        url = "http://%s/api/v1/orders?sort=%s" % (bb_address, ORDER_ORDERNUMBER)
        debug_file.write("%s\n" % (url))

        req = requests.get(url, timeout=(1, 30))

        if req.status_code != 200:
            print("Data query: %s" % url)
            print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
            return -1

        orders = req.json()

        debug_file.write("Number of orders in query: %i\n" % (len(orders)))

        # now we will open a file for writing
        curr_time = localtime()
        curr_date = datetime.date.today()
        output_file = dir_path + "oo_orders_%s_%s-%s-%s.csv" % ( curr_date, str(curr_time.tm_hour), str(curr_time.tm_min), str(curr_time.tm_sec) )
        with open(output_file, 'w') as data_file:

            # create the csv writer object
            csv_writer = csv.writer(data_file)

            csv_writer.writerow((REPORT_BEGIN, start_date_time_str))
            csv_writer.writerow((REPORT_END, end_date_time_str))

            # Writing headers of CSV file
            header = ["Order Number", "Instructions", "Status", "Customer Name", "SMS", "Origin", "Store Number",
                      "Date/Time Created", "Date/Time Requested Pickup", "Start date/time", "Finish date/time", "Wait time (minutes)",
                      "Operators", "Item Count", "Allow Substitutions?", "Number of Items Substituted"]
            csv_writer.writerow(header)

            for order in orders:
                if use_order(order) == False:
                    # Write order to the debug file
                    write_order_debug(order, 'NOT')
                else:
                    # Write order to the debug file
                    write_order_debug(order, '')

                    # Writing data of CSV file
                    status = ORDER_STATUS_LIST[0]
                    if order[ORDER_ORDERSTATUS] is not None and order[ORDER_ORDERSTATUS] < 6:
                        status = ORDER_STATUS_LIST[order[ORDER_ORDERSTATUS]]

                    origin_text = ORDER_ORIGIN_LIST[0]
                    if order[ORDER_ORDERORIGIN] is not None and order[ORDER_ORDERORIGIN] < 4:
                        origin = order[ORDER_ORDERORIGIN]
                        origin_text = ORDER_ORIGIN_LIST[origin]

                    wait_time_seconds = -1
                    if (origin == 3):  # Take-A-Number
                        if order[ORDER_ORDERCREATION] is not None and order[ORDER_ORDERSTART] is not None:
                            created = datetime.datetime.strptime(order[ORDER_ORDERCREATION], datetime_format)
                            started = datetime.datetime.strptime(order[ORDER_ORDERSTART], datetime_format)
                            wait_time_seconds = (started - created).total_seconds()
                    elif (origin > 0):  #Online or Kiosk
                        if order[ORDER_ORDERCREATION] is not None and order[ORDER_ORDERFINISH] is not None:
                            created = datetime.datetime.strptime(order[ORDER_ORDERCREATION], datetime_format)
                            finished = datetime.datetime.strptime(order[ORDER_ORDERFINISH], datetime_format)
                            wait_time_seconds = (finished - created).total_seconds()
                    wait_time = " "
                    if (wait_time_seconds > -1):
                        wait_time = "%6.2f" % ( wait_time_seconds / 60.0 )

                    onum = order[ORDER_ORDERNUMBER]
                    details_url = "http://%s/api/v1/orderdetails/?%s.eq=%s" % (bb_address, DETAIL_ORDERNUMBER, str(onum))
                    details_req = requests.get(details_url, timeout=(1, 30))

                    if req.status_code != 200:
                        print("Data query: %s" % details_url)
                        print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
                        sys.exit(-1)

                    details = details_req.json()
                    item_count = len(details)

                    details_url = "http://%s/api/v1/orderdetails/?%s.eq=%s&%s=1" % (bb_address, DETAIL_ORDERNUMBER, str(onum), DETAIL_SUBSTITUTED)
                    details_req = requests.get(details_url, timeout=(1, 30))

                    if req.status_code != 200:
                        print("Data query: %s" % details_url)
                        print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
                        sys.exit(-1)

                    details = details_req.json()
                    substituted_count = len(details)

                    debug_file.write("\nOrder: %s\n" % (order[ORDER_ORDERNUMBER]))

                    csv_writer.writerow([ order[ORDER_ORDERNUMBER], order[ORDER_ORDERINSTRUCTIONS], status, order[ORDER_CUSTOMERNAME],
                                          order[ORDER_ORDERSMS], origin_text, order[ORDER_STORENUMBER], order[ORDER_ORDERCREATION], order[ORDER_ORDERPICKUP],
                                          order[ORDER_ORDERSTART], order[ORDER_ORDERFINISH], wait_time, order[ORDER_OPERATORS],
                                          item_count, order[ORDER_ALLOWSUBSTITUTIONS], substituted_count ])

        debug_file.write("\n------------- Orders Report Finished -------------------\n")

    except Exception as e:
        eee = str(e)
        print(eee)

#///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
def run_hourly_orders():
    debug_file.write("\n------------- Hourly Report -------------------\n")

    # We are only dealing with completed orders, those with OrderFinish not None!

    arr_order_count    = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    arr_total_packages = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    arr_total_price    = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    arr_total_weight   = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    try:
        url = "http://%s/api/v1/orders/" % (bb_address)
        req = requests.get(url, timeout=(1, 30))

        if req.status_code != 200:
            print("Data query: %s" % url)
            print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
            return -1

        orders = req.json()

        for order in orders:
            if use_order(order) == False:
                # Write order to the debug file
                write_order_debug(order, 'NOT')
            else:
                if order[ORDER_ORDERFINISH] is None:
                    # Write order to the debug file
                    write_order_debug(order, 'NOT')
                else:
                    finished = datetime.datetime.strptime(order[ORDER_ORDERFINISH], datetime_format)
                    hour = finished.hour

                    if hour >= 0:
                        # Write order to the debug file
                        write_order_debug(order, '')

                        onum = order[ORDER_ORDERNUMBER]
                        details_url = "http://%s/api/v1/orderdetails/?%s.eq=%s" % (bb_address, DETAIL_ORDERNUMBER, str(onum))
                        details_req = requests.get(details_url, timeout=(1, 30))

                        if req.status_code != 200:
                            print("Data query: %s" % details_url)
                            print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
                            continue

                        arr_order_count[hour] += 1

                        details = details_req.json()

                        arr_total_packages[hour] += len(details)

                        for detail in details:
                            # Write detail to the debug file
                            write_detail_debug(detail)

                            if detail[DETAIL_FINALPRICE] is not None:
                                arr_total_price[hour] += detail[DETAIL_FINALPRICE]

                            # Don't have to check if the item is random weight to add in the final weight,
                            # chobart_d only sends the weight for random weight and by count with weight items,
                            # so just add in the final weight
                            if detail[DETAIL_FINALWEIGHT] is not None:
                                arr_total_weight[hour] += detail[DETAIL_FINALWEIGHT]

        # now we will open a file for writing
        curr_time = localtime()
        curr_date = datetime.date.today()
        output_file = dir_path + "oo_hourly_%s_%s-%s-%s.csv" % (curr_date, str(curr_time.tm_hour), str(curr_time.tm_min), str(curr_time.tm_sec))

        with open(output_file, 'w') as data_file:
            # create the csv writer object
            csv_writer = csv.writer(data_file)

            csv_writer.writerow((REPORT_BEGIN, start_date_time_str))
            csv_writer.writerow((REPORT_END, end_date_time_str))

            # Writing headers of CSV file
            header = ["Hour Number", "Number of completed orders", "Number of items", "Total price", "Total Weight"]
            csv_writer.writerow(header)
			
            for h in range (0, 24):
                csv_writer.writerow([ h, arr_order_count[h], arr_total_packages[h], arr_total_price[h], arr_total_weight[h] ])

    except Exception as e:
        eee = str(e)
        print(eee)

#///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
def run_daily_orders():
    debug_file.write("\n------------- Daily Report -------------------\n")

    # We are only dealing with completed orders, those with OrderFinish not None!

    arr_date = []
    arr_order_count = []
    arr_total_packages = []
    arr_total_price = []
    arr_total_weight = []

    try:
        url = "http://%s/api/v1/orders/" % (bb_address)
        req = requests.get(url, timeout=(1, 30))

        if req.status_code != 200:
            print("Data query: %s" % url)
            print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
            return -1

        orders = req.json()

        for order in orders:
            if use_order(order) == False:
                # Write order to the debug file
                write_order_debug(order, 'NOT')
            else:
                # Write order to the debug file
                write_order_debug(order, '')

                if order[ORDER_ORDERFINISH] is not None:
                    finished = datetime.datetime.strptime(order[ORDER_ORDERFINISH], datetime_format)
                    finished_date = finished.date().strftime('%Y/%m/%d')
                    if finished_date not in arr_date:
                        arr_date.append(finished_date)
                        arr_order_count.append(0)
                        arr_total_packages.append(0)
                        arr_total_price.append(0)
                        arr_total_weight.append(0)

                    if finished_date in arr_date:
                        position = arr_date.index(finished_date)

                        onum = order[ORDER_ORDERNUMBER]
                        details_url = "http://%s/api/v1/orderdetails/?%s.eq=%s" % (bb_address, DETAIL_ORDERNUMBER, str(onum))
                        details_req = requests.get(details_url, timeout=(1, 30))

                        if req.status_code != 200:
                            print("Data query: %s" % details_url)
                            print("Could not retrieve data from Online Order server: status=%s" % req.status_code)
                            continue

                        arr_order_count[position] += 1

                        details = details_req.json()

                        arr_total_packages[position] += len(details)

                        for detail in details:
                            # Write detail to the debug file
                            write_detail_debug(detail)

                            if detail[DETAIL_FINALPRICE] is not None:
                                arr_total_price[position] += detail[DETAIL_FINALPRICE]

                            # Don't have to check if the item is random weight to add in the final weight,
                            # chobart_d only sends the weight for random weight and by count with weight items,
                            # so just add in the final weight
                            if detail[DETAIL_FINALWEIGHT] is not None:
                                arr_total_weight[position] += detail[DETAIL_FINALWEIGHT]

        # now we will open a file for writing
        curr_time = localtime()
        curr_date = datetime.date.today()
        output_file = dir_path + "oo_daily_%s_%s-%s-%s.csv" % (curr_date, str(curr_time.tm_hour), str(curr_time.tm_min), str(curr_time.tm_sec))

        with open(output_file, 'w') as data_file:
            # create the csv writer object
            csv_writer = csv.writer(data_file)

            csv_writer.writerow((REPORT_BEGIN, start_date_time_str))
            csv_writer.writerow((REPORT_END, end_date_time_str))

            # Writing headers of CSV file
            header = ["Day", "Number of completed orders", "Number of items", "Total price", "Total Weight"]
            csv_writer.writerow(header)
			
            for i in range (0, len(arr_date)):
                csv_writer.writerow([ arr_date[i], arr_order_count[i], arr_total_packages[i], arr_total_price[i], arr_total_weight[i] ])

    except Exception as e:
        eee = str(e)
        print(eee)


# converts a price like 1299 into "$12.99"
def StringToPrice(price):
    price.toString().slice[-2]

    if price:
        new_price = price / 100
        str_price = str(new_price)
        end_price = str_price[-2]
        return str_price + end_price
    else:
        return 0.00


# Parameters:  Online Ordering server address (on blackbox), directory for data output, which report to run,
#              data start date/time (order creation), data end date/time (order creation)
if __name__ == "__main__":
    try:
        bb_address = sys.argv[1]
        dir_path = ''

        if (len(sys.argv) > 3):
            dir_path = sys.argv[2]
            if dir_path[-1] != '/':
                dir_path += '/'
            which_report = sys.argv[3]

            if (len(sys.argv) > 4):
                start_date_time_str = sys.argv[4]
                start_date_time = datetime.datetime.strptime(start_date_time_str, datetime_format)

            if (len(sys.argv) > 5):
                end_date_time_str = sys.argv[5]
                end_date_time = datetime.datetime.strptime(end_date_time_str, datetime_format)

            debug_file.write("\n------------- oo_export_statistics_script -------------------\n")
            debug_file.write("\n%s\n%s\n%s\n%s\n%s\n" % (bb_address, dir_path, which_report, start_date_time_str, end_date_time_str))

            if ('hourly' in which_report.lower()):
                run_hourly_orders()
            elif ('daily' in which_report.lower()):
                run_daily_orders()
            elif ( 'detail' in which_report.lower() ):
                run_order_details()
            elif ('summary' in which_report.lower()):
                # run_summary()
                run_summary()
            elif ( 'order' in which_report.lower() ):
                run_orders()

        debug_file.close()

    except ConnectionError:
        print("failure")
        sys.exit(-1)
    except Timeout:
        print("timeout failure")
        sys.exit(-1)
    except:
        print("some other exception failure")
        sys.exit(-1)

    print("success")
    sys.exit(0)