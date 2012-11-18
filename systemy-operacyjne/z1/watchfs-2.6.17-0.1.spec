Summary: Monitoring
Summary(pl): Monitorowanie
Name: watchfs-2.6.17
Version: 0.1
Release: 1
Group: Applications/System
License: GPL
Source: %{name}-%{version}.tgz
BuildRoot: /tmp/buildroot-%{name}-%{version}

%description
Monitoring.

%description -l pl
Monitorowanie zajetosci systemow plikow i programow

%prep
%setup

%build
make

%install
install -d $RPM_BUILD_ROOT%{_bindir}
install monitor $RPM_BUILD_ROOT%{_bindir}
install watchfs $RPM_BUILD_ROOT%{_bindir}

install -d $RPM_BUILD_ROOT%{_mandir}/man1
install -d $RPM_BUILD_ROOT%{_mandir}/man8
install monitor.8 $RPM_BUILD_ROOT%{_mandir}/man8
install watchfs.1 $RPM_BUILD_ROOT%{_mandir}/man1
gzip -9fn $RPM_BUILD_ROOT%{_mandir}/man8/monitor.8
gzip -9fn $RPM_BUILD_ROOT%{_mandir}/man1/watchfs.1

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(644,root,root,755)
%attr(755,root,root) /usr/bin/watchfs
%attr(755,root,root) /usr/bin/monitor
%{_mandir}/man1/watchfs.1.gz
%{_mandir}/man8/monitor.8.gz
