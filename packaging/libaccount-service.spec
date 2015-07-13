
Name:       libaccount-service
Summary:    Account DB library
Version:    0.3.0
Release:    1
Group:      Social & Content/API
License:    Apache-2.0
Source0:    libaccount-service-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(vconf)
BuildRequires:	pkgconfig(glib-2.0) >= 2.26
BuildRequires:  pkgconfig(gio-unix-2.0)
BuildRequires:  pkgconfig(account-common)
BuildRequires:  pkgconfig(db-util)

%description
Account DB libraryXB-Public-Package: no

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
%description devel
Development files for %{name}

%prep
%setup -q

%post
/sbin/ldconfig
/usr/bin/sqlite3

%postun -p /sbin/ldconfig

%build
#export   CFLAGS+=" -Wextra -Wcast-align -Wcast-qual -Wshadow -Wwrite-strings -Wswitch-default"
#export CXXFLAGS+=" -Wextra -Wcast-align -Wcast-qual -Wshadow -Wwrite-strings -Wswitch-default -Wnon-virtual-dtor -Wno-c++0x-compat"
#export   CFLAGS+=" -Wno-unused-parameter -Wno-empty-body"
#export CXXFLAGS+=" -Wno-unused-parameter -Wno-empty-body"

#export   CFLAGS+=" -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-strict-aliasing -fno-unroll-loops -fsigned-char -fstrict-overflow -fno-common"
#export CXXFLAGS+=" -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-strict-aliasing -fno-unroll-loops -fsigned-char -fstrict-overflow"

export CFLAGS="${CFLAGS} -fPIC -fvisibility=hidden"
#cmake . -DCMAKE_INSTALL_PREFIX=/usr
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIBDIR=%{_libdir} \
	-DBINDIR=%{_bindir} \
	-DINCLUDEDIR=%{_includedir} \
%if "%{?tizen_profile_name}" == "mobile"
	-DFEATURE_PROFILE_MOBILE:BOOL=ON
%else
	-DFEATURE_PROFILE_MOBILE:BOOL=OFF
%endif

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants

rm -rf %{buildroot}%{_libdir}/accounts-svc

%files
%manifest libaccount-service.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%manifest libaccount-service.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/accounts-svc.pc
%{_includedir}/*.h
