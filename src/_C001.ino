//#######################################################################################################
//########################### Controller Plugin 001: Domoticz HTTP ######################################
//#######################################################################################################

#define CPLUGIN_001
#define CPLUGIN_ID_001         1
#define CPLUGIN_NAME_001       "Domoticz HTTP"

boolean CPlugin_001(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_001;
        Protocol[protocolCount].usesMQTT = false;
        Protocol[protocolCount].usesAccount = true;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 8080;
        Protocol[protocolCount].usesID = true;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = PSTR(CPLUGIN_NAME_001);
        break;
      }

    case CPLUGIN_PROTOCOL_SEND:
      {
        if (event->idx != 0)
        {
          ControllerSettingsStruct ControllerSettings;
          LoadControllerSettings(event->ControllerIndex, (byte*)&ControllerSettings, sizeof (ControllerSettings));

          String authHeader = "";
          if ((SecuritySettings.ControllerUser[event->ProtocolIndex][0] != 0) && (SecuritySettings.ControllerPassword[event->ProtocolIndex][0] != 0))
          {
            base64 encoder;
            String auth = SecuritySettings.ControllerUser[event->ControllerIndex];
            auth += ":";
            auth += SecuritySettings.ControllerPassword[event->ControllerIndex];
            authHeader = PSTR("Authorization: Basic ");
            authHeader += encoder.encode(auth);
            authHeader += PSTR(" \r\n");
          }

          char log[80];
          boolean success = false;
          char host[20];
          sprintf_P(host, PSTR("%u.%u.%u.%u"), ControllerSettings.IP[0], ControllerSettings.IP[1], ControllerSettings.IP[2], ControllerSettings.IP[3]);

          sprintf_P(log, PSTR("%s%s using port %u"), "HTTP : connecting to ", host, ControllerSettings.Port);
          addLog(LOG_LEVEL_DEBUG, log);

          // Use WiFiClient class to create TCP connections
          WiFiClient client;
          if (!client.connect(host, ControllerSettings.Port))
          {
            connectionFailures++;
            strcpy_P(log, PSTR("HTTP : connection failed"));
            addLog(LOG_LEVEL_ERROR, log);
            return false;
          }
          statusLED(true);
          if (connectionFailures)
            connectionFailures--;

          // We now create a URI for the request
          String url = PSTR("/json.htm?type=command&param=udevice&idx=");
          url += event->idx;

          switch (event->sensorType)
          {
            case SENSOR_TYPE_SINGLE:                      // single value sensor, used for Dallas, BH1750, etc
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              break;
            case SENSOR_TYPE_LONG:                      // single LONG value, stored in two floats (rfid tags)
              url += PSTR("&svalue=");
              url += (unsigned long)UserVar[event->BaseVarIndex] + ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
              break;
            case SENSOR_TYPE_DUAL:                       // any sensor that uses two simple values
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += (";");
              url += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              break;
            case SENSOR_TYPE_TEMP_HUM:                      // temp + hum + hum_stat, used for DHT11
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              url += PSTR(";");
              url += humStat(UserVar[event->BaseVarIndex + 1]);
              break;
            case SENSOR_TYPE_TEMP_BARO:                      // temp + hum + hum_stat + bar + bar_fore, used for BMP085
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += PSTR(";0;0;");
              url += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              url += PSTR(";0");
              break;
            case SENSOR_TYPE_TRIPLE:
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 2], ExtraTaskSettings.TaskDeviceValueDecimals[2]);
              break;
            case SENSOR_TYPE_TEMP_HUM_BARO:                      // temp + hum + hum_stat + bar + bar_fore, used for BME280
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              url += PSTR(";");
              url += humStat(UserVar[event->BaseVarIndex + 1]);
              url += PSTR(";");
               url += toString(UserVar[event->BaseVarIndex + 2], ExtraTaskSettings.TaskDeviceValueDecimals[2]);
              url += PSTR(";0");
              break;
            case SENSOR_TYPE_QUAD:
              url += PSTR("&svalue=");
              url += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 2], ExtraTaskSettings.TaskDeviceValueDecimals[2]);
              url += PSTR(";");
              url += toString(UserVar[event->BaseVarIndex + 3], ExtraTaskSettings.TaskDeviceValueDecimals[3]);
              break;
            case SENSOR_TYPE_SWITCH:
              url = PSTR("/json.htm?type=command&param=switchlight&idx=");
              url += event->idx;
              url += PSTR("&switchcmd=");
              if (UserVar[event->BaseVarIndex] == 0)
                url += PSTR("Off");
              else
                url += PSTR("On");
              break;
            case SENSOR_TYPE_DIMMER:
              url = PSTR("/json.htm?type=command&param=switchlight&idx=");
              url += event->idx;
              url += PSTR("&switchcmd=");
              if (UserVar[event->BaseVarIndex] == 0)
                url += ("Off");
              else
              {
                url += PSTR("Set%20Level&level=");
                url += UserVar[event->BaseVarIndex];
              }
              break;
            case (SENSOR_TYPE_WIND):
              url += PSTR("&svalue=");                   // WindDir in degrees; WindDir as text; Wind speed average ; Wind speed gust; 0
              url += toString(UserVar[event->BaseVarIndex],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
              url += ";";
              url += getBearing(UserVar[event->BaseVarIndex]);
              url += ";";
              // Domoticz expects the wind speed in (m/s * 10)
              url += toString((UserVar[event->BaseVarIndex + 1] * 10),ExtraTaskSettings.TaskDeviceValueDecimals[1]);
              url += ";";
              url += toString((UserVar[event->BaseVarIndex + 2] * 10),ExtraTaskSettings.TaskDeviceValueDecimals[2]);
              url += ";0";
              break;
          }

          url.toCharArray(log, 80);
          addLog(LOG_LEVEL_DEBUG_MORE, log);

          // This will send the request to the server
          String request = PSTR("GET ");
          request += url;
          request += PSTR(" HTTP/1.1\r\n");
          request += PSTR("Host: ");
          request += host;
          request += PSTR("\r\n");
          request += authHeader;
          request += PSTR("Connection: close\r\n\r\n");
          client.print(request);

          unsigned long timer = millis() + 200;
          while (!client.available() && millis() < timer)
            delay(1);

          // Read all the lines of the reply from server and print them to Serial
          while (client.available()) {
            String line = client.readStringUntil('\n');
            line.toCharArray(log, 80);
            addLog(LOG_LEVEL_DEBUG_MORE, log);
            if (line.substring(0, 15) == PSTR("HTTP/1.1 200 OK"))
            {
              strcpy_P(log, PSTR("HTTP : Success"));
              addLog(LOG_LEVEL_DEBUG, log);
              success = true;
            }
            delay(1);
          }
          strcpy_P(log, PSTR("HTTP : closing connection"));
          addLog(LOG_LEVEL_DEBUG, log);

          client.flush();
          client.stop();
        } // if ixd !=0
        else
        {
          String log = PSTR("HTTP : IDX cannot be zero!");
          addLog(LOG_LEVEL_ERROR, log);
        }
        break;
      }
  }
  return success;
}

/*
  boolean Domoticz_getData(int idx, float *data, byte index) // todo
  {
  boolean success = false;
  char host[20];
  sprintf_P(host, PSTR("%u.%u.%u.%u"), Settings.Controller_IP[index][0], Settings.Controller_IP[index][1], Settings.Controller_IP[index][2], Settings.Controller_IP[index][3]);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, Settings.ControllerPort[index]))
  {
    connectionFailures++;
    return false;
  }
  if (connectionFailures)
    connectionFailures--;

  // We now create a URI for the request
  String url = PSTR("/json.htm?type=devices&rid=");
  url += idx;

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timer = millis() + 200;
  while (!client.available() && millis() < timer)
    delay(1);

  // Read all the lines of the reply from server and print them to Serial

  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line.substring(10, 14) == "Data")
    {
      String strValue = line.substring(19);
      byte pos = strValue.indexOPSTR(' ');
      strValue = strValue.substring(0, pos);
      strValue.trim();
      float value = strValue.toFloat();
       data = value;
      success = true;
    }
  }
  return success;
  }

*/

int humStat(int hum){
  int lHumStat;
  if(hum<30){
     lHumStat = 2;
  }else if(hum<40){
    lHumStat = 0;
  }else if(hum<59){
    lHumStat = 1;
  }else{
    lHumStat = 3;

  }
  return lHumStat;
}
