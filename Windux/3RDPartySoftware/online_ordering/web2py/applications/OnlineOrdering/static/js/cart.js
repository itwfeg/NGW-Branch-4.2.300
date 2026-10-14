$().ready(function() {
  load_cart();
});

function show_times()
{
  $('#pickupModal-submit').addClass('disabled');
  $("#pickupModal").modal("show");
  
  $.getJSON("/api/v1/timeslots/"+getParams("DeptNum")).done(function(data) {
    if (data.length == 0)
    {
      $("#pickupModal").modal("hide");
      failed("Too close to closing time to place an order!");
    }
    $("#pickupModal-times").find("option").remove();
    
    $.each(data, function (i, slot) {
      $('#pickupModal-times').append($('<option>', { value: slot[1],  text: slot[0] }));
    });
  
    $('#pickupModal-submit').removeClass('disabled');
  });
}

function load_cart()
{
  $("#cart-container").empty();
  
  var cart = localStorage.getItem("cart");

  if (!cart) return;

  cart = JSON.parse(cart);

  for (line in cart)
  {
    var row = cart[line];

    var $new_row = $("#cart-template-row").clone().removeClass("template-div");
    $new_row.find(".subtotal").text(row["subtotal"]);
    $new_row.find(".name").text(row["name"]);
    $new_row.find(".instructions").text(row["instructions"]);
    $new_row.find(".amount").text(row["amount"] + " " + row["units"]);
    
    $new_row.find(".examine").click(function(event) {
      window.location.href = "/OnlineOrdering/kiosk/item.html?DeptNum=" + getParams("DeptNum") + "&PrNum=" + row["PrNum"];
    });
    
    $new_row.find(".remove").click(function(event) {
      $("#removeModal-remove").unbind('click');
      var item = $(event.currentTarget).parent().parent().index();
      $("#removeModal-remove").click(function(event) {
        var edit_cart = JSON.parse(localStorage.getItem("cart"));
        edit_cart.splice(item, 1);
        localStorage.setItem("cart", JSON.stringify(edit_cart));
      
        load_cart();
      });
      
      $("#removeModal").modal("show");
    });
    
    $new_row.appendTo("#cart-container");
  }
};

function examine(index)
{
  alert(index);
  // index;
}

function place_order()
{
  $(".modal").modal('hide');
  var cart = localStorage.getItem("cart");

  if (!cart)
  {
    failed("You have nothing in your cart!");
    return;
  }
  
  $("body").addClass("grayOut");
  $("#orderButton").unbind('click');
  
  var order = {};
  order["DeptNum"] = getParams("DeptNum");
  order["OrderCreation"] = time_now();
  order["OrderPickup"] = $("#pickupModal-times option:selected").val();
  order["OrderStatus"] = orderStatusVal["Waiting"];
  order["OrderOrigin"] = orderTypeVal["Kiosk"];
  order["CustomerName"] = "Kiosk User";
  order["Items"] = get_items();
  
  $.ajax({
      url: "/api/v1/orders",
      type: 'POST',
      data: JSON.stringify([order]),
      contentType: "application/json"
  }).done(function(data) {
    var OrderNumber = data[0];
    window.location.href = "/OnlineOrdering/kiosk/customer.html?DeptNum=" + getParams("DeptNum") + "&OrderNumber=" + OrderNumber + "&pickupTime=" + $("#pickupModal-times option:selected").text();
  }).fail(function() {
    failed("Failed to submit order!");
    $("body").removeClass("grayOut");
    $("#orderButton").click(place_order);
  });
};

function get_items()
{
    var cart = JSON.parse(localStorage.getItem("cart"));
    var details = [];

    for (line in cart)
    {
      var row = cart[line];
      var detail = {};
      
      detail["DeptNum"] = parseInt(getParams("DeptNum"));
      //detail["OrderNumId"] = parseInt(line) + 1;
      //detail["OrderNumber"] = parseInt(OrderNumber);
      detail["PluInstructions"] = row["instructions"];
      detail["PrNum"] = row["PrNum"];
      
      if (row["PrType"] == 3 || row["PrType"] == 2)
      {
        detail["PluByCount"] = parseInt(row["amount"]);
      }
      else
      {
        detail["PluWeight"] = Math.floor(parseFloat(row["amount"])*1000);
        console.log(row["amount"]);
        console.log(parseFloat(row["amount"])*1000);
        console.log(Math.floor(parseFloat(row["amount"])*1000));
      }
      
      details.push(detail);
    }
    
    console.log(details);
    
    return details;
}
