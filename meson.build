project('binkd', 'c', version: '1.0.4')

c_args = []

c_args += [ '-DPACKAGE_NAME=""' ]
c_args += [ '-DPACKAGE_TARNAME=""' ]
c_args += [ '-DPACKAGE_VERSION=""' ]
c_args += [ '-DPACKAGE_STRING=""' ]
c_args += [ '-DPACKAGE_BUGREPORT=""' ]
c_args += [ '-DPACKAGE_URL=""' ]

c_args += [ '-DSTDC_HEADERS=1' ]
c_args += [ '-DHAVE_SYS_TYPES_H=1' ]
c_args += [ '-DHAVE_SYS_STAT_H=1' ]
c_args += [ '-DHAVE_STDLIB_H=1' ]
c_args += [ '-DHAVE_STRING_H=1' ]
c_args += [ '-DHAVE_MEMORY_H=1' ]
c_args += [ '-DHAVE_STRINGS_H=1' ]
c_args += [ '-DHAVE_INTTYPES_H=1' ]
c_args += [ '-DHAVE_STDINT_H=1' ]
c_args += [ '-DHAVE_UNISTD_H=1' ]
c_args += [ '-DHAVE_INTTYPES_H=1' ]
c_args += [ '-DHAVE_STDINT_H=1' ]
c_args += [ '-DHAVE_UNISTD_H=1' ]
c_args += [ '-DHAVE_SYS_VFS_H=1' ]
c_args += [ '-DHAVE_SYS_STATFS_H=1' ]
c_args += [ '-DHAVE_SYS_STATVFS_H=1' ]
c_args += [ '-DHAVE_SYS_PARAM_H=1' ]
c_args += [ '-DHAVE_SYS_MOUNT_H=1' ]
c_args += [ '-DHAVE_ARPA_INET_H=1' ]
c_args += [ '-DHAVE_SYS_IOCTL_H=1' ]
c_args += [ '-DHAVE_SYS_TIME_H=1' ]
c_args += [ '-DHAVE_STDARG_H=1' ]
c_args += [ '-DHAVE_NETINET_IN_H=1' ]
c_args += [ '-DHAVE_NETDB_H=1' ]
c_args += [ '-DHAVE_ARPA_NAMESER_H=1' ]
c_args += [ '-DHAVE_RESOLV_H=1' ]
c_args += [ '-DHAVE_SNPRINTF=1' ]
c_args += [ '-DHAVE_VSNPRINTF=1' ]
c_args += [ '-DHAVE_VSYSLOG=1' ]
c_args += [ '-DHAVE_WAITPID=1' ]
c_args += [ '-DHAVE_STATVFS=1' ]
c_args += [ '-DHAVE_STATFS=1' ]
c_args += [ '-DHAVE_UNAME=1' ]
c_args += [ '-DHAVE_DAEMON=1' ]
c_args += [ '-DHAVE_SETSID=1' ]
c_args += [ '-DHAVE_GETOPT=1' ]
c_args += [ '-DHAVE_LOCALTIME_R=1' ]
c_args += [ '-DHAVE_STRTOUMAX=1' ]
c_args += [ '-DHAVE_SIGPROCMASK=1' ]
c_args += [ '-DHAVE_GETTIMEOFDAY=1' ]
c_args += [ '-DHAVE_FSEEKO=1' ]
c_args += [ '-DHAVE_NS_INITPARSE=1' ]
c_args += [ '-DSIZEOF_SHORT=2' ]
c_args += [ '-DSIZEOF_INT=4' ]
c_args += [ '-DSIZEOF_LONG=8' ]
c_args += [ '-DHAVE_SOCKLEN_T=1' ]
c_args += [ '-DHAVE_INTMAX_T=1' ]
c_args += [ '-DHAVE_FACILITYNAMES=1' ]
c_args += [ '-DHAVE_TIOCNOTTY=1' ]
c_args += [ '-DHAVE_MSG_NOSIGNAL=1' ]
c_args += [ '-DWITH_ZLIB=1' ]

c_args += [ '-DHAVE_FORK' ]
c_args += [ '-DUNIX' ]
c_args += [ '-DOS="UNIX"' ]

c_args += [ '-Wall' ]
c_args += [ '-Wno-char-subscripts' ]
c_args += [ '-O0' ]
c_args += [ '-g' ]

dependencies = []

z_dep = dependency('zlib', version : '>=1.2.8')
dependencies += [ z_dep ]

compiler = meson.get_compiler('c')

resolv_dep = compiler.find_library('resolv')
dependencies += [ resolv_dep ]

binkd_sources = [
  'src/md5b.c',
  'src/binkd.c',
  'src/readcfg.c',
  'src/tools.c',
  'src/ftnaddr.c',
  'src/ftnq.c',
  'src/client.c',
  'src/server.c',
  'src/protocol.c',
  'src/bsy.c',
  'src/inbound.c',
  'src/breaksig.c',
  'src/branch.c',
  'src/unix/rename.c',
  'src/unix/getfree.c',
  'src/ftndom.c',
  'src/ftnnode.c',
  'src/srif.c',
  'src/pmatch.c',
  'src/readflo.c',
  'src/prothlp.c',
  'src/iptools.c',
  'src/rfc2553.c',
  'src/run.c',
  'src/binlog.c',
  'src/exitproc.c',
  'src/getw.c',
  'src/xalloc.c',
  'src/crypt.c',
  'src/unix/setpttl.c',
  'src/unix/daemonize.c',
  'src/srv_gai.c',
  'src/compress.c',
]

executable('binkd', binkd_sources,
  dependencies: dependencies,
  c_args: c_args,
)
