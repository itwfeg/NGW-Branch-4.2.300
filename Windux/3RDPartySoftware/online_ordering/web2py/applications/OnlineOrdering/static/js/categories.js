$().ready(function(){
  load_categories();
});

var categories = [];
var current_page = 0;

function load_categories()
{
  var DeptNum = getParams("DeptNum");
  
  if (!DeptNum) DeptNum = 0;
  
  $.getJSON("/api/v1/categories/" + DeptNum + "?sort=CaNum&dir=asc").done(function(data, status, x) {
    categories = data;
    current_page = 0;
    draw_page();
  }).fail(function(){failed("Failed to load categories!");});
}

function get_prod_link(CaNum, DeptNum)
{
  return function() {window.location = "/OnlineOrdering/kiosk/products?DeptNum="+DeptNum+"&CaNum="+CaNum;};
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
  
  if ((current_page+1)*9 >= categories.length)
  {
    $("#nextButton").addClass("grayOut");
  }
  else
  {
    $("#nextButton").removeClass("grayOut");
  }
  
  $(".kiosk-row").empty();
  
  // no categories? put some buttons up instead
  if (categories.length == 0)
  {
    // make a search button
    var $add_cat = $("#category-template").clone(true, true);
    var $button = $add_cat.find("button").first();
    $button.text("Search");
    $button.click(show_search);
    
    $add_cat.removeClass("template-div");
    $add_cat.appendTo($(".kiosk-row")[1]);
    
    $("#spacer-template").removeClass("template-div").clone(true, true).appendTo($(".kiosk-row")[0]);
    
    // make a best sellers button
    $add_cat = $("#category-template").clone(true, true);
    $button = $add_cat.find("button").first();
    $button.text("Best Sellers");
    $button.click(best_sellers);
    
    $add_cat.removeClass("template-div");
    $add_cat.appendTo($(".kiosk-row")[1]);
    
    return;
  }
  
  var r = 0;
  for (var i = 9*current_page; i < 9*(current_page+1); i++)
  {
    if (i != 9*current_page && i % 3 == 0) r += 1;
    
    var $add_cat;
    
    if (i < categories.length)
    {
      $add_cat = $("#category-template").clone(true, true);
      $button = $add_cat.find("button").first();
      $button.text(categories[i].CaName1);
      
      if (categories[i].CaImage)
      {
        var url = "/OnlineOrdering/static/images/categories/" + categories[i].CaImage ;
        $button.css('background-image', 'url("' + url + '")');
      }
      
      $button.click(get_prod_link(categories[i].CaNum, categories[i].DeptNum));
    }
    else
    {
      $add_cat = $("#spacer-template").clone(true, true);
    }
    
    $add_cat.removeClass("template-div");
    $add_cat.appendTo($(".kiosk-row")[r]);
  }
}

function next_page()
{
  if (current_page < (categories.length / 9) - 1)
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

function show_search()
{
  $('#searchModal-string').val('');
  $('#searchModal').modal('show');
  $('#searchModal-string').focus();
}

function best_sellers()
{
  window.location = "/OnlineOrdering/kiosk/products?DeptNum="+getParams("DeptNum")+"&CaNum=0&bestSellers=1";
}

function search()
{
  window.location = "/OnlineOrdering/kiosk/products?DeptNum="+getParams("DeptNum")+"&CaNum=0&search="+$("#searchModal-string").val();
}
