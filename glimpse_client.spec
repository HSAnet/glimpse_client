Name:		glimpse_client
Summary:	The glimpse measurement platform
Version:	0.1
Release:	1
Source:		%{name}-%{version}.tar.gz
BuildRequires:	qt5-qtbase
BuildRequires:	qt5-qtquickcontrols
BuildRequires:	qt5-qtdeclarative-devel
URL:		http://www.measure-it.net
License:	GPLv3

%description
The glimpse measurement platform

%prep
%setup

%build
qmake-qt5 -r
make -j4

%install
%make_install
make install INSTALL_ROOT=%{buildroot}

%files
/*
