//#######################################################################################################
//########################### Notification Plugin 001: Email ############################################
//#######################################################################################################

#define NPLUGIN_001
#define NPLUGIN_ID_001         1
#define NPLUGIN_NAME_001       "Email (SMTP)"

#define NPLUGIN_001_TIMEOUT 3000

boolean NPlugin_001(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case NPLUGIN_PROTOCOL_ADD:
      {
        Notification[++notificationCount].Number = NPLUGIN_ID_001;
        Notification[notificationCount].usesMessaging = true;
        Notification[notificationCount].usesGPIO=0;
        break;
      }

    case NPLUGIN_GET_DEVICENAME:
      {
        string = F(NPLUGIN_NAME_001);
        break;
      }

    case NPLUGIN_WRITE:
      {
        String log = "";
        String command = parseString(string, 1);

        if (command == F("email"))
        {
          NotificationSettingsStruct NotificationSettings;
          LoadNotificationSettings(event->NotificationIndex, (byte*)&NotificationSettings, sizeof(NotificationSettings));
          NPlugin_001_send(NotificationSettings.Domain, NotificationSettings.Receiver, NotificationSettings.Sender, NotificationSettings.Subject, NotificationSettings.Body, NotificationSettings.Server, NotificationSettings.Port);
          success = true;
        }
        break;
      }

    case NPLUGIN_NOTIFY:
      {
        NotificationSettingsStruct NotificationSettings;
        LoadNotificationSettings(event->NotificationIndex, (byte*)&NotificationSettings, sizeof(NotificationSettings));
        String subject = NotificationSettings.Subject;
        String body = "";
        if (string.length() >0)
          body = string;
        else
          body = NotificationSettings.Body;
        subject = parseTemplate(subject, subject.length());
        body = parseTemplate(body, body.length());
        NPlugin_001_send(NotificationSettings.Domain, NotificationSettings.Receiver, NotificationSettings.Sender, subject, body, NotificationSettings.Server, NotificationSettings.Port);
        success = true;
      }

  }
  return success;
}

boolean NPlugin_001_send(String aDomain , String aTo, String aFrom, String aSub, String aMesg, String aHost, int aPort)
{
  boolean myStatus = false;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(aHost.c_str(), aPort)) {
    myStatus = false;
  }
  else {

    // Wait for Client to Start Sending
    // The MTA Exchange
    while (true) {

      if (NPlugin_001_MTA(client, "",                                                         PSTR("220 ")) == false) break;
      if (NPlugin_001_MTA(client, PSTR("EHLO ") + aDomain,                                    PSTR("250 ")) == false) break;
      if (NPlugin_001_MTA(client, PSTR("MAIL FROM:") + aFrom + "",                            PSTR("250 ")) == false) break;
      if (NPlugin_001_MTA(client, PSTR("RCPT TO:") + aTo + "",                                PSTR("250 ")) == false) break;
      if (NPlugin_001_MTA(client, PSTR("DATA"),                                               PSTR("354 ")) == false) break;
      if (NPlugin_001_MTA(client, PSTR("Subject:") + aSub + PSTR("\r\n\r\n") + aMesg + PSTR("\r\n.\r\n"), PSTR("250 ")) == false) break;

      myStatus = true;
      break;

    }

    client.flush();
    client.stop();

    if (myStatus == true) {
      addLog(LOG_LEVEL_INFO, F("EMAIL: Connection Closed Successfully"));
    }
    else {
      addLog(LOG_LEVEL_ERROR, F("EMAIL: Connection Closed With Error"));
    }

  }

  return myStatus;

}


boolean NPlugin_001_MTA(WiFiClient client, String aStr, String aWaitForPattern)
{

  boolean myStatus = false;

  addLog(LOG_LEVEL_DEBUG, aStr);

  if (aStr.length() ) client.println(aStr);

  yield();

  // Wait For Response
  unsigned long ts = millis();
  while (true) {
    if ( ts + NPLUGIN_001_TIMEOUT < millis() ) {
      myStatus = false;
      break;
    }

    yield();

    // String line = client.readStringUntil('\n');
    String line;
    safeReadStringUntil(client, line, '\n');

    addLog(LOG_LEVEL_DEBUG, line);

    if (line.indexOf(aWaitForPattern) >= 0) {
      myStatus = true;
      break;
    }
  }

  return myStatus;
}
