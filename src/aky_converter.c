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
#include "aky_private.h"

int main(int argc, char **argv)
{
  if (argc == 1) {
    printf("%s {rastro-0-0.rst rastro-1-0.rst ...}\n", argv[0]);
    return 1;
  }

  hcreate(1000000);

  rst_file_t data;
  rst_event_t event;
  int i;

  for (i = 1; i < argc; i++) {
    int ret = rst_open_file(argv[i], &data, NULL, 100000);
    if (ret == -1) {
      printf("%s: trace %s could not be opened\n", argv[0], argv[i]);
      return 1;
    }
  }

  name_init();
  paje_header();
  paje_hierarchy();

  while (rst_decode_event(&data, &event)) {
    char mpi_process[100];
    char value[100];
    snprintf(mpi_process, 100, "rank%ld", event.id1);
    snprintf(value, 100, "%s", name_get(event.type));
    double timestamp = (double) event.timestamp / 1000000;
    switch (event.type) {
    case AKY_PTP_SEND:
      {
        char key[AKY_DEFAULT_STR_SIZE];
        aky_put_key("n", event.id1, event.v_uint32[0], key,
                    AKY_DEFAULT_STR_SIZE);
        pajeStartLink(timestamp, "0", "LINK", mpi_process, "PTP", key);
      }
      break;
    case AKY_PTP_RECV:
      {
        char key[AKY_DEFAULT_STR_SIZE];
        aky_get_key("n", event.v_uint32[0], event.id1, key,
                    AKY_DEFAULT_STR_SIZE);
        pajeEndLink(timestamp, "0", "LINK", mpi_process, "PTP", key);
      }
      break;
    case MPI_INIT:
      pajeCreateContainer(timestamp, mpi_process,
                          "PROCESS", "0", mpi_process);
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
      pajePushState(timestamp, mpi_process, "STATE", value);
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
      pajePopState(timestamp, mpi_process, "STATE");
      break;
    case MPI_FINALIZE_OUT:
      pajePopState(timestamp, mpi_process, "STATE");
      pajeDestroyContainer(timestamp, "PROCESS", mpi_process);
      break;
    }
  }

  rst_close_file(&data);
  hdestroy();
  return 0;
}
