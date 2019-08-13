#ifndef PLUGIN_HELPER_H
#define PLUGIN_HELPER_H

// Defines to make plugins more readable.

#ifndef PCONFIG
  # define PCONFIG(n) (Settings.TaskDevicePluginConfig[event->TaskIndex][(n)])
#endif // ifndef PCONFIG
#ifndef PCONFIG_FLOAT
  # define PCONFIG_FLOAT(n) (Settings.TaskDevicePluginConfigFloat[event->TaskIndex][(n)])
#endif // ifndef PCONFIG_FLOAT
#ifndef PCONFIG_LONG
  # define PCONFIG_LONG(n) (Settings.TaskDevicePluginConfigLong[event->TaskIndex][(n)])
#endif // ifndef PCONFIG_LONG
#ifndef PIN

// Please note the 'offset' of N compared to normal pin numbering.
  # define PIN(n) (Settings.TaskDevicePin[n][event->TaskIndex])
#endif // ifndef PIN
#ifndef CONFIG_PIN1
  # define CONFIG_PIN1 (Settings.TaskDevicePin1[event->TaskIndex])
#endif // ifndef CONFIG_PIN1
#ifndef CONFIG_PIN2
  # define CONFIG_PIN2 (Settings.TaskDevicePin2[event->TaskIndex])
#endif // ifndef CONFIG_PIN2
#ifndef CONFIG_PIN3
  # define CONFIG_PIN3 (Settings.TaskDevicePin3[event->TaskIndex])
#endif // ifndef CONFIG_PIN3
#ifndef CONFIG_PORT
  # define CONFIG_PORT (Settings.TaskDevicePort[event->TaskIndex])
#endif // ifndef CONFIG_PORT

String PCONFIG_LABEL(int n) {
  if (n < PLUGIN_CONFIGVAR_MAX) {
    String result = "pconf_";
    result += n;
    return result;
  }
  return "error";
}

// ==============================================
// Data used by instances of plugins.
// =============================================

// base class to be able to delete a data object from the array.
// N.B. in order to use this, a data object must inherit from this base class.
//      This is a compile time check.
struct PluginTaskData_base {
  virtual ~PluginTaskData_base() {}

  // We cannot use dynamic_cast, so we must keep track of the plugin ID to
  // perform checks on the casting.
  // This is also a check to only use these functions and not to insert pointers
  // at random in the Plugin_task_data array.
  int _taskdata_plugin_id = -1;
};

PluginTaskData_base *Plugin_task_data[TASKS_MAX] = { NULL, };

void resetPluginTaskData() {
  for (byte i = 0; i < TASKS_MAX; ++i) {
    Plugin_task_data[i] = nullptr;
  }
}

void clearPluginTaskData(byte taskIndex) {
  if (taskIndex < TASKS_MAX) {
    if (Plugin_task_data[taskIndex] != nullptr) {
      delete Plugin_task_data[taskIndex];
      Plugin_task_data[taskIndex] = nullptr;
    }
  }
}

void initPluginTaskData(byte taskIndex, PluginTaskData_base *data) {
  clearPluginTaskData(taskIndex);

  if ((taskIndex < TASKS_MAX) && Settings.TaskDeviceEnabled[taskIndex]) {
    Plugin_task_data[taskIndex]                      = data;
    Plugin_task_data[taskIndex]->_taskdata_plugin_id = Task_id_to_Plugin_id[taskIndex];
  }
}

PluginTaskData_base* getPluginTaskData(byte taskIndex) {
  if (taskIndex >= TASKS_MAX) {
    return nullptr;
  }

  if ((Plugin_task_data[taskIndex] != nullptr) && (Plugin_task_data[taskIndex]->_taskdata_plugin_id == Task_id_to_Plugin_id[taskIndex])) {
    return Plugin_task_data[taskIndex];
  }
  return nullptr;
}

bool pluginTaskData_initialized(byte taskIndex) {
  // FIXME TD-er: Must check for type also.
  if (taskIndex < TASKS_MAX) {
    return Plugin_task_data[taskIndex] != nullptr;
  }
  return false;
}

String getPluginCustomArgName(int varNr) {
  String argName = F("plugin_custom_arg");
  argName += varNr + 1;
  return argName;
}

// Helper function to create formatted custom values for display in the devices overview page.
// When called from PLUGIN_WEBFORM_SHOW_VALUES, the last item should add a traling div_br class
// if the regular values should also be displayed.
// The call to PLUGIN_WEBFORM_SHOW_VALUES should only return success = true when no regular values should be displayed
// Note that the varNr of the custom values should not conflict with the existing variable numbers (e.g. start at VARS_PER_TASK)
String pluginWebformShowValue(byte taskIndex, byte varNr, const String& label, const String& value, bool addTrailingBreak) {
  String result;
  size_t length = 96 + label.length() + value.length();
  String breakStr = F("<div class='div_br'></div>");
  if (addTrailingBreak) {
    length += breakStr.length();
  }
  result.reserve(length);
  if (varNr > 0) {
    result += breakStr;
  }
  result += F("<div class='div_l' id='valuename_");
  result += String(taskIndex);
  result += '_';
  result += String(varNr);
  result += "'>";
  result += label;
  result += F(":</div><div class='div_r' id='value_");
  result += String(taskIndex);
  result += '_';
  result += String(varNr);
  result += "'>";
  result += value;
  result += "</div>";
  if (addTrailingBreak) {
    result += breakStr;
  }
  return result;
}

String pluginWebformShowValue(byte taskIndex, byte varNr, const String& label, const String& value) {
  return pluginWebformShowValue(taskIndex, varNr, label, value, false);
}

#endif // PLUGIN_HELPER_H
