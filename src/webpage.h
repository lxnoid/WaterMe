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
     background: rgba(0, 128, 15, 0.521);
     padding: 10px;
     font-weight: bold;
     font: 25px calibri;
     text-align: center;
     box-sizing: border-box;
     color: white;
     margin:20px;
  }
</style>
<!----------------------------HTML--------------------------->
<body>
  <div class="card">
    <h1>Pflanzen Web Server</h1>
    <h3>
      Aktueller Wert: <span style="color:yellow" id="ADCvalue">0</span>
    </h3>
  </div>
  <div class="card">
      <h2>Einstellungen</h1>
        <form action="some/file" method="POST" name="myForm" id="myForm">
            <table>
                <tr>
                    <td>Name:</td>
                    <td><input type="text" name="name"></td>
                </tr>
                <tr>
                    <td>Minimaler Wert:</td>
                    <td><input type="text" name="min"></td>
                </tr>
                <tr>
                    <td>Maximaler Wert:</td>
                    <td><input type="text" name="max"></td>
                </tr>
            </table>    
        </form> 
  </div>
  <h4>
    <button onclick="setValues()">Speichern</button><br><br>
    <button onclick="help()">Help</button><br><br>
    <div id="myDIV"> </div>
  </h4>
<!-------------------------JavaScrip------------------------->
<script>
    document.addEventListener("load", readConfig());
    setInterval(function()
    {
      getADCval();
    }, 2000);
    //-------------------------------------------------------
    function getADCval()
    {
      var ADCvalRequest = new XMLHttpRequest();
      ADCvalRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("ADCvalue").innerHTML =
          this.responseText;
        }
      };
      ADCvalRequest.open("GET", "readADC", true);
      ADCvalRequest.send();
    }
    //-------------------------------------------------------
    function readConfig() {
        var configRequest = new XMLHttpRequest();
        configRequest.onreadystatechange = function() {
            if(this.readyState == 4 && this.status == 200) {
                let config_json = this.responseText;
                let obj = JSON.parse(config_json);
                document.forms["myForm"]["name"].value = obj.name;
                document.forms["myForm"]["min"].value = obj.min; 
                document.forms["myForm"]["max"].value = obj.max;
            }
        };
        configRequest.open("GET", "readConfig", true);
        configRequest.send();
    }
    //-------------------------------------------------------
    function validateForm() {
        let result = true;
        let name = document.forms["myForm"]["name"].value;
        let min = parseInt(document.forms["myForm"]["min"].value);
        let max = parseInt(document.forms["myForm"]["max"].value);

        if (name == "") {
            alert("Name muss definiert sein");
            result = false;
        }
        if (isNaN(min) && min < 0 && min > 4095) {
            alert("Keine Zahl, oder im Bereich 1 bis 4095");
            result = false;
        }
        if (isNaN(max) && max < 0 && max > 4095) {
            alert("Keine Zahl, oder im Bereich 1 bis 4095");
            result = false;
        }
        return result;
    }
    //-------------------------------------------------------
    function setValues()
    {
        if (validateForm())
        {
            var configRequest = new XMLHttpRequest();
            let parsedData = {};
            parsedData.name = document.forms["myForm"]["name"].value;
            parsedData.min = parseInt(document.forms["myForm"]["min"].value);
            parsedData.max = parseInt(document.forms["myForm"]["max"].value);
            let config_json = JSON.stringify(parsedData);
            configRequest.open("POST", "updateConfig");
            configRequest.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
            configRequest.send(config_json);
        }
    }
    //-------------------------------------------------------
    function help()
    {
      var x = document.getElementById("myDIV");
      var message = "Sensor verbunden: 12-bit value (0 --> 4095)";
      if (x.innerHTML == "") x.innerHTML = message;
      else x.innerHTML = "";
    }
</script>
</body>
</html>
)***";