#include "HelpPage.h"

const char HELP_PAGE[] PROGMEM = R"rawliteral(
<title>Help</title>
<style>
body{font-family:Arial;background:#f2f2f2;margin:20px}
.card{background:white;margin:10px auto;padding:15px;border-radius:10px;max-width:900px}
table{border-collapse:collapse;width:100%}
th,td{border:1px solid #ccc;padding:8px;text-align:left}
th{background:#e8e8e8}
h1,h2{color:#003366}
code{background:#eeeeee;padding:2px 4px}
</style>
</head>
<body>
<div class='card'>
<h1>Timer Relay - Help</h1>
<p>This page describes the digital inputs and their functions.</p>
<h2>Digital Inputs</h2>
<table>
<tr><th>Function</th><th>ESP32 GPIO</th><th>MCP23017 #2</th><th>Description</th></tr>
<tr><td>START</td><td>GPIO D4</td><td>GPA7</td><td>Short press: Start irrigation cycle.</td></tr>
<tr><td>STOP</td><td>GPIO D5</td><td>GPA6</td><td>Short press: Stop irrigation cycle.<br>Long press: Toggle Wifi activation.</td></tr>
<tr><td>Factory Reset</td><td>GPIO D18</td><td>-</td><td>Read only during boot. Loads default configuration and overwrites stored Preferences.</td></tr>
<tr><td>Debug Setup</td><td>GPIO D19</td><td>-</td><td>Read only during boot. When active, enables setup debug mode, pauses after initialization and displays diagnostic information before entering the main loop.</td></tr>
</table>
<h2>Input Logic</h2>
<ul>
<li>All digital inputs are active LOW.</li>
<li>Internal pull-up resistors are enabled.</li>
<li>Open input = HIGH.</li>
<li>Input connected to GND = LOW (active).</li>
</ul>
<h2>MCP23017 #2 Input Mapping</h2>
<table>
<tr><th>BankD Bit</th><th>MCP Pin</th><th>Function</th></tr>
<tr><td>7</td><td>GPA7</td><td>START</td></tr>
<tr><td>6</td><td>GPA6</td><td>STOP</td></tr>
<tr><td>5</td><td>GPA5</td><td>Unused</td></tr>
<tr><td>4</td><td>GPA4</td><td>Unused</td></tr>
<tr><td>3</td><td>GPA3</td><td>Unused</td></tr>
<tr><td>2</td><td>GPA2</td><td>Unused</td></tr>
<tr><td>1</td><td>GPA1</td><td>Unused</td></tr>
<tr><td>0</td><td>GPA0</td><td>Unused</td></tr>
</table>
<h2>GP Output Mapping</h2>
<table>
<tr><th>BankC Bit</th><th>MCP Pin</th><th>Function</th></tr>
<!-- RELAY_NUMBER 22 outputs 23-24  not available to timer-->
<tr><td>7</td><td>GPB7</td><td>Unused</td></tr>
<tr><td>6</td><td>GPB6</td><td>Unused</td></tr>
<tr><td>5</td><td>GPB5</td><td>IRR22</td></tr>
<tr><td>4</td><td>GPB4</td><td>IRR21</td></tr>
<tr><td>3</td><td>GPB3</td><td>IRR20</td></tr>
<tr><td>2</td><td>GPB2</td><td>IRR19</td></tr>
<tr><td>1</td><td>GPB1</td><td>IRR18</td></tr>
<tr><td>0</td><td>GPB0</td><td>IRR17</td></tr>
</table>
<h2>Notes</h2>
<ul>
<li>Factory Reset is sampled only during startup.</li>
<li>Relay outputs are active LOW.</li>
<li>RTC and MCP23017 status can be checked from the System page.</li>
</ul>
</div>
</body>
</html>
)rawliteral";
