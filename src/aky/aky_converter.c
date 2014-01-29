/*
    This file is part of Akypuera

    Akypuera is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Akypuera is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Akypuera. If not, see <http://www.gnu.org/licenses/>.
*/
#include "aky2paje.h"

int main(int argc, char **argv)
{
  bzero (&arguments, sizeof(struct arguments));
  if (argp_parse (&argp, argc, argv, 0, 0, &arguments) == ARGP_KEY_ERROR){
    fprintf(stderr,
            "[%s] at %s, "
            "error during the parsing of parameters\n",
            PROGRAM, __FUNCTION__);
    return 1;
  }

  if (aky_key_init() == 1){
   fprintf(stderr,
           "[%s] at %s,"
           "error during hash table allocation\n",
           PROGRAM, __FUNCTION__);
    return 1;
  }

  rst_rastro_t rastro;
  bzero (&rastro, sizeof(rst_rastro_t));
  rst_event_t event;
  int i;
  int fail = 0;

  for (i = 0; i < arguments.input_size; i++){
    int ret = rst_open_file(&rastro, 100000,
                            arguments.input[i],
                            arguments.synchronization_file);
    if (ret == -1) {
      fprintf(stderr,
              "[%s] at %s, "
              "trace file %s could not be opened\n",
              PROGRAM, __FUNCTION__, arguments.input[i]);
      return 1;
    }
  }

  name_init();

  if (!arguments.dummy){
    /* start output with comments */
    if (arguments.comment){
      aky_dump_comment (PROGRAM, arguments.comment);
    }
    if (arguments.comment_file){
      if (aky_dump_comment_file (PROGRAM, arguments.comment_file) == 1){
        return 1;
      }
    }

    /* output contents of synchronization file if used */
    if (arguments.synchronization_file){
      if (aky_dump_comment_file (PROGRAM, arguments.synchronization_file) == 1){
        return 1;
      }
    }

    /* output build version, date and conversion for aky in the trace */
    aky_dump_version (PROGRAM, argv, argc);
    poti_header (arguments.basic, 0);
    aky_paje_hierarchy();
  }


  double timestamp = -1;
  while (rst_decode_event(&rastro, &event) && !fail) {
    static int root_created = 0;
    char mpi_process[100];
    snprintf(mpi_process, 100, "rank%"PRIu64"", event.id1);
    timestamp = event.timestamp;
    switch (event.type) {
    case AKY_PTP_SEND:
      if (!arguments.no_links){
        char key[AKY_DEFAULT_STR_SIZE];
        int messageSize = -1;
        int mark = -1;
        if (event.ct.n_uint32 == 2){
          /* has message size */
          messageSize = event.v_uint32[1];
          if (event.ct.n_uint64 == 1){
            /* has message mark */
            mark = event.v_uint64[0];
          }
        }
        aky_put_key("n", event.id1, event.v_uint32[0], key, 
                    AKY_DEFAULT_STR_SIZE);
        if (messageSize != -1 && mark != -1){
          poti_StartLinkSizeMark(timestamp, "root", "LINK",
                                 mpi_process, "PTP", key,
                                 messageSize, mark);
        }else{
          poti_StartLink(timestamp, "root", "LINK", mpi_process, "PTP", key);
        }
      }
      break;
    case AKY_PTP_RECV:
      if (!arguments.no_links){
        char key[AKY_DEFAULT_STR_SIZE];
        char *result = aky_get_key("n", event.v_uint32[0], event.id1, key,
                                   AKY_DEFAULT_STR_SIZE);
        if (result == NULL){
          fprintf (stderr,
                   "[aky_converter] at %s, no key to generate a pajeEndLink,\n"
                   "[aky_converter] got a receive at dst = %"PRIu64" from src = %d\n"
                   "[aky_converter] but no send for this receive yet,\n"
                   "[aky_converter] do you synchronize your input traces?\n",
                   __FUNCTION__, event.id1, event.v_uint32[0]);
          if (!arguments.ignore_errors){
            fail = 1;
          }
        }
        poti_EndLink(timestamp, "root", "LINK", mpi_process, "PTP", key);
      }
      break;
    case MPI_INIT:
      if (root_created == 0){
        poti_CreateContainer (timestamp, "root", "ROOT", "0", "root");
        root_created = 1;
      }
      poti_CreateContainer(timestamp, mpi_process,
                           "PROCESS", "root", mpi_process);
      break;
    case MPI_COMM_SPAWN_IN:
    case MPI_COMM_GET_NAME_IN:
    case MPI_COMM_SET_NAME_IN:
    case MPI_REDUCE_IN:
    case MPI_ALLREDUCE_IN:
    case MPI_REDUCE_SCATTER_IN:
    case MPI_ALLGATHER_IN:
    case MPI_ALLGATHERV_IN:
    case MPI_SCATTER_IN:
    case MPI_SCATTERV_IN:
    case MPI_WAIT_IN:
    case MPI_IRECV_IN:
    case MPI_ISEND_IN:
    case MPI_RECV_IN:
    case MPI_SEND_IN:
    case MPI_BCAST_IN:
    case MPI_BARRIER_IN:
    case MPI_GATHER_IN:
    case MPI_GATHERV_IN:
    case MPI_ALLTOALL_IN:
    case MPI_ALLTOALLV_IN:
    case MPI_OP_CREATE_IN:
    case MPI_OP_FREE_IN:
    case MPI_SCAN_IN:
    case MPI_ATTR_DELETE_IN:
    case MPI_ATTR_GET_IN:
    case MPI_ATTR_PUT_IN:
    case MPI_COMM_COMPARE_IN:
    case MPI_COMM_CREATE_IN:
    case MPI_COMM_DUP_IN:
    case MPI_COMM_FREE_IN:
    case MPI_COMM_GROUP_IN:
    case MPI_COMM_RANK_IN:
    case MPI_COMM_REMOTE_GROUP_IN:
    case MPI_COMM_REMOTE_SIZE_IN:
    case MPI_COMM_SIZE_IN:
    case MPI_COMM_SPLIT_IN:
    case MPI_COMM_TEST_INTER_IN:
    case MPI_GROUP_COMPARE_IN:
    case MPI_GROUP_DIFFERENCE_IN:
    case MPI_GROUP_EXCL_IN:
    case MPI_GROUP_FREE_IN:
    case MPI_GROUP_INCL_IN:
    case MPI_GROUP_INTERSECTION_IN:
    case MPI_GROUP_RANK_IN:
    case MPI_GROUP_RANGE_EXCL_IN:
    case MPI_GROUP_RANGE_INCL_IN:
    case MPI_GROUP_SIZE_IN:
    case MPI_GROUP_TRANSLATE_RANKS_IN:
    case MPI_GROUP_UNION_IN:
    case MPI_INTERCOMM_CREATE_IN:
    case MPI_INTERCOMM_MERGE_IN:
    case MPI_KEYVAL_CREATE_IN:
    case MPI_KEYVAL_FREE_IN:
    case MPI_ABORT_IN:
    case MPI_ERROR_CLASS_IN:
    case MPI_ERRHANDLER_CREATE_IN:
    case MPI_ERRHANDLER_FREE_IN:
    case MPI_ERRHANDLER_GET_IN:
    case MPI_ERROR_STRING_IN:
    case MPI_ERRHANDLER_SET_IN:
    case MPI_GET_PROCESSOR_NAME_IN:
    case MPI_INITIALIZED_IN:
    case MPI_WTICK_IN:
    case MPI_WTIME_IN:
    case MPI_ADDRESS_IN:
    case MPI_BSEND_IN:
    case MPI_BSEND_INIT_IN:
    case MPI_BUFFER_ATTACH_IN:
    case MPI_BUFFER_DETACH_IN:
    case MPI_CANCEL_IN:
    case MPI_REQUEST_FREE_IN:
    case MPI_RECV_INIT_IN:
    case MPI_SEND_INIT_IN:
    case MPI_GET_ELEMENTS_IN:
    case MPI_GET_COUNT_IN:
    case MPI_IBSEND_IN:
    case MPI_IPROBE_IN:
    case MPI_IRSEND_IN:
    case MPI_ISSEND_IN:
    case MPI_PACK_IN:
    case MPI_PACK_SIZE_IN:
    case MPI_PROBE_IN:
    case MPI_RSEND_IN:
    case MPI_RSEND_INIT_IN:
    case MPI_SENDRECV_IN:
    case MPI_SENDRECV_REPLACE_IN:
    case MPI_SSEND_IN:
    case MPI_SSEND_INIT_IN:
    case MPI_START_IN:
    case MPI_STARTALL_IN:
    case MPI_TEST_IN:
    case MPI_TESTALL_IN:
    case MPI_TESTANY_IN:
    case MPI_TEST_CANCELLED_IN:
    case MPI_TESTSOME_IN:
    case MPI_TYPE_COMMIT_IN:
    case MPI_TYPE_CONTIGUOUS_IN:
    case MPI_TYPE_EXTENT_IN:
    case MPI_TYPE_FREE_IN:
    case MPI_TYPE_HINDEXED_IN:
    case MPI_TYPE_HVECTOR_IN:
    case MPI_TYPE_INDEXED_IN:
    case MPI_TYPE_LB_IN:
    case MPI_TYPE_SIZE_IN:
    case MPI_TYPE_STRUCT_IN:
    case MPI_TYPE_UB_IN:
    case MPI_TYPE_VECTOR_IN:
    case MPI_UNPACK_IN:
    case MPI_WAITALL_IN:
    case MPI_WAITANY_IN:
    case MPI_WAITSOME_IN:
    case MPI_CART_COORDS_IN:
    case MPI_CART_CREATE_IN:
    case MPI_CART_GET_IN:
    case MPI_CART_MAP_IN:
    case MPI_CART_SHIFT_IN:
    case MPI_CARTDIM_GET_IN:
    case MPI_DIMS_CREATE_IN:
    case MPI_GRAPH_CREATE_IN:
    case MPI_GRAPH_GET_IN:
    case MPI_GRAPH_MAP_IN:
    case MPI_GRAPH_NEIGHBORS_IN:
    case MPI_GRAPH_NEIGHBORS_COUNT_IN:
    case MPI_GRAPHDIMS_GET_IN:
    case MPI_TOPO_TEST_IN:
    case MPI_RECV_IDLE_IN:
    case MPI_CART_RANK_IN:
    case MPI_CART_SUB_IN:
    case MPI_FINALIZE_IN:
      if (!arguments.no_states){
        char value[100];
        snprintf(value, 100, "%s", name_get(event.type));
        if (event.ct.n_uint64 == 1){
          /* has message mark */
          int mark = event.v_uint64[0];
          poti_PushStateMark(timestamp, mpi_process, "STATE", value, mark);
        }else{
          poti_PushState(timestamp, mpi_process, "STATE", value);
        }
      }
      break;
    case MPI_COMM_SPAWN_OUT:
    case MPI_COMM_GET_NAME_OUT:
    case MPI_COMM_SET_NAME_OUT:
    case MPI_REDUCE_OUT:
    case MPI_ALLREDUCE_OUT:
    case MPI_REDUCE_SCATTER_OUT:
    case MPI_ALLGATHER_OUT:
    case MPI_ALLGATHERV_OUT:
    case MPI_SCATTER_OUT:
    case MPI_SCATTERV_OUT:
    case MPI_WAIT_OUT:
    case MPI_IRECV_OUT:
    case MPI_ISEND_OUT:
    case MPI_RECV_OUT:
    case MPI_SEND_OUT:
    case MPI_BCAST_OUT:
    case MPI_BARRIER_OUT:
    case MPI_GATHER_OUT:
    case MPI_GATHERV_OUT:
    case MPI_ALLTOALL_OUT:
    case MPI_ALLTOALLV_OUT:
    case MPI_OP_CREATE_OUT:
    case MPI_OP_FREE_OUT:
    case MPI_SCAN_OUT:
    case MPI_ATTR_DELETE_OUT:
    case MPI_ATTR_GET_OUT:
    case MPI_ATTR_PUT_OUT:
    case MPI_COMM_COMPARE_OUT:
    case MPI_COMM_CREATE_OUT:
    case MPI_COMM_DUP_OUT:
    case MPI_COMM_FREE_OUT:
    case MPI_COMM_GROUP_OUT:
    case MPI_COMM_RANK_OUT:
    case MPI_COMM_REMOTE_GROUP_OUT:
    case MPI_COMM_REMOTE_SIZE_OUT:
    case MPI_COMM_SIZE_OUT:
    case MPI_COMM_SPLIT_OUT:
    case MPI_COMM_TEST_INTER_OUT:
    case MPI_GROUP_COMPARE_OUT:
    case MPI_GROUP_DIFFERENCE_OUT:
    case MPI_GROUP_EXCL_OUT:
    case MPI_GROUP_FREE_OUT:
    case MPI_GROUP_INCL_OUT:
    case MPI_GROUP_INTERSECTION_OUT:
    case MPI_GROUP_RANK_OUT:
    case MPI_GROUP_RANGE_EXCL_OUT:
    case MPI_GROUP_RANGE_INCL_OUT:
    case MPI_GROUP_SIZE_OUT:
    case MPI_GROUP_TRANSLATE_RANKS_OUT:
    case MPI_GROUP_UNION_OUT:
    case MPI_INTERCOMM_CREATE_OUT:
    case MPI_INTERCOMM_MERGE_OUT:
    case MPI_KEYVAL_CREATE_OUT:
    case MPI_KEYVAL_FREE_OUT:
    case MPI_ABORT_OUT:
    case MPI_ERROR_CLASS_OUT:
    case MPI_ERRHANDLER_CREATE_OUT:
    case MPI_ERRHANDLER_FREE_OUT:
    case MPI_ERRHANDLER_GET_OUT:
    case MPI_ERROR_STRING_OUT:
    case MPI_ERRHANDLER_SET_OUT:
    case MPI_GET_PROCESSOR_NAME_OUT:
    case MPI_INITIALIZED_OUT:
    case MPI_WTICK_OUT:
    case MPI_WTIME_OUT:
    case MPI_ADDRESS_OUT:
    case MPI_BSEND_OUT:
    case MPI_BSEND_INIT_OUT:
    case MPI_BUFFER_ATTACH_OUT:
    case MPI_BUFFER_DETACH_OUT:
    case MPI_CANCEL_OUT:
    case MPI_REQUEST_FREE_OUT:
    case MPI_RECV_INIT_OUT:
    case MPI_SEND_INIT_OUT:
    case MPI_GET_ELEMENTS_OUT:
    case MPI_GET_COUNT_OUT:
    case MPI_IBSEND_OUT:
    case MPI_IPROBE_OUT:
    case MPI_IRSEND_OUT:
    case MPI_ISSEND_OUT:
    case MPI_PACK_OUT:
    case MPI_PACK_SIZE_OUT:
    case MPI_PROBE_OUT:
    case MPI_RSEND_OUT:
    case MPI_RSEND_INIT_OUT:
    case MPI_SENDRECV_OUT:
    case MPI_SENDRECV_REPLACE_OUT:
    case MPI_SSEND_OUT:
    case MPI_SSEND_INIT_OUT:
    case MPI_START_OUT:
    case MPI_STARTALL_OUT:
    case MPI_TEST_OUT:
    case MPI_TESTALL_OUT:
    case MPI_TESTANY_OUT:
    case MPI_TEST_CANCELLED_OUT:
    case MPI_TESTSOME_OUT:
    case MPI_TYPE_COMMIT_OUT:
    case MPI_TYPE_CONTIGUOUS_OUT:
    case MPI_TYPE_EXTENT_OUT:
    case MPI_TYPE_FREE_OUT:
    case MPI_TYPE_HINDEXED_OUT:
    case MPI_TYPE_HVECTOR_OUT:
    case MPI_TYPE_INDEXED_OUT:
    case MPI_TYPE_LB_OUT:
    case MPI_TYPE_SIZE_OUT:
    case MPI_TYPE_STRUCT_OUT:
    case MPI_TYPE_UB_OUT:
    case MPI_TYPE_VECTOR_OUT:
    case MPI_UNPACK_OUT:
    case MPI_WAITALL_OUT:
    case MPI_WAITANY_OUT:
    case MPI_WAITSOME_OUT:
    case MPI_CART_COORDS_OUT:
    case MPI_CART_CREATE_OUT:
    case MPI_CART_GET_OUT:
    case MPI_CART_MAP_OUT:
    case MPI_CART_SHIFT_OUT:
    case MPI_CARTDIM_GET_OUT:
    case MPI_DIMS_CREATE_OUT:
    case MPI_GRAPH_CREATE_OUT:
    case MPI_GRAPH_GET_OUT:
    case MPI_GRAPH_MAP_OUT:
    case MPI_GRAPH_NEIGHBORS_OUT:
    case MPI_GRAPH_NEIGHBORS_COUNT_OUT:
    case MPI_GRAPHDIMS_GET_OUT:
    case MPI_TOPO_TEST_OUT:
    case MPI_RECV_IDLE_OUT:
    case MPI_CART_RANK_OUT:
    case MPI_CART_SUB_OUT:
      if (!arguments.no_states){
        poti_PopState(timestamp, mpi_process, "STATE");
      }
      break;
    case MPI_FINALIZE_OUT:
      if (!arguments.no_states){
        poti_PopState(timestamp, mpi_process, "STATE");
      }
      poti_DestroyContainer(timestamp, "PROCESS", mpi_process);
      break;
    }
  }
  if (timestamp >= 0){
    poti_DestroyContainer(timestamp, "ROOT", "root");
  }

  rst_close (&rastro);
  aky_key_free();
  return 0;
}
