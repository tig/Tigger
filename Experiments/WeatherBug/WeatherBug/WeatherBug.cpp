// WeatherBug.cpp : Defines the entry point for the console application.
// Copyright (c) 2012 Bogus Labs
// By Charlie Kindel
//
// Source code available at https://github.com/tig/WeatherBug.
//
// Licensed under the MS-Pl license.
//

#include "stdafx.h"

// Version History
//
// 1.0.0 - First version
// 1.0.1 - Added version #
// 1.0.2 - Moved to github
//
#define _VERSION_STRING L"1.0.2"

// Global web service variables
// Since this is a command line app we don't worry about multi-use
//
WsHeap heap;
WsError error;
WsServiceProxy serviceProxy;
LiveWeatherData *data = NULL;

// Define _ACODE to be the Unicode string for your 
// Weatherbug developer ACode. Sign up to get an ACode at
// http://weather.weatherbug.com/desktop-weather/api.html
//
#define _ACODE L"A4535921357" // L"A5473807782" // TODO: Put your ACode here
#define _DEFAULTELEMENT L"Temperature"
#define _DEFAULTSTATIONID L"BLLVV"  // In Bellevue, WA

void PrintHelpAndExit()
{
    wprintf(L"WeatherBug Command Line Client v%s\n", _VERSION_STRING);
    wprintf(L"Copyright (c) 2012 Bogus Labs. By Charlie Kindel\n");
    wprintf(L"Source code available at https://github.com/tig/WeatherBug. \nLicensed under the MS-Pl license.\n\n");
    wprintf(L"Usage:\n");
    wprintf(L"WeatherBug [Element] [-d] [-a:<ACode>]\n");
    wprintf(L"           [[-s:<StationID>] | [-l:<lat>,<long>]] [-?]\n");
    wprintf(L"  Element - The name of the weather element to retrieve.\n");
    wprintf(L"            Use -d see available elements.\n");
    wprintf(L"            The default element is %s.\n", _DEFAULTELEMENT);
    wprintf(L"  -a - Override the compiled in WeatherBug developer license key (ACode).\n");
    wprintf(L"       The default ACode is %s.\n", _ACODE);
    wprintf(L"  -s - The WeatherBug station identifier to get the weather from.\n");
    wprintf(L"       Default StationID is BLLVV (Phantom Lake Middleschool).\n");
    wprintf(L"  -l - Get the data from the weather station nearest to the location specified.\n");
    wprintf(L"  -d - Dump all data.\n");
    exit (0);
}

// This map enables lookup of the data returned by the web service using
// a string from the command line.
//
typedef struct Elements 
{
    WCHAR* 	Element;
    UINT 	Offset;
} ELEMENTS;

// Note that not all elements provided by the GetLiveWeatherData SOAP call are
// represented here. Only those represented as strings are included.
// TODO: Expand this to include other data types. Would require a more complex system.
//
ELEMENTS map[] = 
{
    {L"City",                           offsetof(LiveWeatherData, City) },
    {L"Country",                        offsetof(LiveWeatherData, Country) },
    {L"CurrDesc",                       offsetof(LiveWeatherData,CurrDesc ) },
    {L"DewPoint",                       offsetof(LiveWeatherData, DewPoint) },
    {L"FeelsLike",                      offsetof(LiveWeatherData, FeelsLike) },
    {L"GustWindSpeed",                  offsetof(LiveWeatherData, GustWindSpeed) },
    {L"GustWindDirectionString",        offsetof(LiveWeatherData, GustWindDirectionString) },
    {L"Humidity",                       offsetof(LiveWeatherData, Humidity) },
    {L"HumidityHigh",                   offsetof(LiveWeatherData, HumidityHigh) },
    {L"HumidityLow",                    offsetof(LiveWeatherData, HumidityLow) },
    {L"HumidityRate",                   offsetof(LiveWeatherData, HumidityRate) },
    {L"Pressure",                       offsetof(LiveWeatherData,Pressure ) },
    {L"PressureHigh",                   offsetof(LiveWeatherData, PressureHigh) },
    {L"PressureLow",                    offsetof(LiveWeatherData, PressureLow) },
    {L"PressureRate",                   offsetof(LiveWeatherData, PressureRate) },
    {L"ObDate",                         offsetof(LiveWeatherData,ObDate ) },
    {L"RainMonth",                      offsetof(LiveWeatherData, RainMonth) },
    {L"RainRate",                       offsetof(LiveWeatherData, RainRate) },
    {L"RainRateMax",                    offsetof(LiveWeatherData, RainRateMax) },
    {L"RainToday",                      offsetof(LiveWeatherData, RainToday) },
    {L"RainYear",                       offsetof(LiveWeatherData,RainYear ) },
    {L"State",                          offsetof(LiveWeatherData,State ) },
    {L"StationIDRequested",             offsetof(LiveWeatherData,StationIDRequested ) },
    {L"StationIDReturned",              offsetof(LiveWeatherData,StationIDReturned ) },
    {L"StationName",                    offsetof(LiveWeatherData, StationName) },
    {L"Temperature",                    offsetof(LiveWeatherData, Temperature) },
    {L"TemperatureHigh",                offsetof(LiveWeatherData,TemperatureHigh ) },
    {L"TemperatureLow",                 offsetof(LiveWeatherData, TemperatureLow) },
    {L"TemperatureRate",                offsetof(LiveWeatherData, TemperatureRate) },
    {L"WindDirection",                  offsetof(LiveWeatherData, WindDirection) },
    {L"WindSpeed",                      offsetof(LiveWeatherData, WindSpeed) },
    {L"WindSpeedAvg",                   offsetof(LiveWeatherData, WindSpeedAvg) }
};
const size_t mapsize = sizeof map / sizeof map[0];

UINT Find(WCHAR* Key)
{
    for (int i = 0 ; i < mapsize ; i++)
    {
        if (_wcsicmp(Key, map[i].Element) == 0)
            return map[i].Offset;
    }
    return (UINT)-1;
}

// With WeatherBug, I've never actually seen this work. IOW, I've never
// actually tested error conditions.
// TODO: Test this
//
void PrintErrorAndExit(WCHAR* msg, WsError* perror)
{
    ULONG stringCount = 0;
    perror->GetProperty(WS_ERROR_PROPERTY_STRING_COUNT, &stringCount);
    for (ULONG i = 0; i < stringCount; ++i)
    {
        WS_STRING string;
        perror->GetString(i, &string);
        wprintf(L"ERROR: %s %d: %.*s\n", msg, i, string.length, string.chars);
    }
    exit(-1);
}

HRESULT SetupServiceProxy()
{
    HRESULT hr = S_OK;
    hr = error.Create(0, 0); 
    if (FAILED(hr)) PrintErrorAndExit(L"error.Create failed: ", &error);

    hr = heap.Create(8096, 0, 0, 0, error);
    if (FAILED(hr)) PrintErrorAndExit(L"heap.Create failed: ", &error);

    hr = WeatherBugWebServicesSoap_CreateServiceProxy(0, 0, 0, &serviceProxy, error);
    if (FAILED(hr)) PrintErrorAndExit(L"CreateServiceproxy failed: ", &error);

    WS_STRING url = WS_STRING_VALUE(L"http://api.wxbug.net/weatherservice.asmx");
    WS_ENDPOINT_ADDRESS address = {};
    address.url = url;

    hr = serviceProxy.Open(&address, 0, error);
    if (FAILED(hr)) PrintErrorAndExit(L"Open failed: ", &error);

    return hr;
}

HRESULT CleanupServiceProxy()
{
    HRESULT hr = serviceProxy.Close(0, // async context
        error);
    if (FAILED(hr)) PrintErrorAndExit(L"Close failed: ", &error);
    return hr;
}

LiveWeatherData* GetDataByLatLong( WCHAR* ACode, double latitude, double longitude)
{
    LocationUSWorldCityData *citydata = NULL;
    HRESULT hr = WeatherBugWebServicesSoap_GetUSWorldCityByLatLong(serviceProxy,
        latitude,
        longitude,
        ACode,
        &citydata,
        heap,
        0, // properties
        0, // property count
        0, // async context
        error);
    if (!SUCCEEDED(hr)) PrintErrorAndExit(L"GetUSWorldCityByLatLong failed: ", &error);

    if (citydata != NULL)
    {
        LiveWeatherData *liveweatherdata = NULL;
        hr = WeatherBugWebServicesSoap_GetLiveWeatherByCityCode(serviceProxy,
            citydata->CityCode,
            UnitTypeEnglish,
            ACode,
            &liveweatherdata,
            heap,
            0, // properties
            0, // property count
            0, // async context
            error);
        if (!SUCCEEDED(hr)) PrintErrorAndExit(L"GetLiveWeatherByCityCode failed: ", &error);
        return liveweatherdata;
    }

    return NULL;

}

LiveWeatherData* GetDataByStationID( WCHAR* ACode, WCHAR* StationID)
{
    LiveWeatherData *liveweatherdata = NULL;
    HRESULT hr = WeatherBugWebServicesSoap_GetLiveWeatherByStationID(serviceProxy,
        StationID,
        UnitTypeEnglish,
        ACode,
        &liveweatherdata,
        heap,
        0, // properties
        0, // property count
        0, // async context
        error);
    if (!SUCCEEDED(hr)) PrintErrorAndExit(L"GetLiveWeatherByStationID failed: ", &error);

    return liveweatherdata;
}

WCHAR* GetElement(WCHAR* Element)
{
    UINT offset = Find(Element);
    if (data!= NULL && offset != (UINT)-1)
        return *(WCHAR**)((byte*)data + offset);
    return NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
    WCHAR* ACode = _ACODE;
    WCHAR* StationID = _DEFAULTSTATIONID;
    WCHAR* Element = _DEFAULTELEMENT;
    double Lat = 0;
    double Lon = 0;
    bool Dump = false;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-' || argv[i][0] == '/')
            {
                switch(tolower(argv[i][1]))
                {
                case 'a':
                    ACode = &argv[i][3];
                    break;

                case 's':
                    StationID = &argv[i][3];
                    break;

                case 'd':
                    Dump = true;
                    break;

                case 'l':
                    WCHAR* stopstring;
                    WCHAR* stopstring2;
                    Lat = wcstod(&argv[i][3], &stopstring);
                    Lon = wcstod(stopstring+1, &stopstring2);
                    break;

                case '?':
                    PrintHelpAndExit();
                    break;
                }
            }
            else
            {
                // Element
                Element = argv[i];
            }
        }
    }

    if (ACode != NULL && *ACode == '\0')
    {
        wprintf(L"No ACode specified.\n\n");
        PrintHelpAndExit();
    }

    HRESULT hr = SetupServiceProxy();
    if (FAILED(hr)) 
    {
        wprintf(L"ERROR: Failed to setup the web service proxy.\n");
        return -1;
    }

    if (Lat != 0 && Lon != 0)
        data = GetDataByLatLong(ACode, Lat, Lon);
    else
        data = GetDataByStationID(ACode, StationID);

    if (data == NULL)
    {
        wprintf(L"ERROR: Failed to get data from StationID: %s\n", StationID);
        return -1;
    }

    if (Dump)
    {
        wprintf(L"WeatherBug - Copyright (c) 2012 Bogus Labs. All Rights Reserved.\n");
        FILETIME ft;
        SYSTEMTIME st;
        WCHAR szLocalDate[255], szLocalTime[255];

        WsDateTimeToFileTime(&data->ObDateTime, &ft, NULL);

        FileTimeToLocalFileTime( &ft, &ft );
        FileTimeToSystemTime( &ft, &st );
        GetDateFormat( LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szLocalDate, 255 );
        GetTimeFormat( LOCALE_USER_DEFAULT, 0, &st, NULL, szLocalTime, 255 );

        wprintf(L"Weather results from %s in %s as of %s on %s.\n",  data->StationName, data->City, szLocalTime, szLocalDate);
        for (int i = 0 ; i < mapsize ; i++)
        {
            wprintf(L"%*s: %s\n", 30, map[i].Element, GetElement(map[i].Element));
        }
    }
    else
    {
        wprintf(L"%s\n", GetElement(Element));
    }

    CleanupServiceProxy();

    return 0;
}

