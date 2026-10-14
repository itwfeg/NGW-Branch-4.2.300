$().ready(function(){
    reload_stats();
    
    var reload_timer = setInterval(reload_stats, 10000);
});

function reload_stats()
{
    var dept = getParams("DeptNum");
    if (!dept) dept = 0;
    
    $.getJSON("/api/v1/timeslots/" + dept, function(data) {
        if (data.length > 0)
        {
          write_kiosk(data[0][1]);
        }
        else
        {
          write_kiosk(null);
        }
    }).fail(function() {write_kiosk(null); console.log("failed to get timeslot info");});
    
    $.getJSON("/api/v1/queue/" + dept, function(data) {
        write_queue(data["QueueLength"], data["EstimatedWait"], data["NowServing"]%100);
    }).fail(function() {write_queue("?", "?", "?"); console.log("failed to get queue info");});
}

function write_queue(queue_size, queue_time, queue_head)
{
    var new_current = queue_head.toString().length == 1 && queue_head != "?" ? "0"+queue_head.toString() : queue_head.toString();
    var new_line = queue_size;
    var new_wait = "" + queue_time + " minutes";
    
    document.getElementById("tspan1694").textContent = new_current;
    document.getElementById("tspan1716").textContent = new_wait;
    document.getElementById("tspan1714").textContent = new_line;
}

function slot_to_date(slot)
{
    var slot_time = slot.split(" ")[1];
    var slot_date = slot.split(" ")[0];
    
    return new Date(slot_date.split("-")[0],
                    parseInt(slot_date.split("-")[1])-1,
                    slot_date.split("-")[2],
                    slot_time.split(":")[0],
                    slot_time.split(":")[1],
                    slot_time.split(":")[2]);
}

function write_kiosk(next_time)
{
  var next_text = "?";
  
  if (next_time)
  {
    var now = new Date();
    var next_slot = slot_to_date(next_time);

    next_text = parseInt((next_slot - now)/60000);
  }
  
  document.getElementById("tspan1714-5").textContent = next_text;
}

