package=zeromq
$(package)_version=eb54966cb9393bfd990849231ea7d10e34f6319e
$(package)_download_path=https://github.com/$(package)/libzmq/archive/
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=24b8eccff926fc1838494babd4494264d5509066db02bb1213ea0a25facad44b
$(package)_patches=0001-fix-build-with-older-mingw64.patch

define $(package)_preprocess_cmds
  patch -p1 < $($(package)_patch_dir)/0001-fix-build-with-older-mingw64.patch && \
  ./autogen.sh && \
  cp -f $(BASEDIR)/config.guess $(BASEDIR)/config.sub config
endef

define $(package)_set_vars
  $(package)_config_opts=--without-documentation --disable-shared --without-libsodium --disable-curve
  $(package)_config_opts_linux=--with-pic
  $(package)_cxxflags=-std=c++11
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE) src/libzmq.la
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install-libLTLIBRARIES install-includeHEADERS install-pkgconfigDATA
endef

define $(package)_postprocess_cmds
  sed -i.old "s/ -lstdc++//" lib/pkgconfig/libzmq.pc && \
  rm -rf bin share
endef