Name:      phnxchown
Version:   0.2
Release:   1%{?dist}
Summary:   RACF User copy-as Software

Group:     Applications/System
License:   RACF INTERNAL USE ONLY
URL:       http://www.racf.bnl.gov/
Source0:   userchown.tar.gz

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

%files
%defattr(-,root,root,-)
%config %{_sysconfdir}/phnxchown.cfg

%defattr(4711,root,root)
%{_bindir}/phnxchown


%changelog
* Fri Sep 28 2012 William Strecker-Kellogg <willsk@bnl.gov>
- Working package, first version
