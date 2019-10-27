#ifndef COMMAND_MQTT_H
#define COMMAND_MQTT_H


String Command_MQTT_Retain(struct EventStruct *event, const char *Line)
{
  return Command_GetORSetBool(event, F("MQTT Retain:"),
                              Line,
                              (bool *)&Settings.MQTTRetainFlag,
                              1);
}

String Command_MQTT_UseUnitNameAsClientId(struct EventStruct *event, const char *Line)
{
  return Command_GetORSetBool(event, F("MQTT Use Unit Name as ClientId:"),
                              Line,
                              (bool *)&Settings.MQTTUseUnitNameAsClientId,
                              1);
}

String Command_MQTT_messageDelay(struct EventStruct *event, const char *Line)
{
  if (HasArgv(Line, 2)) {
    Settings.MessageDelay = event->Par1;
  }
  else {
    String result = F("MQTT message delay:");
    result += Settings.MessageDelay;
    serialPrintln();
    serialPrintln(result);
    return result;
  }
  return return_command_success();
}

String Command_MQTT_Publish(struct EventStruct *event, const char *Line)
{
  if (WiFiConnected()) {
    // ToDo TD-er: Not sure about this function, but at least it sends to an existing MQTTclient
    int enabledMqttController = firstEnabledMQTTController();

    if (enabledMqttController >= 0) {
      // Command structure:  Publish,<topic>,<value>
      String topic = parseStringKeepCase(Line, 1);
      String value = parseStringKeepCase(Line, 2);

      if ((topic.length() > 0) && (value.length() > 0)) {
        // @giig1967g: if payload starts with '=' then treat it as a Formula and evaluate accordingly
        // The evaluated value is already present in event->Par2
        // FIXME TD-er: Is the evaluated value always present in event->Par2 ?
        // Should it already be evaluated, or should we evaluate it now?
        if (value.c_str()[0] != '=') {
          MQTTpublish(enabledMqttController, topic.c_str(), value.c_str(), Settings.MQTTRetainFlag);
        }
        else {
          MQTTpublish(enabledMqttController, topic.c_str(), String(event->Par2).c_str(), Settings.MQTTRetainFlag);
        }
      }
      return return_command_success();
    }
    return F("No MQTT controller enabled");
  }
  return return_not_connected();
}

#endif // COMMAND_MQTT_H
