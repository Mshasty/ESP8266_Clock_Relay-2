#include <Time.h>                 // https://github.com/PaulStoffregen/Time
//#include <Timezone.h>             // https://github.com/JChristensen/Timezone
#include <NtpClientLib.h>

bool wifiFirstConnected = false;
ipv4_addr my_ip;
String strIPaddr;
bool time_setup = false;

//Edit These Lines According To Your Timezone and Daylight Saving Time
//TimeZone Settings Details https://github.com/JChristensen/Timezone
//TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Time (Frankfurt, Paris)
//TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Time (Frankfurt, Paris)
//Timezone CE(CEST, CET);
//TimeChangeRule msk = {"MSK", Last, Sun, Mar, 1, 180};
//Timezone CE(msk);
//Pointer To The Time Change Rule, Use to Get The TZ Abbrev
//TimeChangeRule *tcr;
//time_t utc ; 

// Start NTP only after IP network is connected
void onSTAGotIP (WiFiEventStationModeGotIP ipInfo) {
  strIPaddr = ipInfo.ip.toString().c_str();
  Serial.printf("DHCP issued IP: %s\r\n", ipInfo.ip.toString().c_str());
  Serial.printf("WiFi connected: %s\r\n", WiFi.status() == WL_CONNECTED ? "yes" : "no");
  //digitalWrite (ONBOARDLED, LOW); // Turn on LED
  wifiFirstConnected = true;
  my_ip = ipInfo.ip;
}

void processSyncEvent(NTPSyncEvent_t ntpEvent)
{
    if (ntpEvent)
    {
        Serial.println("Ошибка NTP: ");
        if (ntpEvent == noResponse)
            Serial.println("Сервер не ответил");
        else if (ntpEvent == invalidAddress)
            Serial.println("Не верный адрес NTP сервера");
    }
    else
    {
        Serial.println("Получено время NTP: ");
        Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
}

void ntp(int interval, String NTPhost, int8_t timeZone)
{
    static bool ntp_setup;
    if (!wifiFirstConnected)
        return;
    if (!ntp_setup)
    {
        NTP.begin(NTPhost, timeZone, true, 0);
        NTP.setInterval(interval);
        ntp_setup = true;
        time_setup = true;
    }
    Serial.println(NTP.getTimeDateString() + " Включено: " + NTP.getUptimeString() + " Обновлено: " + NTP.getTimeDateString(NTP.getFirstSync()).c_str());
}

void update_handle(int interval) {
  static unsigned long timer;
  static unsigned int interval_ = interval;

  if(timer + interval_ > millis()) // Если интервал еще не истек
    return;
  timer = millis();
  ntp(25, ntp_srv, tz); // время NTP
}