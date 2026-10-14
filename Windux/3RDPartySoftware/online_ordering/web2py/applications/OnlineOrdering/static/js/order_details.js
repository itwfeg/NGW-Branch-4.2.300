$().ready(function(){  
  checkOrder();
  
  if (runningOnCenterPanel())
  {
    $("#fillByCountModal-print").addClass("grayOut");
  }
  
  $("#tableDataDivDetails").scroll(noSwipe);
});

function updateRows(completionCallback)
{
   var orderNumber   = getParams('order_num');
   var orderDue      = getParams('order_due');
   var customerName  = getParams('order_name');

   // update the order information
   // Main Screen information
   $("#orderNum").html(orderNumber);
   $("#orderName").html(customerName);
   $("#orderTime").html(orderDue);
   // Fill Order Dialog
   $("#orderFillNum").html(orderNumber);
   $("#orderFillName").html(customerName);
   $("#orderFillTime").html(orderDue);

   var url_details = 'http://' + getBlackBox() + '/api/v1/orderdetails/' + orderNumber + "/" + getDept() + "/" + getZone();

   $("#tableDataDiv").addClass("grayOut");
   $.getJSON(url_details, function(data)
   {
       var json_objArr = data;
       var numOfItems = data.length;
       
       $("#orderData").empty();

       var currentRows    = document.getElementById("orderData").children;
       var table_rows     = currentRows.length;
       var dataRow        = null;
       var tmpRowStr      = "";
       
       for ( var i = 0; i < numOfItems; ++i )
       {
          // no more visible rows exist, so create a new row
          dataRow = document.getElementById("orderDataRowToClone").cloneNode(true);
          document.getElementById("orderData").appendChild(dataRow);
          $(dataRow).addClass("orderDataRow");
          
          if (!json_objArr[i].PluInstructions) json_objArr[i].PluInstructions = "";
          if (!json_objArr[i].FinalWeight) json_objArr[i].FinalWeight = "";
          if (!json_objArr[i].FinalByCount) json_objArr[i].FinalByCount = "";
          if (!json_objArr[i].PluWeight) json_objArr[i].PluWeight ="0";

          dataRow.id = "rowId_" + i;
	        dataRow.children[0].innerHTML = json_objArr[i].PrNum;
          tmpRowStr = '<div class="tableDiv descDetails">' + json_objArr[i].PluDescription;
          tmpRowStr = tmpRowStr + '</div><div class="tableDiv prepDetails">' + json_objArr[i].PluInstructions + '</div>';
          dataRow.children[1].innerHTML = tmpRowStr;
          
          if (json_objArr[i].FinalPrice)
          {
             $(dataRow.children[2]).addClass("final-actual");
             $(dataRow.children[3]).addClass("final-actual");
          }

          // insert the weight
          if( "1" == json_objArr[i].ProductType )
          {
              var tmpWeight = json_objArr[i].FinalWeight ? json_objArr[i].FinalWeight : json_objArr[i].PluWeight;
              var w1 = parseFloat(tmpWeight, 10);
              var w2 = (w1/1000.0).toFixed(3);
              dataRow.children[2].innerHTML = w2.toString() + " lb";
          }
          else if( "3" == json_objArr[i].ProductType || "2" == json_objArr[i].ProductType)
	            dataRow.children[2].innerHTML = json_objArr[i].FinalByCount ? json_objArr[i].FinalByCount : json_objArr[i].PluByCount;
          else if( "4" == json_objArr[i].ProductType )
          {
              // insert decimal into weight
              var w = json_objArr[i].FinalWeight ? json_objArr[i].FinalWeight : json_objArr[i].PluWeight;
              dataRow.children[2].innerHTML = w;

              if( ( 0 == w) || ( '' == w ) )
                  dataRow.children[2].innerHTML = '0 oz';
              else
                  dataRow.children[2].innerHTML = w.toString() + ' oz';
          }

          // insert decimal into price
          var price = json_objArr[i].FinalPrice ? json_objArr[i].FinalPrice : json_objArr[i].PluPrice;
          dataRow.children[3].innerHTML = PriceToString(price);

          dataRow.children[5].innerHTML = json_objArr[i].OrderNumId;
          dataRow.children[6].innerHTML = json_objArr[i].ProductType;
          
          // if subs are disabled for the order, override/disable subs for all
          if ($("#actionModal-substitute").prop("disabled") == true)
          {
            dataRow.children[7].innerHTML = "0";
          }
          else
          {
            dataRow.children[7].innerHTML = json_objArr[i].AllowSubstitutions;
          }
          
          if (json_objArr[i].Cancelled == 1)
          {
            $(dataRow).addClass("unavailableRow");
          }
          else if (json_objArr[i].Filled == 1)
          {
            $(dataRow).addClass("filledRow");
          }
          
          if (json_objArr[i].Substituted == 1)
          {
            $(dataRow).addClass("substitutedRow");
          }
       }
       
       $add_row = $("#addItemRowToClone").clone(true, true);
       $add_row.appendTo("#orderData");
       
       $("#tableDataDivDetails").removeClass("grayOut");
       
       if (typeof completionCallback === 'function') completionCallback();
   }).fail(function(){failed("Failed to load order details!");});
}

function selectResetOrder(element)
{
  var reset = function() {
    $("#continueModal").modal("hide");
    $(".orderDataDivContent").addClass("grayOut");
    
    $.ajax({
      url: "/api/v1/orderdetails/" + parseInt($("#orderNum").text()) + "/reset",
      type: 'PUT',
      data: JSON.stringify({}),
      contentType: "application/json"
    }).done(function() {
      updateRows();
    })
    .fail(function() {
      failed("Failed to reset order!");
      updateRows();
    });
  }
  
  $("#continueModal-body").text("Reset all items in order?");
  $("#continueModal-continue").unbind('click');
  $("#continueModal-continue").click(reset);
  $("#continueModal").modal("show");
  
  return false;
}

function selectResetItem(orderNumID)
{  
  $(".orderDataDivContent").addClass("grayOut");
  
  $.ajax({
    url: "/api/v1/orderdetails/" + parseInt($("#orderNum").text()) + "/" + orderNumID + "/reset",
    type: 'PUT',
    data: JSON.stringify({}),
    contentType: "application/json"
  }).done(function() {
    updateRows();
  })
  .fail(function() {
    failed("Failed to reset item!");
    updateRows();
  });
  
  return false;
}

function checkCompletion()
{
  // subtract one for the "add item" pseudo-row
  if ($(".unavailableRow").length + $(".filledRow").length == $("#orderData").children().length - 1)
  {
    $("#completeModal").modal("show");
  }
}

function completeItem(OrderNumId, OrderNumber, DeptNum)
{
  $(".orderDataDivContent").addClass("grayOut");
  $.ajax({
    url: "/api/v1/orderdetails/",
    type: 'PUT',
    data: JSON.stringify({"OrderNumId": OrderNumId, "ReasonCancelled": null, "Cancelled": 0, "Filled": 1, "OrderNumber": OrderNumber, "DeptNum": DeptNum}),
    contentType: "application/json"
  }).done(function() {
    updateRows(checkCompletion);
  })
  .fail(function() {
    failed("Failed to fill item!");
    updateRows();
  });
}

function fillModalPrint(OrderNumId, PLU, productType, count)
{
  var OrderNumber = getParams('order_num');
  
  if (runningOnScale)
  {
    BrowserView.lookupPLU(PLU, getParams('order_name'), getParams('order_due'), OrderNumber, OrderNumId, count);
  }
  
  if (runningOnScale && runningOnCenterPanel())
  {
    BrowserView.setEnhancedURL(window.location.href + "&continue=1");
  }
  
  if (runningOnScale)
  {
    var url = "http://" + getBlackBox() + "/OnlineOrdering/default/fillItemPopup.html/" + OrderNumber + "/" + OrderNumId;
    BrowserView.setMainURL(url);
    if (!runningOnCenterPanel()) { BrowserView.swipeLeft(); };
  }
  else
  {
    // not on a scale? item won't be printed, so just complete it immediately
    completeItem(OrderNumId, OrderNumber, getDept());
  }
  
  return false;
}

function actionButton(element)
{
  var row = $(element.parentElement.parentElement);
  
  var PLU = row.find(".itemNumberCol").text();
  var qty = row.find(".byCountCol").text();
  var productType = parseInt(row.find(".productTypeCol").text());
  var description = row.find(".descDetails").text();
  var orderNumId = parseInt(row.find(".orderNumIdCol").text());
  var allowSubstitutions = parseInt(row.find(".allowSubstitutionsCol").text());
  
  if (productType == 3 || productType == 2)
  {
    qty += " count"
  }
  else if (productType && productType != 1)
  {
    alert("unexpected product type");
  }
  
  $("#actionModal-description").text(description);
  $("#actionModal-PLU").text("PLU: " + PLU);
  $("#actionModal-qty").text(qty);
  
  $("#substituteModal-oldPLU").val(PLU);
  $("#substituteModal-PLU").val("");
  $("#substituteModal-reason").val("Unavailable.");
  $("#substituteModal-submit").unbind('click');
  

  $("#substituteModal-submit").click(function() {
    var reason = $("#substituteModal-reason").val();
    var newPLU = $("#substituteModal-PLU").val();
    substituteItem(orderNumId, reason, newPLU);
  });

  $("#actionModal").modal("show");
  
  $("#actionModal-fill").unbind('click');
  $("#actionModal-fill").click(function(e) {
    $("#actionModal").modal("hide");
    lockOrder(orderStatusVal["In progress"]);
    fillOrder(element);
  });
  
  $("#actionModal-cancel").unbind('click');
  $("#actionModal-cancel").click(function(e) {
    $("#actionModal").modal("hide");
    lockOrder(orderStatusVal["In progress"]);
    cancelRowItem(element);
  });
  
  $("#actionModal-reset").unbind('click');
  $("#actionModal-reset").click(function(e) {
    $("#actionModal").modal("hide");
    selectResetItem(orderNumId);
  });
  
  if (allowSubstitutions != 1)
  {
    $("#actionModal-substitute").attr("disabled", true);
  }
  else
  {
    $("#actionModal-substitute").attr("disabled", false);
  }
  
  $("#actionModal-substitute").unbind('click');
  
  if (row.hasClass("filledRow") || row.hasClass("unavailableRow"))
  {
    $("#actionModal-substitute").addClass("grayOut");
    $("#actionModal-fill").addClass("grayOut");
    $("#actionModal-reset").removeClass("grayOut");
    
    $("#actionModal-substitute").click(function(e) {
      $("#actionModal").modal("hide");
      lockOrder(orderStatusVal["In progress"]);
      $("#failureModal-body").text("Cannot substitute a completed item!");
      $("#failureModal").modal("show");
    });
    
    $("#actionModal-fill").unbind('click');
    $("#actionModal-fill").click(function(e) {
      $("#actionModal").modal("hide");
      $("#failureModal-body").text("Cannot fill a completed item!");
      $("#failureModal").modal("show");
    });
  }
  else
  {
    $("#actionModal-fill").removeClass("grayOut");
    $("#actionModal-substitute").removeClass("grayOut");
    $("#actionModal-reset").addClass("grayOut");
  
    $("#actionModal-substitute").click(function(e) {
      $("#substituteModal-reason").val("Unavailable");
      $("#actionModal").modal("hide");
      lockOrder(orderStatusVal["In progress"]);
      $("#substituteModal").modal("show");
    });
    
    $("#actionModal-reset").unbind('click');
    $("#actionModal-reset").click(function(e) {
      $("#actionModal").modal("hide");
      $("#failureModal-body").text("Cannot reset an unfilled item!");
      $("#failureModal").modal("show");
    });
  }
  
  return false;
}

function fillOrder(element)
{
  var row = $(element.parentElement.parentElement);
  
  var PLU = row.find(".itemNumberCol").text();
  var qty = row.find(".byCountCol").text();
  var instructions = row.find(".prepDetails").text();
  var OrderNumId = parseInt(row.find(".orderNumIdCol").text());
  var productType = parseInt(row.find(".productTypeCol").text());
  var description = row.find(".descDetails").text();
  var count = "";
  
  if (productType == 3 || productType == 2)
  {
    qty += " count"
    count = qty;
  }
  else if (productType && productType != 1)
  {
    alert("unexpected product type");
  }
  
  $("#fillByCountModal-description").text(description);
  $("#fillByCountModal-PLU").text("PLU: " + PLU);
  $("#fillByCountModal-qty").text(qty);
  $("#fillByCountModal-instructions").val(instructions);
  
  $("#fillByCountModal-print").unbind("click");
  $("#fillByCountModal-lookup").unbind("click");
  $("#fillByCountModal-keep").unbind("click");
  
  $("#fillByCountModal-print").click(function() {    
    BrowserView.lookupPLU(PLU, getParams('order_name'), niceTime(getParams('order_due')), getParams('order_num'), OrderNumId, count);
    BrowserView.print();
    
    if (runningOnScale && runningOnCenterPanel())
    {
      BrowserView.setEnhancedURL(window.location.href + "&continue=1");    
    }
  });
  
  $("#fillByCountModal-lookup").click(function() {
    fillModalPrint(OrderNumId, PLU, productType, count);
  });
  
  $("#fillByCountModal-keep").click(function() {
    // TODO dept num
    completeItem(OrderNumId, getParams('order_num'), getDept());
  });
  
  

  fillModalPrint(OrderNumId, PLU, productType, count);

  return false;
}

function cancelItem(itemId, reason)
{
  var OrderNumber = getParams('order_num');
  
  $.ajax({
    url: "/api/v1/orderdetails/",
    type: 'PUT',
    data: JSON.stringify({"OrderNumId": itemId, "ReasonCancelled": reason, "Cancelled": 1, "Filled": 0, "OrderNumber": OrderNumber, "DeptNum": getDept()}),
    contentType: "application/json"
  }).done(function() {
    updateRows(checkCompletion);
  })
  .fail(function() {
    failed("Failed to cancel item!");
    updateRows();
  });
}

function cancelRowItem(element)
{
  $("#tableDataDivDetails").addClass("grayOut");
  
  var row = $(element.parentElement.parentElement);
  var PLU = row.find(".itemNumberCol").text();
  var qty = row.find(".byCountCol").text();
  var orderNumId = parseInt(row.find(".orderNumIdCol").text());
  
  $.getJSON("/api/v1/orderparts/" + getDept() + "/" + getZone() + "/" + getParams('order_num')).done(function(data) {    
    $("#cannotModal-PLU").val(PLU);
    $("#cannotModal-reason").val("Unavailable.");
    $("#cannotModal-qty").val(qty);
    $("#cannotModal-cancel").unbind('click');
    $("#cannotModal-cancel").click(function() { var reason = $("#cannotModal-reason").val(); cancelItem(orderNumId, reason); });
    
    $("#substituteModal-oldPLU").val(PLU);
    $("#substituteModal-PLU").val("");
    $("#substituteModal-reason").val("Unavailable.");
    $("#substituteModal-submit").unbind('click');
    $("#substituteModal-submit").click(function() {
      var reason = $("#substituteModal-reason").val();
      var newPLU = $("#substituteModal-PLU").val();
      lockOrder(orderStatusVal["In progress"]);
      substituteItem(orderNumId, reason, newPLU);
    });
    
    $("#tableDataDivDetails").removeClass("grayOut");
    
    $("#cannotModal").modal("show");
  }).fail(function(){failed("Failed to cancel item!");});
  
  return false;
}

function selectSubstitute(element)
{
  $("#substituteModal-reason").val($("#cannotModal-reason").val());
  $("#substituteModal").modal("show");
  
  return false;
}

function substituteItem(orderNumId, reason, newPLU)
{
  var OrderNumber = getParams('order_num');
  
  $("#tableDataDivDetails").addClass("grayOut");
  
  $.ajax({
    url: "/api/v1/orderdetails/substitute",
    type: 'PUT',
    data: JSON.stringify({"OrderNumId": orderNumId, "ReasonCancelled": reason, "Substituted": 1, "OrderNumber": OrderNumber, "DeptNum": getDept(), "NewPlu": newPLU}),
    contentType: "application/json"
  }).done(function() {
    updateRows(function() {
      // get the button that would normally be passed to fillOrder
      var element = $(".orderNumIdCol").filter(function() {
        return $(this).text() == orderNumId.toString();
      }).get(0).parentElement;
      
      element = $(element).find(".fillButton").get(0);
      fillOrder(element);
    });
  })
  .fail(function() {
    failed("Failed to substitute item!");
    updateRows();
  });
}

function checkOrder(orderNum)
{  
  $.get("/api/v1/orderparts/" + getParams('order_num') + "/" + getDept() + "/" + getZone(), function(data) {
    $("#orderStatus").text(orderStatus[data[0].PartStatus]);
    
    if (data[0].Operator)
    {
      $("#orderOperator").text(data[0].Operator);
    }
    else
    {
      $("#orderOperator").text("N/A");
    }
    
    if (data[0].PartFinish)
    {
      $("#orderFinish").text(niceTime(data[0].PartFinish));
    }
    else
    {
      $("#orderFinish").text("N/A");
    }
    
    var new_status = data[0].PartStatus;
    
    if (data[0].PartStatus == orderStatusVal["In progress"])
    {
      // if "continue=1" in the params, skip the warning
      if (getParams('continue') != '1')
      {
        $('#lockedModal').modal('show');
      }
      
      orderLocked = true;
    }
    else if (data[0].PartStatus == orderStatusVal["Cancelled"])
    {
      $("#cancelledModal-reason").text(data[0].ReasonCancelled);
      $("#cancelledModal").modal('show');
    }
    
    if (data[0].AllowSubstitutions == 0)
    {
      $("#actionModal-substitute").attr("disabled", true);
    }
    
    if (data[0].OrderOrigin != 3)
    {
      $(".addItemRow").css("display", "none");
    }
  }).done(updateRows).fail(function() {failed("Failed to load order info!");});
}

var orderLocked = false;
function lockOrder(new_status)
{
  if (orderLocked) return;
  orderLocked = true;
  
  var start = time_now();

  $.ajax({
    url: "/api/v1/orderparts/" + getParams('order_num') + "/" + getDept() + "/" + getZone(),
    type: 'PUT',
    data: JSON.stringify({"PartStatus": new_status, "PartStart": start}),
    contentType: "application/json"
  }).done(function() {
    updateRows();
  })
  .fail(function() {
    failed("Failed to lock order!");
    updateRows();
  });
}

function completeOrder(operator, permission) {
  var data = {"PartStatus": orderStatusVal["Ready"]};

  if (operator)
  {
    data["Operator"] = operator;
  }
  
  $(".orderDataDivContent").addClass("grayOut");

  $.ajax({
    url: "/api/v1/orderparts/" +getParams('order_num') + "/" +  getDept() + "/" + getZone(),
    type: 'PUT',
    data: JSON.stringify(data),
    contentType: "application/json"
  }).done(function() {
    selectHome();
  })
  .fail(function() {
    failed("Failed to complete order!");
    updateRows();
  });

};

function selectComplete(element)
{
  var complete = function() {
    $("#continueModal").modal("hide");
    
    if (currentSettings.OperatorLogging == 1)
    {
      get_operator_and_run(completeOrder, "Don't Complete", "Complete Order");
    }
    else
    {
      completeOrder(null, null);
    }
  }
  
  // check the number of rows in the table, minus the add item button
  if ($("#orderData").children().not(".addItemRow").length != $(".filledRow").length + $(".unavailableRow").length)
  {
    $("#continueModal-body").text("Not all items are marked as complete.  Complete order anyway?");
    $("#continueModal-continue").unbind('click');
    $("#continueModal-continue").click(complete);
    $("#continueModal").modal("show");
  }
  else
  {
    complete();
  }
}

function showSearchSubstitute()
{
  $('#searchModal').off();
  $('#searchModal').on('hide.bs.modal', function () {
    $("#searchModal hr").css("display", "none");
    $("#substituteModal").modal("show");
  });
  
  $('#searchModal').on('keyup', function (event) {
    if ($("#searchModal-string").val()) $('#searchModal-lookup').click();
    return true;
  });

  $("#searchModal-string").val("");
  $(".search-result").not(".template").remove();
  $("#searchModal .row.template button").unbind("click");
  $("#searchModal .row.template button").click(function() {
    $("#searchModal").modal("hide");
    
    var PLU = $(this).parent().parent().find(".searchModal-PLU").first().text();
    $("#substituteModal-PLU").val(PLU);
    
    $("#substituteModal").modal("show");
  });
  
  $("#searchModal").modal("show");
}

function showSearchAdd()
{
  $('#searchModal').off();
  $('#searchModal').on('hide.bs.modal', function () {
    $("#searchModal hr").css("display", "none");
    $("#addItemModal").modal("show");
  });
  
  $('#searchModal').on('keyup', function (event) {
    if ($("#searchModal-string").val()) $('#searchModal-lookup').click();
    return true;
  });

  $("#searchModal-string").val("");
  $(".search-result").not(".template").remove();
  $("#searchModal .row.template button").unbind("click");
  $("#searchModal .row.template button").click(function() {
    $("#searchModal").modal("hide");
    
    var PLU = $(this).parent().parent().find(".searchModal-PLU").first().text();
    $("#addItemModal-PLU").val(PLU);
    
    $("#addItemModal").modal("show");
  });

  $("#searchModal").modal("show");
}

function showAddItemModal()
{
  $("#searchModal-string").val("");
  $(".search-result").not(".template").remove();
  $("#searchModal .row.template button").unbind("click");
  $("#searchModal .row.template button").click(function() {
    $("#searchModal").modal("hide");
    
    var PLU = $(this).parent().parent().find(".searchModal-PLU").first().text();
    $("#addItemModal-PLU").val(PLU);
    
    $("#addItemModal").modal("show");
  });

  $("#addItemModal-PLU").val("");
  $("#addItemModal").modal("show");
}

var pending = false;

function searchProds()
{
  // don't have multiple GETs going at the same time
  if (pending) return;
  
  pending = true;
  
  $.get("/api/v1/products/?DeptNum=" + getDept() + "&limit=15&PrDesc.search=" + $("#searchModal-string").val())
  .done(function(data) {
    $(".search-result").not(".template").remove();
    $("#searchModal hr").css("display", "block");
    
    for (i = 0; i < Math.min(15, data.length); i++)
    {
      var $next = $("#searchModal .row.template").clone(true);
      $next.removeClass("template");
      $next.find(".searchModal-lookupName").text(data[i].PrDesc);
      
      $next.find(".searchModal-lookupPrice").text(PriceToString(data[i].PrPrice));

      $next.find(".searchModal-PLU").text(data[i].PrNum);

      //$next.insertAfter("#searchModal hr");
      $("#searchModal-results").append($next);
    }
    pending = false;
  })
  .fail(function() {
    $(".search-result").not(".template").remove();
    $("#searchModal hr").css("display", "none");
    failed("Failed to search!");
    pending = false;
  });
}

function addItem()
{
  var PLU = $("#addItemModal-PLU").val();
  var desc = "";
  /* we'll mark it as filled up front, since we're going to load it up */
  var d = { "DeptNum": getDept(), "OrderNumber": getParams('order_num'), "PrNum": PLU, "Filled": 1, "PluInstructions": "", "ZoneNum": getZone() };
  
  $.ajax({
    url: "/api/v1/orderdetails/",
    type: 'PUT',
    data: JSON.stringify(d),
    contentType: "application/json"
  }).done(function(data) {
    // update rows, then simulate pressing the fill button
    updateRows(function() {
      // get the button that would normally be passed to fillOrder
      var element = $(".orderNumIdCol").filter(function() {
        return $(this).text() == data["OrderNumId"].toString();
      }).get(0).parentElement;
      
      element = $(element).find(".fillButton").get(0);
      
      fillOrder(element);
    });
  })
  .fail(function() {
    failed("Failed to add item!");
    updateRows();
  });
}
/*
function printReport()
{
  var orderNumber = "98765678901";
  var customerName = "Excessivelyfirstname Unreasonablylastname";
  var count = "1";
  
  if (runningOnScale)
  {
    BrowserView.printOrderReport(orderNumber, customerName, count);
  }
  else
  {
    alert("honk");
  }
}*/

function printReport()
{
  var orderNumber = $("#orderFillNum").text();
  var customerName = $("#orderFillName").text();
  var count = "1";
  
  var p = "\x1B" + "\x1B" + "fn=Hobart;ps=5" + "\x1B" + " ";
  p += "\x1B" + "\x1B" + "fn=Hobart;ps=26" + "\x1B" + orderNumber + "\x0A";
  p += "\x1B" + "\x1B" + "fn=Hobart;ps=14" + "\x1B" + customerName + "\x0A" + "\x0A";
  p += "\x1B" + "\x1B" + "fn=Hobart;ps=8" + "\x1B" + "Printed: 1:23 PM";

  if (runningOnScale)
  {
    BrowserView.printOrderReport(orderNumber, customerName, count);
    //BrowserView.printLabelString(p);
  }
  else
  {
    alert("must be on a scale to print label!");
  }
}
