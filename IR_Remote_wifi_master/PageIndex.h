const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
  <head>
    <title>EMMERICH ESP32 AC REMOTE ESP-NOW & WEB SERVER (CONTROLLING)</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <link rel="icon" href="data:,">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center;}
      p {font-size: 1.2rem;}
      body {margin: 0;}
      .topnav {overflow: hidden; background-color: #2adb5f; color: white; font-size: 1.5rem;}
      .content {padding: 20px; }
      .bigCard {
        background-color: white;
        box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5);
        border: 1px solid #2adb5f; 
        border-radius: 20px;
        max-width: 700px; 
        margin: 0 auto;
        margin-bottom: 30px;
      }
      .card {background-color: white; box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5); border: 1px solid #2adb5f; border-radius: 15px;}
      .card.header {background-color: #2adb5f; color: white; border-bottom-right-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 12px; border-top-left-radius: 12px;}
      .cards {
        max-width: 700px; 
        margin: 0 auto;
        display: grid; 
        grid-gap: 2rem; 
        grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      }
      .reading {font-size: 2.8rem;}
      .packet {color: #bebebe;}
      .temperatureColor {color: #fd7e14;}
      .humidityColor {color: #1b78e2;}
      .LEDColor {color: #183153;}
      
      /* ----------------------------------- Toggle Switch */
      .switch {
        position: relative;
        display: inline-block;
        width: 70px;
        height: 34px;
      }

      .switch input {display:none;}

      .sliderTS {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #D3D3D3;
        -webkit-transition: .4s;
        transition: .4s;
        border-radius: 34px;
      }

      .sliderTS:before {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        background-color: #f7f7f7;
        -webkit-transition: .4s;
        transition: .4s;
        border-radius: 50%;
      }

      input:checked + .sliderTS {
        background-color: #62c934;
      }

      input:focus + .sliderTS {
        box-shadow: 0 0 1px #2196F3;
      }

      input:checked + .sliderTS:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(36px);
      }

      .sliderTS:after {
        content:'OFF';
        color: white;
        display: block;
        position: absolute;
        transform: translate(-50%,-50%);
        top: 50%;
        left: 70%;
        font-size: 10px;
        font-family: Verdana, sans-serif;
      }

      input:checked + .sliderTS:after {  
        left: 25%;
        content:'ON';
      }
      /* ----------------------------------- */

      /* ----------------------------------- Button*/
      .custom-button {
        padding: 10px 20px;
        margin-top: 10px;
        font-size: 16px;
        background-color: #616462;
        color: white;
        border: none;
        border-radius: 5px;
        cursor: pointer;
      }

      .custom-button:hover {
        background-color: #33d162;
      }
      
      /* ----------------------------------- Slider */
      .slidecontainer {
        width: 100%;
      }

      .slider {
        -webkit-appearance: none;
        width: 50%;
        height: 10px;
        border-radius: 5px;
        background: #d3d3d3;
        outline: none;
        opacity: 0.7;
        -webkit-transition: .2s;
        transition: opacity .2s;
      }

      .slider:hover {
        opacity: 1;
      }

      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 20px;
        height: 20px;
        border-radius: 50%;
        background: #1b78e2;
        cursor: pointer;
      }

      .slider::-moz-range-thumb {
        width: 20px;
        height: 20px;
        border-radius: 50%;
        background: #1b78e2;
        cursor: pointer;
      }
      /* ----------------------------------- */
    </style>
  </head>
  
  <body>
    <div class="topnav">
      <h3>AC Control by ESP32</h3>
    </div>
    
    <br>
    <div class="content">
      
      <div class="bigCard">
        <div class="card header">
          <h2>BIG REMOTE</h2>
        </div>
        <br>
          <h4 class="LEDColor"><i class="fas fa-power-off"></i> Display</h4> 
          <p id="State1">State - OFF</p>
          <button class="custom-button" onclick="change_display('State1','ON'); send_cmd('ESP32AllSlave','1','1')"> On </button> 
          <button class="custom-button" onclick="change_display('State1','OFF');send_cmd('ESP32AllSlave','1','0')"> Off </button>

          <h4 class="LEDColor"><i class="fas fa-temperature-low"></i> TEMP Ver 1 </h4>
          <p id="State2">State - 16 Celcius Degrees</p>
          <button class="custom-button" onclick="change_display('State2','1');send_cmd('ESP32AllSlave','2','1')"> Up </button> 
          <button class="custom-button" onclick="change_display('State2','-1');send_cmd('ESP32AllSlave','2','0')"> Down </button>
          
          <h4 class="LEDColor"><i class="fas fa-wind"></i> Fan Mode </h4> 
          <p id="State3">State - High</p>
          <button class="custom-button" onclick="change_display('State3','High');send_cmd('ESP32AllSlave','3','4')"> High </button> 
          <button class="custom-button" onclick="change_display('State3','Medium');send_cmd('ESP32AllSlave','3','3')"> Medium </button>
          <button class="custom-button" onclick="change_display('State3','Low');send_cmd('ESP32AllSlave','3','2')"> Low </button>
          <button class="custom-button" onclick="change_display('State3','Auto');send_cmd('ESP32AllSlave','3','0')"> Auto </button>

      <br><br></div>
      
      <div class="bigCard">
        <div class="card">
          <div class="card header">
            <h2>AC Timer</h2>
          </div>
          <br>
          <p>Today is <span id="currentDate"></span>. The current time is <span id="currentTime"></span>.</p>
          <p>The AC will turn on at <input type="time" id="acTime" value="08:00"> and off at <input type="time" id="offTime" value="18:00"> every day.</p>
          <button class="custom-button" onclick="setTimer()">Set Timer</button>
          <button class="custom-button" onclick="stopTimer()">Stop Timer</button>
          <p id="timerStatus"></p>
        </div>
      </div>

      <div class="cards">
        <div class="card">
          <div class="card header">
            <h2>ESP32 BOARD #1</h2>
          </div>
          <br>
          
          <h4 class="LEDColor"><i class="fas fa-power-off"></i> POWER </h4> 
          <label class="switch">
            <input type="checkbox" id="togPower1" onclick="send_LED_State_Cmd('ESP32Slave1','togPower1','1')">
            <div class="sliderTS"></div>
          </label>
          <br><br>
          
          <!-- Not For Big Remote -->
          
          <h4 class="LEDColor"><i class="fas fa-temperature-low"></i> TEMP </h4> 
          <div class="slidecontainer">
            <input type="range" min="16" max="32" value="16" class="slider" id="tempSlider1">&nbsp<span id="tempTextSlider1">16</span><sup>o</sup>
          </div>
          <br>
        </div>
      
        <!-- Can Copy For Multiple Slave 
        <div class="card">
          <div class="card header">
            <h2>ESP32 BOARD #2</h2>
          </div>
          <br>
          
          <h4 class="LEDColor"><i class="fas fa-power-off"></i> POWER </h4> 
          <label class="switch">
            <input type="checkbox" id="togPower1" onclick="send_LED_State_Cmd('ESP32Slave1','togPower1','1')">
            <div class="sliderTS"></div>
          </label>
          <br><br>
          
          <h4 class="LEDColor"><i class="fas fa-temperature-low"></i> TEMP </h4> 
          <div class="slidecontainer">
            <input type="range" min="16" max="32" value="16" class="slider" id="tempSlider1">&nbsp<span id="tempTextSlider1">16</span><sup>o</sup>
          </div>
          <br>
        </div>
      </div> -->
    </div>
    
    <script>
      var degrees = 16;
      //------------------------------------------------------------ The variables for the slider.
      var sliderTemp1 = document.getElementById("tempSlider1");
      var last_sliderTemp1_val = 0;

      let sliders_used = "";
      //------------------------------------------------------------
      
      //------------------------------------------------------------ The variables for the Timer.
      var myTmr_send;
      var myTmr_send_interval = 250;
      var myTmr_send_start = 1;
      var myTmr_count_to_stop = 0;
      //------------------------------------------------------------
      
      //------------------------------------------------------------ The function called by "Toggle Switch" to control the LED.
      function send_LED_State_Cmd(board,id,gpio) {
        var tgLEDFlash = document.getElementById(id);
        var tgState;

        if (tgLEDFlash.checked == true) tgState = 1;
        if (tgLEDFlash.checked == false) tgState = 0;

        send_cmd(board,gpio,tgState);
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------ Function to detect when the slider is used and get its value and activate the Timer.
      sliderTemp1.oninput = function() {
        sliders_used = "SL1";
        myTmr_count_to_stop = 0;
        if (myTmr_send_start == 1) {
          myTmr_send = setInterval(myTmr_send_LED_PWM_Cmd, myTmr_send_interval);
          myTmr_send_start = 0;
        }
      }
      // ------------------------------------------------------------
      
      // ---------------------------------------------------------------------- Timer for sending slider values and other data from this page to ESP32.
      function myTmr_send_LED_PWM_Cmd() {
        if (sliders_used == "SL1") {
          if (last_sliderTemp1_val != sliderTemp1.value) {
            send_cmd('ESP32Slave1',12,sliderTemp1.value);
            document.getElementById("tempTextSlider1").innerHTML = sliderTemp1.value;
          }
          last_sliderTemp1_val = sliderTemp1.value;
        }
        
        myTmr_count_to_stop++;
        if (myTmr_count_to_stop > 5) {
          myTmr_send_start = 1;
          clearInterval(myTmr_send);
        }
      }
      // ---------------------------------------------------------------------- 
      function change_display(id,value){
        var displayText = document.getElementById(id);
        if(id == "State1" && value == "ON"){
          displayText.textContent = "State - ON";
          document.getElementById("togPower1").checked = true;
        }
        else if (id == "State1" && value == "OFF"){
          displayText.textContent = "State - OFF";
          document.getElementById("togPower1").checked = false;
        }
        else if(id == "State2" && value == "1" && degrees<32){
          degrees++;
          displayText.textContent = "State - " + degrees + " Celcius Degrees";
          sliderTemp1.value = degrees;
          document.getElementById("tempTextSlider1").innerHTML = degrees;
        }
        else if(id == "State2" && value == "-1" && degrees>16){
          degrees--;
          displayText.textContent = "State - " + degrees + " Celcius Degrees";
          sliderTemp1.value = degrees;
          document.getElementById("tempTextSlider1").innerHTML = degrees;
        }
        else if(id == "State3" && value == "High"){
          displayText.textContent = "State - High";
        }
        else if(id == "State3" && value == "Low"){
          displayText.textContent = "State - Low";
        }
        else if(id == "State3" && value == "Medium"){
          displayText.textContent = "State - Medium";
        }
        else if(id == "State3" && value == "Auto"){
          displayText.textContent = "State - Auto";
        }

      }
      
      // ---------------------------------------------------------------------- XMLHttpRequest to submit data.
      function send_cmd(board,gpio,value) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "set_LED?board="+board+"&gpio_output="+gpio+"&val="+value, true);
        xhr.send();
      }
      // ---------------------------------------------------------------------- 

      // ---------------------------------------------------------------------- AC Timer
      let timerInterval;
      let acTimeValue;
      let offTimeValue;
      let timeValue;
      let acState = false; // The initial state is OFF

      function getCurrentDate() {
        const now = new Date();
        const options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' };
        return now.toLocaleDateString(undefined, options);
      }

      function displayCurrentDate() {
        const currentDateElement = document.getElementById("currentDate");
        currentDateElement.textContent = getCurrentDate();
      }

      function getCurrentTime() {
        const now = new Date();
        const options = { hour: 'numeric', minute: 'numeric', second: 'numeric' };
        return now.toLocaleTimeString(undefined, options);
      }

      function displayCurrentTime() {
        const currentTimeElement = document.getElementById("currentTime");
        currentTimeElement.textContent = getCurrentTime();
      }

      function setTimer() {
        const acTimeInput = document.getElementById("acTime");
        acTimeValue = acTimeInput.value;
        const offTimeInput = document.getElementById("offTime");
        offTimeValue = offTimeInput.value;
        const [acHours, acMinutes] = acTimeValue.split(":");
        const [offHours, offMinutes] = offTimeValue.split(":");

        // Clear any existing timer interval
        clearInterval(timerInterval);

        // Start the timer to check for the specified times
        timerInterval = setInterval(checkTime, 1000);

        // Save the time values in local storage
        localStorage.setItem('acTimeValue', acTimeValue);
        localStorage.setItem('offTimeValue', offTimeValue);
        timeValue = 1;
        localStorage.setItem('timeValue', timeValue);

        // Display a message indicating the timer has been set
        const timerStatusElement = document.getElementById("timerStatus");
        timerStatusElement.textContent = `Timer set: AC ON at ${acTimeValue}, AC OFF at ${offTimeValue}.`;
      }

      function checkTime() {
        const now = new Date();
        const [acHours, acMinutes] = acTimeValue.split(":");
        const [offHours, offMinutes] = offTimeValue.split(":");
        const acTargetTime = new Date();
        const offTargetTime = new Date();

        acTargetTime.setHours(acHours, acMinutes, 0, 0);
        offTargetTime.setHours(offHours, offMinutes, 0, 0);

        // If the current time is later than the specified time for the day, add one day to the target time
        if (now > acTargetTime) {
          acTargetTime.setDate(acTargetTime.getDate() + 1);
        }

        if (now > offTargetTime) {
          offTargetTime.setDate(offTargetTime.getDate() + 1);
        }

        if (!acState && now >= acTargetTime) {
          // Send the command to the ESP32 board to turn on the AC
          send_cmd('ESP32AllSlave', 15, 1);
          console.log("AC is now ON");
          acState = true;
        }

        if (acState && now >= offTargetTime) {
          // Send the command to the ESP32 board to turn off the AC
          send_cmd('ESP32AllSlave', 15, 0);
          console.log("AC is now OFF");
          acState = false;
        }
      }

      function stopTimer() {
        // Clear the interval to stop the timer
        clearInterval(timerInterval);

        // Reset the timer values and update the input fields
        acTimeValue = '08:00';
        offTimeValue = '18:00';
        document.getElementById('acTime').value = acTimeValue;
        document.getElementById('offTime').value = offTimeValue;
        timeValue = 0;
        localStorage.setItem('timeValue', timeValue);

        // Display a message indicating the timer has been stopped
        const timerStatusElement = document.getElementById('timerStatus');
        timerStatusElement.textContent = 'Timer has been stopped.';

        // Remove the timer values from localStorage
        localStorage.removeItem('acTimeValue');
        localStorage.removeItem('offTimeValue');
      }


      // Function to send the command to the ESP32 board
      function send_cmd(board, gpio_output, val) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", `set_LED?board=${board}&gpio_output=${gpio_output}&val=${val}`, true);
        xhr.send();
      }

      // Load saved time values from local storage on page load
      window.addEventListener('load', () => {
        acTimeValue = localStorage.getItem('acTimeValue') || '08:00';
        offTimeValue = localStorage.getItem('offTimeValue') || '18:00';
        timeValue = localStorage.getItem('timeValue');

        document.getElementById('acTime').value = acTimeValue;
        document.getElementById('offTime').value = offTimeValue;

        if(timeValue != 0){
        // Display a message indicating the timer has been set
        const timerStatusElement = document.getElementById("timerStatus");
        timerStatusElement.textContent = `Timer set: AC ON at ${acTimeValue}, AC OFF at ${offTimeValue}.`;
        }
      });

      // Display the current date and time when the page loads
      displayCurrentDate();
      displayCurrentTime();

      // Update the current time every second
      setInterval(displayCurrentTime, 1000);
      // ---------------------------------------------------------------------- AC Timer
    </script>
  </body>
</html>
<!-- )=====";