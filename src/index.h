const char MAIN[] PROGMEM =  R"=====(
<!DOCTYPE html>
<html><head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Todd's Sprinkler</title>
  <style>
      @viewport{width:device-width;zoom:1.0;}
      @-ms-viewport {width: device-width ;}
      html{
          font-family: Arial;
          display: inline-block;
          margin: 0px auto;
          text-align: left;
          min-width: 544px;
          min-height:777px;
          background-color: dodgerblue;
      }
      h3 {
          font-size: 2.8vw;
          font-weight: bold;
          text-align: center;
          background-color: rgb(255,255,255);
          color: blue;
          vertical-align: top;
          margin-top:0px;
      }
      .red{
          color:rgb(0, 0, 0);
          background-color:rgb(255,0,0);
          font-weight:700;
          padding: .3vw .3vw .3vw .45vw;
          min-height: 3.2vw;
          max-height: 10px;
          min-width: 3.2vw;
          max-width: 26.42px;
      }
      .grn{
          color:rgb(0, 0, 0);
          background-color:rgb(0,255,0);
          font-weight:700;
          padding: .3vw .3vw .3vw .45vw;
          min-height: 3.2vw;
          max-height: 10px;
          min-width: 3.2vw;
          max-width: 26.42px;
        }
      .addMins{
          padding:.5vw .5vw .5vw .7vw;
          border-radius: 17%;
          box-shadow: inset 2px 2px 3px rgba(153, 150, 150, .6),
          inset -2px -2px 3px rgba(0, 0, 0, 0.6);
          background-color: white;
          border-bottom: black solid 3px;
          border-spacing: 45px;
          cursor: pointer;
      }
      .help {
          border: 0;
          line-height: 0vw;

          padding: 0 0vw;
          text-align: left;
          font-weight:bold;
          font-size: 1.5vw;

          cursor: pointer;
          background-color: rgb(255, 255, 255);
          box-shadow: inset 1vw 1vw 1vw rgba(255, 255, 255, 0.6),
              inset -.3vw -.25vw .3vw rgba(0, 0, 0, 0.6);

          margin-left: 0vw;
          margin-top: 0vw;
          padding: 1vw;

          margin-inline: 0vw;
      }
      .sensors-grid{
          display:grid;
          grid-template-columns:12vw 11vw 4vw 10vw;
          align-items: top;
          text-align: center;

          box-sizing: border-box;
          width: min(100%, 35.5vw);
          min-width: 35.5vw;
          height: 6vw;

          margin-left: 5vw;
          margin-right: 5vw;

          background-color: #ced3c7;
          justify-items: left;

          padding: 1.3vw 1.0vw;

          font-size:2.8vw;
          font-weight: bold;
      }
      .status-grid{
          display:grid;
          grid-template-columns:7vw 15vw 9vw;

          box-sizing: border-box;
          width: min(100%, 35.5vw);
          min-width: 35.5vw;
          height: 15vw;

          margin-left: 5vw;
          margin-right: 5vw;

          background-color: #ced3c7;
          justify-items: center;

          padding: 1vw 2.8vw;

          font-size:2.0vw;
          font-weight: bold;
          text-align:center;
      }
      .notes-grid{
         display:grid;
         grid-template-columns: 22vw 3vw 3Vw 3vw;
         grid-auto-rows: minmax(auto, auto);

         margin-left: 5vw;
         margin-right: 5vw;

         padding:1.0vw 1vw 1vw 1vw;

         color:rgb(0, 0, 0);
         background-color:rgb(255,255,255);
         font-weight: 300;
         font-size:2.0vw;
         font-family: Calibri;
         text-align:left;
         width:33.5vw;
      }
     .two {
         grid-column: 1 / 4;
         grid-row: 3 / 4;
         padding: 15px 0 0 0;
      }
      .settings-grid{
         display:grid;
         grid-template-columns: 18vw 9vw;
         grid-template-rows: repeat(1, 2.3vw);
      }
      .time{
          grid-column: 3.4;
          color: rgb(0, 0, 255);
          font-size:1.5vw;
          text-align:left;
      }
      .temp{
          grid-column: 3.4;
          color: rgb(0,0,0);
          font-size:1.5vw;
          padding: 1vw, 1vw, 1vw, 1vw;
          text-align:center;
      }
  </style>
</head>
  <body><h3 style="margin: 1vw 5vw;width: 35.5vw;">Sprinkler Timer</h3>

<!-- grid sensors 1st row Temp  Date Time -->
  <main class="sensors-grid" style="font-size:2.4vw;">
    <div>
        <span id="bt_help" class="help" role="submit" onclick="f_help()" value="Help" title="Hide/Show WebSock Messages">Help</span>
    </div>
    <div name="temp" id="temp" class="temp"></div>
    <div class="time">Date<br>Time</div>
    <div id="clock" class="time">5/6/2022<br>15:53:00</div>
  </main>

<!-- grid: 'yellow' 1st row Sprinkler OnMinutes Adjust  -->
  <main class="status-grid" style="background:yellow;">
    <div>Sprinkler</div><div>On Minutes</div><div>&#177 Mins</div>
<!-- grid: 'yellow' 2nd row: ON/OFF  RemainingMins ± Mins -->
    <div><br>
        <submit
            id="Sprinkler"
            style="padding: 1.5vw 1.4vw 2vw 1.6vw; border-radius: 25%; box-shadow: rgba(153, 150, 150, 0.6) 2px 2px 3px inset, rgba(0, 0, 0, 0.6) -2px -2px 3px inset; background-color: rgb(250, 0, 0);"
            title='Red shows Sprinkler OFF Green ON CLick "ON" adds 60 minutes'
            onclick="f_setMinutes('Sprinkler', this.innerHTML=='ON'?60:0);">ON
        </submit>
    </div>
    <div><br>
      <text id="Timer" style="font-size: 3.6vw">0.0</text>
    </div>
    <div style="padding:1vw 1.5vw 0vw 1.5vw">
      <submit class="addMins" role="submit" onclick="f_setMinutes('Minutes', +15)" title="add 15 minutes to Sprinkler ON">+15</submit>
      <p></p>
      <submit class="addMins" role="submit" onclick="f_setMinutes('Minutes', -15)" title="remove 15 minutes from Sprinkler OFF">&nbsp;-15</submit>
    </div>
  </main>
<!-- comment: NOTES -->
  <div id="help" hidden="true">
    <p class="notes-grid" >Sprinkler Button<br>Color Shows State <a class="grn">ON</a>&nbsp;<a class="red">OFF</a>
        <i class="two">
            Click when "ON" adds 60 minutes<br>
            Click when "OFF" turns OFF pump
        </i>
    </p>
    <p class="settings-grid">
        <span class="help" value="WiFi Settings" type="button" onclick="f_redirect('wifi')" style="background: orange;margin-left: 5vw;margin-right: 1vw;" title="Web Page for setting WiFi SSID/PWD &amp; IP">WiFi Settings</span>
        <span class="help" value="Firmware" type="button" onclick="f_redirect('update')" style="background:rgb(249, 253, 0);color: red;" title="Web Page for updating firmware">Firmware</span>
        <details>
            JSON Inbound: 
            <aside id="json1" style="font-weight: normal; font-size: 2.0vw; word-wrap:  break-word;">{"src":"mcu","ms":0,"t":-196.6}</aside>
            <br>JSON Outbound:
            <aside id="json2" style="font-weight: normal; font-size: 2.0vw; word-wrap:  break-word;"></aside>
        </details>
    </p>
  </div>

<script language="javascript" type="text/javascript">
  // Logic: Minutes is everything. i.e. 0 minutes & system is off >0 mins & system is on
  window.onload = function() { setInterval( f_clock, 1000);}
  let webSock = new WebSocket("ws://" + window.location.hostname + ":80");

  webSock.onopen = function(){f_webSockConnect();};
  webSock.onmessage = function(evt) {f_webSockOnMessage(evt);};
  webSock.onerror = function(evt) {console.warn(evt);};
  webSock.onclose = function(){console.log("webSock Connection Closed.")};

  const min2ms = Number(60000);
  let updating = false;
  let j = {
        src: "mcu"
      , ms: -71 * min2ms
      , t:65
  };
  function f_webSockOnMessage(evt){
      if (typeof evt.data === "string"){
          let s = evt.data;
          j = JSON.parse(s);
      if(updating || j.src != "mcu")return;
          f_setMinutes('mcu', j.ms);
          f_Sprinkler(j.ms);
          f_temp(j.t);
          document.getElementById('json1').innerHTML = JSON.stringify(j);
      }
  }
  function f_webSockConnect(){
      j.src="main";
      webSock.send(JSON.stringify(j));
  }
  function f_redirect(where) {window.location.href = "/" + where;}
  function f_setMinutes(source, p1){
      // if p1 == zero then zero the timer & WebSock Send
      // source: mcu, Sprinkler, Minutes
      const doc = document.getElementById("Timer");
      if(source!="mcu"){
          updating=true;
          p1*=min2ms;
          let ms = Boolean(p1)*(j.ms+=p1);

          j.ms = ms>0?ms:0;
          j.src = "main";
          webSock.send(JSON.stringify(j));
          document.getElementById('json2').innerHTML = JSON.stringify(j);
          updating=false;
      }
      doc.innerHTML = (Math.ceil(10*(j.ms/min2ms))/10).toFixed(1);
  }
  function f_Sprinkler(p1) { // Sets ON/OFF Button, WebSock inbound: p1 = j.Minutes, p1 > 0 set "OFF" & GREEN, else "ON" & RED
      const sprinkler = document.getElementById("Sprinkler");
      sprinkler.innerHTML = Boolean(p1) ? "OFF" :"ON" ;
      sprinkler.style.backgroundColor = Boolean(p1)? "rgb(0, 250, 0)" : "rgb(250, 0, 0)";
  }
  function f_temp(temp){
      const DEVICE_DISCONNECTED_F = -196.6;
      const tmp=document.getElementById("temp");

      if(temp == DEVICE_DISCONNECTED_F)
          tmp.innerHTML="";
      else
          tmp.innerHTML = "Temp<br><aside>" + temp + "<sup>&deg;</sup>F</aside>";
  }
  let f_clock = ()=>{
      const clock = document.getElementById("clock");
      d = new Date();
      clock.innerHTML
       = d.toLocaleDateString()
       +"<br>"+d.toLocaleTimeString(
           'en-US'
          , {
               timeZone:'America/Los_Angeles'
              ,hour12:false
              ,hour:'numeric'
              ,minute:'numeric'
              , second:'numeric'
          }
       );
  }
  let f_help = () => {
      if (document.getElementById("help").getAttribute("hidden")){
          document.getElementById("help").removeAttribute("hidden");
          document.getElementById("bt_help").innerHTML = "Un-Help";
      }else{
          document.getElementById("help").setAttribute("hidden", true);
          document.getElementById("bt_help").innerHTML = "Help";
      }
  }
</script>

</body></html>)=====";

  const char NETWORK[] PROGMEM =  R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="file" content="set_bounds.html">
    <meta name="author" content="Joe Belson 20220422">
    <meta name="what" content="esp8266 html for WiFi connected temp/humidity bounds.">
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title id="title" updated="0">Todd's Sprinkler: Network Setup</title>
  </head>
<style>
  .reboot{
    background-color: red;
    color:rgb(255, 255, 255);
    padding:1.5vw 1.5vw 1.5vw 1.5vw;
    border-radius: 17%;
    box-shadow: inset 2px 2px 3px rgba(153, 150, 150, .6),
    inset -2px -2px 3px rgba(0, 0, 0, 0.6);
    border-bottom: black solid 3px;
    border-spacing: 45px;
    cursor: pointer;
    font: size 2vw;
    font-weight: bold;
    font-size: 17px;
  }
  .updated{
    background-color: green;
    color:rgb(255, 255, 255);
    padding:1vw 1vw 1vw 1vw;
    border-radius: 0%;
    box-shadow: inset 2px 2px 3px rgba(153, 150, 150, .6),
    inset -2px -2px 3px rgba(0, 0, 0, 0.6);
    border-bottom: black solid 3px;
    border-spacing: 45px;
    cursor: pointer;
    font-weight: bold;
  }


</style>
  <body>
    <hr>
    <div onclick="updating=true">
      <table class="calibrate">
        <caption style="font-weight: bolder;padding-bottom:1rem">
          <h1>WiFi SSID & PWD & IP</h1>
          <h2 id="updated" class="updated" hidden>UPDATED</h2>
        </caption>
         <thead>
            <tr>
              <th>SSID</th>
              <th>PWD</th>
            </tr>
          </thead>
            <tr>
              <td><input  id="ssid"   type="text"   name="cred"></td>
              <td><input  id="pwd"    type="text"   name="cred"></td>
            </tr>
      </table>
      <table>
            <tr><td><br></td></tr>
            <tr>
              <td>DHCP: <input type="checkbox" id="isDHCP" name="isDHCPServer" onclick="f_dhcp(this.checked)"/></td>
            </tr>
            <tr id="dhcp">
              <td><label for="ip"   name="dhcp">IP:     </label><input id="ip"    type="text" name="dhcp" style="width: 5rem;"></td>
              <td><label for="gw"   name="dhcp">Gateway:</label><input id="gw"    type="text" name="dhcp" style="width: 5rem;"></td>
              <td><label for="mask" name="dhcp">Mask:   </label><input id="mask"  type="text" name="dhcp" style="width: 5rem;"></td>
            </tr>
      </table>
    </div>
    <p><input  id="submit2" type="submit" onclick="f_submit()">
      <input type="submit" onclick="history.back()" value="Sprinkler">
      <input id="reboot" type="submit" onclick="f_reboot('reboot')" class="reboot" value="REBOOT" hidden>
    </p>
    <hr>
  </body>
    <script language = "javascript" type = "text/javascript">
  
        let webSock       = new WebSocket("ws://" + window.location.hostname + ":80?source=wifi");
        webSock.onopen    = function(evt){webSock.send('{"src":"network","updated":0}');}
        webSock.onmessage = function(evt){f_webSockOnMessage(evt);}
        webSock.onerror   = function(evt){f_webSockOnError(evt);}
  
        let updating = false;  // local flag: don't refresh text boxes using mcu json.  "We are updating values"
        let j = {
              src : "mcuNetwork"
            , ssid : "ssid"
            , pwd : "pwd"
            , isDHCP : false
            , ip : "11.1.1.11"
            , gw : "11.1.1.254"
            , mask : "255.255.255.0"
            , updated : false         // flag did MCU just save WiFi credentials?
            , reboot : false          // flag - tell the MCU to reboot.  Try AP_Mode STA
          };

        function f_webSockOnMessage(evt){
          console.log(evt.data);
          if(updating)return;
          if(typeof evt.data === "string"){
              const j=JSON.parse(evt.data);
              if(j.src != "mcuNetwork")return;
              for(key in j){
                if(key == "src")continue;
                dv = document.getElementById(key);
                switch(key){
                  case "ssid":dv.value = j.ssid;break;
                  case "isDhcp":f_dhcp(j.isDHCP);break;
                  case "ip":dv.value = j.ip;break;
                  case "gw":dv.value = j.gw;break;
                  case "mask":dv.value = j.mask;break;
                  case "updated":
                    if(j.updated){
                      f_reboot('updated');
                      dv.removeAttribute("hidden");
                    }
                  break;
                }
              }
          }
        }
        function f_webSockOnError(evt){}
        function f_submit(){
          for(var key in j){
            switch(key){
              case "src":break;
              case "isDHCP":j[key]=document.getElementById(key).checked;break;
              case "updated":j.updated = 1; break;
              case "reboot":j.reboot = false;break;
              default: j[key]=document.getElementById(key).value;
            }
          }
          j.src = "network";
          console.log(j);
          webSock.send(JSON.stringify(j));
          updating=0;
        }
        function f_dhcp(tf){
          console.log(tf);
          const dhcp = document.getElementById("dhcp");
          if(tf)dhcp.setAttribute("hidden", "hidden");
        else
        dhcp.removeAttribute("hidden");
       }
       function f_reboot(what){
        switch (what){
          case 'reboot':
              // send reboot to MCU, Redirect to Main
              j.updated=1;j.reboot=1;
              webSock.send(JSON.stringify(j));
              window.location.href='http://' + j.ip;
            break;
          case 'updated':
            // make visible REBOOT
              document.getElementById("reboot").removeAttribute("hidden");
            break;
        }
       }
     </script>
  </html>
  )=====";
