#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

SUBDIRS = plpa

# To find plpa_bottom.h
AM_CPPFLAGS = -I$(top_srcdir)/opal/mca/paffinity/linux/plpa/src/libplpa

sources = \
        paffinity_linux.h \
        paffinity_linux_component.c \
        paffinity_linux_module.c

# Make the output library in this directory, and name it either
# mca_<type>_<name>.la (for DSO builds) or libmca_<type>_<name>.la
# (for static builds).

if OMPI_BUILD_paffinity_linux_DSO
component_noinst =
component_install = mca_paffinity_linux.la
else
component_noinst = libmca_paffinity_linux.la
component_install =
endif

mcacomponentdir = $(pkglibdir)
mcacomponent_LTLIBRARIES = $(component_install)
mca_paffinity_linux_la_SOURCES = $(sources)
mca_paffinity_linux_la_LDFLAGS = -module -avoid-version
mca_paffinity_linux_la_LIBADD = \
        $(top_ompi_builddir)/opal/mca/paffinity/linux/plpa/src/libplpa/libplpa_included.la

noinst_LTLIBRARIES = $(component_noinst)
libmca_paffinity_linux_la_SOURCES =$(sources)
libmca_paffinity_linux_la_LDFLAGS = -module -avoid-version
libmca_paffinity_linux_la_LIBADD = \
        $(top_ompi_builddir)/opal/mca/paffinity/linux/plpa/src/libplpa/libplpa_included.la
