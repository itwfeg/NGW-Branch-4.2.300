var easyFields = ["OrderDetails", "CustomerName", "OrderSMS", "OrderCreation", "OrderPickup", "OrderStart", "OrderFinish"];

var nextNumId = 0;

function select(selectObj, value)
{
  for (var i = 0; i < selectObj.options.length; i++)
  {
    if (selectObj.options[i].text == value)
    {
      selectObj.options[i].selected = true;
      return;
    }
  }
}

function lookup()
{
   var number = $("#OrderNumber").val();
   
   $.getJSON("/api/v1/orders/" + number, function(data)
   {
     if (data.length != 1)
     {
       alert("no such order!");
     }
     else
     {
       for (var i = 0; i < easyFields.length; i++)
       {
        $("#" + easyFields[i]).val(data[0][easyFields[i]]);
       }
       
       select($("#OrderStatus").get(0), orderStatus[data[0]["OrderStatus"]]);
       select($("#OrderOrigin").get(0), orderType[data[0]["OrderOrigin"]]);
       
       $("#AllowSubstitutions").prop("checked", data[0]["AllowSubstitutions"] == 1);
       
       $.getJSON("/api/v1/orderdetails/" + number, function(details)
       {
         $("#detailsTable").find("tr").each(function(i) { if ($(this).attr('id') != "detailsEmpty") $(this).remove();});
         
         for (var i = 0; i < details.length; i++)
         {
           var n = details[i]["OrderNumId"];
           if (!nextNumId) nextNumId = n;
           if (nextNumId <= n) nextNumId = n+1;
           
           var newRow = document.getElementById("detailsEmpty").cloneNode(true);
           document.getElementById("detailsTable").appendChild(newRow);
           newRow.removeAttribute("id");
           newRow.children[0].innerHTML = n;
           newRow.children[1].innerHTML = details[i]["DeptNum"];
           newRow.children[2].innerHTML = details[i]["ZoneNum"];
           newRow.children[3].innerHTML = details[i]["PrNum"];
           $(newRow).css("display", "table-row");
         }
       }).fail(function() {failed("Failed to load order details!");});
     }
   }).fail(function(){failed("Failed to look up order!");});
}

function orderSubmit()
{
  var number = $("#OrderNumber").val();
  
  var data = {};
  
  for (var i = 0; i < easyFields.length; i++)
  {
    data[easyFields[i]] = $("#"+easyFields[i]).get(0).value;
  }
  
  data["OrderStatus"] = parseInt($("#OrderStatus").get(0).value);
  data["OrderOrigin"] = parseInt($("#OrderOrigin").get(0).value);
  
  data["AllowSubstitutions"] = $("#AllowSubstitutions").prop("checked") ? 1 : 0;
  
  $.ajax({
  url: "/api/v1/orders/" + number,
  type: 'PUT',
  data: JSON.stringify(data),
  contentType: "application/json"
  }).done(function() {
    alert("submitted successfully");
  })
  .fail(function() {
    alert("failed to submit!");
  });
}

function removeItem(element)
{
  var numID = parseInt($(element.parentElement.parentElement).find(".itemOrderNumId").get(0).innerHTML);
  var number = $("#OrderNumber").val();
  
  $.ajax({
  url: "/api/v1/orderdetails/" + number + "/" + numID,
  type: 'DELETE',
  data: JSON.stringify(null),
  contentType: "application/json"
  }).done(function() {
    alert("removed successfully");
    lookup();
  })
  .fail(function() {
    alert("failed to remove!");
  });
}

function editItem(element)
{
  var numID = parseInt($(element.parentElement.parentElement).find(".itemOrderNumId").get(0).innerHTML);
  var zone = parseInt($(element.parentElement.parentElement).find(".itemZoneNum").get(0).innerHTML)
  var dept = parseInt($(element.parentElement.parentElement).find(".itemDeptNum").get(0).innerHTML)
  var number = $("#OrderNumber").val();
  
 $.getJSON("/api/v1/orderdetails/" + number + "/" + dept + "/" + zone + "/" + numID, function(data)
 {
  if (data.length != 1)
  {
    alert("no such item?");
    return;
  }
  
  $("#itemPrNum").val(data[0]["PrNum"]);
  $("#itemDeptNum").val(data[0]["DeptNum"]);
  $("#itemZoneNum").val(data[0]["ZoneNum"]);
  $("#itemOrderNumId").val(data[0]["OrderNumId"]);
  $("#itemProductType").val(data[0]["ProductType"]);
  $("#itemPluPrice").val(data[0]["PluPrice"]);
  $("#itemPluByCount").val(data[0]["PluByCount"]);
  $("#itemPluWeight").val(data[0]["PluWeight"]);
  $("#itemPluDescription").val(data[0]["PluDescription"]);
  $("#itemPluInstructions").val(data[0]["PluInstructions"]);
  $("#itemReasonCancelled").val(data[0]["ReasonCancelled"]);
  $("#itemOriginalPrNum").val(data[0]["OriginalPrNum"]);
  $("#itemFinalPrice").val(data[0]["FinalPrice"]);
  $("#itemFinalWeight").val(data[0]["FinalWeight"]);
  $("#itemFinalCount").val(data[0]["FinalCount"]);

  $("#itemCancelled").prop("checked", data[0]["Cancelled"] == 1);
  $("#itemFilled").prop("checked", data[0]["Filled"] == 1);
  $("#itemSubstituted").prop("checked", data[0]["Substituted"] == 1);
  $("#itemAllowSubstitutions").prop("checked", data[0]["AllowSubstitutions"] == 1);
  
  $("#editModal-submit").unbind("click");
  $("#editModal-submit").click(function() { submitItem(numID);} );
  
  $("#editModal").modal("show");
 }).fail(function(){failed("Failed to load item details!")});
}

function submitItem(numID)
{
  var number = $("#OrderNumber").val();
  
  var data = {"OrderNumber": number, "OrderNumId": numID};
  
  data["PrNum"] = $("#itemPrNum").val();
  data["DeptNum"] = $("#itemDeptNum").val();
  data["ZoneNum"] = $("#itemZoneNum").val();
  data["ProductType"] = $("#itemProductType").val();
  data["PluPrice"] = $("#itemPluPrice").val();
  data["PluByCount"] = $("#itemPluByCount").val();
  data["PluWeight"] = $("#itemPluWeight").val();
  data["PluDescription"] = $("#itemPluDescription").val();
  data["PluInstructions"] = $("#itemPluInstructions").val();
  data["ReasonCancelled"] = $("#itemReasonCancelled").val();
  data["OriginalPrNum"] = $("#itemOriginalPrNum").val();
  data["FinalPrice"] = $("#itemFinalPrice").val();
  data["FinalWeight"] = $("#itemFinalWeight").val();
  data["FinalCount"] = $("#itemFinalCount").val();

  data["Cancelled"] = $("#itemCancelled").prop("checked") ? 1 : 0;
  data["Filled"] = $("#itemFilled").prop("checked") ? 1 : 0;
  data["Substituted"] = $("#itemSubstituted").prop("checked") ? 1 : 0;
  data["AllowSubstitutions"] = $("#itemAllowSubstitutions").prop("checked") ? 1 : 0;
  
  $.ajax({
  url: "/api/v1/orderdetails/",
  type: 'PUT',
  data: JSON.stringify(data),
  contentType: "application/json"
  }).done(function() {
    alert("submitted successfully");
    lookup();
  })
  .fail(function() {
    alert("failed to submit!");
    lookup();
  });
}

function lookupItem()
{
  var PLU = $("#itemPrNum").val();
  var dept = $("#itemDeptNum").val();
  
  if (PLU != "" && dept != "")
  {
    $.ajax({
        url: "/api/v1/products/?PrNum=" + PLU + "&DeptNum=" + dept,
        type: 'GET',
        data: JSON.stringify(""),
        contentType: "application/json"
    }).done(function(data) {
      if (data && data.length != 1)
      {
        alert("didn't get one item!");
      }
      else
      {
        $("#itemZoneNum").val(data[0]["ZoneNum"]);
        $("#itemPrice").val(data[0]["PrPrice"]);
        $("#itemPluDescription").val(data[0]["PrDesc"]);
        
        $("#itemProductType").val(data[0]["PrType"]);
        
        if (data[0]["PrType"] == 3 || data[0]["PrType"] == 2)
        {
          $("#itemPluByCount").val("1");
        }
        else
        {
           $("#itemPluWeight").val("1000");
        }
      }
    })
    .fail(function() {
        alert("failed to look up PLU!");
        lookup();
    });
  }
  else
  {
    alert("enter a PLU and dept to look up");
  }
}

function addItem()
{
  $("#itemPLU").val("");
  $("#itemType").val("");
  $("#itemPrice").val("");
  $("#itemByCount").val("");
  $("#itemWeight").val("");
  $("#itemDescription").val("");
  $("#itemInstructions").val("");
  $("#itemReason").val("");
  $("#itemOriginalPlu").val("");
  $("#itemFinalWeight").val("");
  $("#itemFinalPrice").val("");
  $("#itemFinalCount").val("");

  $("#itemCancelled").prop("checked", 0);
  $("#itemFilled").prop("checked", 0);
  $("#itemSubstituted").prop("checked", 0);
  $("#itemSubstitutions").prop("checked", 0);

  $("#editModal-submit").unbind("click");
  $("#editModal-submit").click(function() { submitItem(nextNumId); });
  
  $("#editModal").modal("show");
}
