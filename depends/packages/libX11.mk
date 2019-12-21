package=libX11
$(package)_version=1.6.9
$(package)_download_path=http://xorg.freedesktop.org/releases/individual/lib/
$(package)_file_name=$(package)-$($(package)_version).tar.bz2
$(package)_sha256_hash=9cc7e8d000d6193fa5af580d50d689380b8287052270f5bb26a5fb6b58b2bed1
$(package)_dependencies=libxcb xtrans xextproto xproto

define $(package)_set_vars
$(package)_config_opts=--disable-xkb --disable-static
$(package)_config_opts_linux=--with-pic
endef

define $(package)_preprocess_cmds
  cp -f $(BASEDIR)/config.guess $(BASEDIR)/config.sub .
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
