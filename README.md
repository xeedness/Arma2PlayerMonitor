**Arma2PlayerMonitor** is an c++ application for windows. It monitors the players on an arma2 server. Whenever a player joins or leaves the windows voice output is used to announce that change.

## Authors

- **xeedness** - Creator

## What can it do?
- Show players in the console.
- Output player changes to windows voice
- Output number of players to windows voice
- Variable Time Scheduling - Check and announce

## How do I use it?
First you'll need the [binary](https://github.com/xeedness/Arma2PlayerMonitor/releases/download/v1.0/Arma2PlayerMonitor.exe).
The binary expects 3 parameters. A config file, the server ip and the port.

Example:
```
Arma2PlayerMonitor.exe Arma2PlayerMonitor.cfg 127.0.0.1 33357
```

I recommend creating a .bat file with the preceding content. Here a working [example](https://github.com/xeedness/Arma2PlayerMonitor/releases/download/v1.0/ProjectXServer.bat).

