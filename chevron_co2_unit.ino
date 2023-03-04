//
//  FILE: chevron_co2_unit.ino
//  AUTHOR: Jamol Yunusov
//  COMPANY: (c) Bridge Analayzers, All Rights Reserved.
//

#include <SPI.h>
#include <Ethernet.h>
#include "cozir.h"
#include "EasyNextionLibrary.h"


COZIR czr(&Serial2);  // CO2 sensor serial
EasyNex myNex(Serial1); // Nextion serial


int co2_ppm = 0;
float co2_ppm_to_percent = 0;


// Enter a MAC address and IP address for your controller below.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// The IP address will depend on your local network: # 192,168,1,100
// Change IP address
IPAddress ip(192, 168, 1, 101);

// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


void setup()
{
  // Begin the object with a baud rate of 9600
  Serial.begin(9600);
  Serial2.begin(9600);
  myNex.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  

  // initialize cozir
  czr.init();
  
  // set to polling explicitly.
  czr.setOperatingMode(CZR_POLLING);
  delay(1000);
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
    if (client) {
      Serial.println("new client");
      // an HTTP request ends with a blank line
      bool currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the HTTP request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard HTTP response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
            client.println("<meta name='Jamol Yunusov' content='Chevron CO2 Unit'>");
            client.println("<link rel='icon' href='https://media-exp1.licdn.com/dms/image/C510BAQGlU7asaciEhA/company-logo_200_200/0/1519886323946?e=2159024400&v=beta&t=5aj9rGvbBwG-V2VZVqfHFVG5bWwTgxr4F4Vh4SWXaq4'>");
            client.println("<title>Bridge Analyzers</title>");
            client.println("<center>");
            client.println("</br>");
            client.println("<img src='https://www.bridgeanalyzers.com/wp-content/uploads/2018/08/bridge-analyzers-logo-1x.jpg'>");
            client.println("<h5 style='font-size:25px; font-weight:bold;' id='displayDateTime'></h5>");
            client.println("</head>");
            client.println("<body style='font-size:40px; color:white; font-weight:bold; background-color:Black; border:white; border-width:5px; border-style:solid;'>");
            client.println("<h4>CO2 %</h4>");
            client.println("<h4 class='co2_data'>");
            client.println(co2_ppm_to_percent);
            client.println("</h4>"); // output the value of co2
            client.println("</body>");
            client.println("<br>");
            client.println("<br>");
            client.println("<footer>");
            client.println("<p style='font-size:18px; font-weight:bold;'>Bridge Analyzers,<br>All Rights Reserved.</p><br>");
            client.println("</footer>");
            // Adding date and time
            client.println("<script type='text/javascript'>");
            client.println("var today = new Date();");
            client.println("var day = today.getDay();");
            client.println("var month = today.getMonth();");
            client.println("var hours = today.getHours();");
            client.println("var minutes = today.getMinutes();");
            client.println("var newformat = hours >= 12 ? 'PM' : 'AM';");
            client.println("hours = hours % 12;");
            client.println("hours = hours ? hours : 12; ");
            client.println("minutes = minutes < 10 ? '0' + minutes : minutes;");
            client.println("var monthlist = ['January', 'Febuary', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];");
            client.println("var daylist = ['Sunday','Monday','Tuesday','Wednesday','Thursday','Friday','Saturday'];");
            client.println("var date = today.getFullYear() + '-' +(today.getMonth()+1) + '-'+ today.getDate() + ', ';");
            client.println("var time = hours + ':' + minutes + ' ' + newformat;");
            client.println("var dateTime = date + ' ' + time;");
            client.println("document.getElementById('displayDateTime').innerHTML=daylist[day] + ', ' + monthlist[month] + '</br>' + dateTime;");
            client.println("</script>");
            client.println("</center>");
            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
    }

  // poll co2
  uint32_t co2_ppm = czr.CO2(); // raw
  co2_ppm *= czr.getPPMFactor();  // get ppm
  co2_ppm_to_percent = co2_ppm / 10000.0; // ppm to percent

  // send value to Nextion
  myNex.writeStr("t1.txt", String(co2_ppm_to_percent));

  // serial output
  // Serial.print("CO2 %: ");
  // Serial.println(co2_ppm_to_percent);
  // Serial.print("CO2 PPM: ");
  // Serial.println(co2_ppm);
  
  // update every 3 seconds
  delay(3000);
}

// -- END OF FILE --
