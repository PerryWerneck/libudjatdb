#
# spec file for package mingw64-libudjatsqlite
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
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

%define module_name sqlite
%define product_name udjat

Summary:		SQLite client library for %{product_name}
Name:			mingw64-libudjat%{module_name}
Version:		2.1.0
Release:		0
License:		LGPL-3.0
Source:			libudjatdb-%{version}.tar.xz
BuildArch:		noarch

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

URL:			https://github.com/PerryWerneck/libudjatdb

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	coreutils

BuildRequires:	mingw64-filesystem
BuildRequires:	mingw64-cross-binutils
BuildRequires:	mingw64-cross-gcc-c++ >= 9.0
BuildRequires:	mingw64-cross-pkg-config
BuildRequires:	mingw64-filesystem
BuildRequires:	mingw64-cross-meson

BuildRequires:	mingw64-libudjat-devel
BuildRequires:	mingw64-sqlite-devel

%description
SQLite Client library for %{product_name}

%package -n mingw64-libudjat%{module_name}%{_libvrs}
Summary:	HTTP Client library for %{product_name}
Group:		Development/Libraries/C and C++
Provides:	mingw64-libudjat%{module_name}
Provides:	mingw64(lib:udjat%{module_name}.dll)
Provides:	mingw64(lib:udjat%{module_name})

%description -n mingw64-libudjat%{module_name}%{_libvrs}
C++ library to abstract a SQLite database.

%package devel
Summary:	C++ development files for {name}
Requires:	mingw64-libudjat%{module_name}%{_libvrs} = %{version}
Provides:	mingw64(lib::libudjat%{module_name}.a)
Group:		Development/Libraries/C and C++

%description devel
Header files for %{name}.

%package -n mingw64-%{product_name}-%{module_name}
Summary: SQLite module for %{name}

%description -n mingw64-%{product_name}-%{module_name}
%{product_name} module with SQLite support.

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n libudjatdb-%{version}
%_mingw64_meson

%build
%_mingw64_meson_build

%install
%_mingw64_meson_install
#%_mingw64_find_lang libudjat%{module_name}-%{MAJOR_VERSION}.%{MINOR_VERSION} langfiles

%files -n mingw64-libudjat%{module_name}%{_libvrs}
%defattr(-,root,root)
%doc README.md
%license LICENSE
%{_mingw64_bindir}/*.dll

%files -n mingw64-%{product_name}-%{module_name}
%defattr(-,root,root)
%{_mingw64_libdir}/*/*/modules/*.dll
%exclude %{_mingw64_libdir}/*/*/modules/*.a

%files devel
%doc README.md
%license LICENSE
%defattr(-,root,root)
%{_mingw64_libdir}/pkgconfig/*.pc
%{_mingw64_libdir}/*.a

%dir %{_mingw64_includedir}/udjat/agent/sql
%{_mingw64_includedir}/udjat/agent/sql/*.h

%dir %{_mingw64_includedir}/udjat/alert/sql
%{_mingw64_includedir}/udjat/alert/sql/*.h

%dir %{_mingw64_includedir}/udjat/tools/sql
%{_mingw64_includedir}/udjat/tools/sql/*.h

%changelog


