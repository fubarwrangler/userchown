Name:      phnxchown
Version:   1.1
Release:   1%{?dist}
Summary:   RACF User copy-as Software

Group:     Applications/System
License:   RACF INTERNAL USE ONLY
URL:       http://www.racf.bnl.gov/
Packager:  William Strecker-Kellogg <willsk@bnl.gov>

Source0:   %{name}-%{version}.tar.gz

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: tar, gzip
BuildRequires: make
BuildRequires: cmake

%description
A setuid program to allow a configured user (currently anatrain) to copy files
to a restricted set of directories as another user (belonging to a specific
group).

%prep
%setup -q -n %{name}-%{version} -c

%build
%cmake \
   -DPROG_NAME=%{name} \
   -DCONFIG_PATH="/etc/phnxchown.cfg" \
   -DCONFIG_SOURCE="cfg/phnxchown.cfg"

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make DESTDIR="%{buildroot}" install

mv %{buildroot}%{_bindir}/ %{buildroot}%{_libexecdir}/

gzip -9 %{buildroot}%{_mandir}/man1/phnxchown.1

%files
%defattr(-,root,root,-)
%config %{_sysconfdir}/phnxchown.cfg
%doc %{_mandir}/man1/phnxchown.1.gz

%defattr(4711,root,root)
%{_libexecdir}/phnxchown


%changelog
* Wed Oct 12 2012 William Strecker-Kellogg <willsk@bnl.gov>
- Add capacity to move multiple inputs to an output directory
* Wed Oct 10 2012 William Strecker-Kellogg <willsk@bnl.gov>
- Fix bug where errno set by read that returns > 0, and move to /usr/libexec/
* Wed Oct 03 2012 William Strecker-Kellogg <willsk@bnl.gov>
- Update pathsplit and fix some bugs Chris found
* Mon Oct 01 2012 William Strecker-Kellogg <willsk@bnl.gov>
- Package manpage and updated version
* Fri Sep 28 2012 William Strecker-Kellogg <willsk@bnl.gov>
- Working package, first version
