## проблема с SSL\_library\_init ##
[[Issue 34](https://code.google.com/p/bezzabot/issues/detail?id=34)]

### решение ###
[http://codeblog.vurdalakov.net/2009/11/solution-qsslsocket-cannot-call.html]

You need to install OpenSSL Win32 or Win64 binaries.

  1. Open Win32 OpenSSL Installation Project page.
  1. Download the latest "light" Win32 or Win64 installation package, for example "Win32 OpenSSL v0.9.8l Light".
  1. Install it to any location. Ignore "Microsoft Visual C++ 2008 Redistributables" warning (click OK) and select copying OpenSSL DLLs to "The OpenSSL binaries (\bin) directory".
  1. Copy libeay32.dll and ssleay32.dll from the \bin folder to the same place where your `QtNetwork4.dll` or `QtNetworkd4.dll` is located.


---


Линк на Win32 OpenSSL: [http://slproweb.com/products/Win32OpenSSL.html]


---


## проблема с неотображаемыми картинками ##

### решение ###

задействовать envvars:
```
QT_DEBUG_PLUGINS=1 QT_PLUGIN_PATH= ./bezzabot 2>&1 | grep "loadPlugin failed"
```

если есть упоминание про ошибку загрузки библиотеки libjpeg.so.62 - установить её
```
apt-get install libjpeg62
```

## проблема с «QSslSocket: cannot resolve SSLv2\_client\_method» ##
### решение ###
на самом деле особо ни на что не влияет, но можно успокоить QSslSocket, дав ему желаемую старую библиотеку:
```
 sudo aptitude install libssl0.9.8
```