Name:		xviewer-plugins-catin136
Version:	0.0.6
Release:	0%{?dist}
Summary:	cat_in_136's set of plugins for xviewer

License:	GPLv2+
URL:		https://github.com/cat-in-136/xviewer-plugins-catin136
Source0:	%{url}/archive/%{version}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  meson
BuildRequires:	libappstream-glib
BuildRequires:	gnome-common
BuildRequires:	pkgconfig(glib-2.0)
BuildRequires:	pkgconfig(gsettings-desktop-schemas)
BuildRequires:	pkgconfig(gtk+-3.0)
BuildRequires:	pkgconfig(libexif)
BuildRequires:	pkgconfig(libgdata)
BuildRequires:	pkgconfig(libpeas-1.0)
BuildRequires:	pkgconfig(xviewer)
BuildRequires:	pkgconfig(glib-2.0)
BuildRequires:	itstool
BuildRequires:	intltool

BuildRequires:  rpm_macro(_metainfodir)

Requires:	python%{python3_pkgversion}%{_isa}
Requires:	python%{python3_pkgversion}-xapps-overrides%{_isa}
Requires:	xviewer%{?_isa}

Requires(postun):	/sbin/ldconfig

%description
cat_in_136's set of plugins for xviewer

%prep
%autosetup -p 1
NOCONFIGURE=1 %{_bindir}/gnome-autogen.sh


%build
%meson
%meson_build


%install
%meson_install
%{_bindir}/find %{buildroot} -type f -name '*.a' -print -delete
%{_bindir}/find %{buildroot} -type f -name '*.la' -print -delete
#%find_lang %{name}


%check
%{_bindir}/appstream-util validate-relax --nonet		\
  %{buildroot}%{_metainfodir}/*.metainfo.xml


%post -p /sbin/ldconfig


%postun
/sbin/ldconfig
%if (0%{?fedora} && 0%{?fedora} <=24) || (0%{?rhel} && 0%{?rhel} <= 7)
if [ $1 -eq 0 ] ; then
	%{_bindir}/glib-compile-schemas %{_datadir}/glib-2.0/schemas &> /dev/null || :
fi
%endif # (0%%{?fedora} && 0%%{?fedora} <=24) || (0%%{?rhel} && 0%%{?rhel} <= 7)


%if (0%{?fedora} && 0%{?fedora} <=24) || (0%{?rhel} && 0%{?rhel} <= 7)
%posttrans
%{_bindir}/glib-compile-schemas %{_datadir}/glib-2.0/schemas &> /dev/null || :
%endif # (0%%{?fedora} && 0%%{?fedora} <=24) || (0%%{?rhel} && 0%%{?rhel} <= 7)


#%files -f %{name}.lang
%files
%license COPYING
#%doc NEWS
%{_prefix}/lib/xviewer/plugins/*
%{_metainfodir}/*.metainfo.xml


%changelog
* Fri Apr 11 2025 cat_in_136 - 0.0.6-1
- Merge workflow to build workflow with release step; remove old release workflow.

* Tue Apr 8 2025 cat_in_136 - 0.0.5-1
- infotxt: Fix bug where infotxt may be not updated after changing image
- infotxt: Add XMP and EXIF

* Sun Dec 1 2024 cat_in_136 - 0.0.4-1
- infotxt: meaningfull icc-profile display

* Sun Jan 28 2024 cat_in_136 - 0.0.3-1

- fix bug where xviewer may crash on image load on infotxt plugin

* Sun Jul 30 2023 cat_in_136 - 0.0.2-1
- special text highlighting and copy button for AUTOMATIC1111/stable-diffusion-webui parameter
- select thumbnail after sort changed

* Sat Jul 29 2023 cat_in_136 - 0.0.1-1
- Initial
