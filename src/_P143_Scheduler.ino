//#######################################################################################################
//#################################### Plugin 143: SCheduler ############################################
//#######################################################################################################
#define PLUGIN_143
#define PLUGIN_ID_143         143
#define PLUGIN_NAME_143       "Scheduler"
#define PLUGIN_VALUENAME1_143 "Scheduler"
#define PLUGIN_143_MAX_SETTINGS 8

boolean Plugin_143(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_143;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_SWITCH;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_143);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_143));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        for (byte x = 0; x < PLUGIN_143_MAX_SETTINGS; x++)
        {
          string += F("<TR><TD>Day,Time ");
          string += x+1;
          string += F(":<TD><input type='text' name='plugin_143_clock");
          string += x;
          string += F("' value='");
          string += timeLong2String(ExtraTaskSettings.TaskDevicePluginConfigLong[x]);
          string += F("'>");

          byte choice = ExtraTaskSettings.TaskDevicePluginConfig[x];
          String options[3];
          options[0] = F("");
          options[1] = F("Off");
          options[2] = F("On");
          int optionValues[3];
          optionValues[0] = 0;
          optionValues[1] = 1;
          optionValues[2] = 2;
          string += F("<select name='plugin_143_state");
          string += x;
          string += F("'>");
          for (byte x = 0; x < 3; x++)
          {
            string += F("<option value='");
            string += optionValues[x];
            string += "'";
            if (choice == optionValues[x])
              string += F(" selected");
            string += ">";
            string += options[x];
            string += F("</option>");
          }
          string += F("</select>");
        }
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        for (byte x = 0; x < PLUGIN_143_MAX_SETTINGS; x++)
        {
          String argc = F("plugin_143_clock");
          argc += x;
          String plugin1 = WebServer.arg(argc);
          ExtraTaskSettings.TaskDevicePluginConfigLong[x] = string2TimeLong(plugin1);

          argc = F("plugin_143_state");
          argc += x;
          String plugin2 = WebServer.arg(argc);
          ExtraTaskSettings.TaskDevicePluginConfig[x] = plugin2.toInt();
        }
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        success = true;
        break;
      }

    case PLUGIN_CLOCK_IN:
      {
        LoadTaskSettings(event->TaskIndex);
        for (byte x = 0; x < PLUGIN_143_MAX_SETTINGS; x++)
        {
          unsigned long clockEvent = (unsigned long)minute() % 10 | (unsigned long)(minute() / 10) << 4 | (unsigned long)(hour() % 10) << 8 | (unsigned long)(hour() / 10) << 12 | (unsigned long)weekday() << 16;
          unsigned long clockSet = ExtraTaskSettings.TaskDevicePluginConfigLong[x];

          if (matchClockEvent(clockEvent,clockSet))
          {
            byte state = ExtraTaskSettings.TaskDevicePluginConfig[x];
            if (state != 0)
            {
              state--;
              pinMode(Settings.TaskDevicePin1[event->TaskIndex], OUTPUT);
              digitalWrite(Settings.TaskDevicePin1[event->TaskIndex], state);
              UserVar[event->BaseVarIndex] = state;
              String log = F("TCLK : State ");
              log += state;
              addLog(LOG_LEVEL_INFO, log);
              sendData(event);
            }
          }
        }
        break;
      }
  }
  return success;
}
