Name: organic
Version: {{version}}
Release: 1
BuildArch: x86_64
Summary: A programming language for algorithmic audio synthesis
License: GPLv3
URL: https://github.com/ERSUCC/Organic
Source: %{name}-%{version}.tar.gz
Requires: libsndfile >= 1.2.2, libsamplerate >= 0.2.2

%global debug_package %{nil}

%description
A programming language for algorithmic audio synthesis

%prep
%autosetup

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}
cp bin/organic %{buildroot}%{_bindir}
cp -r lib64 %{buildroot}%{_libdir}

%files
%{_bindir}/organic
%{_libdir}/organic
