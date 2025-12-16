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
%{?!udjat_module:%define udjat_module() %{udjat_product_name}%{udjat_module_version}-module-%{**}}

Summary:		Database library for %{udjat_product_name}  
Name:			libudjat%{module_name}
Version: 2.2.1
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/libudjat%{module_name}

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:  meson
BuildRequires:	gcc-c++ >= 5

BuildRequires:	pkgconfig(libudjat)
BuildRequires:	pkgconfig(sqlite3)
BuildRequires:	cppdb-devel

%description
Database library for %{udjat_product_name}

C++ database components for use with lib%{udjat_product_name}

#---[ Libraries ]-----------------------------------------------------------------------------------------------------

# CPPDB based library
%package -n lib%{udjat_product_name}cppdb%{udjat_major}_%{udjat_minor}
Summary: CPPdb library for %{udjat_product_name}

%description -n lib%{udjat_product_name}cppdb%{udjat_major}_%{udjat_minor}
Database library for %{udjat_product_name}

C++ Database components for use with lib%{udjat_product_name} using CPPDB backend.

# SQLite based library
%package -n lib%{udjat_product_name}sqlite%{udjat_major}_%{udjat_minor}
Summary: SQLite library for %{udjat_product_name}

%description -n lib%{udjat_product_name}sqlite%{udjat_major}_%{udjat_minor}
SQLite library for %{udjat_product_name}

C++ SQLite classes for use with lib%{udjat_product_name} using sqlite backend.

#---[ Development ]---------------------------------------------------------------------------------------------------

%package devel
Summary: Development files for %{name}
Requires: lib%{udjat_product_name}cppdb%{udjat_major}_%{udjat_minor} = %{version}
Requires: lib%{udjat_product_name}sqlite%{udjat_major}_%{udjat_minor} = %{version}
Provides: %{name}%{udjat_major}-devel = %{version}
Provides: %{name}%{udjat_major}_%{udjat_minor}-devel = %{version}

%description devel
Database library for %{udjat_product_name}

C++ database classes for use with lib%{udjat_product_name}

#---[ Modules ]-------------------------------------------------------------------------------------------------------

%package -n %{udjat_module cppdb}
Summary: CPPdb module for %{name}
Provides: %{udjat_module cppdb} = %{version}

%description -n %{udjat_module cppdb}
CPPdb database module for %{udjat_product_name}

%package -n %{udjat_module sqlite}
Summary: SQLite module for %{name}

%description -n %{udjat_module sqlite}
SQLite database module for %{udjat_product_name}

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%autosetup
%meson

%build
%meson_build

%install
%meson_install

%files -n lib%{udjat_product_name}cppdb%{udjat_major}_%{udjat_minor}
%defattr(-,root,root)
%{_libdir}/*cppdb*.so.%{udjat_major}.%{udjat_minor}

%files -n lib%{udjat_product_name}sqlite%{udjat_major}_%{udjat_minor}
%defattr(-,root,root)
%{_libdir}/*sqlite*.so.%{udjat_major}.%{udjat_minor}

%files -n %{udjat_module cppdb}
%{udjat_module_path}/*cppdb*.so

%files -n %{udjat_module sqlite}
%{udjat_module_path}/*sqlite*.so

%files devel
%defattr(-,root,root)

%dir %{_includedir}/udjat/agent/sql
%{_includedir}/udjat/agent/sql/*.h

%dir %{_includedir}/udjat/alert/sql
%{_includedir}/udjat/alert/sql/*.h

%dir %{_includedir}/udjat/tools/sql
%{_includedir}/udjat/tools/sql/*.h
%{_includedir}/udjat/tools/sql.h
%{_includedir}/udjat/tools/actions/*.h

%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%post -n lib%{udjat_product_name}cppdb%{udjat_major}_%{udjat_minor} -p /sbin/ldconfig

%postun -n lib%{udjat_product_name}cppdb%{udjat_major}_%{udjat_minor} -p /sbin/ldconfig

%post -n lib%{udjat_product_name}sqlite%{udjat_major}_%{udjat_minor} -p /sbin/ldconfig

%postun -n lib%{udjat_product_name}sqlite%{udjat_major}_%{udjat_minor} -p /sbin/ldconfig

%changelog

