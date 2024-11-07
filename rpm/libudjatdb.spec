#
# spec file for package libudjatdb
#
# Copyright (c) <2024> Perry Werneck <perry.werneck@gmail.com>.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://github.com/PerryWerneck/libudjatdb/issues
#

%define module_name db

%define product_name %(pkg-config --variable=product_name libudjat)
%define product_version %(pkg-config --variable=product_version libudjat)
%define module_path %(pkg-config --variable=module_path libudjat)

Summary:		Database library for %{product_name}  
Name:			libudjat%{module_name}
Version: 2.1.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/libudjat%{module_name}

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	binutils
BuildRequires:	coreutils

%if "%{_vendor}" == "debbuild"
BuildRequires:  meson-deb-macros
BuildRequires:	libudjat-dev
BuildRequires:	sqlite3-dev
BuildRequires:	cppdb-dev
%else
BuildRequires:	gcc-c++ >= 5
BuildRequires:	pkgconfig(libudjat)
BuildRequires:	pkgconfig(sqlite3)
BuildRequires:	cppdb-devel
%endif

%if 0%{?suse_version} == 01500
BuildRequires:  meson = 0.61.4
%else
BuildRequires:  meson
%endif

%description
Database library for %{product_name}

C++ database classes for use with lib%{product_name}

#---[ Libraries ]-----------------------------------------------------------------------------------------------------

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%package -n lib%{product_name}cppdb%{_libvrs}
Summary: Database library for %{product_name}

%description -n lib%{product_name}cppdb%{_libvrs}
Database library for %{product_name}

C++ Database classes for use with lib%{product_name}

%package -n lib%{product_name}sqlite%{_libvrs}
Summary: SQLite library for %{product_name}

%description -n lib%{product_name}sqlite%{_libvrs}
SQLite library for %{product_name}

C++ SQLite classes for use with lib%{product_name}

#---[ Development ]---------------------------------------------------------------------------------------------------

%package devel
Summary: Development files for %{name}
Requires: %{product_name}cppdb%{_libvrs} = %{version}
Requires: %{product_name}sqlite%{_libvrs} = %{version}

%if "%{_vendor}" == "debbuild"
Provides:	%{name}-dev
Provides:	pkgconfig(%{product_name}cppdb)
Provides:	pkgconfig(%{product_name}cppdb-static)
Provides:	pkgconfig(%{product_name}sqlite)
Provides:	pkgconfig(%{product_name}sqlite-static)
%endif

%description devel
Database library for %{product_name}

C++ database classes for use with lib%{product_name}

#---[ Modules ]-------------------------------------------------------------------------------------------------------

%package -n %{product_name}-module-cppdb
Summary: Database module for %{name}

%description -n %{product_name}-module-cppdb
%{product_name} module with http client support.

%package -n %{product_name}-module-sqlite
Summary: Database module for %{name}

%description -n %{product_name}-module-sqlite
%{product_name} module with http client support.

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%autosetup
%meson

%build
%meson_build

%install
%meson_install

%files -n lib%{product_name}cppdb%{_libvrs}
%defattr(-,root,root)
%{_libdir}/*cppdb*.so.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files -n lib%{product_name}sqlite%{_libvrs}
%defattr(-,root,root)
%{_libdir}/*sqlite*.so.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files -n %{product_name}-module-cppdb
%{module_path}/*cppdb*.so

%files -n %{product_name}-module-sqlite
%{module_path}/*sqlite*.so

%files devel
%defattr(-,root,root)

%dir %{_includedir}/udjat/agent/sql
%{_includedir}/udjat/agent/sql/*.h

%dir %{_includedir}/udjat/alert/sql
%{_includedir}/udjat/alert/sql/*.h

%dir %{_includedir}/udjat/tools/sql
%{_includedir}/udjat/tools/sql/*.h

%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%post -n lib%{product_name}cppdb%{_libvrs} -p /sbin/ldconfig

%postun -n lib%{product_name}cppdb%{_libvrs} -p /sbin/ldconfig

%post -n lib%{product_name}sqlite%{_libvrs} -p /sbin/ldconfig

%postun -n lib%{product_name}sqlite%{_libvrs} -p /sbin/ldconfig

%changelog

