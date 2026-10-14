$().ready(function(){
  var DeptNum = getParams("DeptNum");

  if (!DeptNum)
  {
    DeptNum = 0;
    window.location.href = "/OnlineOrdering/kiosk/index.html?DeptNum=0";
  }

  specialize();
  check_open();
  
  var refresh_timer = setInterval(check_open, 30000);
});

function check_open()
{
  var DeptNum = getParams("DeptNum");
  
  if (!DeptNum) DeptNum = 0;
  
  $.getJSON("/api/v1/departmentsettings/"+DeptNum).done(function(data) {
    if (data.length > 0)
    {
      var now = new Date();
      var todayOpen;
      var todayClose;
      
      switch(now.getDay())
      {
        case 0:
          todayOpen = data[0]["SundayOpen"]
          todayClose = data[0]["SundayClose"]
          break;
        case 1:
          todayOpen = data[0]["MondayOpen"]
          todayClose = data[0]["MondayClose"]
          break;
        case 2:
          todayOpen = data[0]["TuesdayOpen"]
          todayClose = data[0]["TuesdayClose"]
          break;
        case 3:
          todayOpen = data[0]["WednesdayOpen"]
          todayClose = data[0]["WednesdayClose"]
          break;
        case 4:
          todayOpen = data[0]["ThursdayOpen"]
          todayClose = data[0]["ThursdayClose"]
          break;
        case 5:
          todayOpen = data[0]["FridayOpen"]
          todayClose = data[0]["FridayClose"]
          break;
        case 6:
          todayOpen = data[0]["SaturdayOpen"]
          todayClose = data[0]["SaturdayClose"]
          break;
      }
      
      todayOpen = todayOpen.split(":");
      todayClose = todayClose.split(":");
      todayOpen = (new Date()).setHours(todayOpen[0], todayOpen[1], todayOpen[2], 0);
      todayClose = (new Date()).setHours(todayClose[0], todayClose[1], todayClose[2], 0);
      todayOpen = new Date(todayOpen);
      todayClose = new Date(todayClose);
      
      if (todayOpen < now && now < todayClose)
      {
        open_kiosk();
      }
      else
      {
        close_kiosk();
      }
      
    }
    else
    {
      close_kiosk();
    }
  }).fail(function() {
    // since this is customer facing and they can't do anything, don't show?
    //failed("Failed to load department information!");
  });
}

function close_kiosk()
{
  $("#startOrderButton").prop("disabled", true);
  $("#startNumberButton").prop("disabled", true);
  $('#closedModal').modal('show');
  // don't allow the closed modal to be dismissed
  $('#closedModal').on('hide.bs.modal', function () { return false; });
}

function open_kiosk()
{
  $("#startOrderButton").prop("disabled", false);
  $("#startNumberButton").prop("disabled", false);
  // re-allow the closed modal to be dismissed
  $('#closedModal').off('hide.bs.modal');
  $('#closedModal').modal('hide');
}

function specialize()
{
  var DeptNum = getParams("DeptNum");

  // only put the department name in the button if it's not "Default"
  if (DeptNum != 0)
  {
    $.getJSON("/api/v1/departments/"+DeptNum).done(function(data) {
      if (data.length > 0 && data[0]["DeptName1"] != "")
      {
        $("#startOrderButton").html("Place<br>" + data[0]["DeptName1"] + "<br>Order");
      }
    }).fail(function(){failed("Failed to load department information!")});
  }
}

function place_order()
{
  var DeptNum = getParams("DeptNum");
  if (!DeptNum) DeptNum = 0;
  
  window.location.href = "/OnlineOrdering/kiosk/categories.html?DeptNum=" + DeptNum;
}

function take_number()
{
  $("body").addClass("grayOut");
  
    var order = {};
    order["DeptNum"] = getParams("DeptNum");
    order["OrderCreation"] = time_now();
    order["OrderPickup"] = time_now();
    order["OrderStatus"] = orderStatusVal["Waiting"];
    order["OrderOrigin"] = orderTypeVal["Number"];
    order["CustomerName"] = "Now Serving User";
  
  $.post("/api/v1/orders", JSON.stringify([order])).done(function(data) {
      var OrderNumber = data[0];
      var dept = getParams("DeptNum");
      if (dept == "") dept = "0";
      
      $.getJSON("/api/v1/queue/"+dept).done(function(data) {
         window.location.href = "/OnlineOrdering/kiosk/number.html?DeptNum=" + getParams("DeptNum") + "&OrderNumber=" + OrderNumber + "&wait=" + data["EstimatedWait"];
      });
    })
    .fail(function() {
      failed("Failed to add to queue!");
      $("body").removeClass("grayOut");
    });
}
