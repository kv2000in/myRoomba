#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <string.h>
#include <ArduinoOTA.h>

#ifndef APSSID
#define APSSID "myRover1"
#define APPSK  "revoRym123"
#endif

#define ROOMBA_READ_TIMEOUT 200;
/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
String mystring;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(8000);

static const char PROGMEM MANIFEST_JSON[] = R"rawliteral(
{
"name": "myROVER",
"short_name": "myrover",
"description": "A simple rover controller app.",
"display": "standalone",
"scope": "/",
"orientation":  "landscape"
}
)rawliteral";

static const char PROGMEM CHECK_SENSORS[] = R"rawliteral(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
</head>
<body>
</body>
</html>
)rawliteral";
static const char PROGMEM INDEX_HTML[] = R"rawliteral(!DOCTYPE html>

<html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=0">
        <link rel="manifest" href="manifest.json">
        <style>
            /*        disable scrolling    */
            html {
                margin: 0;
                padding: 0;
                overflow: hidden;
            }

            body {
                position: absolute;
                width: 100%;
                height: 100%;
                overflow: auto;
                margin: 0px;
                background: black;
            }

            * {
                margin: 0;
                padding: 0;
                box-sizing: border-box;
            }

            /*
         input
         {
         width:0px;
         height:0px;
         
         
         }
         input:checked + .slider-knob
         {
         background: green;
         -webkit-transform: translateY(-26px);
         -ms-transform: translateY(-26px);
         transform: translateY(-26px);
         }
         */
            .sliderbuttons {
                position: absolute;
                bottom: 40%;
                width: 100%;
                height: 20%;
            }

            .slidercolumn {
                width: 100%;
                height: 100%;
                position: relative;
            }

            .servosliderbuttons {
                position: absolute;
                bottom: 0%;
                width: 100%;
                height: 20%;
            }

            .fuelgaugecolumn {
                width: 49%;
                height: 100%;
            }

            .individual-status-lights {
                width: 100%;
                box-sizing: border-box;
                height: 33.3%;
            }

            /*     top 10%           */
            .notification-bar {
                width: 100%;
                display: flex;
                position: fixed;
                height: 10%;
                background: white;
                top: 0%;
            }

            .selectables {
                width: 20%;
            }

            .console {
                width: 20%;
                left: 0px;
            }

            .numericals {
                border: solid;
                border-radius: 20%;
                width: 10%;
            }

            /*                bottom 90%*/
            .half {
                width: 100%;
                display: flex;
                position: fixed;
                height: 45%;
            }

            /*                position top half-10% away from top*/
            #top-half {
                top: 10%;
            }

            /*                position bottom half -at bottom*/
            .bottom {
                right: 0;
                bottom: 0;
            }

            /*                divide 5 even width columns*/
            .twenty {
                width: 20%;
            }

            .ten {
                width: 10%;
            }

            .fifty {
                width: 50%;
                position: relative;
            }

            .sixty {
                width: 60%;
            }

            .eighty {
                width: 80%;
            }

            .horizontalslider {
                position: relative;
                height: 50%;
            }

            /*                if height is not sufficient - make room for steering*/
            @media only screen and (max-height: 444px) {
                .horizontalslider {
                    position: relative;
                    height: 10%;
                }
            }

            .slider-knob {
                height: 33.3%;
                top: 33.3%;
                position: relative;
                background: green;
                transition: .4s;
                border-radius: 1%;
            }

            .gearbuttons {
                width: 100%;
                height: 25%;
                border-radius: 20%;
            }

            .vertical3slider {
                position: relative;
                top: 0px;
                left: 0px;
                border: 2px solid blueviolet;
                align-self: center;
                height: 100%;
                margin: 0 auto;
            }

            .genericbuttons {
                height: 100%;
                border-radius: 10%;
            }

            .vertical2slider {
                position: relative;
                top: 0px;
                left: 0px;
                border: 2px solid blueviolet;
                align-self: center;
                height: 100%;
                margin: 0 auto;
            }

            .verticalslider {
                position: relative;
                top: 0px;
                left: 0px;
                border: 2px solid blueviolet;
                align-self: center;
                height: 100%;
                margin: 0 auto;
                display: flex;
            }

            .steering-container {
                position: relative;
                z-index: 1;
                bottom: -50px;
            }

            .steeringbuttonswrapper {
                position: absolute;
                top: 21%;
                left: 40%;
            }

            /*    CSS for Outer ring of joystick*/
            .steering-outer-circle {
                position: relative;
                top: 0px;
                left: 0px;
                border: 2px solid blueviolet;
                align-self: center;
                width: 300px;
                height: 300px;
                border-radius: 300px;
                margin: 0 auto;
            }

            /*CSS for inner ring of joystick*/
            .steering-inner-circle {
                position: absolute;
                border: 0px;
                top: 50px;
                left: 50px;
                /*opacity: 0.2;*/
                border: solid blueviolet;
                width: 250px;
                height: 250px;
                border-radius: 250px;
            }

            .steering-knob {
                position: absolute;
                border: solid blueviolet;
                top: -50px;
                left: 100px;
                background: blueviolet;
                /*opacity: 0.2;*/
                border: solid blueviolet;
                width: 100px;
                height: 100px;
                border-radius: 100px;
            }

            .three-slider {
                width: 100%;
                height: 100%;
                \ }

            .three-lights {
                width: 49%;
                height: 100%;
                background: red;
            }

            .switch {
                position: relative;
                display: inline-block;
                width: 60px;
                height: 34px;
            }

            .svrlight {
                background-color: red;
            }

            .switch input {
                display: none;
            }

            .pwrlight {
                position: relative;
                height: 26px;
                width: 26px;
            }

            .pwrlight:active {
                background-color: red;
                border: none;
            }

            .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #bf1f1f;
                -webkit-transition: .4s;
                transition: .4s;
            }

            .slider:before {
                position: absolute;
                content: "";
                height: 26px;
                width: 26px;
                left: 4px;
                bottom: 4px;
                background-color: white;
                -webkit-transition: .4s;
                transition: .4s;
            }

            input:checked + .slider {
                background-color: #2196F3;
            }

            input:focus + .slider {
                box-shadow: 0 0 1px #2196F3;
            }

            input:checked + .slider:before {
                -webkit-transform: translateX(26px);
                -ms-transform: translateX(26px);
                transform: translateX(26px);
            }

            #top-left-4 {
                margin: 0;
            }

            #top-left-5 {
                position: absolute;
                top: 0%;
                right: 0%;
            }

            .contrast {
                color: white;
            }

            th {
                font-size: 8;
                font-style: normal;
            }
        </style>
        <title>myRover</title>
    </head>
    <body>
        <!--<img id="mjpeg_dest"src="/cam_pic_new.php" style="width:100%;"> -->
        <!--<div class="joystick-container" id="joystick-container">
     <div class="outer" id="outer"><div class="inner" id="inner"></div></div>
     </div>-->
        <div class="notification-bar" id="notification">
            <div class="console" id="mylocalconsole"></div>
            <div class="selectables"></div>
            <div class="smbusnumericals numericals" id="smbusbatteryI"></div>
            <div class="smbusnumericals numericals" id="smbusbatteryV"></div>
            <div class="smbusnumericals numericals" id="smbusbatteryT"></div>
            <div class="smbusnumericals numericals" id="smbusbatteryc"></div>
            <div class="smbusnumericals numericals" id="smbusbatteryR"></div>
            <div class="smbusnumericals numericals" id="smbusbatteryF"></div>
            <div class="numericals" id="analogbatteryV"></div>
        </div>
        <div class="half" id="top-half">
            <div class="ten vertical3slider" id="top-left-1" id="gearB">
                <button class="gearbuttons" id="forwardgearbuttonB" onclick='doSend(Start)'>Start
          </button>
                <button class="gearbuttons" id="stopgearbuttonB" onclick='doSend(Stop)'>Stop
          </button>
                <button class="gearbuttons" id="reversegearbuttonB" onclick='doSend(Safe)'>Safe
          </button>
                <button class="gearbuttons" id="reversegearbuttonB" onclick='doSend(Full)'>Full
          </button>
            </div>
            <div class="ten vertical2slider" id="top-left-2">
                <button class="gearbuttons" id="forwardgearbuttonB" onclick='doSend(Reset)'>Reset
          </button>
                <button class="gearbuttons" id="forwardgearbuttonB" onclick='doSend(PowerDown)'>Power
          </button>
            </div>
            <div class="ten vertical2slider" id="top-left-3">
                <button class="gearbuttons" id="forwardgearbuttonB" onclick='doSend(Clean)'>Clean
          </button>
                <button class="gearbuttons" id="forwardgearbuttonB" onclick='doSend(MaxClean)'>MaxClean
          </button>
                <button class="gearbuttons" id="stopgearbuttonB" onclick='doSend(Spot)'>Spot
          </button>
                <button class="gearbuttons" id="reversegearbuttonB" onclick='doSend(SeekDock)'>Dock
          </button>
            </div>
            <div class="fifty">
                <div class="twenty vertical2slider" id="top-left-4">
                    <button class="gearbuttons" id="connectWS" onclick='doConnect()'>Connect
        </button>
                    <button class="gearbuttons" id="closeWS" onclick='doClose()'>Close
        </button>
                    <button class="gearbuttons" id="reloadfromsource" onclick='location.reload()'>Reload
        </button>
                    <button class="gearbuttons" id="getbatteryinforbtn" onclick='getbatteryinfo'>Battery
        </button>
                </div>
                <div class="eighty" id="top-left-5">
                    <table>
                        <tr>
                            <th rowspan="2">
                                <label class="contrast">Side Brush</label>
                            </th>
                            <td>
                                <input type="range" id="sidebrushPWM" min="-127" max="127" value="0" step="5">
                            </td>
                            <td>
                                <input type="radio" id="sidebrushccw" name="sidebrushdirection" checked>
                                <label class="contrast">ccw</label>
                                <input type="radio" id="sidebrushcw" name="sidebrushdirection">
                                <label class="contrast">cw</label>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <button id="resetsidebrush" onclick='document.getElementById("sidebrushPWM").value="0"'>Reset</button>
                            </td>
                            <td>
                                <input type="radio" id="sidebrushon" name="sidebrushonoff">
                                <label class="contrast">on</label>
                                <input type="radio" id="sidebrushoff" name="sidebrushonoff" checked>
                                <label class="contrast">off</label>
                            </td>
                        </tr>
                        <tr>
                            <th rowspan="2">
                                <label class="contrast">Main Brush</label>
                            </th>
                            <td>
                                <input type="range" id="mainbrushPWM" min="-127" max="127" value="0" step="5">
                            </td>
                            <td>
                                <input type="radio" id="mainbrushin" name="mainbrushdirection" checked>
                                <label class="contrast">in</label>
                                <input type="radio" id="mainbrushout" name="mainbrushdirection">
                                <label class="contrast">out</label>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <button id="resetmainbrush" onclick='document.getElementById("mainbrushPWM").value = "0"'>Reset</button>
                            </td>
                            <td>
                                <input type="radio" id="mainbrushon" name="mainbrushonoff">
                                <label class="contrast">on</label>
                                <input type="radio" id="mainbrushoff" name="mainbrushonoff" checked>
                                <label class="contrast">off</label>
                            </td>
                        </tr>
                        <tr>
                            <th rowspan="2">
                                <label class="contrast">Vacuum</label>
                            </th>
                            <td>
                                <input type="range" id="vacuumPWM" min="0" max="127" value="0" step="5">
                            </td>
                            <td></td>
                        </tr>
                        <tr>
                            <td>
                                <button id="resetvacuum" onclick='document.getElementById("vacuumPWM").value = "0"'>Reset</button>
                            </td>
                            <td>
                                <input type="radio" id="vacuumon" name="vacuumonoff">
                                <label class="contrast">on</label>
                                <input type="radio" id="vacuumoff" name="vacuumonoff" checked>
                                <label class="contrast">off</label>
                            </td>
                        </tr>
                        <tr>
                            <th></th>
                            <td>
                                <button id="sendmotorspwm" onclick='sendPWMMotors(document.getElementById("mainbrushPWM").value,document.getElementById("sidebrushPWM").value,document.getElementById("vacuumPWM").value)'>Send</button>
                                <button id="allmotorsoff" onclick='sendRunMotorsFullSpeed(0,0,0,0,0)'>OFF</button>
                            </td>
                            <td>
                                <button id="sendmotors" onclick='sendRunMotorsFullSpeed(document.getElementById("mainbrushout").checked? 1:0, document.getElementById("sidebrushcw").checked? 1:0, document.getElementById("mainbrushon").checked? 1:0,document.getElementById("vacuumon").checked? 1:0,document.getElementById("sidebrushon").checked? 1:0)'>Send</button>
                            </td>
                        </tr>
                    </table>
                </div>
            </div>
        </div>
        <div class="half bottom" id="bottom-half">
            <div class="sixty" id="bottom-middle">
                <div class="horizontalslider"></div>
                <div class="steering-container" id="steering-container">
                    <div class="steering-outer-circle" id="steering-outer-circle">
                        <!--                            <div class="steering-inner-circle" id="steering-inner-circle"></div>-->
                        <div class="steering-knob" id="steering-knob"></div>
                        <div class="steeringbuttonswrapper"></div>
                    </div>
                </div>
            </div>
            <div class="twenty vertical3slider" id="gearA">
                <div class="slidercolumn" id="slidercolumnA">
                    <button class="sliderbuttons" id="speedcontrolA">Move
        </button>
                </div>
            </div>
            <div class="twenty verticalslider" id="bottom-left">
                <div class="sendcommand" id="sendcommand">
                    <input list="availablecommands" id="mycommands">
                    <button id="sendcommandbutton" class="genericbutton" onclick='sendcommandfromtext()'>Send</button>
                    <datalist id="availablecommands"></datalist>
                </div>
            </div>
        </div>
        <div class="detailedlistofcommands">
        <!--todo!-->
        </div>
    </body>
    <script>

        /*       For eventhandler passive support option, see here: https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
     */

        let passiveSupported = false;
        try {
            const options = {
                get passive() {
                    // This function will be called when the browser
                    //   attempts to access the passive property.
                    passiveSupported = true;
                    return false;
                }
            };
            window.addEventListener("test", null, options);
            window.removeEventListener("test", null, options);
        } catch (err) {
            passiveSupported = false;
        }

        /* function for 2s complement*/
        function createToInt(size) {
            if (size < 2) {
                throw new Error('Minimum size is 2');
            } else if (size > 64) {
                throw new Error('Maximum size is 64');
            }

            // Determine value range
            const maxValue = (1 << (size - 1)) - 1;
            const minValue = -maxValue - 1;

            return (value)=>{
                if (value > maxValue || value < minValue) {
                    throw new Error(`Int${size} overflow`);
                }

                if (value < 0) {
                    return (1 << size) + value;
                } else {
                    return value;
                }
            }
            ;
        }

        const toInt16 = createToInt(16);
        //using 2 byte numbers for velocity and radius

        /*

myval = toInt16(-200).toString(16); // trying to run roomba backwards (-ve velocity) at 200 mm/s, myval 2 complements ff 38 . HIgh byte ff, low byte 38
send as Serial sequence: [137] [Velocity high byte] [Velocity low byte] [Radius high byte] [Radius low byte]


[137] [255] [56] [1] [244]

if (myval.length>2){
parseInt(myval.slice(-2), 16); //gives 56
parseInt(myval.slice(0,myval.length-2), 16); // gives 255
}
else
{
parseInt(myval, 16);
}


*/

        //ROOMBA OPCODES
        var Start = 128;
        var Stop = 173;
        var Reset = 7;
        var Safe = 131;
        var Full = 132;
        var Clean = 135;
        var MaxClean = 136;
        var Spot = 134;
        var SeekDock = 143;
        var PowerDown = 133;
        var Drive = 137;
        var DriveDirect = 145;
        var DrivePWM = 146;
        var RunMotorsFullSpeed = 138;
        var PWMMotors = 144;
        var maxFvelocity = 500;
        //mm/s
        var maxRvelocity = -500;
        var maxCWRadius = -2000;
        // mm
        var maxCCWRadius = 2000;
        var velocitySteps = 20;
        //steps in mm/s
        var currentRadius = 32767;
        //default straight
        var currentVelocity = 0;
        /*
It takes four data bytes, interpreted as two 16-bit signed
values using two’s complement. (http://en.wikipedia.org/wiki/Two%27s_complement) The first two bytes
specify the average velocity of the drive wheels in millimeters per second (mm/s), with the high byte
being sent first. The next two bytes specify the radius in millimeters at which Roomba will turn. The
longer radii make Roomba drive straighter, while the shorter radii make Roomba turn more. The radius is
measured from the center of the turning circle to the center of Roomba. A Drive command with a
positive velocity and a positive radius makes Roomba drive forward while turning toward the left. A
negative radius makes Roomba turn toward the right. Special cases for the radius make Roomba turn in
place or drive straight, as specified below. A negative velocity makes Roomba drive backward.
        
        */

        function HbyteLbyte(val) {
            var myval = toInt16(val).toString(16);
            var Hbyte = 0;
            var Lbyte = 0;
            if (myval.length > 2) {

                Lbyte = parseInt(myval.slice(-2), 16);
                Hbyte = parseInt(myval.slice(0, myval.length - 2), 16);
            } else {

                Lbyte = parseInt(myval, 16);
            }
            return Hbyte + ":" + Lbyte;

        }

        function sendDrive(velocity, radius) {
            /*
        Velocity (-500 – 500 mm/s)
        Radius (-2000 – 2000 mm) 
        
        Radius Special Cases:
        Straight = 32768 or 32767 = 0x8000 or 0x7FFF //int16 overflow with 32768. use 32767
        Turn in place clockwise = -1 = 0xFFFF
        Turn in place counter-clockwise = 1 = 0x0001
        
        (maxFVelocity<velocity)||(velocity<maxRvelocity)
        */

            var myvelocity = HbyteLbyte(velocity);
            var myradius = HbyteLbyte(radius);
            var myvelocityHbyte = myvelocity.split(":")[0];
            var myvelocityLbyte = myvelocity.split(":")[1];
            var myradiusHbyte = myradius.split(":")[0];
            var myradiusLbyte = myradius.split(":")[1];

            //console.log(myvelocityHbyte, myvelocityLbyte, myradiusHbyte, myradiusLbyte);

            doSend(Drive);
            doSend(myvelocityHbyte);
            doSend(myvelocityLbyte);
            doSend(myradiusHbyte);
            doSend(myradiusLbyte);
        }

        function sendDriveDirect(rightwheelvelocity, leftwheelvelocity) {
            /*Serial sequence: [145] [Right velocity high byte] [Right velocity low byte] [Left velocity high byte]
[Left velocity low byte]*/

            var myRvelocity = HbyteLbyte(rightwheelvelocity);
            var myLvelocity = HbyteLbyte(leftwheelvelocity);
            var myRvelocityHbyte = myRvelocity.split(":")[0];
            var myRvelocityLbyte = myRvelocity.split(":")[1];
            var myLvelocityHbyte = myLvelocity.split(":")[0];
            var myLvelocityLbyte = myRvelocity.split(":")[1];

            console.log(myRvelocityHbyte, myRvelocityLbyte, myLvelocityHbyte, myLvelocityLbyte);

            doSend(DriveDirect);
            doSend(myRvelocityHbyte);
            doSend(myRvelocityLbyte);
            doSend(myLvelocityHbyte);
            doSend(myLvelocityLbyte);
        }
        function sendDrivePWM(rightwheelpwm, leftwheelpwm) {
            /*Serial sequence: [146] [Right PWM high byte] [Right PWM low byte] [Left PWM high byte] [Left PWM
low byte] 
-255 to 255 A positive PWM makes that wheel
drive forward, while a negative PWM makes it drive backward
*/
            var myRpwm = HbyteLbyte(rightwheelpwm);
            var myLpwm = HbyteLbyte(leftwheelpwm);
            var myRpwmHbyte = myRpwm.split(":")[0];
            var myRpwmLbyte = myRpwm.split(":")[1];
            var myLpwmHbyte = myLpwm.split(":")[0];
            var myLpwmLbyte = myLpwm.split(":")[1];

            console.log(myRpwmHbyte, myRpwmLbyte, myLpwmHbyte, myLpwmLbyte);
            doSend(DrivePWM);
            doSend(myRpwmHbyte);
            doSend(myRpwmLbyte);
            doSend(myLpwmHbyte);
            doSend(myLpwmLbyte);

        }
        function sendRunMotorsFullSpeed(mainbrushoutward, sidebrushCW, mainbrush, vacuum, sidebrush) {
            /*
        Bits 0-2: 0 = off, 1 = on at 100% pwm duty cycle
 Bits 3 & 4: 0 = motor’s default direction, 1 = motor’s opposite direction. Default direction for the side
brush is counterclockwise. Default direction for the main brush/flapper is inward.
        To turn on the main brush inward and the side brush clockwise, send: [138] [13]
        */
            myval = "000" + mainbrushoutward + sidebrushCW + mainbrush + vacuum + sidebrush;
            //console.log(parseInt(myval, 2));
            doSend(RunMotorsFullSpeed);
            doSend(parseInt(myval, 2));

        }
        function sendPWMMotors(mainbrushPWM, sidebrushPWM, vacuumPWM) {
            /*
            [144] [Main Brush PWM] [Side Brush PWM] [Vacuum PWM]
             With each data byte, you specify the duty cycle for the low side driver (max 128). For
example, if you want to control a motor with 25% of battery voltage, choose a duty cycle of 128 * 25%
= 32. The main brush and side brush can be run in either direction. The vacuum only runs forward.
Positive speeds turn the motor in its default (cleaning) direction. Default direction for the side brush is
counterclockwise. Default direction for the main brush/flapper is inward.

 Main Brush and Side Brush duty cycle (-127 – 127)
 Vacuum duty cycle (0 – 127)

           doSend(144);doSend(-32);doSend(32);doSend(0); //works
           */
            doSend(PWMMotors);
            doSend(mainbrushPWM);
            doSend(sidebrushPWM);
            doSend(vacuumPWM);
        }

        function sendcommandfromtext() {

            doSend(document.getElementById("mycommands").value);
        }

        function parse_incoming_websocket_messages(data) {
            //Data is S-255:OK or S-255:FAIL or C-F:OK or C-F:FAIL
            //Determine if it is OK response or FAIL response, splice it on ":", log it if it is FAIL, proceed if it is OK
            var myResponse = data.split(":")[1];
            var returningdata = data.split(":")[0];
            switch (myResponse) {
            case ("FAIL"):
                document.getElementById("mylocalconsole").innerHTML = data;
                break;
            case ("OK"):
                var returningdata = data.split(":")[0];
                //Now find the first character and do something
                response_based_on_first_char(returningdata);
            default:
                document.getElementById("mylocalconsole").innerHTML = data;
                break;

            }

        }

        /*function handleOrientation(event) {
   var y = event.gamma;
   var z = event.alpha;
   var x = event.beta;
   //console.log("alpha: " + z + "\n");
   //document.getElementById("mylocalconsole").innerHTML = "alpha: " + z + "\n";
   //Device has moved more than the step
   if(Math.abs(y-previousY)>steeringanlgesteps){
   
   document.getElementById("mylocalconsole").innerHTML = "<W-"+(y+90)+">";  
   //previousY=y;
   }
   
   
   }*/

        /*
   works on chrome 52 on android (GalaxyS5). I think beyond chrome 60 - needs window.isSecureContext to be true - for access to sensors
   secure context means everything has to be ssl. Doesn't work even if I save the file locally on the phone and just connect to websocket. On the other hand, saving it locally on the computer and using it - becomes a secure context and sensors can be accessed. Yes, macbook has orientation sensors too.
   
   window.addEventListener('deviceorientation', handleOrientation);
   */
        /*    with x1 and y1 being the coordinates of the center of a circle, (x-x1)^2+(y-y1)^2 = radius^2
   so for any given value of x, y = sqrt(radius^2-(x-x1)^2)+y1*/

        var draggable = document.getElementById('steering-knob');
        var outer = document.getElementById('steering-outer-circle');
        var touchstartX;
        var touchstartY;
        //get center coordinates of the steering-outer-circle
        var centerofsteeringcircleX;
        var centerofsteeringcircleY;
        var rect1 = outer.getBoundingClientRect();
        var rect;
        var myX;
        var myY;
        var myoffsetfromcontainerX;
        var myoffsetfromcontainerY;
        var mymaxY;
        var myangle;
        var steeringanlgesteps = 5;
        var previoussentangle = 0;
        var steeringrange = 4000;
        var steeringservomidangle = 0;
        var steeringservomaxangle = 2000;
        var steeringservominangle = -2000;
        centerofsteeringcircleX = rect1.left + ((rect1.right - rect1.left) / 2);
        centerofsteeringcircleY = rect1.top + ((rect1.bottom - rect1.top) / 2);

        draggable.addEventListener('touchstart', function(event) {
            var touch = event.targetTouches[0];
            rect = draggable.getBoundingClientRect();
            //Start point -center of steering knob
            touchstartX = rect.left + ((rect.width) / 2);
            touchstartY = rect.top + ((rect.height) / 2);
            rect1 = outer.getBoundingClientRect();
            //don't want the steering knob to go below horizon. Find the lowermost allowable steeringknob center Y pixel. it should be steering-knob's radius away from the bottom of the steering container
            mymaxY = window.innerHeight - (rect.height) / 2;
            //                                myoffsetfromcontainerX=rect.left-rect1.left;
            //                                myoffsetfromcontainerY=rect.top-rect1.top;
            myoffsetfromcontainerX = draggable.offsetLeft;
            myoffsetfromcontainerY = draggable.offsetTop;

            //                 console.log("touchstartx="+touchstartX+", touchstartY="+touchstartY+", myoffsetX="+myoffsetfromcontainerX+", myoffsetY="+myoffsetfromcontainerY+", mymaxY="+mymaxY);
        }, passiveSupported ? {
            passive: true
        } : false);
        draggable.addEventListener('touchmove', function(event) {
            var touch = event.targetTouches[0];
            //get the x point of touch;
            myX = touch.pageX;
            myY = centerofsteeringcircleY - Math.sqrt((150 * 150) - (Math.pow((myX - centerofsteeringcircleX), 2)));

            //check if falling below horizon
            if (myY < mymaxY) {
                draggable.style.left = myoffsetfromcontainerX + (myX - touchstartX) + 'px';
                draggable.style.top = myoffsetfromcontainerY + (myY - touchstartY) + 'px';

                /*                            calculate angle off center at current position
                                 myangle = -135(Left) to -45 (right). multiplied by 2 to get -270 to -90.
                                 convert to 0-180 by adding 270.
                                 Convert 180 scale to "Range" scale by *Range/180
                                 Shift the mid point of the scale so that Range/2 = midservoangle.*/
                //myangle=parseInt((((Math.atan2(myY - centerofsteeringcircleY, myX - centerofsteeringcircleX) * 180 / Math.PI)*2)+270)*(steeringrange/180))+(steeringservomidangle-steeringrange/2);
                myangle = parseInt((((Math.atan2(myY - centerofsteeringcircleY, myX - centerofsteeringcircleX) * 180 / Math.PI) * 2) + 270));
                if (myangle < 0) {
                    myangle = 1;
                }
                if (myangle > 179) {
                    myangle = 179;
                }
                myangle = parseInt(myangle * (steeringrange / 180));
                if (myangle > 2000) {
                    myangle = myangle - 4000;
                }
                //else if (myangle<2000){myangle= 2000-myangle;}  
                //else {}
                //myangle = 4000-myangle;

            }

            if (Math.abs(myangle - previoussentangle) > steeringanlgesteps) {

                //send command only if it is with in the bounds of the steering servo
                //if (myangle >= steeringservominangle && myangle <= steeringservomaxangle){
                //console.log(myangle); 
                currentRadius = myangle;
                sendDrive(currentVelocity, currentRadius);
                previoussentangle = myangle;

                // }

            }
            //event.preventDefault();
        }, passiveSupported ? {
            passive: true
        } : false);

        draggable.addEventListener('touchend', function(event) {
            draggable.style.left = '100px';
            draggable.style.top = '-50px';
            currentRadius = 32767;
            sendDrive(currentVelocity, currentRadius);
        }, passiveSupported ? {
            passive: true
        } : false);

        //sliderbasefunction returns at what percent of total slidable height -is the slider at
        function sliderbasefunction(event, whocalledme, whichelement) {
            var mytotalheight;
            var myminY;
            var mymaxY;
            var mytouchY;
            var mystylefromtop;
            var mypercentageslide;
            var myouterboundrect = document.getElementById('slidercolumn' + whichelement).getBoundingClientRect();
            //var myslider = document.getElementById('speedcontrol'+whichelement);
            var mysliderrect = whocalledme.getBoundingClientRect();
            mytotalheight = myouterboundrect.height;
            myminY = myouterboundrect.top + (mysliderrect.height / 2);
            mymaxY = myminY + (mytotalheight - mysliderrect.height);
            var myslidertouch = event.targetTouches[0];
            mytouchY = myslidertouch.pageY;
            if (mytouchY >= myminY && mytouchY <= mymaxY) {
                //move whocalledme to mytouchY

                mypercentageslide = ((mymaxY - mytouchY) / (mymaxY - myminY)) * 100;

                if ((80 - mypercentageslide) > 0) {
                    mystylefromtop = 80 - mypercentageslide;
                } else {
                    mystylefromtop = 0;
                }
                whocalledme.style.top = mystylefromtop + '%';
                //console.log("mytouchY="+mytouchY+" myminY="+myminY+" mymaxy="+mymaxY+" mypercentageslide ="+mypercentageslide+" style from top % ="+mystylefromtop);

            } else {

                //sliding out of bounds - lower
                if (mytouchY >= myminY) {
                    whocalledme.style.top = 80 + '%';
                    mypercentageslide = 0;
                }//sliding out of bounds - upper
                else if (mytouchY <= mymaxY) {

                    whocalledme.style.top = 0 + '%';
                    mypercentageslide = 100;
                }

            }
            return Math.round(mypercentageslide);
        }
        var mysliderA = document.getElementById('speedcontrolA');
        var mypreviousPWMAsent = 0;
        var mypreviousPWMBsent = 0;
        var mypreviousvalueXsent = 0;
        var mypreviousvalueYsent = 0;
        var mypreviousvalueZsent = 0;
        var valuetosendA = 0;
        var valuetosendB = 0;
        var valuetosendX = 0;
        var valuetosendY = 0;
        var valuetosendZ = 0;
        var servoXmaxangle = 150;
        var servoXminangle = 30;
        var servoXanglesteps = 5;
        var servoYmaxangle = 150;
        var servoYminangle = 30;
        var servoYanglesteps = 5;
        var servoZmaxangle = 150;
        var servoZminangle = 30;
        var servoZanglesteps = 5;
        var motorAmovingforward = false;
        var motorAmoving = false;

        mysliderA.addEventListener('touchmove', function(event) {
            var whatpercentslide = sliderbasefunction(event, this, "A");

            //if slider is between 60-100% - move forward with increasing speed. 
            //if slider is between 40-0% - move backwards with increasing speed.
            //if slider is between 40-60% - stop.

            if (whatpercentslide < 40) {

                valuetosendA = Math.round(((maxRvelocity) * ((Math.abs(50 - whatpercentslide) * 2) / 100)));
                //console.log(valuetosendA);
                if (Math.abs(valuetosendA - mypreviousPWMAsent) > velocitySteps) {

                    console.log(valuetosendA);
                    sendDrive(valuetosendA, currentRadius);

                    mypreviousPWMAsent = valuetosendA;
                    currentVelocity = valuetosendA;
                }

            } else if (whatpercentslide > 60) {
                valuetosendA = Math.round(((maxFvelocity) * ((Math.abs(50 - whatpercentslide) * 2) / 100)));
                //console.log(valuetosendA);
                if (Math.abs(valuetosendA - mypreviousPWMAsent) > velocitySteps) {

                    console.log(valuetosendA);
                    sendDrive(valuetosendA, currentRadius);

                    mypreviousPWMAsent = valuetosendA;
                    currentVelocity = valuetosendA;

                } else {//stop

                }

            }

        }, passiveSupported ? {
            passive: true
        } : false);

        //when releasing speedcontrol button - it returns to mid position and motor stops.
        mysliderA.addEventListener('touchend', function(event) {
            motorAmoving = false;
            currentVelocity = 0;
            sendDrive(currentVelocity, currentRadius);

            mysliderA.style.top = 40 + '%';

        }, passiveSupported ? {
            passive: true
        } : false);

        var onlongtouch;
        var timer;
        var touchduration = 750;
        //length of time we want the user to touch before we do something

        function touchstart(id, val) {
            timer = setTimeout(onlongtouch, touchduration, id, val);

        }

        function touchend() {

            //stops short touches from firing the event
            if (timer)
                clearTimeout(timer);
            // clearTimeout, not cleartimeout..
        }
        /*
   //if holding the steering knob for longer than 750 ms, send the current angle. 
   onlongtouch=doSend("<W-"+myangle+">");*/

        var websock;
        var iswebsocketconnected = false;
        function doConnect() {
            // websock = new WebSocket('ws://' + window.location.hostname + ':8000/');
            websock = new WebSocket('ws://192.168.4.1:8000/');
            websock.onopen = function(evt) {
                console.log('websock open');
                iswebsocketconnected = true;

            }

            websock.onclose = function(evt) {
                iswebsocketconnected = false;
                console.log('websock close');
                //clear all the intervals - doesn't work. still dealing with "ghost" setIntervals

            }
            websock.onerror = function(evt) {
                console.log(evt);
            }
            websock.onmessage = function(evt) {
                //console.log(evt); 
                console.log(evt.data);
                //parse_incoming_websocket_messages(evt.data);
            }
        }
        function doClose() {
            //detach steering servo
            //attachordetachservos("w","d");

            websock.close();
        }
        function doSend(message) {
            if (iswebsocketconnected == true) {
                if (websock.readyState == websock.OPEN) {
                    websock.send(message);
                    //console.log(message);
                } else {

                    console.log("websocket is in an indeterminate state");
                }
            }
        }
    </script>
</html>
)rawliteral";


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
//Serial1.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
switch(type) {
case WStype_DISCONNECTED:
//Serial1.printf("[%u] Disconnected!\r\n", num);
break;
case WStype_CONNECTED:
{
IPAddress ip = webSocket.remoteIP(num);
//Serial1.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
//itoa( cm, str, 10 );
//  webSocket.sendTXT(num, str, strlen(str));
}
break;
case WStype_TEXT:
{
//Serial1.printf("[%u] get Text: %s\r\n", num, payload);
//Send whatever comes on the WS to Atmega.
char *mystring1 = (char *)payload;
mystring = mystring1;
Serial.write(mystring.toInt());

}
break;
case WStype_BIN:
//Serial1.printf("[%u] get binary length: %u\r\n", num, length);
break;
default:
//Serial1.printf("Invalid WStype [%d]\r\n", type);
break;
}
}

void handleRoot()
{
server.sendHeader("Cache-Control","max-age=604800");
server.send(200, "text/html", INDEX_HTML);

}
void handleIndex()
{
//server.send_P(200, "text/html", INDEX_HTML);
server.sendHeader("Cache-Control","max-age=604800");
server.send(200, "text/html", INDEX_HTML);

}


void handleManifest()
{
server.sendHeader("Cache-Control","max-age=604800");
server.send(200, "application/json", MANIFEST_JSON);

}
void handleCheck()
{
server.sendHeader("Cache-Control","max-age=604800");
server.send(200, "text/html", CHECK_SENSORS);

}

void handleNotFound()
{
String message = "File Not Found\n\n";
message += "URI: ";
message += server.uri();
message += "\nMethod: ";
message += (server.method() == HTTP_GET)?"GET":"POST";
message += "\nArguments: ";
message += server.args();
message += "\n";
for (uint8_t i=0; i<server.args(); i++){
message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
}
server.send(404, "text/plain", message);
}


/*******************Serial Read Functions ************************/
//Serial Read stuff

const byte numChars = 32;
char receivedChars[numChars];



void recvWithStartEndMarkers() {
static boolean recvInProgress = false;
static byte ndx = 0;
char startMarker = '<';
char endMarker = '>';
char rc;

while (Serial.available() > 0 ) {
rc = Serial.read();

if (recvInProgress == true) {
if (rc != endMarker) {
receivedChars[ndx] = rc;
ndx++;
if (ndx >= numChars) {
ndx = numChars - 1;
}
}
else {
receivedChars[ndx] = '\0'; // terminate the string
recvInProgress = false;
webSocket.sendTXT(0,receivedChars,ndx);
//Serial1.println(receivedChars);
//Serial.println(receivedChars);
ndx = 0;

}
}

else if (rc == startMarker) {
recvInProgress = true;
}
}
}




/*******************Serial Read Functions ************************/
/*****generic serial read and send to websocket function ***************/

void recvAndSendtoWebSocket(){
byte  mybytes[64];
if (Serial.available()>0){
  int mylength=Serial.available();
for (int p=0;p<mylength;p++) {
mybytes[p] = Serial.read();
  
  }
  webSocket.sendBIN(0,mybytes,mylength);
}
}
/*
bool getDataFromROOMBA()(uint8_t* dest, uint8_t len)
{
  while (len-- > 0)
  {
    unsigned long startTime = millis();
    while (!Serial.available())
    {
      // Look for a timeout
      if (millis() > startTime + ROOMBA_READ_TIMEOUT)
        return false; // Timed out
    }
    *dest++ = Serial.read();
  }
  return true;
}
*/
/*****END *** generic serial read and send to websocket function ***************/
void setup()
{ 

//Change Roomba Baud rate to 19200
/*
Use the Baud Rate Change pin (pin 5 on the Mini-DIN connector) to change Roomba’s baud rate. After
turning on Roomba, wait 2 seconds and then pulse the Baud Rate Change low three times. Each pulse
should last between 50 and 500 milliseconds. Roomba will communicate at 19200 baud until the
processor loses battery power or the baud rate is explicitly changed by way of the OI.
*/
delay(5000);
pinMode(2,OUTPUT);
digitalWrite(2, HIGH);
delay(1000);
digitalWrite(2, LOW);
delay(400);
digitalWrite(2, HIGH);
delay(100);
digitalWrite(2, LOW);
delay(400);
digitalWrite(2, HIGH);
delay(100);
digitalWrite(2, LOW);
delay(400);
digitalWrite(2, HIGH);
delay(100);

Serial.begin(19200);

/***************** AP mode*******************/

WiFi.softAP(ssid, password);
//WiFi.printDiag(Serial1);
IPAddress myIP = WiFi.softAPIP();


/***********************************************/

/*****************Client Mode******************/
/*
WiFiMulti.addAP(ssid, password);
while(WiFiMulti.run() != WL_CONNECTED) {
Serial1.print(".");
delay(100);
}
Serial1.println("");
Serial1.print("Connected to ");
Serial1.println(ssid);
Serial1.print("IP address: ");
Serial1.println(WiFi.localIP());
*/
/**********************************************/

server.on("/", handleRoot);
server.on("/index.html",handleIndex);
server.on("/manifest.json",handleManifest);
server.on("/check.html",handleCheck);
server.onNotFound(handleNotFound);
//  server.onNotFound([]() {                              // If the client requests any URI
//    if (!handleFileRead(server.uri()))                  // send it if it exists
//      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
//  });

server.begin();

webSocket.begin();
webSocket.onEvent(webSocketEvent);


/* ************SPIFFS********************* */
//  if (SPIFFS.begin()){Serial1.println("file system mounted");};
//
//  //Open the "Save.txt" file and check if we were saving before the reset happened
//  File q = SPIFFS.open("/Save.txt", "r");
//  if (q.find("Y")){saveData=true;}
//  q.close();

/*********************************************/

/* ************OTA********************* */

// Port defaults to 8266
// ArduinoOTA.setPort(8266);

// Hostname defaults to esp8266-[ChipID]
// ArduinoOTA.setHostname("myesp8266");

// No authentication by default
// ArduinoOTA.setPassword((const char *)"123");

ArduinoOTA.onStart([]() {
//Serial1.println("Start");
});
ArduinoOTA.onEnd([]() {
//Serial1.println("\nEnd");
});
ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//Serial1.printf("Progress: %u%%\r", (progress / (total / 100)));
});
ArduinoOTA.onError([](ota_error_t error) {
/*
Serial1.printf("Error[%u]: ", error);
if (error == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
else if (error == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
else if (error == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
else if (error == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
else if (error == OTA_END_ERROR) Serial1.println("End Failed");
*/
});
ArduinoOTA.begin();

/****************************************************/  


}



void loop()
{
webSocket.loop();
server.handleClient();
ArduinoOTA.handle();
//recvWithStartEndMarkers();
recvAndSendtoWebSocket();

}
