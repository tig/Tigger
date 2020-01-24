# Project Description
WeatherBug Command Line Client utility for accessing WeatherBug.com's free SOAP web services. It makes it easy to access single data points from any of WB's 1000s of weather stations. It was written in C++ using the Windows Web Services (WSS) framework. 

##Example usage:
    C:\WeatherBug\Release>WeatherBug.exe /?
    WeatherBug Command Line Client v1.0.2

    Copyright (c) 2012 Bogus Software
    Source code available at http://weatherbug.codeplex.com.
    Licensed under the MS-Pl license.

    Usage:
    WeatherBug [Element] [-d] [-a:<ACode>]
               [[-s:<StationID>] | [-l:<lat>,<long>]] [-?]
      Element - The name of the weather element to retrieve.
                Use -d see available elements.
                The default element is Temperature.
      -a - Override the compiled in WeatherBug developer license key (ACode).
      -s - The WeatherBug station identifier to get the weather from.
           Default StationID is BLLVV (Phantom Lake Middleschool).
      -l - Get the data from the weather station nearest to the location specified.
      -d - Dump all data.

    C:\WeatherBug\Release>WeatherBug.exe Temperature -l:20.879183,-156.68375
    76.7

    C:\WeatherBug\Release>WeatherBug.exe -d -l:20.879183,-156.68375
    WeatherBug - Copyright (c) 2009 Charles E. Kindel, Jr. All Rights Reserved.
    Weather results from Princess Nahienaena ES in Lahaina as of 9:41:01 AM on 
    Friday, December 18, 2009.
                              City: Lahaina
                           Country: USA
                          CurrDesc: Mostly Cloudy
                          DewPoint: 62
                         FeelsLike: 77
                     GustWindSpeed: 31
           GustWindDirectionString: NW
                          Humidity: 60
                      HumidityHigh: 73
                       HumidityLow: 41
                      HumidityRate: 2
                          Pressure: 30.04
                      PressureHigh: 30.10
                       PressureLow: 29.99
                      PressureRate: 0.02
                            ObDate: 12/18/2009
                         RainMonth: 1.51
                          RainRate: 0.00
                       RainRateMax: 0.00
                         RainToday: 0.00
                          RainYear: 17.76
                             State:  HI
                StationIDRequested:
                 StationIDReturned: LHINA
                       StationName: Princess Nahienaena ES
                       Temperature: 76.7
                   TemperatureHigh: 87
                    TemperatureLow: 71
                   TemperatureRate: -1.2
                     WindDirection: NNW
                         WindSpeed: 3
                      WindSpeedAvg: 2

    C:\WeatherBug\Release>