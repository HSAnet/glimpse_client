glimpse_client
==============

### Build Status
* Linux: [![Linux](http://mplane.informatik.hs-augsburg.de:8111/app/rest/builds/buildType:(id:MPlane_ClientMasterDebug)/statusIcon)](http://mplane.informatik.hs-augsburg.de:8111/viewType.html?buildTypeId=MPlane_ClientMasterDebug)
* Mac: [![Mac](http://mplane.informatik.hs-augsburg.de:8111/app/rest/builds/buildType:(id:MPlane_ClientDebugMac)/statusIcon)](http://mplane.informatik.hs-augsburg.de:8111/viewType.html?buildTypeId=MPlane_ClientDebugMac)
* Windows: [![Windows](http://mplane.informatik.hs-augsburg.de:8111/app/rest/builds/buildType:(id:MPlane_ClientDebugWindows)/statusIcon)](http://mplane.informatik.hs-augsburg.de:8111/viewType.html?buildTypeId=MPlane_ClientDebugWindows) 
* Android armv7: [![Android armv7](http://mplane.informatik.hs-augsburg.de:8111/app/rest/builds/buildType:(id:MPlane_ClientMasterDebugBundledAndroidArmv7)/statusIcon)](http://mplane.informatik.hs-augsburg.de:8111/viewType.html?buildTypeId=MPlane_ClientMasterDebugBundledAndroidArmv7) 

### Pre-Build version
You don't need to compile GLIMPSE yourself to use it, pre-build packages can be found here.

#### Linux
Packages for various Linux distributions are available here: http://distributor.measure-it.net/packages/

See http://beta.measure-it.net for more information.

#### Windows
No installer at the moment.

#### Mac
No installer at the moment.

#### Android
After the release the Android version can be found here: https://play.google.com/store/apps/details?id=net.measureit.glimpse

Contact us at info@measure-it.net if you want Alpha access for the Android-version.

### Build requirements
* Mobile UI requires at least Qt 5.2.0
* Console version may also work with Qt 5.0.0
* **Linux**: libwnck (Arch & Gentoo), libwnck-dev (Debian & Ubuntu based), libwnck-devel (for RPM), pkg-config, make
* **Android**: Android SDK and NDK

### Running requirements
* **Linux**: openssl
* **Windows**: [OpenSSL](http://slproweb.com/products/Win32OpenSSL.html), [WinPcap](http://www.winpcap.org/install/default.htm)

### Building
1. Download and install the latest Qt version for your platform from http://download.qt-project.org/
2. Clone this repository or fork it first if you're going to contribute
```
git clone git@github.com:YOURUSERNAME/glimpse_client.git && 
cd glimpse_client &&
git submodule init && 
git submodule update
```
3. Run QtCreator, open client.pro, add your Qt configuration and select the "mobile" or "console" project near the "Play" button
