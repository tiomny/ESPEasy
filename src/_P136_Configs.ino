//ifdef USES_P136
//#######################################################################################################
//################################## Plugin 136: Configs ################################################
//#######################################################################################################

#define PLUGIN_136
#define PLUGIN_ID_136         33
#define PLUGIN_NAME_136       "Generic - Configs (Experimental)"
#define PLUGIN_VALUENAME1_136 "Config"

#define P136_CFG_VALUE_COUNT  PCONFIG(0)

#define P136_ID_VALUE_COUNT   "p136_valuecount"

boolean Plugin_136(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_136;
        Device[deviceCount].Type = DEVICE_TYPE_DUMMY;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].DecimalsOnly = true;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_136);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_136));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        addFormNumericBox(String(F("Number of values")), String(F(P136_ID_VALUE_COUNT)), P136_CFG_VALUE_COUNT, 1, VARS_PER_TASK);
        
        addFormSubHeader(F("Variables"));
        html_end_table();
        html_table_class_normal();

        // table header
        TXBuffer += F("<TR><TH style='width:30px;' align='center'>#");
        html_table_header("Name");
        
        html_table_header(F("Value"), 30);

      // table body
      for (byte varNr = 0; varNr < P136_CFG_VALUE_COUNT; varNr++)
      {
        html_TR_TD();
        TXBuffer += varNr + 1;
        html_TD();
        String id = F("TDVN"); // ="taskdevicevaluename"
        id += (varNr + 1);
        addTextBox(id, ExtraTaskSettings.TaskDeviceValueNames[varNr], NAME_FORMULA_LENGTH_MAX);

        if (Device[DeviceIndex].FormulaOption || Device[DeviceIndex].DecimalsOnly)
        {
          html_TD();
          String id = F("TDVD"); // ="taskdevicevaluedecimals"
          id += (varNr + 1);
          addNumericBox(id, ExtraTaskSettings.TaskDeviceValueDecimals[varNr], 0, 6);
        }
      }


        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        pconfig_webformSave(event, 0);
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        // Do not set the sensor type, or else it will be set for all instances of the Dummy plugin.
        //sensorTypeHelper_setSensorType(event, 0);

        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        event->sensorType = PCONFIG(0);
        for (byte x = 0; x < getValueCountFromSensorType(PCONFIG(0)); x++)
        {
          String log = F("Dummy: value ");
          log += x+1;
          log += F(": ");
          log += UserVar[event->BaseVarIndex+x];
          addLog(LOG_LEVEL_INFO,log);
        }
        success = true;
        break;
      }

      case PLUGIN_SET_CONFIG:
      {
        String command = parseString(string, 1);
        if (command == F("dummysave"))
        {
          SaveCustomTaskSettings(event->TaskIndex, (byte *)&UserVar[event->BaseVarIndex], VARS_PER_TASK * sizeof(float));
          success = true;
        }
        else if (command == F("dummyload"))
        {
          LoadCustomTaskSettings(event->TaskIndex, (byte *)&UserVar[event->BaseVarIndex], VARS_PER_TASK*sizeof(float));
          success = true;
        }
        
        break;
      }
      
      case PLUGIN_WRITE:
      {
        String command = parseString(string, 1);
        if (command == F("dummyvalueset"))
        {
          if (event->Par1 == event->TaskIndex+1) // make sure that this instance is the target
          {
            float floatValue=0;
            if (string2float(parseString(string, 4),floatValue))
            {
              if (loglevelActiveFor(LOG_LEVEL_INFO))
              {
                String log = F("Dummy: Index ");
                log += event->Par1;
                log += F(" value ");
                log += event->Par2;
                log += F(" set to ");
                log += floatValue;
                addLog(LOG_LEVEL_INFO,log);
              }
              UserVar[event->BaseVarIndex+event->Par2-1]=floatValue;
              success = true;
            } else { // float conversion failed!
              if (loglevelActiveFor(LOG_LEVEL_ERROR))
              {
                String log = F("Dummy: Index ");
                log += event->Par1;
                log += F(" value ");
                log += event->Par2;
                log += F(" parameter3: ");
                log += parseString(string, 4);
                log += F(" not a float value!");
                addLog(LOG_LEVEL_ERROR,log);
              }
            }
          }
        }
        break;
      }
  }
  return success;
}
#endif // USES_P136
