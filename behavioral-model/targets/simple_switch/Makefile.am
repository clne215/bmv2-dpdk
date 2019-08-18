if COND_NANOMSG
MAYBE_TESTS = tests
endif

if COND_PI
AM_CPPFLAGS += \
-DWITH_PI \
-I$(top_srcdir)/PI
PI_LIB = $(top_builddir)/PI/libbmpi.la
else
PI_LIB =
endif

SUBDIRS = . $(MAYBE_TESTS)

#INCLUDES = -Idpdk/build/lib dpdk/examples/dpdk_api
AM_CPPFLAGS += -Idpdk/build/include -Ldpdk/build/lib

THRIFT_IDL = $(srcdir)/thrift/simple_switch.thrift

noinst_LTLIBRARIES = libsimpleswitch.la
lib_LTLIBRARIES = libsimpleswitch_runner.la

libsimpleswitch_la_SOURCES = \
simple_switch.cpp \
simple_switch.h \
primitives.cpp

libsimpleswitch_runner_la_SOURCES = \
runner.cpp

nobase_include_HEADERS = bm/simple_switch/runner.h \
dpdk/build/include/rte_eal.h \
dpdk/build/include/rte_ethdev.h \
dpdk/build/include/rte_common.h \
dpdk/build/include/rte_log.h \
dpdk/build/include/rte_mbuf.h

libsimpleswitch_la_LIBADD = \
$(top_builddir)/src/bm_sim/libbmsim.la \
$(top_builddir)/src/bf_lpm_trie/libbflpmtrie.la \
$(top_builddir)/src/BMI/libbmi.la \
$(top_builddir)/third_party/jsoncpp/libjson.la \
-lboost_system $(THRIFT_LIB) -lboost_program_options -lboost_filesystem

libsimpleswitch_runner_la_LIBADD = \
$(PI_LIB) \
libsimpleswitch.la

if COND_THRIFT

simple_switch_thrift_py_files = \
gen-py/sswitch_runtime/constants.py \
gen-py/sswitch_runtime/__init__.py \
gen-py/sswitch_runtime/SimpleSwitch.py \
gen-py/sswitch_runtime/ttypes.py
simple_switch_thrift_files = \
gen-cpp/bm/simple_switch_constants.cpp \
gen-cpp/bm/simple_switch_constants.h \
gen-cpp/bm/SimpleSwitch.cpp \
gen-cpp/bm/SimpleSwitch.h \
gen-cpp/bm/simple_switch_types.cpp \
gen-cpp/bm/simple_switch_types.h

BUILT_SOURCES = $(simple_switch_thrift_files) $(simple_switch_thrift_py_files)

sswitchpydir = $(pythondir)/sswitch_runtime
nodist_sswitchpy_PYTHON = $(simple_switch_thrift_py_files)

python_PYTHON = sswitch_CLI.py

# See
# http://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Installation-Directory-Variables.html
edit = sed \
	-e 's|@pythondir[@]|$(pythondir)|g'

simple_switch_CLI: Makefile
	rm -f $@ $@.tmp
	$(edit) $(srcdir)/$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

simple_switch_CLI: simple_switch_CLI.in

EXTRA_DIST = \
simple_switch_CLI.in \
sswitch_CLI

bin_SCRIPTS = \
simple_switch_CLI

AM_CPPFLAGS += \
-I$(srcdir)/thrift/src/ \
-I$(builddir)/gen-cpp \
-Wall -march=native

libsimpleswitch_la_SOURCES += thrift/src/SimpleSwitch_server.cpp

bin_PROGRAMS = simple_switch

simple_switch_LDADD = \
$(top_builddir)/src/bm_runtime/libbmruntime.la \
$(top_builddir)/thrift_src/libruntimestubs.la \
libsimpleswitch.la \
libsimpleswitch_thrift.la

simple_switch_SOURCES = main.cpp
simple_switch_LDFLAGS = -Wl,--export-dynamic \
-Wl,--whole-archive,dpdk/examples/dpdk_api/build/libdpdk_api.so,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_ethdev.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_mbuf.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_mempool.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_eal.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_acl.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_kni.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_af_packet.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_vhost.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_kvargs.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_virtio.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_cfgfile.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_vmxnet3_uio.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_cmdline.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_lpm.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_bnxt.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_null.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_port.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_cryptodev.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_bond.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_null_crypto.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_power.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_distributor.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_reorder.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_cxgbe.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_qede.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_ring.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_e1000.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_ring.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_sched.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_meter.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_ena.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_table.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_enic.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_timer.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_hash.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_net.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_fm10k.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_vhost.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_ip_frag.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pdump.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_i40e.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_jobstats.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pipeline.a,--no-whole-archive \
-Wl,--whole-archive,dpdk/build/lib/librte_pmd_ixgbe.a,--no-whole-archive

if ENABLE_MODULES
simple_switch_LDFLAGS += -rdynamic
endif

lib_LTLIBRARIES += libsimpleswitch_thrift.la

nodist_libsimpleswitch_thrift_la_SOURCES = \
$(simple_switch_thrift_files)

# See http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

EXTRA_DIST += $(THRIFT_IDL)

# We copy the generated Python code to srcdir/ if we have permissions (i.e. not
# for 'make distcheck'). This is to ensure we can run the CLI easily. Maybe I
# will think of a better solution later.
thrift_files.ts: $(THRIFT_IDL)
	@rm -f thrift_files.tmp
	@touch thrift_files.tmp
	@mkdir -p $(builddir)/gen-cpp/bm
	$(THRIFT) -out $(builddir)/gen-cpp/bm --gen cpp -r $(THRIFT_IDL)
	$(THRIFT) -o $(builddir) --gen py -r $(THRIFT_IDL)
	if mkdir $(srcdir)/sswitch_runtime.test 2>/dev/null; then \
	  rm -rf $(srcdir)/sswitch_runtime/; \
	  cp -r $(builddir)/gen-py/sswitch_runtime/ $(srcdir)/; \
	  rm -rf $(srcdir)/sswitch_runtime.test; else :; \
        fi
	@mv -f thrift_files.tmp $@

$(BUILT_SOURCES): thrift_files.ts
## Recover from the removal of $@
	@if test -f $@; then :; else \
	  trap 'rm -rf thrift_files.lock thrift_files.ts' 1 2 13 15; \
## mkdir is a portable test-and-set
	if mkdir thrift_files.lock 2>/dev/null; then \
## This code is being executed by the first process.
	  rm -f thrift_files.ts; \
	  $(MAKE) $(AM_MAKEFLAGS) thrift_files.ts; \
	  result=$$?; rm -rf thrift_files.lock; exit $$result; \
	else \
## This code is being executed by the follower processes.
## Wait until the first process is done.
	  while test -d thrift_files.lock; do sleep 1; done; \
## Succeed if and only if the first process succeeded.
	    test -f thrift_files.ts; \
	  fi; \
	fi

nodist_pkginclude_HEADERS = \
gen-cpp/bm/simple_switch_constants.h \
gen-cpp/bm/SimpleSwitch.h \
gen-cpp/bm/simple_switch_types.h \
dpdk/examples/dpdk_api/dpdk_api.h \
dpdk/build/include/rte_eal.h \
dpdk/build/include/rte_ethdev.h \
dpdk/build/include/rte_common.h \
dpdk/build/include/rte_log.h \
dpdk/build/include/rte_mbuf.h \
dpdk/examples/dpdk_api/build/libdpdk_api.so

CLEANFILES = $(BUILT_SOURCES) \
thrift_files.ts \
gen-cpp/bm/SimpleSwitch_server.skeleton.cpp \
$(bin_SCRIPTS)

# I used to do a rm on gen-cpp, but it was removing a .deps directory, instead I
# am adding the skeleton file to CLEANFILES
clean-local:
	rm -rf gen-py

endif  # COND_THRIFT
