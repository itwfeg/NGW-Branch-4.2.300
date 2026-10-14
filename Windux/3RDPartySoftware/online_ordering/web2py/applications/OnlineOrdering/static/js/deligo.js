var current_orders = null;
var warning_minutes = 20;
var max_wait_time = 20;
var idle_time = 0; // seconds since last mouse, click, keyboard event

$(document).ready(function(){
  if (runningOnScale && runningOnCenterPanel())
  {
    BrowserView.setEnhancedURL(window.location.href);
  }
  
  load_orders();
  
  $("#reload-button").unbind('click');
  $("#reload-button").mouseup(function(){
    load_orders();

    return false;
  });
  

  // timer to track idle time
  var refresh_timer = setInterval(timer_increment, 1000);

  $(this).mousemove(function (e) {
    idle_time = 0;
  });

  $(this).keypress(function (e) {
    idle_time = 0;
  });
});

function timer_increment()
{
    idle_time++;
    
    if (idle_time > currentSettings.AutorefreshTime)
    {
        idle_time = 0;
        load_orders();
    }
}

function card_click()
{
  var order_number = $(this).find(".card-title").text().replace("#","");
  
  for (i in current_orders)
  {
    if (current_orders[i].OrderNumber == order_number)
    {
      url = "./orderDetails.html?black_box=";
      url += "&order_num=" + order_number;
      url += "&order_due=" + niceTime(current_orders[i].OrderPickup);
      url += "&order_name=" + current_orders[i].CustomerName;
      url += "&prev=deligo"
      window.location.href = url.replace(/ /g, '+');
    }
  }
  
  return false;
}

function display_orders(data, status, x)
{
  $("#failureModal").modal("hide");
  $("#order-container").empty();
  
  $new_row = null;
  
  current_orders = data;

  for (order in data)
  {
    if (order % 4 == 0)
    {
      $new_row = $("#template-row").clone().removeClass("template-div");
      $new_row.appendTo("#order-container");
    }
    
    var show_progress_bar = false;
    
    var order_origin = parseInt(data[order].OrderOrigin);
    var order_type = "";
    var order_time_display = "";
    
    var item_count = data[order].ItemCount;
    item_count += item_count == 1 ? " item" : " items";
    
    var border_color = "border-primary";
    var header_color = "bg-primary";

    /*"2021-03-06 14:15:43"*/
    var pickup_time = data[order].OrderPickup.split(" ")[1];
    var pickup_date = data[order].OrderPickup.split(" ")[0];
    
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
    
    var warning_time = new Date(Date.now() + warning_minutes*60*1000);
    
    if (order_pickup.getTime() < Date.now())
    {
      border_color = "border-danger";
      header_color = "bg-danger";
    }
    else if (order_pickup.getTime() < warning_time)
    {
      border_color = "border-warning";
      header_color = "bg-warning";
    }
    
    var today = new Date();
    if ((today.toDateString() == order_pickup.toDateString()))
    {
      order_time_display = pickup_hours + ":" + pickup_minutes + " " + am_pm;
    }
    else
    {
      order_time_display = order_pickup.getMonth()+1 + "/" + order_pickup.getDate();
    }
    
    $new_card = $("#template-card").clone().removeClass("template-div");
        
    switch (order_origin)
    {
      case 1:
        order_type = "Online";
        break;
        
      case 2:
        order_type = "Kiosk";
        break;
        
      case 3:
        order_type = "Waiting";
        var waiting_time = Math.ceil((Date.now() - order_pickup.getTime())/(60*1000));
        var wait_percent = Math.abs(max_wait_time - waiting_time)*100 / max_wait_time;
        
        order_time_display = "waiting " + waiting_time + (waiting_time != 1 ? " minutes" : " minute");
        item_count = "";
        show_progress_bar = true;
        
        if (waiting_time > max_wait_time)
        {
            border_color = "border-danger";
            $new_card.find("#bar-ok").css("width",   "0%");
            $new_card.find("#bar-bad").css("width", "100%");
        }
        else
        {
            border_color = "border-info";
            $new_card.find("#bar-ok").css("width", wait_percent + "%");
            $new_card.find("#bar-bad").css("width", (100 - wait_percent) + "%");
        }
        break;
        
      default:
        order_type = "Unknown";
        break;
    }
    
    $new_card.on('mouseup', card_click);
    $new_card.find(".header-text").text(order_type);
    $new_card.find(".card-header").addClass(header_color);
    $new_card.addClass(border_color);
    $new_card.find(".card-title").text("#" + data[order].OrderNumber);
    $new_card.find("#time").text(order_time_display);
    
    $new_card.find("#size").text(item_count);
    
    if (!show_progress_bar)
    {
      $new_card.find("#waiting-bars").css("display","none");
    }
    
    $new_card.appendTo($new_row);
  }

  $("#order-container").hide();
  $("#order-container").removeClass("invisible");
  $("#order-container").fadeTo(300, 1);
}

function load_orders()
{
  var futureTime = new Date(Date.now());

  futureTime = new Date(futureTime.getTime() + currentSettings.OrderListMinutes*60000);

  var dateString = new Date(futureTime.getTime() - (futureTime.getTimezoneOffset() * 60000)).toISOString().split("T")[0];
  dateString += " " + futureTime.getHours() + ":" + futureTime.getMinutes();

  var url = "/api/v1/orderparts/?sort=OrderPickup&limit=" + currentSettings.OrderListMax + "&OrderPickup.neq=null&OrderStatus.gte=1&OrderStatus.lte=2&DeptNum=" + getDept() + "&ZoneNum=" + getZone();

  url += "&OrderPickup.lte=" + dateString;
  
  warning_minutes = currentSettings.WarningTime;
  max_wait_time = currentSettings.NowServingWaitTime;
  
  var disp = function() {
      $.getJSON(url, null, display_orders).fail(function(){
        failed("Failed to load orders!");
      });
  };
  
  if (!$("#order-container").hasClass("invisible"))
  {
    $("#order-container").fadeTo(300, 0, disp);
  }
  else
  {
    disp();
  }
};
