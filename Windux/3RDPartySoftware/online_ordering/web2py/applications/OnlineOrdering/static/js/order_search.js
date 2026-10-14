$().ready(function(){
  //clearSearch();
  // using selectSearch lets the browser-saved input data apply (i.e. if back)
  selectSearchRefine();
  
  $("#tableDataDivSearch").scroll(noSwipe);
});

function updateRows( searchUrl )
{
  $(".orderDataDivContent").addClass("grayOut");
  
  $.getJSON(searchUrl, function(data)
  {
    var json_objArr = data;
    var numOfOrders = data.length;

    $("#orderData").empty();
    var currentRows = document.getElementById("orderData").children;
    var table_rows = currentRows.length;
    var dataRow = null;

    for ( var i = 0; i < numOfOrders; ++i )
    {
      dataRow = document.getElementById("orderDataRowToClone").cloneNode(true);
      document.getElementById("orderData").appendChild(dataRow);
      $(dataRow).addClass("orderDataRow");

      dataRow.id = json_objArr[i].OrderNumber;

      $(dataRow).find(".orderIDCol").text(json_objArr[i].OrderNumber);
      $(dataRow).find(".customerCol").text(json_objArr[i].CustomerName);
      $(dataRow).find(".createdTimeCol").text(json_objArr[i].OrderCreation);
      $(dataRow).find(".deliveryTimeCol").text(json_objArr[i].OrderPickup);

      var mystatus = parseInt(json_objArr[i].PartStatus, 10);
      var myorigin = parseInt(json_objArr[i].OrderOrigin, 10);

      if( mystatus < orderStatus.length)
        $(dataRow).find(".orderStatusCol").text(orderStatus[mystatus]);
      else
        $(dataRow).find(".orderStatusCol").text("Unknown status");

      if( myorigin < orderType.length)
        $(dataRow).find(".originCol").text(orderType[myorigin]);
      else
        $(dataRow).find(".originCol").text("Unknown origin");
    }

    $(".orderDataDivContent").removeClass("grayOut");

  }).fail(function() {failed("Failed to fetch orders!");});
}

///////////////////////////////////////////////////////////////////////////
// Search routines SECTION

function selectSearchRefine(element)
{
  // check all search parameters and build a searchable jquery

  var baseUrl   = 'http://' + getParams('black_box') + '/api/v1/orderparts';
  var searchUrl = "";

  // create own error check method if I come up with too many error checks
  var pluStart  = document.getElementById('orderNumStart').value;
  var pluEnd    = document.getElementById('orderNumEnd').value;
  
  if ( isNaN(pluStart) )
  {
    failed("Leading product number range is not a number.");
    document.getElementById('orderNumStart').value = "";
    return;
  }
  
  if ( isNaN(pluEnd) )
  {
    failed("Ending product number range is not a number.");
    document.getElementById('orderNumEnd').value = "";
    return;
  }

  $(".orderDataDivContent").addClass("grayOut");

  // build the plu range part
  searchUrl = baseUrl + buildPluNumberURL();

  // Search on customer - Right now only finds EXACT name. May need to do better bc last names
  var custName  = document.getElementById('orderName').value;
  if( ( 0 < custName.length ) )
    searchUrl = searchUrl + "&CustomerName.search=" + custName;

  // Search on Order Creation
  searchUrl = searchUrl + buildCreationTimeURL();

  // Search on Order Pickup
  searchUrl = searchUrl + buildPickupTimeURL();

  // Search on Status
  var orderOrigin = document.getElementById('orderOriginSearch');
  if( parseInt(orderOrigin.selectedIndex, 10) > 0 )
    searchUrl = searchUrl + "&OrderOrigin=" + orderOrigin.value;

  // Search on Origin
  var custStatus = document.getElementById('orderStatusSearch');
  if( parseInt(custStatus.selectedIndex, 10) > 0 )
    searchUrl = searchUrl + "&PartStatus=" + custStatus.value;
    
  searchUrl += "&limit=" + currentSettings.OrderListMax;
  
  searchUrl += "&DeptNum=" + getDept();
  searchUrl += "&ZoneNum=" + getZone();

  updateRows( searchUrl );
  
  return false;
}

function buildPluNumberURL()
{
  // 1. search if there is only initial value (search it specific)
  // 2. search if there is only ending value (search it specific)
  // 3. search if there is leading and ending values (search range)

  var pluNumURL = '';
  var pluStart  = document.getElementById('orderNumStart').value;
  var pluEnd    = document.getElementById('orderNumEnd').value;
  if( ( 0 < pluStart.length ) && ( 0 == pluEnd.length ) )
    pluNumURL = "?OrderNumber=" + pluStart;
  else if( ( 0 == pluStart.length ) && ( 0 < pluEnd.length ) )
    pluNumURL = "?OrderNumber=" + pluEnd;
  else if( ( 0 < pluStart.length ) && ( 0 < pluEnd.length ) )
    pluNumURL = '?OrderNumber.gte=' + pluStart + '&OrderNumber.lte=' + pluEnd;
  else
    pluNumURL = '?OrderNumber.gte=1';

  return pluNumURL;
}

function buildCreationTimeURL()
{
  // Purpose: Build the time format string for searching created order times
  // 1: If just initial time is given, search from that time to the end of the day
  // 2: If just final time is given, search from the beginning of the day to this time
  // 3: If both times given, search the range between the times

  var d = new Date();
  var dateToday = d.getFullYear() + "-" + (d.getMonth()+1) + "-" + d.getDate();

  var creationURL = '';
  var createStart = document.getElementById('orderCreatedTimeStart');
  var createEnd   = document.getElementById('orderCreatedTimeEnd');
  
  if( ( 0 < createStart.selectedIndex ) && ( 0 == createEnd.selectedIndex ) )
  {
    var createStartVal = dateToday + ' ' + createStart.value;
    creationURL = '&OrderCreation.gte=' + createStartVal + '&OrderCreation.lte=' + dateToday + ' 23:59:59';
  }
  else if( ( 0 == createStart.selectedIndex ) && ( 0 < createEnd.selectedIndex ) )
  {
    var createEndVal = dateToday + ' ' + createEnd.value;
    creationURL = '&OrderCreation.gte=' + dateToday + ' 00:00:01' + '&OrderCreation.lte=' + createEndVal;
  }
  else if( ( 0 < createStart.selectedIndex ) && ( 0 < createEnd.selectedIndex ) )
  {
    var createStartVal = dateToday + ' ' + createStart.value;
    var createEndVal   = dateToday + ' ' + createEnd.value;
    creationURL = '&OrderCreation.gte=' + createStartVal + '&OrderCreation.lte=' + createEndVal;
  }

  return creationURL;
}

function buildPickupTimeURL()
{
  // Purpose: Build the time format string for searching pickup order times
  // 1: If just initial time is given, search from that time to the end of the day
  // 2: If just final time is given, search from the beginning of the day to this time
  // 3: If both times given, search the range between the times
  
  var d = new Date();
  var dateToday = d.getFullYear() + "-" + (d.getMonth()+1) + "-" + d.getDate();
  
  var pickupURL = '';
  var pickupStart = document.getElementById('orderPickupTimeStart');
  var pickupEnd   = document.getElementById('orderPickupTimeEnd');
  
  if( ( 0 < pickupStart.selectedIndex ) && ( 0 == pickupEnd.selectedIndex ) )
  {
    var pickupStartVal = dateToday + ' ' + pickupStart.value;
    pickupURL = '&OrderPickup.gte=' + pickupStartVal + '&OrderPickup.lte=' + dateToday + ' 23:59:59';
  }
  else if( ( 0 == pickupStart.selectedIndex ) && ( 0 < pickupEnd.selectedIndex ) )
  {
    var pickupEndVal = dateToday + ' ' + pickupEnd.value;
    pickupURL = '&OrderPickup.gte=' + dateToday + ' 00:00:01' + '&OrderPickup.lte=' + pickupEndEndVal;
  }
  else if( ( 0 < pickupStart.selectedIndex ) && ( 0 < pickupEnd.selectedIndex ) )
  {
    var pickupStartVal = dateToday + ' ' + pickupStart.value;
    var pickupEndEndVal   = dateToday + ' ' + pickupEnd.value;
    pickupURL = '&OrderPickup.gte=' + pickupStartVal + '&OrderPickup.lte=' + pickupEndEndVal;
  }
  
  return pickupURL;
}

function clearSearch(element)
{
  var searchUrl   = 'http://' + getParams('black_box') + '/api/v1/orders';
  searchUrl += "?limit=" + currentSettings.OrderListMax;
  searchUrl += "&DeptNum=" + getDept();

  document.getElementById('orderNumStart').value = "";
  document.getElementById('orderNumEnd').value = "";
  document.getElementById('orderName').value = "";
  document.getElementById('orderCreatedTimeStart').selectedIndex = 0;
  document.getElementById('orderCreatedTimeEnd').selectedIndex = 0;
  document.getElementById('orderPickupTimeStart').selectedIndex = 0;
  document.getElementById('orderPickupTimeEnd').selectedIndex = 0;
  document.getElementById('orderStatusSearch').selectedIndex = 0;

  updateRows( searchUrl );
}

function selectStatus(element)
{
    var orderNumber = 0;
    var customerName = "";
    var deliveryTime = "";
    var currentStatus = "0";
    
    // read info either from the details page or a row in a table
    if ($("#orderNum").length)
    {
      orderNumber = parseInt($("#orderNum").text());
      customerName = $("#orderName").text();
      deliveryTime = $("#orderTime").text();
    }
    else
    {
      var row = $('html').find(".selectedRow");
    
      if( row.length == 0 )
      {
          failed("No order is selected to modify!");
          return;
      }
      
      orderNumber = parseInt($(row).find(".orderIDCol").text());
      customerName = $(row).find(".customerCol").text();
      deliveryTime = $(row).find(".deliveryTimeCol").text();
      currentStatus = orderStatusVal[$(row).find(".orderStatusCol").text()];
    }
    
    // put data into the modal
    $("#statusModal-orderNumber").val(orderNumber);
    $("#statusModal-customer").val(customerName);
    $("#statusModal-deliveryTime").val(deliveryTime);
    $("#statusModal-select").val(currentStatus);
    $('#statusModal-status').mouseup(function() { var newStatus = $("#statusModal-select").val(); statusOrder(orderNumber, newStatus); });
    
    $('#statusModal').modal('show');
    
    return false;
}

function statusOrder(orderNumber, newStatus)
{
  $('#statusModal').modal('hide');
  
  if (newStatus == orderStatusVal["Cancelled"])
  {
    selectCancel();
    return;
  }
  
  var order = {"OrderNumber": orderNumber, "DeptNum": getDept(), "PartStatus": newStatus};
  
  // if uncancelling an order part, uncancel the order as well
  order["OrderStatus"] = newStatus;
  
  $.ajax({
    url: "/api/v1/orderparts/"+orderNumber+"/"+getDept()+"/"+getZone(),
    type: 'PUT',
    data: JSON.stringify(order),
    contentType: "application/json"
  }).done(function() {
    selectSearchRefine();
  }).fail(function() {
    failed("Failed to change order status!");
  });
}
