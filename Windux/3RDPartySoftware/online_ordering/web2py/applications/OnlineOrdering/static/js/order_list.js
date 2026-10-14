var idle_time = 0;

$().ready(function(){
    updateRows();

    // timer to track idle time
    var refresh_timer = setInterval(timer_increment, 1000);

    $(this).mousemove(function (e) {
        idle_time = 0;
    });

    $(this).keypress(function (e) {
        idle_time = 0;
    });
    
    $("#tableDataDiv").scroll(noSwipe);
});


function timer_increment()
{
    idle_time++;
    
    if (idle_time > currentSettings.AutorefreshTime)
    {
        idle_time = 0;
        updateRows();
    }
}

function colorButton(element, dueTime, order_origin)
{
  var button_color = "btn-hobart";
  var show_progress_bar = false;

  var pickup_time = dueTime.split(" ")[1];
  var pickup_date = dueTime.split(" ")[0];

  var order_pickup = new Date(pickup_date.split("-")[0],
                              parseInt(pickup_date.split("-")[1])-1,
                              pickup_date.split("-")[2],
                              pickup_time.split(":")[0],
                              pickup_time.split(":")[1],
                              pickup_time.split(":")[2]);
                              
  var warning_time = new Date(Date.now() + currentSettings.WarningTime*60*1000);

  if (order_origin == 3)
  {
    button_color = "btn-info";
  }

  if (order_pickup.getTime() < Date.now())
  {
    button_color = "btn-danger";
  }
  else if (order_pickup.getTime() < warning_time)
  {
    button_color = "btn-warning";
  }
  
  $(element).addClass(button_color);
  $(element).css("display", "inline");
}

 function updateRows()
 {
     unselectRow();
     
     $(".orderDataRowTitle").children().each(function(){
       $(this).removeClass("sortAsc");
       $(this).removeClass("sortDesc");
       $(this).addClass("sortNone");
     });
     
     $(".orderDataDivContent").addClass("grayOut");
     
     var futureTime = new Date(Date.now());
     
     futureTime = new Date(futureTime.getTime() + currentSettings.OrderListMinutes*60000);
     
     var dateString = new Date(futureTime.getTime() - (futureTime.getTimezoneOffset() * 60000 )) .toISOString() .split("T")[0];
     dateString += " " + futureTime.getHours() + ":" + futureTime.getMinutes();
     
     var baseURL = 'http://' + getBlackBox() + '/api/v1/orderparts/?PartStatus.gte=1&PartStatus.lte=2&OrderPickup.neq=null&sort=OrderPickup';
     
     baseURL += "&DeptNum=" + getDept() + "&ZoneNum=" + getZone();
     baseURL += "&OrderPickup.lte=" + dateString;
     baseURL += "&limit=" + currentSettings.OrderListMax;

     $.getJSON(baseURL, function(data)
     {
         var json_objArr = data;
         var numOfOrders = data.length;

         $("#orderData").empty();
         var currentRows = document.getElementById("orderData").children;
         var table_rows = currentRows.length;                       // initially 0 on first visit
         var dataRow = null;


         for ( var i = 0; i < numOfOrders; ++i )
         {
            dataRow = document.getElementById("orderDataRowToClone").cloneNode(true);
            document.getElementById("orderData").appendChild(dataRow);
            dataRow.className = "orderDataRow";

            dataRow.id = json_objArr[i].OrderNumber;
            dataRow.children[0].innerHTML = json_objArr[i].OrderNumber;
            dataRow.children[1].innerHTML = json_objArr[i].CustomerName;
            dataRow.children[2].innerHTML = niceTime(json_objArr[i].OrderPickup);
            dataRow.children[3].innerHTML = orderType[json_objArr[i].OrderOrigin];
            dataRow.children[4].innerHTML = json_objArr[i].ItemCount;

            var mystatus = parseInt(json_objArr[i].PartStatus, 10);
            if( mystatus < orderStatus.length)
            {
                dataRow.children[5].children[0].innerHTML = orderStatus[mystatus];
                colorButton(dataRow.children[5].children[0], json_objArr[i].OrderPickup, json_objArr[i].OrderOrigin);
            }
            else
                dataRow.children[5].innerHTML = "Unknown error";
                
            dataRow.children[6].innerHTML = json_objArr[i].OrderPickup;
         }
         
        $(".orderDataDivContent").removeClass("grayOut");

        if ($("#failureModal-body").text() == "Failed to load orders!")
        {
          $("#failureModal").modal("hide");
        }
    }).fail(function() {
      failed("Failed to load orders!");
    });
}
