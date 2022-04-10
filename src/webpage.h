//============
//Webpage Code
//============
String webpageCode = R"***(
<!DOCTYPE html>
<head>
  <title> Pflanzen Web Server </title>
</head>
<html>
<!----------------------------CSS---------------------------->
<style>
  body {background-color: rgba(128, 128, 128, 0.884)}
  h4 {font-family: arial; text-align: center; color: white;}
  .card
  {
     max-width: 450px;
     min-height: 100px;
     background: rgba(255, 0, 0, 0.521);
     padding: 10px;
     font-weight: bold;
     font: 25px calibri;
     text-align: center;
     box-sizing: border-box;
     color: blue;
     margin:20px;
     box-shadow: 0px 2px 15px 15px rgba(0,0,0,0.75);
  }
</style>
<!----------------------------HTML--------------------------->
<body>
  <div class="card">
    <h1><span style="background-color:white">ESP32 Web Server</span></h1>
    <h2>
      POT Value : <span style="color:yellow" id="POTvalue">0</span>
    </h2>
  </div>
  <h4>
    <button onclick="help()">Help</button><br><br>
    <div id="myDIV"> </div>
  </h4>
<!-------------------------JavaScrip------------------------->
  <script>
    setInterval(function()
    {
      getPOTval();
    }, 2000);
    //-------------------------------------------------------
    function getPOTval()
    {
      var POTvalRequest = new XMLHttpRequest();
      POTvalRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("POTvalue").innerHTML =
          this.responseText;
        }
      };
      POTvalRequest.open("GET", "readPOT", true);
      POTvalRequest.send();
    }
    //-------------------------------------------------------
    function help()
    {
      var x = document.getElementById("myDIV");
      var message = "POT connected to ADC0 : 12-bit value (0 ---> 4095)";
      if (x.innerHTML == "") x.innerHTML = message;
      else x.innerHTML = "";
    }
  </script>
</body>
</html>
)***";