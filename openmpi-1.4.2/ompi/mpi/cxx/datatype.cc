// -*- c++ -*-
// 
// Copyright (c) 2006      Los Alamos National Security, LLC.  All rights
//                         reserved. 
// Copyright (c) 2007-2008 Sun Microsystems, Inc.  All rights reserved.
// Copyright (c) 2007-2008 Cisco Systems, Inc.  All rights reserved.
// $COPYRIGHT$
// 
// Additional copyrights may follow
// 
// $HEADER$
//

// do not include ompi_config.h because it kills the free/malloc defines
#include "mpi.h"
#include "ompi/mpi/cxx/mpicxx.h"

#include "opal/threads/mutex.h"
#include "opal/class/opal_list.h"

#include "ompi/communicator/communicator.h"
#include "ompi/attribute/attribute.h"
#include "ompi/errhandler/errhandler.h"


void
MPI::Datatype::Free()
{
    (void)MPI_Type_free(&mpi_datatype);
}

int
MPI::Datatype::do_create_keyval(MPI_Type_copy_attr_function* c_copy_fn,
                                MPI_Type_delete_attr_function* c_delete_fn,
                                Copy_attr_function* cxx_copy_fn,
                                Delete_attr_function* cxx_delete_fn,
                                void* extra_state, int &keyval)
{
    int ret, count = 0;
    ompi_attribute_fn_ptr_union_t copy_fn;
    ompi_attribute_fn_ptr_union_t delete_fn;
    keyval_intercept_data_t *cxx_extra_state;

    // If both the callbacks are C, then do the simple thing -- no
    // need for all the C++ machinery.
    if (NULL != c_copy_fn && NULL != c_delete_fn) {
        copy_fn.attr_datatype_copy_fn = 
            (MPI_Type_internal_copy_attr_function*) c_copy_fn;
        delete_fn.attr_datatype_delete_fn = c_delete_fn;
        ret = ompi_attr_create_keyval(COMM_ATTR, copy_fn, delete_fn,
                                      &keyval, extra_state, 0, NULL);
        if (MPI_SUCCESS != ret) {
            return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, ret,
                                          "MPI::Datatype::Create_keyval");
        }
    }

    // If either callback is C++, then we have to use the C++
    // callbacks for both, because we have to generate a new
    // extra_state.  And since we only get one extra_state (i.e., we
    // don't get one extra_state for the copy callback and another
    // extra_state for the delete callback), we have to use the C++
    // callbacks for both (and therefore translate the C++-special
    // extra_state into the user's original extra_state).
    cxx_extra_state = (keyval_intercept_data_t*) 
        malloc(sizeof(keyval_intercept_data_t));
    if (NULL == cxx_extra_state) {
        return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_NO_MEM, 
                                      "MPI::Datatype::Create_keyval");
    }
    cxx_extra_state->c_copy_fn = c_copy_fn;
    cxx_extra_state->cxx_copy_fn = cxx_copy_fn;
    cxx_extra_state->c_delete_fn = c_delete_fn;
    cxx_extra_state->cxx_delete_fn = cxx_delete_fn;
    cxx_extra_state->extra_state = extra_state;

    // Error check.  Must have exactly 2 non-NULL function pointers.
    if (NULL != c_copy_fn) {
        ++count;
    }
    if (NULL != c_delete_fn) {
        ++count;
    }
    if (NULL != cxx_copy_fn) {
        ++count;
    }
    if (NULL != cxx_delete_fn) {
        ++count;
    }
    if (2 != count) {
        free(cxx_extra_state);
        return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_ARG, 
                                      "MPI::Datatype::Create_keyval");
    }

    // We do not call MPI_Datatype_create_keyval() here because we need to
    // pass in a special destructor to the backend keyval creation
    // that gets invoked when the keyval's reference count goes to 0
    // and is finally destroyed (i.e., clean up some caching/lookup
    // data here in the C++ bindings layer).  This destructor is
    // *only* used in the C++ bindings, so it's not set by the C
    // MPI_Comm_create_keyval().  Hence, we do all the work here (and
    // ensure to set the destructor atomicly when the keyval is
    // created).

    copy_fn.attr_datatype_copy_fn =
        (MPI_Type_internal_copy_attr_function*) 
        ompi_mpi_cxx_type_copy_attr_intercept;
    delete_fn.attr_datatype_delete_fn =
        ompi_mpi_cxx_type_delete_attr_intercept;
    ret = ompi_attr_create_keyval(TYPE_ATTR, copy_fn, delete_fn,
                                  &keyval, cxx_extra_state, 0,
                                  cxx_extra_state);
    if (OMPI_SUCCESS != ret) {
        return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, ret,
                                      "MPI::Datatype::Create_keyval");
    }

    return MPI_SUCCESS;
}