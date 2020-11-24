#include "WebServer_commandHelper.h"

#include "../../ESPEasy-Globals.h"
#include "../Commands/InternalCommands.h"
#include "../Globals/EventQueue.h"
#include "../Helpers/StringConverter.h"
#include "../Helpers/StringParser.h"
#include "../WebServer/AccessControl.h"


#include <ArduinoJson.h>

HandledWebCommand_result handle_command_from_web(EventValueSource::Enum source, String& webrequest)
{
  if (!clientIPallowed()) { return HandledWebCommand_result::IP_not_allowed; }
  webrequest.trim();
  if (webrequest.length() == 0) { return HandledWebCommand_result::NoCommand; }

  addLog(LOG_LEVEL_INFO,  String(F("HTTP: ")) + webrequest);
  webrequest = parseTemplate(webrequest);
#ifndef BUILD_NO_DEBUG
  addLog(LOG_LEVEL_DEBUG, String(F("HTTP after parseTemplate: ")) + webrequest);
#endif // ifndef BUILD_NO_DEBUG

  bool handledCmd = false;
  bool sendOK     = false;
  printWebString = "";
  printToWeb     = false;
  printToWebJSON = false;

  // in case of event, store to buffer and return...
  String command = parseString(webrequest, 1);

  if ((command == F("event")) || (command == F("asyncevent")))
  {
    eventQueue.add(parseStringToEnd(webrequest, 2));
    handledCmd = true;
    sendOK     = true;
  } else if (command.equalsIgnoreCase(F("taskrun")) ||
             command.equalsIgnoreCase(F("taskvalueset")) ||
             command.equalsIgnoreCase(F("taskvaluetoggle")) ||
             command.equalsIgnoreCase(F("let")) ||
             command.equalsIgnoreCase(F("logPortStatus")) ||
             command.equalsIgnoreCase(F("jsonportstatus")) ||
             command.equalsIgnoreCase(F("rules"))) {
    handledCmd = ExecuteCommand_internal(source, webrequest.c_str());
    sendOK     = true;

    // handledCmd = true;
  } else {
    printToWeb     = true;
    handledCmd     = ExecuteCommand_all_config(source, webrequest.c_str());
    sendOK         = false;
  }

  if (handledCmd) {
    if (sendOK) {
      if (printToWebJSON) {
        // Format "OK" to JSON format
        DynamicJsonDocument root(1024);
        root[F("return")]  = F("OK");
        root[F("command")] = webrequest;
        serializeJson(root, printWebString);
      } else {
        printWebString = F("OK");
      }
    }
    return HandledWebCommand_result::CommandHandled;
  }

  if (printToWebJSON) {
    // Format error to JSON format
    DynamicJsonDocument root(1024);
    root[F("return")]  = F("Unknown or restricted command");
    root[F("command")] = webrequest;
    serializeJson(root, printWebString);
  }
  return HandledWebCommand_result::Unknown_or_restricted_command;
}
