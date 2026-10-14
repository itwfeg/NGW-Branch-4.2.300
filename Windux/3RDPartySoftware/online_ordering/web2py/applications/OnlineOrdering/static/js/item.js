$().ready(function(){
  $('#item-amount').on("keypress", function(e){
    var txt = String.fromCharCode(e.which);
    
    // if enter, clean it up
    if (e.which == 13) { clean_amount();return true;/**/}
    
    // only allow numbers and period
    if (!txt.match(/[0-9\.]/) && e.which != 8)
    {
      e.preventDefault();
      return false;
    }
    
    // don't allow a period if we have one already
    if (txt == "." && $("#item-amount").val().indexOf(".") != -1)
    {
      e.preventDefault();
      return false;
    }
    
    anything_input = true;
    
    return true;
  });

  $('#item-amount').val("0.00");
  
  $('#item-amount').focus(function(){
    anything_input = false;

    old_amount = $('#item-amount').val();
    
    $('#item-amount').val("");
    return true;
  });
  
  $('#item-amount').focusout(function() {
    if (!anything_input) $('#item-amount').val(old_amount);
    
    clean_amount();
    return true;
  });

  //$.getJSON("/api/v1/products/" + getParams("DeptNum") + "/" + getParams("PrNum"), draw_item).fail(function(){failed("Failed to load item!");});
  $.getJSON("/api/v1/departmentsettings/"+getParams("DeptNum")).done(function(data) {
    get_prep_options(data);
    $.getJSON("/api/v1/products/" + getParams("DeptNum") + "/" + getParams("PrNum"), draw_item).fail(function(){failed("Failed to load item!");});;
  }).fail(function(){failed("Failed to load item prep options!");});
});

// preserve the "old" value when the amount input is clicked, and put back maybe
var anything_input = false;
var old_amount = "0.00";

var prod = null;
var prodActualPrice = null;

function draw_item(data)
{
  prod = data[0];

  $("#item-name").text(prod.PrDesc);
  
  if (onSale(prod))
  {
    prodActualPrice = salePrice(prod);
    $("#product-price").addClass("on-sale");
  }
  else
  {
    prodActualPrice = normalPrice(prod);
    $("#product-price").addClass("not-on-sale");
  }
  
  $("#sale-price").text(PriceToStringUnits(salePrice(prod), parseInt(prod.PrType)));
  $("#normal-price").text(PriceToStringUnits(normalPrice(prod), parseInt(prod.PrType)));
  
  switch (parseInt(prod.PrType))
  {
    case 1:
      $("#units").text("lbs");
      $('#item-amount').val(currentSettings.OrderItemWeightMin);
      break;
    case 4:
      $('#item-amount').val(currentSettings.OrderItemWeightMin);
      $("#units").text("oz");
      break;
    case 2:
    case 3:
      $("#units").text("");
      $('#item-amount').val("1");
      $(".quarter-change").css("display","none");
      break;
    default:
      break;
  }
  
  update_subtotal($('#item-amount').val());

  var image_url = "/OnlineOrdering/static/images/no_image.svg";
  if (prod.PrImage)
  {
    image_url = "/OnlineOrdering/static/images/products/" + prod.PrImage;
  }
  
  $("#item-image").css('background-image', 'url("' + image_url + '")');
  
  var cart = localStorage.getItem("cart");
  if (cart)
  {
    cart = JSON.parse(cart);
  }
  else
  {
    cart = [];
  }
  
  // check if we already have this item in the cart
  var revisit = false;
  var cart_index = -1;
  
  for (i in cart)
  {
    if (cart[i]["PrNum"] == prod.PrNum)
    {
      revisit = true;
      cart_index = i;
    }
  }
  
  if (revisit)
  {
    var row = cart[cart_index];
    
    $('#item-amount').val(row["amount"]);
    var instructions = row["instructions"];
    
    if (instructions.startsWith("Prep: "))
    {
      instructions = instructions.slice(6);
      
      var prep = instructions.split(" ")[0];
      prep = prep.slice(0, -1);
      instructions = instructions.slice(prep.length+4, instructions.length);
      
      $(".prep-button").addClass("grayOut");
      $(".prep-text:contains('" + prep + "')").parent().removeClass("grayOut");
    }
    
    $("#instructions").val(instructions);
    
    $("#item-subtotal").text(row["subtotal"]);
  }
  
  update_buttons();
}

function update_subtotal(qty)
{
  $("#item-subtotal").text(PriceToString(Math.round(qty*prodActualPrice)));
}

function update_buttons()
{
  var amount = parseFloat($("#item-amount").val());
  
  var maximum = null;
  
  var minimum = null;
  
  if (prod.PrType == 1) minimum = parseFloat(currentSettings.OrderItemWeightMin); else minimum = 1;
  
  if (prod.PrType == 1) maximum = parseFloat(currentSettings.OrderItemWeightMax); else maximum = parseFloat(currentSettings.OrderItemQtyMax);
  
    if (amount > maximum - 1)
    {
      $(".full-change").first().addClass("grayOut");
    }
    else
    {
      $(".full-change").first().removeClass("grayOut");
    }
    
    if (amount > maximum - 0.25)
    {
      $(".quarter-change .btn-custom-color").first().addClass("grayOut");
    }
    else
    {
      $(".quarter-change .btn-custom-color").first().removeClass("grayOut");
    }
    
    if (amount - 1 < minimum)
    {
      $(".full-change").eq(1).addClass("grayOut");
    }
    else
    {
      $(".full-change").eq(1).removeClass("grayOut");
    }
    
    if (amount - 0.25 < minimum)
    {
      $(".quarter-change .btn-custom-color").eq(1).addClass("grayOut");
    }
    else
    {
      $(".quarter-change .btn-custom-color").eq(1).removeClass("grayOut");
    }
}

function clean_amount()
{
  var amount =  $("#item-amount").val();
  var whole = "";
  var fractional = "";
  
  if (amount.indexOf(".") != -1)
  {
    whole = amount.split(".")[0];
    fractional = amount.split(".")[1];
    
    if (!whole)
    {
    whole = "0";
    }
    if (!fractional)
    {
    fractional = "00";
    }
  }
  else
  {
    whole = amount;
    fractional = "00";
  }
  
  if (fractional.length < 2) fractional += "0";
  if (fractional.length > 2) fractional = fractional.slice(0,2);
  
  if (prod.PrType == 1)
  {
    if (parseFloat(whole+"."+fractional) > parseFloat(currentSettings.OrderItemWeightMax))
    {
      failed("The maximum weight is " + currentSettings.OrderItemWeightMax + " " + $("#units").text());
      $("#item-amount").val(currentSettings.OrderItemWeightMax);
      update_subtotal(parseFloat(whole+"."+fractional));
      update_buttons();
      clean_amount();
      return false;
    }
    
    if (parseFloat(whole+"."+fractional) < parseFloat(currentSettings.OrderItemWeightMin))
    {
      failed("The minimum weight is " + currentSettings.OrderItemWeightMin + " " + $("#units").text());
      $("#item-amount").val(currentSettings.OrderItemWeightMin);
      update_subtotal(parseFloat(whole+"."+fractional));
      update_buttons();
      clean_amount();
      return false;
    }
    
    $("#item-amount").val(whole+"."+fractional);
    update_subtotal(parseFloat(whole+"."+fractional));
  }
  else
  {
    if (parseInt(whole) > currentSettings.OrderItemQtyMax)
    {
      failed("The maximum quantity is " + currentSettings.OrderItemQtyMax);
      $("#item-amount").val(currentSettings.OrderItemQtyMax);
      update_subtotal(parseFloat(whole));
      update_buttons();
      clean_amount();
      return false;
    }
    
    if (parseInt(whole) < 1)
    {
      $("#item-amount").val(1);
      update_subtotal(parseFloat(1));
      update_buttons();
      clean_amount();
      return false;
    }
  
    $("#item-amount").val(whole);
    update_subtotal(parseFloat(whole));
  }
  
  update_buttons();
  
  return false;
}

function change_amount(element, diff)
{
  // don't change if the button is disabled
  if ($(element).hasClass("grayOut")) return;
  
  clean_amount();
  
  var amount = $("#item-amount").val();
  var whole = parseInt(amount.split(".")[0]);
  var fractional = parseInt(amount.split(".")[1]);
  
  switch (diff)
  {
    case "1":
      whole += 1;
      break;
    case "-1":
      whole -= 1;
      break;
    case "0.25":
      fractional += 25;
      break;
    case "-0.25":
      fractional -= 25;
      break;
  }
  
  if (fractional >= 100)
  {
    whole += 1;
    fractional -= 100;
  }
  
  if (fractional < 0 && whole > 0)
  {
    whole -= 1;
    fractional += 100;
  }
  
  if (fractional < 0) fractional = 0;
  
  if (whole < 0)
  {
    fractional = 0;
    whole = 0;
  }
  
  if (fractional == 0) fractional = "00";
  
  if (prod.PrType == 1)
  {
    $("#item-amount").val(whole+"."+fractional);
  }
  else
  {
    $("#item-amount").val(whole);
  }
  
  update_subtotal(parseFloat(whole+"."+fractional));
  
  update_buttons();
}

function select_prep(element)
{  
  $(".prep-button").addClass("grayOut");
  
  $(element).removeClass("grayOut");
}

function add_to_cart()
{
  var cart = localStorage.getItem('cart');
  
  if (cart)
  {
    cart = JSON.parse(cart);
  }
  else
  {
    cart = [];
  }
  
  var row = {};
  
  var revisit = false;
  var cart_index = -1;
  
  for (i in cart)
  {
    if (cart[i]["PrNum"] == prod.PrNum)
    {
      revisit = true;
      cart_index = i;
    }
  }
  
  // overwrite existing item if we have one
  if (revisit)
  {
    row = cart[cart_index];
  }
  
  console.log(row);
  
  row["instructions"] = "";

  var prep = $(".prep-button").not(".grayOut");
  
  if (prep.length == 1)
  {
    row["instructions"] += "Prep: " + $(prep).find("div").first().text() + ". ";
  }
  
  row["instructions"] += $("#instructions").val();
  
  row["amount"] = $("#item-amount").val();
  
  row["PrNum"] = prod.PrNum;
  
  row["PrType"] = prod.PrType;
  
  row["subtotal"] = $("#item-subtotal").text();
  
  row["units"] = $("#units").text();
  
  row["name"] = $("#item-name").text();
  
  if (revisit)
  {
    cart[cart_index] = row;
  }
  else
  {
    cart.push(row);
  }
  
  localStorage.setItem('cart', JSON.stringify(cart));
  
  $("#addedModal").modal("show");
}

function get_prep_options(data)
{
  var options = data[0]["PrepOptions"].split(",");
  
  if (options[0] === "")
  {
    $("#prep-options").hide();
  }
  
  for (i in options)
  {
    var $prep = $("#prep-template").clone();
    $prep.removeClass("template-div");
    $prep.find(".prep-text").text(options[i]);
    $("#prep-options").append($prep);
  }
}
