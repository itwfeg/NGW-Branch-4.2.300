var idle_time = 0;

$().ready(function() {
  $("#orderNumber").text(getParams("OrderNumber"));

  $("#wait").text(getParams("wait"));
  
  // timer to track idle time
  var refresh_timer = setInterval(timer_increment, 1000);
});

function timer_increment()
{
    if (!currentSettings.KioskTimeout) return;
    
    idle_time++;
    
    if (idle_time >= currentSettings.KioskTimeout)
    {
      start_over();
    }
}

