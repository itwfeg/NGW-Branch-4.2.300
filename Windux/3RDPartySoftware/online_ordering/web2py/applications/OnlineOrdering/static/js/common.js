var orderStatus = ["Received", "Waiting", "In progress", "Ready", "Delivered", "Cancelled", "Rejected", "Update error"];
var orderType = ["Unknown", "Online", "Kiosk", "Number"];

var orderStatusVal = { "Received":0, "Waiting":1, "In progress":2, "Ready":3, "Delivered":4, "Cancelled":5, "Rejected":6, "Update error":7 };
var orderTypeVal = { "Unknown":0, "Online":1, "Kiosk":2, "Number":3 };

var runningOnScale = (!(typeof BrowserView === 'undefined'));

// DiscountMethod enum from FrequentShopper.h
var fsMethod = Object.freeze({
  RII_DISCOUNT_METHOD: 0,
  IBM_DISCOUNT_METHOD: 1,
  IBM_EM2_DISCOUNT_METHOD: 2,
  PERCENT_DISCOUNT_METHOD: 3,
  NCR_DISCOUNT_METHOD: 4,
  SPECIAL_SAVINGS: 5,
  PERCENT_ADDED_METHOD: 6,
  ICL_SASI_DISCOUNT_METHOD: 7,
  IBM_CENTS_OFF_DISCOUNT_METHOD: 8,
  FLEX_DISCOUNT_METHOD_PER_SCALE: 9,
  FLEX_DISCOUNT_METHOD_PER_PLU: 10
});

var fsType = Object.freeze({
    Default_dept_type: 0,
    Cents_off_unit_price: 1,
    Discount_price_by_weight: 2,
    Percent_off_net_value: 3,
    Special_price:4 ,
    Percent_added_net_value: 5,
    Discount_per_package: 6,
});

var fsFormula = Object.freeze({
    RII_FS_FORMULA: 0,
    IBM_FS_FORMULA: 1,
    IBM_EM2_FS_FORMULA: 2,
    PERCENT_OFF_FS_FORMULA: 3,
    NCR_FS_FORMULA: 4,
    SPECIAL_SAVINGS_FORMULA: 5,
    PERCENT_ADDED_FS_FORMULA: 6,
    ICL_SASI_FS_FORMULA: 7,
    PACKAGE_DISCOUNT_FS_FORMULA: 8,
    IBM_ACE_DISCOUNT_FS_FORMULA: 9,
});

function getDept()
{
  var dept = 0;
  
  // if we have dept try that
  var deptParam = getParams("DeptNum");
  if (deptParam != "") dept = deptParam;
  
  // if we're on a scale, get the configured dept instead
  if (runningOnScale)
  {
    dept = BrowserView.getDept();
  }
  
  if ($("#scaleLocation").length != 0)
  {
    $("#scaleLocation-dept").text("D:"+dept);
  }
  
  return dept;
}

function getZone()
{
  var zone = 0;
  
  // if we have zone try that
  var zoneParam = getParams("ZoneNum");
  if (zoneParam != "") zone = zoneParam;
  
  // if we're on a scale, get the configured zone instead
  if (runningOnScale)
  {
    zone = BrowserView.getZone();
  }
  
  if ($("#scaleLocation").length != 0)
  {
    $("#scaleLocation-zone").text("Z:"+zone);
  }
  
  return zone;
}

function runningOnCenterPanel()
{
  return window.innerWidth < 920;
}

$('#operatorModal').on('keypress', function (event) {
  /* activate on enter */
  if((event.keyCode ? event.keyCode : event.which) == '13'){
    $('#operatorModal-button').mouseup();
    return false;
  }
  return true;
});

function failed(s)
{
    $('#failureModal-body').text(s);
    $('#failureModal').modal('show');
}

function getParams(paramName)
{
   var pageURL = window.location.search.substring(1);
   var orderParams = pageURL.split('&');

   for (var i = 0; i < orderParams.length; i++)
   {
       var paramId = orderParams[i].split('=');
       if( paramId[0] == paramName )
       {
          return paramId[1].replace(/\+/g, ' ');
       }
   }
   return '';     // failsafe
}

function getBlackBox()
{
   // either get BB from a parameter or use the current host
   var response = getParams('black_box');
   
   if (response == '')
   {
       response = window.location.host;
   }
   
   return response;
}

function selectRow(element)
{
  var row = $('html').find(".selectedRow");
  
  $(row).removeClass("selectedRow");
  
  // return if we have clicked an already selected row
  if (row.is(element))
  {
    return;
  }
  
  // don't select empty rows
  if (element == null || $(element).first().html() == "")
  {
    return;
  }
  
  $(element).addClass("selectedRow");
}

function unselectRow()
{
  var row = $('html').find(".selectedRow");
  
  if (row.length > 0)
  {
    $(row).removeClass("selectedRow");
  }
}

function niceTime(timestamp)
{
    if (timestamp == null) return "";
    
    var niceTime = "";
    var pickup_time = timestamp.split(" ")[1];
    var pickup_date = timestamp.split(" ")[0];
    
    var order_pickup = new Date(pickup_date.split("-")[0],
                                parseInt(pickup_date.split("-")[1])-1,
                                pickup_date.split("-")[2],
                                pickup_time.split(":")[0],
                                pickup_time.split(":")[1],
                                pickup_time.split(":")[2]);
    
    var pickup_hours = parseInt(pickup_time.split(":")[0]);
    var pickup_minutes = pickup_time.split(":")[1];
    var am_pm = pickup_hours < 12 ? "AM" : "PM";
    if (pickup_hours > 12) pickup_hours -= 12;
    
    var today = new Date();
    if ((today.toDateString() == order_pickup.toDateString()))
    {
      niceTime = pickup_hours + ":" + pickup_minutes + " " + am_pm;
    }
    else
    {
      niceTime = order_pickup.getMonth()+1 + "/" + order_pickup.getDate();
    }
    
    return niceTime;
}


function clearTableRows(cur_row, table_rows, row_data )
{
    // Enter here if we have filled up all of our rows, now see if there are any that need cleared or deleted
    // 1. blank out existing rows up to the number of allowable visible rows (11)
    //    if we dont have enough data to fill all of them
    // 2. delete rows past the allowable number of visible rows (11)
    //    if we don't have enough data to fill them

    // there are left over rows from the last query, so delete or clear
    for(var j = cur_row+1; j < table_rows; j++)
    {
        if (false)
        {
            // clear it
            dataRow = row_data[j];
            dataRow.id = "";
            $(dataRow).children('td').each(function() {$(this).html("");});
        }
        else
        {
            // or delete it
            var b = document.getElementById("orderData");
            b.removeChild(b.lastChild);
        }
    }
}

// sorts orders in the orderData table
// toggle direction upon second sort of a same column
// deliverytime sorts based on the actual full timestring, not the pretty string
// ordernumber sorts like a number, not like a string
function sortOrders(element, sortClass)
{
  unselectRow();
  
  var tbody = $('#orderData');
  
  $(".orderDataRowTitle").children().each(function(){
    $(this).removeClass("sortAsc");
    $(this).removeClass("sortDesc");
    $(this).addClass("sortNone");
  });
  
  if (sortClass == "prettyDeliveryTimeCol") sortClass = "deliveryTimeCol";
  
  // trick to save these between function calls
  sortOrders.lastClass = sortOrders.lastClass || null;
  sortOrders.direction = sortOrders.direction || 'desc';
  
  // flip direction if we last clicked on this same column
  if (sortOrders.lastClass && sortOrders.lastClass == sortClass)
  {
    sortOrders.direction = sortOrders.direction == 'desc' ? 'asc' : 'desc';
  }
  else
  {
    sortOrders.direction = 'desc';
  }
  
  $(element).removeClass("sortNone");
  $(element).addClass(sortOrders.direction == 'desc' ? "sortDesc" : "sortAsc");
  
  function numberCompare(a,b)
  {
    var x = parseInt($('.'+sortClass, a).text());
    var y = parseInt($('.'+sortClass, b).text());
    
    if (sortOrders.direction == 'desc')
      return x - y;
    else
      return y - x;
  }
  
  function stringCompare(a, b)
  {
    if (sortOrders.direction == 'desc')
    {
      return $('.'+sortClass, a).text().localeCompare($('.'+sortClass, b).text());
    }
    else
    {
      return $('.'+sortClass, b).text().localeCompare($('.'+sortClass, a).text());
    }
  }
  
  var sortFunction = (sortClass == "orderIDCol") ? numberCompare : stringCompare;
  
  tbody.find('tr').sort(sortFunction).appendTo(tbody);
  
  sortOrders.lastClass = sortClass;

  return false;
}

///////////////////////////////////////////////////////////////////////////
// navigate SECTION

function selectDetails(element)
{
    var row = $('html').find(".selectedRow");
    
    if( row.length == 0 && element.id == "detailsButton")
    {
        failed("No order is selected to view details!");
        return;
    }
    else if (element.id != "detailsButton")
    {
        unselectRow();
        selectRow(element);
        row = $('html').find(".selectedRow");
    }
    
    if (row.length == 0)
    {
        return;
    }

    var details_url = "./orderDetails.html?";
    if (getBlackBox())
    {
        details_url += "black_box=" + getBlackBox();
    }
    
    var order_num = $(row).find('.orderIDCol').text();
    details_url += "&order_num=" + order_num;
    
    details_url += "&ZoneNum=" + getZone();
    
    details_url += "&DeptNum=" + getDept();

    var order_name = $(row).find('.customerCol').text();
    details_url += '&order_name=' + order_name.replace(/ /g, '+');

    var order_due = $(row).find('.prettyDeliveryTimeCol').text();
    if (!(order_due)) order_due = niceTime($(row).find('.deliveryTimeCol').text());
    
    details_url += "&order_due=" + order_due.replace(/ /g, '+');
      

    window.location = details_url;
}

function cancelOrder(orderNumber, operator, permissions, back)
{
  $('#cancelModal').modal('hide');
  
  var order = {"OrderNumber": orderNumber, "DeptNum": getDept(), "PartStatus": orderStatusVal["Cancelled"]};
  
  if (operator)
  {
    order["OperatorNumber"] = operator;
  }
  
  if (permissions != null && parseInt(currentSettings.AccessLevelDelete) > parseInt(permissions))
  {
    failed("Insufficient permission level to cancel order!");
    return;
  }
  
  order["ReasonCancelled"] = $("#cancelModal-reason").val();
  
  $.ajax({
    url: "/api/v1/orderparts/"+orderNumber+"/"+getDept()+"/"+getZone(),
    type: 'PUT',
    data: JSON.stringify(order),
    contentType: "application/json"
  }).done(function() {
    if ((typeof selectSearchRefine === 'undefined'))
    {
      if ((typeof addItem === 'undefined')) updateRows();
    }
    else
    {
      selectSearchRefine();
    }
    
    if (back) selectBack();
  }).fail(function() {
    $(".orderDataDivContent").removeClass("grayOut");
    failed("Failed to cancel order!");
  });
}

///////////////////////////////////////////////////////////////////////////
// button actions SECTION

function selectSearch(element)
{
    window.location = "./orderSearch.html?black_box=" + getBlackBox();
    
    return false;
}

function selectRefresh(element)
{
     updateRows();
     return false;
}

function selectMain(element)
{
  if (runningOnScale)
  {
    BrowserView.swipeLeft();
  }
  
  return false;
}

function selectCancel(element)
{
    var orderNumber = 0;
    var customerName = "";
    var deliveryTime = "";
    var back = false;
    
    // read info either from the details page or a row in a table
    if ($("#orderNum").length)
    {
      orderNumber = parseInt($("#orderNum").text());
      customerName = $("#orderName").text();
      deliveryTime = $("#orderTime").text();
      back = true;
    }
    else
    {
      var row = $('html').find(".selectedRow");
    
      if( row.length == 0 )
      {
          failed("No order is selected to cancel!");
          return;
      }
      
      orderNumber = parseInt($(row).find(".orderIDCol").text());
      customerName = $(row).find(".customerCol").text();
      deliveryTime = $(row).find(".deliveryTimeCol").text();
    }
    
    // put data into the modal
    $("#cancelModal-orderNumber").val(orderNumber);
    $("#cancelModal-customer").val(customerName);
    $("#cancelModal-deliveryTime").val(deliveryTime);
    $("#cancelModal-reason").val("");
    
    if (currentSettings.OperatorLogging == 1)
    {
      $("#cancelModal-cancel").unbind('mouseup');
      $('#cancelModal-cancel').mouseup(function() {
        get_operator_and_run(function(a, b) { cancelOrder(orderNumber, a, b, back) }, "Keep Order", "Cancel Order");
      });
    }
    else
    {
      $("#cancelModal-cancel").unbind('mouseup');
      $('#cancelModal-cancel').mouseup(function() { cancelOrder(orderNumber, null, null, back); });
    }
    
    $('#cancelModal').modal('show');
    
    return false;
}

function selectHome(element)
{
  var params = "?black_box=" + getBlackBox() + "&DeptNum=" + getDept() + "&ZoneNum=" + getZone();
  if (getParams("prev") == "deligo")
  {
    window.location = "./deligo.html" + params;
  }
  else
  {
    window.location = "./orderList.html" + params;
  }
  
  return false;
}

function selectBack(element)
{
  if (getParams("prev") == "deligo")
  {
    window.location = "./deligo.html";
  }
  else
  {
    history.back();
    //window.location = "./orderList.html?black_box=" + getBlackBox();
  }
  
  return false;
}

function get_operator_and_run(action, continueText, stopText)
{
    var current_operator = null;
    var current_access_level = null;
    
    if (runningOnScale)
    {
        if (BrowserView.getIsOperatorLoggedIn())
        {
            current_operator = parseInt(BrowserView.getLoggedOperatorId());
            current_access_level = BrowserView.getLoggedOperatorAccess();
        }
    }
    
    if (current_operator === null)
    {
        $('#operatorModal-button').off("mouseup");
        $("#operatorModal-button").unbind('mouseup');
        $('#operatorModal-button').text(stopText);
        $('#operatorModal-cancel').text(continueText);
        
        $('#operatorModal-button').mouseup(function() {
            $('#operatorModal-button').addClass('disabled');
            $('#operatorModal-operator').prop('readonly', true);
            $('#operatorModal-bodytext').text("Checking permissions...");
            
            current_operator = parseInt($('#operatorModal-operator').val());
            
            $.getJSON("/api/v1/operators/" + current_operator, null, function(data, status, x) {
                $('#operatorModal').modal('hide');
                $('#operatorModal-button').removeClass('disabled');
                $('#operatorModal-operator').prop('readonly', false);
                $('#operatorModal-bodytext').text("Enter your operator ID");
                
                if (data.length < 1)
                {
                    $('#operatorModal').modal('hide');
                    failed("No such operator found!");
                    return;
                }
                current_access_level = parseInt(data[0].OpAccLev);
                
                action(current_operator, current_access_level);
                
            }).fail(function() {
                $('#operatorModal').modal('hide');
                $('#operatorModal-button').removeClass('disabled');
                $('#operatorModal-operator').prop('readonly', false);
                $('#operatorModal-bodytext').text("Enter your operator ID");
                failed("Operator not found!");
            });
        });
        
        $('#operatorModal-operator').val("");
        $('#operatorModal').modal('show');
    }
    else
    {
        action(current_operator, current_access_level);
    }
    
    return false;
}

/* converts a price like 1299 into "$12.99" */
function PriceToString(price)
{
  if (!price) return "$0.00";
  
  var string = "$" + Math.floor(price / 100) + ".";
  string += price.toString().slice(-2);
  
  // e.g. turn "$0.4" into "$0.40"
  if (string.split(".")[1].length < 2) string += "0";
  
  return string;
}

function PriceToStringUnits(price, type)
{
  var out = PriceToString(price);
  
  switch (type)
  {
    case 1:
      out += "/lb"
      break;
    case 4:
      out += "/oz"
      break;
    case 2:
    case 3:
      out += "/ea"
      break;
    default:
      break;
  }
  
  return out;
}

function reportFinal(totals, swipe)
{
  if (!runningOnCenterPanel() && swipe) BrowserView.swipeRight();
  
  totals["Filled"] = 1;
  
  $.ajax({
    url: "/api/v1/orderdetails",
    type: 'PUT',
    data: JSON.stringify(totals),
    contentType: "application/json"
  }).done(function() {
    
    if (!(typeof updateRows === 'undefined'))
    {
      if (!(typeof checkCompletion === 'undefined'))
      {
        updateRows(checkCompletion);
      }
      else
      {
        updateRows(null);
      }
    }
  }).fail(function() {failed("Failed to save filled item!")});
}

function isFrequentShopper(prod)
{ 
  switch (currentSettings.SalesOrFrequentShopper)
  {
    case 0:
      return false;
    case 1:
      return false;
    case 2:
      return true;
    default:
      return false;
  }
}

function frequentShopperPrice(prod, weight)
{
  var discountMethod;
  var discountFormula;
  var discountType;
  var price = 0;
  var fsSavings = 0;
  var fsTotalPrice = 0;
  var discountValue = prod.PrDscPr1;
  
  switch (currentSettings.FrequentShopperType)
  {
    case 0: // per PLU
      discountMethod = prod.PrDscMe;
      break;
    case 1: // global
      discountMethod = currentSettings.FrequentShopperDiscountFormula;
      break;
    default:
      console.log("unexpected FrequentShopperType setting!");
      break;
  }
  
  // since we're "estimating" cost don't do bother with all rounding types
  switch(discountMethod)
  {
    case fsMethod.PERCENT_DISCOUNT_METHOD:
      price = prod.PrPrice * (1 - (discountValue/10000.0));
      break;
      
    case fsMethod.IBM_CENTS_OFF_DISCOUNT_METHOD:
    case fsMethod.RII_DISCOUNT_METHOD:
      price = discountValue;
      break;
      
    case fsMethod.SPECIAL_SAVINGS: // aka advertised price
      price = discountValue;
      break;
      
    case fsMethod.PERCENT_ADDED_METHOD:
      price = prod.PrPrice * (1 + (discountValue/10000.0));
      break;

    case fsMethod.FLEX_DISCOUNT_METHOD_PER_PLU:
    case fsMethod.FLEX_DISCOUNT_METHOD_PER_SCALE:
      console.log("fs method flex discount per plu/scale? should not get here");

    default:
      console.log("unexpected discountMethod! defaulting to IBM_DISCOUNT_METHOD");
      
    case fsMethod.IBM_EM2_DISCOUNT_METHOD:
    case fsMethod.ICL_SASI_DISCOUNT_METHOD:
    case fsMethod.NCR_DISCOUNT_METHOD:
    case fsMethod.IBM_DISCOUNT_METHOD:
      price = discountValue/1000;
      break;
  }
  
  // always round up, for safety???
  return Math.ceil(price * weight / 1000);
}

function onSale(prod)
{
  // on sale if: sales are enabled, there's a price, and there's a discount price which isn't zero
  return currentSettings.SalesOrFrequentShopper == 1 && prod.PrPrice && prod.PrDscPr1 && prod.PrDscPr1 > prod.PrPrice;
}

function normalPrice(prod)
{
  if (onSale(prod) && prod.PrDscPr1 && prod.PrDscPr1 > prod.PrPrice)
  {
    return prod.PrDscPr1;
  }
  else
  {
    return prod.PrPrice;
  }
}

function salePrice(prod)
{
  return prod.PrPrice;
}

function view_cart()
{
  window.location = "/OnlineOrdering/kiosk/cart.html?DeptNum=" + getParams("DeptNum");
}

function add_more()
{
  window.location = "/OnlineOrdering/kiosk/categories.html?DeptNum=" + getParams("DeptNum");
}

if (!String.prototype.startsWith) {
    Object.defineProperty(String.prototype, 'startsWith', {
        value: function(search, rawPos) {
            var pos = rawPos > 0 ? rawPos|0 : 0;
            return this.substring(pos, pos + search.length) === search;
        }
    });
}

function time_now()
{
  var now = new Date();
  var offsetMs = now.getTimezoneOffset() * 60 * 1000;
  var dateLocal = new Date(now.getTime() - offsetMs);

  return dateLocal.toISOString().slice(0, 19).replace("T", " ");
}

// API-formatted time minutes into the future
function time_future(future)
{
  if (!future) future = 0;
  var now = new Date();
  var offsetMs = (-future + now.getTimezoneOffset()) * 60 * 1000;
  
  var dateLocal = new Date(now.getTime() - offsetMs);

  return dateLocal.toISOString().slice(0, 19).replace("T", " ");
}

/* can be used e.g. to prevent swipe while scrolling */
function noSwipe(e)
{
  if (runningOnScale) BrowserView.noSwipe();
}
