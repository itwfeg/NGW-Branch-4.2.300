function view_cart()
{
  window.location.href = "/OnlineOrdering/kiosk/cart.html?DeptNum=" + getParams("DeptNum");
}

function start_over_button()
{
  $("#restartModal").modal("show");
}

function start_over()
{
  localStorage.setItem("cart", "");
  window.location.href = "/OnlineOrdering/kiosk/index.html?DeptNum=" + getParams("DeptNum");
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
