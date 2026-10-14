$(document).ready(function(){
  load_settings();
  
  $("#settings-reload").mouseup(function(){
    load_settings();
  });
  
  $("#settings-default").mouseup(function(){
    load_default_settings();
  });
  
  $("#settings-submit").mouseup(function(){
    submit_settings();
    submit_dept_settings();
  });
  
  /*$('[data-toggle="popover"]').popover()*/
  $(".popover-button").popover();
  
  $('.popover-dismiss').popover({
    trigger: 'focus'
  });
  
  $('input.timepicker').timepicker({
    timeFormat: 'h:mm p',
    interval: 15,
    dynamic: false,
    dropdown: false,
    scrollbar: false
  });
  
  load_zones();
  draw_upload();
});


/* takes a time like "17:00:00" and returns "5:00 PM" */
function db_time_to_am_pm(t)
{
  var hour = parseInt(t.split(":")[0], 10);
  var minutes = t.split(":")[1];
  var ampm = "XM";

  if (hour == 0)
  {
    ampm = "AM";
    hour = "12";
  }
  else if (hour < 12)
  {
    ampm = "AM";
  }
  else if (hour == 12)
  {
    ampm = "PM";
  }
  else if (hour > 12)
  {
    hour -= 12;
    ampm = "PM";
  }

  return hour + ":" + minutes + " " + ampm;
}

function am_pm_to_db_time(t)
{
  var hour = parseInt(t.split(":")[0],10);
  var minutes = t.split(":")[1].split(" ")[0];
  
  if (t.split(" ")[1] == "AM")
  {
    if (hour == 12) hour -= 12;
  }
  else
  {
    if (hour != 12) hour += 12;
  }
  
  return hour + ":" + minutes + ":00";
}

function display_settings(data, status, x)
{
  if (status != "success")
  {
    failed("failed to get settings data\nstatus: " + status);
    return;
  }
  
  settings = data[0];
  for (key in settings)
  {
      var value = settings[key];
      
      if (value != null && key != "id")
      {
        if ($("#"+key).is(':checkbox'))
        {
          if (value) $("#"+key).prop('checked', true);
        }
        else if ($("input[name='" + key + "']").is(':radio'))
        {
          $("input[name='" + key + "']").each(function(){
            if (this.value==value)
            {
              $(this).prop('checked', true);
            }
            else
            {
              $(this).prop('checked', false);
            };
          });
        }
        else
        {
          if ($("#"+key).hasClass("timepicker"))
          {            
            $("#"+key).val(db_time_to_am_pm(value));
          }
          else
          {
            $("#"+key).val(value);
          }
        }
      }
  }
};

var days = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'];

function display_dept_settings(data)
{
  for (order in data)
  {
    $new_dept = $(".template-div").clone().removeClass("template-div");
    $($new_dept.find(".departmentSettings-label")[0]).text(data[order].DeptName1 + " Department Hours");
    $new_dept.appendTo("#departmentSettings-form");
    $($new_dept.find(".departmentSettings-BG")[0]).val(data[order].KioskDeptBGImage);
    $($new_dept.find(".departmentSettings-PrepOptions")[0]).val(data[order].PrepOptions);
    $($new_dept.find(".departmentSettings-DeptNum")[0]).text(data[order].DeptNum);
    
    for (i = 0; i < days.length; i++)
    {
      var $open = $($new_dept.find(".departmentSettings-" + days[i] + "Open")[0]);
      $open.val(db_time_to_am_pm(data[order][days[i] + "Open"]));
      $open.timepicker({
        timeFormat: 'h:mm p',
        interval: 15,
        dynamic: false,
        dropdown: false,
        scrollbar: false
      });
      
      var $close = $($new_dept.find(".departmentSettings-" + days[i] + "Close")[0]);
      $close.timepicker({
        timeFormat: 'h:mm p',
        interval: 15,
        dynamic: false,
        dropdown: false,
        scrollbar: false
      });
      $close.val(db_time_to_am_pm(data[order][days[i] + "Close"]));
    }
    
    if (order != data.length - 1) $("#departmentSettings-form").append("<hr />");
    
    $($new_dept.find('[data-toggle="popover"]')[0]).popover();
    $($new_dept.find('.popover-dismiss')[0]).popover({ trigger: 'focus' });
  }
};

function submit_dept_settings()
{
  var all_depts = [];
  
  $(".departmentSettings-container").not(".template-div").each(function() {
    var this_dept = {};
    
    this_dept["DeptNum"] = $($(this).find(".departmentSettings-DeptNum")[0]).text();
    
    for (i = 0; i < days.length; i++)
    {
      this_dept[days[i] + "Open"] = am_pm_to_db_time($($(this).find(".departmentSettings-" + days[i] + "Open")[0]).val());
      this_dept[days[i] + "Close"] = am_pm_to_db_time($($(this).find(".departmentSettings-" + days[i] + "Close")[0]).val());
    }
    
    this_dept["KioskDeptBGImage"] = $($(this).find(".departmentSettings-BG")[0]).val();
    
    this_dept["PrepOptions"] = $($(this).find(".departmentSettings-PrepOptions")[0]).val();
    
    all_depts.push(this_dept);
  });
  
  $.ajax({
    url: "/api/v1/departmentsettings",
    type: 'PUT',
    data: JSON.stringify(all_depts),
    contentType: "application/json"
  }).fail(function() {failed("Failed to save department settings!")});
}

function load_default_settings()
{
  $.getJSON("/api/v1/settings/defaults", null, display_settings).done(function(){
    // department settings only stores the current settings for each depts, so manually default everything
    $(".departmentSettings-open").each(function() {$(this).val("09:00:00")});
    $(".departmentSettings-close").each(function() {$(this).val("17:00:00")});
    $(".departmentSettings-BG").each(function() {$(this).val("")});
    $(".departmentSettings-PrepOptions").each(function() {$(this).val("")});
  }).fail(function() {failed("Failed to load default settings!");});
};

function load_settings()
{
  $.getJSON("/api/v1/settings", null, display_settings).done(function() {
    $.getJSON("/api/v1/departmentsettings", null, display_dept_settings).fail(function() { failed("Failed to load settings!");});
  }).fail(function() { failed("Failed to load settings!");});
};

function submit_settings()
{
  var data = {};

  $(".tab-content input").not(".departmentSettings").each(function(index)
  {
    var name = $(this).attr('id');
    
    switch($(this).attr('type'))
    {
      case 'number':
      case 'text':
        if ($(this).hasClass("timepicker"))
        {
          data[name] = am_pm_to_db_time(this.value);
        }
        else
        {
          data[name] = this.value;
        }
        break;
        
      case 'checkbox': // read all checkboxes as 0 or 1
        data[this.name] = 0;
        $("."+this.name).each(function(){
          if ($(this).prop("checked"))
          {
            data[this.name] = 1;
          }
        });
        break;
        
      case 'radio': // get the value out of the selected radio button
        data[this.name] = this.value;
        $("."+this.name).each(function(){
          if ($(this).prop("checked"))
          {
            data[this.name] = this.value;
          }
        });
        break;
      default:
        break;
    };
  });
  
  $(".tab-content select").not(".departmentSettings").each(function(index)
  {
    data[$(this).attr('id')] = this.value;
  });
  
  // prepare the modal
  
  $('#submitModal-body').text("Saving settings...");
  $('#submitModal-button').addClass('disabled');
  $('#submitModal-button').removeClass('btn-danger');
  $('#submitModal').modal('show');

  $.post("/api/v1/settings", JSON.stringify(data)).done(function() {
    $('#submitModal-body').text("Settings saved.");
    $('#submitModal-button').removeClass('disabled');
    $('#submitModal-button').attr("data-dismiss","modal");
  }).fail(function() {
    $('#submitModal-body').text("Failed to save settings!");
    $('#submitModal-button').removeClass('disabled');
    $('#submitModal-button').addClass('btn-danger');
    $('#submitModal-button').attr("data-dismiss","modal");
  });
  
};

function collapse_departments(event)
{
  var $dept = $(event.target);
  var collapsed = $dept.hasClass("header-shown");
  $(".departmentSettings-collapsible").collapse('hide');
  
  $(".departmentSettings-label").removeClass("header-shown");
  $(".departmentSettings-label").addClass("header-collapsed");
  
  if (collapsed)
  {
    $dept.removeClass("header-shown");
    $dept.addClass("header-collapsed");
  }
  else
  {
    $dept.removeClass("header-collapsed");
    $dept.addClass("header-shown");
  }
  
  $(event.target).parent().find(".departmentSettings-collapsible").collapse("toggle");
  $(".popover-button").popover();
}


var all_zones = null;
var all_depts = null;

function load_zones()
{
  $.getJSON("/api/v1/zones/", null, draw_zones).fail(function() {failed("Failed to load zones!")});
}

function draw_zones(data)
{
  all_zones = data;
  
  $("#zones").children().not('.template').not("#newZone").remove();
  
  for (var i = 0; i < data.length; i++)
  {
    var $new_dept = $(".zoneTemplate").clone().removeClass("d-none").removeClass("template").removeClass("zoneTemplate");
    $new_dept.find(".zones-DeptNum").text(data[i].DeptNum);
    $new_dept.find(".zones-ZoneNum").text(data[i].ZoneNum);
    $new_dept.find(".zones-ZoneName").text(data[i].ZoneName);
    
    // no removing the first zone for each dept
    if (data[i].ZoneNum == 0)
    {
      var button = $new_dept.find(".btn").first();
      button.prop('disabled', true);
      button.addClass("disabled");
    }
    
    $new_dept.appendTo("#zones");
  }
  
  $.getJSON("/api/v1/departments/", null, draw_depts).fail(function() {failed("Failed to load departments!")});
}

function draw_depts(data)
{
  all_depts = data;
  $('#deptSelect').empty();
  $("#newZoneName").val("");
  
  for (var i = 0; i < data.length; i++)
  {
    $("#zones").find("tr").not("#newZone").find(".zones-DeptName").each(function() {
      
      if ($(this).parent().find(".zones-DeptNum").first().text() == data[i].DeptNum)
      {
        $(this).text(data[i].DeptName1);
      }
    });
    
    $('#deptSelect').append($('<option>', {
      value: data[i].DeptNum,
      text: data[i].DeptName1
    }));
  }
}

function remove_zone(element)
{
  if ($(element).hasClass("disabled"))
  {
    failed("Can't delete the default zone!");
  }
  
  var $row = $(element).parent().parent();
  var dept = $row.find(".zones-DeptNum").first().text();
  var zone = $row.find(".zones-ZoneNum").first().text();
  
  $.ajax({
    url: '/api/v1/zones/',
    type: 'DELETE',
    data: {"DeptNum": dept, "ZoneNum": zone},
  }).fail(function() {failed("Failed to delete zones!");}).done(load_zones);
}

function add_zone(element)
{
  var $row = $(element).parent().parent();
  var dept = $row.find("#deptSelect").find(":selected").val();
  var zone_name = $row.find("#newZoneName").val();
  
  // get the highest current zone number in the selected dept
  var max_zone = -1;
  for (var i = 0; i < all_zones.length; i++)
  {
    if (all_zones[i].DeptNum == dept)
    {
      max_zone = Math.max(max_zone, all_zones[i].ZoneNum);
    }
  }
  
  max_zone += 1;
  
  $.ajax({
    url: '/api/v1/zones/',
    type: 'POST',
    data: JSON.stringify({"DeptNum": dept, "ZoneNum": max_zone, "ZoneName": zone_name}),
  }).fail(function() {failed("Failed to create zone!");}).done(load_zones);
}

function draw_upload()
{
    $('#pics').find("tr").not(".picsTemplate").remove();
    
    $.getJSON("/api/v1/images/uploads/all", null, draw_depts).fail(function() {failed("Failed to load pics!")}).done(function(data){
      for (var i = 0; i < data.length; i++)
      {
        var $new_pic = $(".picsTemplate").not("#newPic").clone().removeClass("d-none").removeClass("template").removeClass("picsTemplate");
        $new_pic.find(".pics-name").text(data[i]);
        $new_pic.appendTo("#pics");
      }
    });
}

function file_upload(button)
{
  var element = $("#pic-file").get(0);
  var reader = new FileReader();
  var file = element.files[0];
  
  if (!file) failed("Select a file to upload!");
  
  reader.addEventListener("load", function() {
    $.ajax({
      url: '/api/v1/images/uploads/',
      type: 'POST',
      /* remove header from front which will be something like "data:image/jpeg;base64," */
      data: JSON.stringify({"filename": file.name, "image": reader.result.split("base64,").pop(1)}),
    }).fail(function() {
      draw_upload();
      failed("Failed to upload image!");
    }).done(function() {
      draw_upload();
      failed("Uploaded successfully.");
    });
  });
  
  reader.readAsDataURL(file);
}

function remove_image(element)
{
  var filename = $(element).parent().parent().find(".pics-name").text();
  
  $.ajax({
  url: '/api/v1/images/uploads/' + filename,
  type: 'DELETE'}).fail(function() {failed("Failed to delete image!")}).done(draw_upload);
}

function select_pic(element)
{
  $("#selectModal-select").unbind('mouseup');
  // TODO
  $('#selectModal-select').mouseup(function() {
    $(element).parent().find("input").val($('#selectModal-pic').val());
  });
  
  $.getJSON("/api/v1/images/uploads/all", null).done(function(data) {
    $("#selectModal-pic").empty();
    
    for (var i = 0; i < data.length; i++)
    {
      $('#selectModal-pic').append($('<option>', {
        value: "/OnlineOrdering/static/images/uploads/" + data[i],
        text: data[i]
      }));
    }
  });
  
  $("#selectModal").modal("show");
}

function region_select(element)
{
  var url = "http://" + location.host + "/OnlineOrdering/kiosk/";
  var update = false;
  
  switch ($(element).find(":selected").text())
  {
    case "Queue Page":
        url += "queue.html";
        update = true;
        break;
    case "Logo Page":
        url += "logo.html";
        update = true;
        break;
  }

  if (update)
  {
    $("body").find("#" + $(element).attr('id') + "URL").val(url);
  }
}
