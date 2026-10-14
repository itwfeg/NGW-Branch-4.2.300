var idle_time = 0;

$().ready(function() {
  $("#orderNumber").text(getParams("OrderNumber"));
  $("#pickupTime").text(decodeURIComponent(getParams("pickupTime")));
  $("#phoneModal-phone").val("(___) ___-____");
  
  if (currentSettings.EnableCustomerSMS != 1)
  {
    $("#textButton").addClass("grayOut");
    $("#textButton").unbind('click');
  }
  else
  {
   $("#textButton").click(function() {
     $("#phoneModal-phone").val("(___) ___-____");
     $('#phoneModal').modal('show');
     $("#phoneModal-phone").focus();
   });
  }
  
  // timer to track idle time
  var refresh_timer = setInterval(timer_increment, 1000);

  $(this).mousemove(function (e) {
    idle_time = 0;
  });

  $(this).keypress(function (e) {
    idle_time = 0;
  });
  
  $(this).mousedown(function (e) {
    idle_time = 0;
    return true;
  });
  
  $('#phoneModal-phone').on("keypress", function(e){
    var txt = String.fromCharCode(e.which);
    var number = get_phone();
    
    // only allow numbers
    if (!txt.match(/[0-9]/))
    {
      e.preventDefault();
      return false;
    }

    number += txt;
    put_phone(number);
    
    e.preventDefault();
    return true;
  }).on("keydown", function(e){
    // backspace has to be handled with keydown
    if (e.which == 8)
    {
      var number = get_phone();
      
      number = number.slice(0,-1);
      
      put_phone(number);
      e.preventDefault();
      return true;
    }
    
    return true;
  }).on("focusin", function(e) {
    put_phone(get_phone());
    e.preventDefault();
    return true;
  }).on("click",  function(e) {
    put_phone(get_phone());
    e.preventDefault();
    return true;
  });
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

function send_text()
{
  var number = get_phone();
  
  $("#phoneModal").addClass("grayOut");
  
  $.ajax({
    url: "/api/v1/orders/" + getParams("OrderNumber"),
    type: 'PUT',
    data: JSON.stringify({"OrderSMS": number}),
    contentType: "application/json"
  }).done(function() {
    start_over();
  })
  .fail(function() {
    failed("Failed save phone number!");
  });

};

function put_phone(number)
{
  var out = "(___) ___-____";
  
  number = number.toString();
  
  while (number.length > 0)
  {
    out = out.replace("_", number[0]);
    number = number.slice(1);
  }
  
  $("#phoneModal-phone").val(out);
  
  var cur = out.indexOf("_");
  if (cur == -1) cur = out.length;
  
  $("#phoneModal-phone")[0].setSelectionRange(cur, cur);
}

function get_phone()
{
  return $("#phoneModal-phone").val().replace(/\D/g,'');
};
