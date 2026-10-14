 /* add jquery function to resize text to fit buttons */

$().ready(function(){
  (function($) {
    $.fn.textfill = function(maxFontSize) {
      maxFontSize = parseInt(maxFontSize, 10);
      return this.each(function(){
        var ourText = $("span", this),
        parent = ourText.parent(),
        maxHeight = parent.height(),
        maxWidth = parent.width(),
        fontSize = parseInt(ourText.css("fontSize"), 10),
        multiplier = maxWidth/ourText.width(),
        newSize = (fontSize*(multiplier-0.1));
        ourText.css(
          "fontSize", 
          (maxFontSize > 0 && newSize > maxFontSize) ? maxFontSize : newSize);
      });
    };
  })(jQuery);

  load_products();
});

var products = [];
var current_page = 0;

function load_products()
{
  var DeptNum = getParams("DeptNum");
  var CaNum = getParams("CaNum");
  var search = getParams("search");
  var bestSellers = getParams("bestSellers");
  
  if (!DeptNum) DeptNum = 0;
  if (!CaNum) CaNum = 0;
  
  if (search)
  {
    // limit=48&PrDesc.search
    $.getJSON("/api/v1/products/?sort=PrDesc&dir=asc&DeptNum="+DeptNum+"&limit=48&PrDesc.search="+search).done(function(data, status, x) {
     products = data;
      current_page = 0;
      draw_page();
    }).fail(function(){failed("Failed to load products!")});
  }
  else if (bestSellers)
  {
    $.getJSON("/api/v1/products/"+DeptNum+"/popular").done(function(data, status, x) {
     products = data;
      current_page = 0;
      draw_page();
    }).fail(function(){failed("Failed to load products!")});
  }
  else
  {
    $.getJSON("/api/v1/products/?sort=PrDesc&dir=asc&DeptNum="+DeptNum+"&CaNum="+CaNum).done(function(data, status, x) {
     products = data;
      current_page = 0;
      draw_page();
    }).fail(function(){failed("Failed to load products!")});
  }
}

function item_click_factory(PrNum)
{
  return function()
  {
    window.location.href = "/OnlineOrdering/kiosk/item.html?DeptNum=" + getParams("DeptNum")+"&PrNum="+PrNum;
  }
}

function draw_page()
{
  if (current_page == 0)
  {
    $("#prevButton").addClass("grayOut");
  }
  else
  {
    $("#prevButton").removeClass("grayOut");
  }
  
  if ((current_page+1)*12 >= products.length)
  {
    $("#nextButton").addClass("grayOut");
  }
  else
  {
    $("#nextButton").removeClass("grayOut");
  }
  
  $(".products-row").empty();
  
  if (products.length == 0)
  {
    $('#nothingModal').modal({backdrop:'static',keyboard:false, show:true});
    $("#nothingModal").modal("show");
    return;
  }
  
  var r = 0;
  for (var i = 12*current_page; i < 12*(current_page+1); i++)
  {
    if (i != 12*current_page && i % 4 == 0) r += 1;
    
    var $add_cat;
    
    if (i < products.length)
    {
      var prod = products[i];
      
      $add_cat = $("#product-template").clone(true, true);
      $add_cat.click(item_click_factory(prod.PrNum));

      $add_cat.find("span").text(prod.PrDesc);
      
      if (prod.PrImage)
      {
        var url = "/OnlineOrdering/static/images/products/" + prod.PrImage ;
        $add_cat.find("button").first().css('background-image', 'url("' + url + '")');
      }
      
      if (prod.PrPrice)
      {
        var price = prod.PrPrice;
        $price = $add_cat.find(".product-price").first();
        
        $normal_price = $price.find(".normal-price").first();
        $normal_price.text(PriceToStringUnits(normalPrice(prod), parseInt(prod.PrType)));
        
        if (onSale(prod))
        {
          $sale_price = $price.find(".sale-price").first();
          $sale_price.text(PriceToStringUnits(salePrice(prod), parseInt(prod.PrType)));
          $price.addClass("on-sale");
        }
        else
        {
          $price.addClass("not-on-sale");
        }
      }
    }
    else
    {
      $add_cat = $("#spacer-template").clone(true, true);
    }
    
    $add_cat.removeClass("template-div");
    $add_cat.appendTo($(".products-row")[r]);
    
    // resize text to fit nicely
    $add_cat.find(".btn").first().textfill(32);
  }
}

function next_page()
{
  if (current_page < (products.length / 12) - 1)
  {
    current_page += 1;
    draw_page();
  }
}

function prev_page()
{
  if (current_page > 0)
  {
    current_page -= 1;
    draw_page();
  }
}


