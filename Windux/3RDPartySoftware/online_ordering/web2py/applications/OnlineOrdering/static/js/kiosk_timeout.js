var idle_time = 0;

$(document).ready(function(){
  // timer to track idle time
  var refresh_timer = setInterval(timer_increment, 1000);

  $(this).mousemove(function (e) {
    if (!$("body").hasClass("modal-open"))
    {
        idle_time = 0;
        draw_timer();
    }
  });

  $(this).keypress(function (e) {
    idle_time = 0;
    draw_timer();
  });
  
  $(this).mousedown(function (e) {
    idle_time = 0;
    draw_timer();
    return true;
  });
});

function draw_timer()
{
    $("#timeoutModal-time").text(currentSettings.KioskTimeout*2 - idle_time);
    
    if (idle_time > currentSettings.KioskTimeout)
    {
        // close any open modals first
        $('.modal').modal('hide');
        
        $("#timeoutModal").modal("show");
    }
}

function timer_increment()
{
    if (!currentSettings.KioskTimeout) return;
    
    idle_time++;
    
    if (idle_time >= currentSettings.KioskTimeout * 2)
    {
      start_over();
    }
    
    draw_timer();
}
