#
# spec file for package udjat-module-database
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

%global flavor @BUILD_FLAVOR@%{nil}

%if "%flavor" == ""
ExclusiveArch:  do_not_build
%endif

%define product_name %(pkg-config --variable=product_name libudjat)
%define module_path %(pkg-config --variable=module_path libudjat)

Summary:		Database module for %{product_name}
Name:			udjat-module-%{flavor}
Version:		2.0
Release:		0
License:		LGPL-3.0
Source:			udjat-module-database-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/udjat-module-database

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:	gcc-c++

%if %{flavor} == cppdb
BuildRequires:	cppdb-devel
%endif

%if %{flavor} == sqlite
BuildRequires:  pkgconfig(sqlite3)
%endif

BuildRequires:	pkgconfig(libudjat)

%description
Database module for %{product_name} using %{flavor} backend.

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n udjat-module-database-%{version}

NOCONFIGURE=1 \
	./autogen.sh

%configure --with-engine=%{flavor}

%build
make all

%install
%makeinstall

%files
%defattr(-,root,root)
%{module_path}/*.so

%changelog

