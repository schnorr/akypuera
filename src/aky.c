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

static u_int64_t send_mark = 0;

int MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount,
                  recvtype, comm)
const void *sendbuf;
int sendcount;
MPI_Datatype sendtype;
void *recvbuf;
int recvcount;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  rst_event(MPI_ALLGATHER_IN);
  int returnVal = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf,
                                 recvcount, recvtype, comm);
  rst_event(MPI_ALLGATHER_OUT);
  return returnVal;
}

int MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts,
                   displs, recvtype, comm)
const void *sendbuf;
int sendcount;
MPI_Datatype sendtype;
void *recvbuf;
const int *recvcounts;
const int *displs;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  rst_event(MPI_ALLGATHERV_IN);
  int returnVal = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf,
                                  recvcounts, displs, recvtype, comm);
  rst_event(MPI_ALLGATHERV_OUT);
  return returnVal;
}

int MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm)
const void *sendbuf;
void *recvbuf;
int count;
MPI_Datatype datatype;
MPI_Op op;
MPI_Comm comm;
{
  rst_event(MPI_ALLREDUCE_IN);
  int returnVal =
      PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
  rst_event(MPI_ALLREDUCE_OUT);
  return returnVal;
}

int MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcnt, recvtype,
                 comm)
const void *sendbuf;
int sendcount;
MPI_Datatype sendtype;
void *recvbuf;
int recvcnt;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  rst_event(MPI_ALLTOALL_IN);
  int returnVal = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf,
                                recvcnt, recvtype, comm);
  rst_event(MPI_ALLTOALL_OUT);
  return returnVal;
}

int MPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts,
                  rdispls, recvtype, comm)
const void *sendbuf;
const int *sendcnts;
const int *sdispls;
MPI_Datatype sendtype;
void *recvbuf;
const int *recvcnts;
const int *rdispls;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  rst_event(MPI_ALLTOALLV_IN);
  int returnVal =
      PMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf,
                     recvcnts, rdispls, recvtype, comm);
  rst_event(MPI_ALLTOALLV_OUT);
  return returnVal;
}

int MPI_Barrier(comm)
MPI_Comm comm;
{
  rst_event(MPI_BARRIER_IN);
  int returnVal = PMPI_Barrier(comm);
  rst_event(MPI_BARRIER_OUT);
  return returnVal;
}

int MPI_Bcast(buffer, count, datatype, root, comm)
void *buffer;
int count;
MPI_Datatype datatype;
int root;
MPI_Comm comm;
{
  int rank;
  PMPI_Comm_rank(comm, &rank);
  if (rank == root) {
    int size;
    int tsize;
    PMPI_Type_size(datatype, &tsize);
    PMPI_Comm_size(comm, &size);
    rst_event_l(MPI_BCAST_IN, size);
    rst_event_iil(AKY_1TA_SEND, size, count * tsize, size);
  } else {
    rst_event(MPI_BCAST_IN);
  }
  int returnVal = PMPI_Bcast(buffer, count, datatype, root, comm);
  if (rank != root)
    rst_event_i(AKY_1TA_RECV, AKY_translate_rank(comm, root));
  rst_event(MPI_BCAST_OUT);
  return returnVal;
}

int MPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcount, recvtype,
               root, comm)
const void *sendbuf;
int sendcnt;
MPI_Datatype sendtype;
void *recvbuf;
int recvcount;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  int rank;
  PMPI_Comm_rank(comm, &rank);
  int size;
  PMPI_Comm_size(comm, &size);
  if (rank == root) {
    rst_event(MPI_GATHER_IN);
  } else {
    /*
     * As with MPI_Scatter the semantics are a bit different. More specifically
     * they are the inverse of MPI_Scatter, i.e. we register the destination
     * in the send and the size in the recv (for aky_converter). The send_mark
     * is registered as the size like in MPI_Scatter (for pj_compensate).
     */
    rst_event_l(MPI_GATHER_IN, size);
    rst_event_iil(AKY_NT1_SEND, AKY_translate_rank(comm, root), sendcnt, size);
  }
  int returnVal =
      PMPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcount,
                  recvtype, root, comm);
  if (rank == root)
    rst_event_i(AKY_NT1_RECV, size);
  rst_event(MPI_GATHER_OUT);
  return returnVal;
}

int MPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs,
                recvtype, root, comm)
const void *sendbuf;
int sendcnt;
MPI_Datatype sendtype;
void *recvbuf;
const int *recvcnts;
const int *displs;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  rst_event(MPI_GATHERV_IN);
  int returnVal =
      PMPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts,
                   displs, recvtype, root, comm);
  rst_event(MPI_GATHERV_OUT);
  return returnVal;
}

int MPI_Op_create(function, commute, op)
MPI_User_function *function;
int commute;
MPI_Op *op;
{
  rst_event(MPI_OP_CREATE_IN);
  int returnVal = PMPI_Op_create(function, commute, op);
  rst_event(MPI_OP_CREATE_OUT);
  return returnVal;
}

int MPI_Op_free(op)
MPI_Op *op;
{
  rst_event(MPI_OP_FREE_IN);
  int returnVal = PMPI_Op_free(op);
  rst_event(MPI_OP_FREE_OUT);
  return returnVal;
}

int MPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype, op, comm)
const void *sendbuf;
void *recvbuf;
const int *recvcnts;
MPI_Datatype datatype;
MPI_Op op;
MPI_Comm comm;
{
  rst_event(MPI_REDUCE_SCATTER_IN);
  int returnVal = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts,
                                      datatype, op, comm);
  rst_event(MPI_REDUCE_SCATTER_OUT);
  return returnVal;
}

int MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm)
const void *sendbuf;
void *recvbuf;
int count;
MPI_Datatype datatype;
MPI_Op op;
int root;
MPI_Comm comm;
{
  int rank;
  PMPI_Comm_rank(comm, &rank);
  int size;
  PMPI_Comm_size(comm, &size);
  if (rank == root) {
    rst_event(MPI_REDUCE_IN);
  } else {
    int tsize;
    PMPI_Type_size(datatype, &tsize);
    rst_event_l(MPI_REDUCE_IN, size);
    rst_event_iil(AKY_AT1_SEND, AKY_translate_rank(comm, root), count * tsize, size);
  }
  int returnVal =
      PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
  if (rank == root)
    rst_event_i(AKY_AT1_RECV, size);
  rst_event(MPI_REDUCE_OUT);
  return returnVal;
}

int MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm)
const void *sendbuf;
void *recvbuf;
int count;
MPI_Datatype datatype;
MPI_Op op;
MPI_Comm comm;
{
  rst_event(MPI_SCAN_IN);
  int returnVal = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
  rst_event(MPI_SCAN_OUT);
  return returnVal;
}

int MPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype,
                root, comm)
const void *sendbuf;
int sendcnt;
MPI_Datatype sendtype;
void *recvbuf;
int recvcnt;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  int rank;
  PMPI_Comm_rank(comm, &rank);
  if (rank == root) {
    /*
     * The semantics are a tad different from the ptp comms: We register the
     * communicator size instead of the recv rank and of the send mark. This is
     * useful for, respectively, aky_converter and pj_compensate.
     */
    int size, tsize;
    PMPI_Comm_size(comm, &size);
    PMPI_Type_size(sendtype, &tsize);
    rst_event_l(MPI_SCATTER_IN, size);
    rst_event_iil(AKY_1TN_SEND, size, sendcnt * tsize, size);
  } else {
    rst_event(MPI_SCATTER_IN);
  }
  int returnVal =
    PMPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt,
        recvtype, root, comm);
  if (rank != root)
    rst_event_i(AKY_1TN_RECV, AKY_translate_rank(comm, root));
  rst_event(MPI_SCATTER_OUT);
  return returnVal;
}

int MPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt,
                 recvtype, root, comm)
const void *sendbuf;
const int *sendcnts;
const int *displs;
MPI_Datatype sendtype;
void *recvbuf;
int recvcnt;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  rst_event(MPI_SCATTERV_IN);
  int returnVal =
      PMPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf,
                    recvcnt, recvtype, root, comm);
  rst_event(MPI_SCATTERV_OUT);
  return returnVal;
}

int MPI_Comm_delete_attr(comm, keyval)
MPI_Comm comm;
int keyval;
{
  rst_event(MPI_ATTR_DELETE_IN);
  int returnVal = PMPI_Comm_delete_attr(comm, keyval);
  rst_event(MPI_ATTR_DELETE_OUT);
  return returnVal;
}

int MPI_Comm_get_attr(comm, keyval, attr_value, flag)
MPI_Comm comm;
int keyval;
void *attr_value;
int *flag;
{
  rst_event(MPI_ATTR_GET_IN);
  int returnVal = PMPI_Comm_get_attr(comm, keyval, attr_value, flag);
  rst_event(MPI_ATTR_GET_OUT);
  return returnVal;
}

int MPI_Comm_set_attr(comm, keyval, attr_value)
MPI_Comm comm;
int keyval;
void *attr_value;
{
  rst_event(MPI_ATTR_PUT_IN);
  int returnVal = PMPI_Comm_set_attr(comm, keyval, attr_value);
  rst_event(MPI_ATTR_PUT_OUT);
  return returnVal;
}

int MPI_Comm_compare(comm1, comm2, result)
MPI_Comm comm1;
MPI_Comm comm2;
int *result;
{
  rst_event(MPI_COMM_COMPARE_IN);
  int returnVal = PMPI_Comm_compare(comm1, comm2, result);
  rst_event(MPI_COMM_COMPARE_OUT);
  return returnVal;
}

int MPI_Comm_create(comm, group, comm_out)
MPI_Comm comm;
MPI_Group group;
MPI_Comm *comm_out;
{
  rst_event(MPI_COMM_CREATE_IN);
  int returnVal = PMPI_Comm_create(comm, group, comm_out);
  rst_event(MPI_COMM_CREATE_OUT);
  return returnVal;
}

int MPI_Comm_dup(comm, comm_out)
MPI_Comm comm;
MPI_Comm *comm_out;
{
  rst_event(MPI_COMM_DUP_IN);
  int returnVal = PMPI_Comm_dup(comm, comm_out);
  rst_event(MPI_COMM_DUP_OUT);
  return returnVal;
}

int MPI_Comm_free(comm)
MPI_Comm *comm;
{
  rst_event(MPI_COMM_FREE_IN);
  int returnVal = PMPI_Comm_free(comm);
  rst_event(MPI_COMM_FREE_OUT);
  return returnVal;
}

int MPI_Comm_group(comm, group)
MPI_Comm comm;
MPI_Group *group;
{
  rst_event(MPI_COMM_GROUP_IN);
  int returnVal = PMPI_Comm_group(comm, group);
  rst_event(MPI_COMM_GROUP_OUT);
  return returnVal;
}

int MPI_Comm_rank(comm, rank)
MPI_Comm comm;
int *rank;
{
  rst_event(MPI_COMM_RANK_IN);
  int returnVal = PMPI_Comm_rank(comm, rank);
  rst_event(MPI_COMM_RANK_OUT);
  return returnVal;
}

int MPI_Comm_remote_group(comm, group)
MPI_Comm comm;
MPI_Group *group;
{
  rst_event(MPI_COMM_REMOTE_GROUP_IN);
  int returnVal = PMPI_Comm_remote_group(comm, group);
  rst_event(MPI_COMM_REMOTE_GROUP_OUT);
  return returnVal;
}

int MPI_Comm_remote_size(comm, size)
MPI_Comm comm;
int *size;
{
  rst_event(MPI_COMM_REMOTE_SIZE_IN);
  int returnVal = PMPI_Comm_remote_size(comm, size);
  rst_event(MPI_COMM_REMOTE_SIZE_OUT);
  return returnVal;
}

int MPI_Comm_size(comm, size)
MPI_Comm comm;
int *size;
{
  rst_event(MPI_COMM_SIZE_IN);
  int returnVal = PMPI_Comm_size(comm, size);
  rst_event(MPI_COMM_SIZE_OUT);
  return returnVal;
}

int MPI_Comm_split(comm, color, key, comm_out)
MPI_Comm comm;
int color;
int key;
MPI_Comm *comm_out;
{
  rst_event(MPI_COMM_SPLIT_IN);
  int returnVal = PMPI_Comm_split(comm, color, key, comm_out);
  rst_event(MPI_COMM_SPLIT_OUT);
  return returnVal;
}

int MPI_Comm_test_inter(comm, flag)
MPI_Comm comm;
int *flag;
{
  rst_event(MPI_COMM_TEST_INTER_IN);
  int returnVal = PMPI_Comm_test_inter(comm, flag);
  rst_event(MPI_COMM_TEST_INTER_OUT);
  return returnVal;
}

int MPI_Group_compare(group1, group2, result)
MPI_Group group1;
MPI_Group group2;
int *result;
{
  rst_event(MPI_GROUP_COMPARE_OUT);
  int returnVal = PMPI_Group_compare(group1, group2, result);
  rst_event(MPI_GROUP_COMPARE_OUT);
  return returnVal;
}

int MPI_Group_difference(group1, group2, group_out)
MPI_Group group1;
MPI_Group group2;
MPI_Group *group_out;
{
  rst_event(MPI_GROUP_DIFFERENCE_IN);
  int returnVal = PMPI_Group_difference(group1, group2, group_out);
  rst_event(MPI_GROUP_DIFFERENCE_OUT);
  return returnVal;
}

int MPI_Group_excl(group, n, ranks, newgroup)
MPI_Group group;
int n;
const int *ranks;
MPI_Group *newgroup;
{
  rst_event(MPI_GROUP_EXCL_IN);
  int returnVal = PMPI_Group_excl(group, n, ranks, newgroup);
  rst_event(MPI_GROUP_EXCL_OUT);
  return returnVal;
}

int MPI_Group_free(group)
MPI_Group *group;
{
  rst_event(MPI_GROUP_FREE_IN);
  int returnVal = PMPI_Group_free(group);
  rst_event(MPI_GROUP_FREE_OUT);
  return returnVal;
}

int MPI_Group_incl(group, n, ranks, group_out)
MPI_Group group;
int n;
const int *ranks;
MPI_Group *group_out;
{
  rst_event(MPI_GROUP_INCL_IN);
  int returnVal = PMPI_Group_incl(group, n, ranks, group_out);
  rst_event(MPI_GROUP_INCL_OUT);
  return returnVal;
}

int MPI_Group_intersection(group1, group2, group_out)
MPI_Group group1;
MPI_Group group2;
MPI_Group *group_out;
{
  rst_event(MPI_GROUP_INTERSECTION_IN);
  int returnVal = PMPI_Group_intersection(group1, group2, group_out);
  rst_event(MPI_GROUP_INTERSECTION_OUT);
  return returnVal;
}

int MPI_Group_rank(group, rank)
MPI_Group group;
int *rank;
{
  rst_event(MPI_GROUP_RANK_IN);
  int returnVal = PMPI_Group_rank(group, rank);
  rst_event(MPI_GROUP_RANK_OUT);
  return returnVal;
}

int MPI_Group_range_excl(group, n, ranges, newgroup)
MPI_Group group;
int n;
int ranges[][3];
MPI_Group *newgroup;
{
  rst_event(MPI_GROUP_RANGE_EXCL_IN);
  int returnVal = PMPI_Group_range_excl(group, n, ranges, newgroup);
  rst_event(MPI_GROUP_RANGE_EXCL_OUT);
  return returnVal;
}

int MPI_Group_range_incl(group, n, ranges, newgroup)
MPI_Group group;
int n;
int ranges[][3];
MPI_Group *newgroup;
{
  rst_event(MPI_GROUP_RANGE_INCL_IN);
  int returnVal = PMPI_Group_range_incl(group, n, ranges, newgroup);
  rst_event(MPI_GROUP_RANGE_INCL_OUT);
  return returnVal;
}

int MPI_Group_size(group, size)
MPI_Group group;
int *size;
{
  rst_event(MPI_GROUP_SIZE_IN);
  int returnVal = PMPI_Group_size(group, size);
  rst_event(MPI_GROUP_SIZE_OUT);
  return returnVal;
}

int MPI_Group_translate_ranks(group_a, n, ranks_a, group_b, ranks_b)
MPI_Group group_a;
int n;
const int *ranks_a;
MPI_Group group_b;
int *ranks_b;
{
  rst_event(MPI_GROUP_TRANSLATE_RANKS_IN);
  int returnVal = PMPI_Group_translate_ranks(group_a, n, ranks_a, group_b,
                                             ranks_b);
  rst_event(MPI_GROUP_TRANSLATE_RANKS_OUT);
  return returnVal;
}

int MPI_Group_union(group1, group2, group_out)
MPI_Group group1;
MPI_Group group2;
MPI_Group *group_out;
{
  rst_event(MPI_GROUP_UNION_IN);
  int returnVal = PMPI_Group_union(group1, group2, group_out);
  rst_event(MPI_GROUP_UNION_OUT);
  return returnVal;
}

int MPI_Intercomm_create(local_comm, local_leader, peer_comm,
                         remote_leader, tag, comm_out)
MPI_Comm local_comm;
int local_leader;
MPI_Comm peer_comm;
int remote_leader;
int tag;
MPI_Comm *comm_out;
{
  rst_event(MPI_INTERCOMM_CREATE_IN);
  int returnVal =
      PMPI_Intercomm_create(local_comm, local_leader, peer_comm,
                            remote_leader, tag, comm_out);
  rst_event(MPI_INTERCOMM_CREATE_OUT);
  return returnVal;
}

int MPI_Intercomm_merge(comm, high, comm_out)
MPI_Comm comm;
int high;
MPI_Comm *comm_out;
{
  rst_event(MPI_INTERCOMM_MERGE_IN);
  int returnVal = PMPI_Intercomm_merge(comm, high, comm_out);
  rst_event(MPI_INTERCOMM_MERGE_OUT);
  return returnVal;
}

int MPI_Comm_create_keyval(copy_fn, delete_fn, keyval, extra_state)
MPI_Copy_function *copy_fn;
MPI_Delete_function *delete_fn;
int *keyval;
void *extra_state;
{
  rst_event(MPI_KEYVAL_CREATE_IN);
  int returnVal =
      PMPI_Comm_create_keyval(copy_fn, delete_fn, keyval, extra_state);
  rst_event(MPI_KEYVAL_CREATE_OUT);
  return returnVal;
}

int MPI_Comm_free_keyval(keyval)
int *keyval;
{
  rst_event(MPI_KEYVAL_FREE_IN);
  int returnVal = PMPI_Comm_free_keyval(keyval);
  rst_event(MPI_KEYVAL_FREE_OUT);
  return returnVal;
}

int MPI_Abort(comm, errorcode)
MPI_Comm comm;
int errorcode;
{
  rst_event(MPI_ABORT_IN);
  int returnVal = PMPI_Abort(comm, errorcode);
  rst_event(MPI_ABORT_OUT);
  return returnVal;
}

int MPI_Error_class(errorcode, errorclass)
int errorcode;
int *errorclass;
{
  rst_event(MPI_ERROR_CLASS_IN);
  int returnVal = PMPI_Error_class(errorcode, errorclass);
  rst_event(MPI_ERROR_CLASS_OUT);
  return returnVal;
}

int MPI_Comm_create_errhandler(function, errhandler)
MPI_Handler_function *function;
MPI_Errhandler *errhandler;
{
  rst_event(MPI_ERRHANDLER_CREATE_IN);
  int returnVal = PMPI_Comm_create_errhandler(function, errhandler);
  rst_event(MPI_ERRHANDLER_CREATE_OUT);
  return returnVal;
}

int MPI_Errhandler_free(errhandler)
MPI_Errhandler *errhandler;
{
  rst_event(MPI_ERRHANDLER_FREE_IN);
  int returnVal = PMPI_Errhandler_free(errhandler);
  rst_event(MPI_ERRHANDLER_FREE_OUT);
  return returnVal;
}

int MPI_Comm_get_errhandler(comm, errhandler)
MPI_Comm comm;
MPI_Errhandler *errhandler;
{
  rst_event(MPI_ERRHANDLER_GET_IN);
  int returnVal = PMPI_Comm_get_errhandler(comm, errhandler);
  rst_event(MPI_ERRHANDLER_GET_OUT);
  return returnVal;
}

int MPI_Error_string(errorcode, string, resultlen)
int errorcode;
char *string;
int *resultlen;
{
  rst_event(MPI_ERROR_STRING_IN);
  int returnVal = PMPI_Error_string(errorcode, string, resultlen);
  rst_event(MPI_ERROR_STRING_OUT);
  return returnVal;
}

int MPI_Comm_set_errhandler(comm, errhandler)
MPI_Comm comm;
MPI_Errhandler errhandler;
{
  rst_event(MPI_ERRHANDLER_SET_IN);
  int returnVal = PMPI_Comm_set_errhandler(comm, errhandler);
  rst_event(MPI_ERRHANDLER_SET_OUT);
  return returnVal;
}

int MPI_Finalize()
{
  rst_event(MPI_FINALIZE_IN);
  int returnVal = PMPI_Finalize();
  rst_event(MPI_FINALIZE_OUT);
  rst_finalize();
  return returnVal;
}

int MPI_Get_processor_name(name, resultlen)
char *name;
int *resultlen;
{
  int returnVal = PMPI_Get_processor_name(name, resultlen);
  return returnVal;
}

int MPI_Init(argc, argv)
int *argc;
char ***argv;
{
  int returnVal = PMPI_Init(argc, argv);
  int rank;
  PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
#ifdef SMPI
  rst_init_timestamp(rank,
                     0,
                     &smpi_rastro_timestamp,
                     &smpi_rastro_resolution);
#else
  rst_init(rank, 0);
#endif
  rst_event(MPI_INIT);
  return returnVal;
}

int MPI_Initialized(flag)
int *flag;
{
  rst_event(MPI_INITIALIZED_IN);
  int returnVal = PMPI_Initialized(flag);
  rst_event(MPI_INITIALIZED_OUT);
  return returnVal;
}

int MPI_Get_address(location, address)
const void *location;
MPI_Aint *address;
{
  rst_event(MPI_ADDRESS_IN);
  int returnVal = PMPI_Get_address(location, address);
  rst_event(MPI_ADDRESS_OUT);
  return returnVal;
}

int MPI_Bsend(buf, count, datatype, dest, tag, comm)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_BSEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
  rst_event(MPI_BSEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  rst_event(MPI_BSEND_INIT_IN);
  int returnVal = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm,
                                  request);
  rst_event(MPI_BSEND_INIT_OUT);
  return returnVal;
}

int MPI_Buffer_attach(buffer, size)
void *buffer;
int size;
{
  rst_event(MPI_BUFFER_ATTACH_IN);
  int returnVal = PMPI_Buffer_attach(buffer, size);
  rst_event(MPI_BUFFER_ATTACH_OUT);
  return returnVal;
}

int MPI_Buffer_detach(buffer, size)
void *buffer;
int *size;
{
  rst_event(MPI_BUFFER_DETACH_IN);
  int returnVal = PMPI_Buffer_detach(buffer, size);
  rst_event(MPI_BUFFER_DETACH_OUT);
  return returnVal;
}

int MPI_Cancel(request)
MPI_Request *request;
{
  rst_event(MPI_CANCEL_IN);
  int returnVal = PMPI_Cancel(request);
  rst_event(MPI_CANCEL_OUT);
  return returnVal;
}

int MPI_Request_free(request)
MPI_Request *request;
{
  rst_event(MPI_REQUEST_FREE_IN);
  int returnVal = PMPI_Request_free(request);
  rst_event(MPI_REQUEST_FREE_OUT);
  return returnVal;
}

int MPI_Recv_init(buf, count, datatype, source, tag, comm, request)
void *buf;
int count;
MPI_Datatype datatype;
int source;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  rst_event(MPI_RECV_INIT_IN);
  int returnVal = PMPI_Recv_init(buf, count, datatype, source, tag, comm,
                                 request);
  rst_event(MPI_RECV_INIT_OUT);
  return returnVal;
}

int MPI_Send_init(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  rst_event(MPI_SEND_INIT_IN);
  int returnVal =
      PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
  rst_event(MPI_SEND_INIT_OUT);
  return returnVal;
}

int MPI_Get_elements(status, datatype, elements)
const MPI_Status *status;
MPI_Datatype datatype;
int *elements;
{
  rst_event(MPI_GET_ELEMENTS_IN);
  int returnVal = PMPI_Get_elements(status, datatype, elements);
  rst_event(MPI_GET_ELEMENTS_OUT);
  return returnVal;
}

int MPI_Get_count(status, datatype, count)
const MPI_Status *status;
MPI_Datatype datatype;
int *count;
{
  rst_event(MPI_GET_COUNT_IN);
  int returnVal = PMPI_Get_count(status, datatype, count);
  rst_event(MPI_GET_COUNT_OUT);
  return returnVal;
}

int MPI_Ibsend(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_IBSEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal =
      PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
  rst_event(MPI_IBSEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Iprobe(source, tag, comm, flag, status)
int source;
int tag;
MPI_Comm comm;
int *flag;
MPI_Status *status;
{
  rst_event(MPI_IPROBE_IN);
  int returnVal = PMPI_Iprobe(source, tag, comm, flag, status);
  rst_event(MPI_IPROBE_OUT);
  return returnVal;
}

int MPI_Irecv(buf, count, datatype, source, tag, comm, request)
void *buf;
int count;
MPI_Datatype datatype;
int source;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  rst_event(MPI_IRECV_IN);
  int returnVal =
      PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
  rst_event(MPI_IRECV_OUT);
  aky_insert_irecv(request);
  return returnVal;
}

int MPI_Irsend(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_IRSEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal =
      PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
  rst_event(MPI_IRSEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Isend(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_ISEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal =
      PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
  rst_event(MPI_ISEND_OUT);
  aky_insert_isend(request, send_mark);
  send_mark++;
  return returnVal;
}

int MPI_Issend(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_ISSEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal =
      PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
  rst_event(MPI_ISSEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Pack(inbuf, incount, type, outbuf, outcount, position, comm)
const void *inbuf;
int incount;
MPI_Datatype type;
void *outbuf;
int outcount;
int *position;
MPI_Comm comm;
{
  rst_event(MPI_PACK_IN);
  int returnVal =
      PMPI_Pack(inbuf, incount, type, outbuf, outcount, position,
                comm);
  rst_event(MPI_PACK_OUT);
  return returnVal;
}

int MPI_Pack_size(incount, datatype, comm, size)
int incount;
MPI_Datatype datatype;
MPI_Comm comm;
int *size;
{
  rst_event(MPI_PACK_SIZE_IN);
  int returnVal = PMPI_Pack_size(incount, datatype, comm, size);
  rst_event(MPI_PACK_SIZE_OUT);
  return returnVal;
}

int MPI_Probe(source, tag, comm, status)
int source;
int tag;
MPI_Comm comm;
MPI_Status *status;
{
  rst_event(MPI_PROBE_IN);
  int returnVal = PMPI_Probe(source, tag, comm, status);
  rst_event(MPI_PROBE_OUT);
  return returnVal;
}

int MPI_Recv(buf, count, datatype, source, tag, comm, status)
void *buf;
int count;
MPI_Datatype datatype;
int source;
int tag;
MPI_Comm comm;
MPI_Status *status;
{
  MPI_Status stat2;
  rst_event(MPI_RECV_IN);
  int returnVal =
      PMPI_Recv(buf, count, datatype, source, tag, comm, &stat2);
  rst_event_i(AKY_PTP_RECV, AKY_translate_rank(comm, stat2.MPI_SOURCE));
  rst_event(MPI_RECV_OUT);
  if (status != MPI_STATUS_IGNORE)
    *status = stat2;
  return returnVal;
}

int MPI_Rsend(buf, count, datatype, dest, tag, comm)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_RSEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
  rst_event(MPI_RSEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  rst_event(MPI_RSEND_INIT_IN);
  int returnVal = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm,
                                  request);
  rst_event(MPI_RSEND_INIT_OUT);
  return returnVal;
}

int MPI_Send(buf, count, datatype, dest, tag, comm)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_SEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal = PMPI_Send(buf, count, datatype, dest, tag, comm);
  rst_event(MPI_SEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf,
                 recvcount, recvtype, source, recvtag, comm, status)
const void *sendbuf;
int sendcount;
MPI_Datatype sendtype;
int dest;
int sendtag;
void *recvbuf;
int recvcount;
MPI_Datatype recvtype;
int source;
int recvtag;
MPI_Comm comm;
MPI_Status *status;
{
  rst_event(MPI_SENDRECV_IN);
  int returnVal =
      PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag,
                    recvbuf, recvcount, recvtype, source, recvtag,
                    comm, status);
  rst_event(MPI_SENDRECV_OUT);
  return returnVal;
}

int MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source,
                         recvtag, comm, status)
void *buf;
int count;
MPI_Datatype datatype;
int dest;
int sendtag;
int source;
int recvtag;
MPI_Comm comm;
MPI_Status *status;
{
  rst_event(MPI_SENDRECV_REPLACE_IN);
  int returnVal =
      PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag,
                            source, recvtag, comm, status);
  rst_event(MPI_SENDRECV_REPLACE_OUT);
  return returnVal;
}

int MPI_Ssend(buf, count, datatype, dest, tag, comm)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int tsize;
  PMPI_Type_size(datatype, &tsize);
  rst_event_l(MPI_SSEND_IN, send_mark);
  rst_event_iil(AKY_PTP_SEND, AKY_translate_rank(comm, dest), count * tsize, send_mark);
  int returnVal = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
  rst_event(MPI_SSEND_OUT);
  send_mark++;
  return returnVal;
}

int MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request)
const void *buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request *request;
{
  rst_event(MPI_SSEND_INIT_IN);
  int returnVal = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm,
                                  request);
  rst_event(MPI_SSEND_INIT_OUT);
  return returnVal;
}

int MPI_Start(request)
MPI_Request *request;
{
  rst_event(MPI_START_IN);
  int returnVal = PMPI_Start(request);
  rst_event(MPI_START_OUT);
  return returnVal;
}

int MPI_Startall(count, array_of_requests)
int count;
MPI_Request *array_of_requests;
{
  rst_event(MPI_STARTALL_IN);
  int returnVal = PMPI_Startall(count, array_of_requests);
  rst_event(MPI_STARTALL_OUT);
  return returnVal;
}

int MPI_Test(request, flag, status)
MPI_Request *request;
int *flag;
MPI_Status *status;
{
  rst_event(MPI_TEST_IN);
  int returnVal = PMPI_Test(request, flag, status);
  rst_event(MPI_TEST_OUT);
  return returnVal;
}

int MPI_Testall(count, array_of_requests, flag, array_of_statuses)
int count;
MPI_Request *array_of_requests;
int *flag;
MPI_Status *array_of_statuses;
{
  rst_event(MPI_TESTALL_IN);
  int returnVal = PMPI_Testall(count, array_of_requests, flag,
                               array_of_statuses);
  rst_event(MPI_TESTALL_OUT);
  return returnVal;
}

int MPI_Testany(count, array_of_requests, index, flag, status)
int count;
MPI_Request *array_of_requests;
int *index;
int *flag;
MPI_Status *status;
{
  rst_event(MPI_TESTANY_IN);
  int returnVal =
      PMPI_Testany(count, array_of_requests, index, flag, status);
  rst_event(MPI_TESTANY_OUT);
  return returnVal;
}

int MPI_Test_cancelled(status, flag)
const MPI_Status *status;
int *flag;
{
  rst_event(MPI_TEST_CANCELLED_IN);
  int returnVal = PMPI_Test_cancelled(status, flag);
  rst_event(MPI_TEST_CANCELLED_OUT);
  return returnVal;
}

int MPI_Testsome(incount, array_of_requests, outcount, array_of_indices,
                 array_of_statuses)
int incount;
MPI_Request *array_of_requests;
int *outcount;
int *array_of_indices;
MPI_Status *array_of_statuses;
{
  rst_event(MPI_TESTSOME_IN);
  int returnVal = PMPI_Testsome(incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses);
  rst_event(MPI_TESTSOME_OUT);
  return returnVal;
}

int MPI_Type_commit(datatype)
MPI_Datatype *datatype;
{
  rst_event(MPI_TYPE_COMMIT_IN);
  int returnVal = PMPI_Type_commit(datatype);
  rst_event(MPI_TYPE_COMMIT_OUT);
  return returnVal;
}

int MPI_Type_contiguous(count, old_type, newtype)
int count;
MPI_Datatype old_type;
MPI_Datatype *newtype;
{
  rst_event(MPI_TYPE_CONTIGUOUS_IN);
  int returnVal = PMPI_Type_contiguous(count, old_type, newtype);
  rst_event(MPI_TYPE_CONTIGUOUS_OUT);
  return returnVal;
}

int MPI_Type_get_extent(type, lb, extent)
MPI_Datatype type;
MPI_Aint *lb;
MPI_Aint *extent;
{
  rst_event(MPI_TYPE_EXTENT_IN);
  int returnVal = PMPI_Type_get_extent(type, lb, extent);
  rst_event(MPI_TYPE_EXTENT_OUT);
  return returnVal;
}

int MPI_Type_free(datatype)
MPI_Datatype *datatype;
{
  rst_event(MPI_TYPE_FREE_IN);
  int returnVal = PMPI_Type_free(datatype);
  rst_event(MPI_TYPE_FREE_OUT);
  return returnVal;
}

int MPI_Type_create_hindexed(count, blocklens, indices, old_type, newtype)
int count;
const int *blocklens;
const MPI_Aint *indices;
MPI_Datatype old_type;
MPI_Datatype *newtype;
{
  rst_event(MPI_TYPE_HINDEXED_IN);
  int returnVal = PMPI_Type_create_hindexed(count, blocklens, indices, old_type,
                                     newtype);
  rst_event(MPI_TYPE_HINDEXED_OUT);
  return returnVal;
}

int MPI_Type_create_hvector(count, blocklen, stride, old_type, newtype)
int count;
int blocklen;
MPI_Aint stride;
MPI_Datatype old_type;
MPI_Datatype *newtype;
{
  rst_event(MPI_TYPE_HVECTOR_IN);
  int returnVal =
      PMPI_Type_create_hvector(count, blocklen, stride, old_type, newtype);
  rst_event(MPI_TYPE_HVECTOR_OUT);
  return returnVal;
}

int MPI_Type_indexed(count, blocklens, indices, old_type, newtype)
int count;
const int *blocklens;
const int *indices;
MPI_Datatype old_type;
MPI_Datatype *newtype;
{
  rst_event(MPI_TYPE_INDEXED_IN);
  int returnVal = PMPI_Type_indexed(count, blocklens, indices, old_type,
                                    newtype);
  rst_event(MPI_TYPE_INDEXED_OUT);
  return returnVal;
}

int MPI_Type_size(datatype, size)
MPI_Datatype datatype;
int *size;
{
  rst_event(MPI_TYPE_SIZE_IN);
  int returnVal = PMPI_Type_size(datatype, size);
  rst_event(MPI_TYPE_SIZE_OUT);
  return returnVal;
}

int MPI_Type_create_struct(count, blocklens, indices, old_types, newtype)
int count;
const int *blocklens;
const MPI_Aint *indices;
const MPI_Datatype *old_types;
MPI_Datatype *newtype;
{
  rst_event(MPI_TYPE_STRUCT_IN);
  int returnVal = PMPI_Type_create_struct(count, blocklens, indices, old_types,
                                   newtype);
  rst_event(MPI_TYPE_STRUCT_OUT);
  return returnVal;
}

int MPI_Type_vector(count, blocklen, stride, old_type, newtype)
int count;
int blocklen;
int stride;
MPI_Datatype old_type;
MPI_Datatype *newtype;
{
  rst_event(MPI_TYPE_VECTOR_IN);
  int returnVal =
      PMPI_Type_vector(count, blocklen, stride, old_type, newtype);
  rst_event(MPI_TYPE_VECTOR_OUT);
  return returnVal;
}

int MPI_Unpack(inbuf, insize, position, outbuf, outcount, type, comm)
const void *inbuf;
int insize;
int *position;
void *outbuf;
int outcount;
MPI_Datatype type;
MPI_Comm comm;
{
  rst_event(MPI_UNPACK_IN);
  int returnVal =
      PMPI_Unpack(inbuf, insize, position, outbuf, outcount, type,
                  comm);
  rst_event(MPI_UNPACK_OUT);
  return returnVal;
}

int MPI_Wait(request, status)
MPI_Request *request;
MPI_Status *status;
{
  MPI_Status stat2;
  int isend = aky_check_isend(request);
  if (isend) {
    aky_remove_isend(request);
    rst_event_l(MPI_WAIT_IN, isend == -1 ? 0 : isend);
  } else {
    rst_event(MPI_WAIT_IN);
  }
  int returnVal = PMPI_Wait(request, &stat2);

  if (status != MPI_STATUS_IGNORE) {
    *status = stat2;
  }
  if (aky_check_irecv(request)) {
    rst_event_i(AKY_PTP_RECV, stat2.MPI_SOURCE);
    aky_remove_irecv(request);
  }
  rst_event(MPI_WAIT_OUT);
  return returnVal;
}

int MPI_Waitall(count, array_of_requests, array_of_statuses)
int count;
MPI_Request *array_of_requests;
MPI_Status *array_of_statuses;
{
  MPI_Status *stat2 = malloc(count * sizeof(MPI_Status));
  rst_event(MPI_WAITALL_IN);
  int returnVal = PMPI_Waitall(count, array_of_requests, stat2);
  int i;
  for (i = 0; i < count; i++) {
    if (array_of_statuses != MPI_STATUS_IGNORE) {
      array_of_statuses[i] = stat2[i];
    }
    MPI_Request *req = &array_of_requests[i];
    if (aky_check_irecv(req)) {
      rst_event_i(AKY_PTP_RECV, stat2[i].MPI_SOURCE);
      aky_remove_irecv(req);
    }
  }
  free(stat2);
  rst_event(MPI_WAITALL_OUT);
  return returnVal;
}

int MPI_Waitany(count, array_of_requests, index, status)
int count;
MPI_Request *array_of_requests;
int *index;
MPI_Status *status;
{
  MPI_Status stat2;
  rst_event(MPI_WAITANY_IN);
  int returnVal = PMPI_Waitany(count, array_of_requests, index, &stat2);
  if (status != MPI_STATUS_IGNORE) {
    *status = stat2;
  }
  MPI_Request *req = &array_of_requests[*index];
  if (aky_check_irecv(req)) {
    rst_event_i(AKY_PTP_RECV, stat2.MPI_SOURCE);
    aky_remove_irecv(req);
  }
  rst_event(MPI_WAITANY_OUT);
  return returnVal;
}

int MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices,
                 array_of_statuses)
int incount;
MPI_Request *array_of_requests;
int *outcount;
int *array_of_indices;
MPI_Status *array_of_statuses;
{
  rst_event(MPI_WAITSOME_IN);
  int returnVal = PMPI_Waitsome(incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses);
  rst_event(MPI_WAITSOME_OUT);
  return returnVal;
}

int MPI_Cart_coords(comm, rank, maxdims, coords)
MPI_Comm comm;
int rank;
int maxdims;
int *coords;
{
  rst_event(MPI_CART_COORDS_IN);
  int returnVal = PMPI_Cart_coords(comm, rank, maxdims, coords);
  rst_event(MPI_CART_COORDS_OUT);
  return returnVal;
}

int MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart)
MPI_Comm comm_old;
int ndims;
const int *dims;
const int *periods;
int reorder;
MPI_Comm *comm_cart;
{
  rst_event(MPI_CART_CREATE_IN);
  int returnVal = PMPI_Cart_create(comm_old, ndims, dims, periods, reorder,
                                   comm_cart);
  rst_event(MPI_CART_CREATE_OUT);
  return returnVal;
}

int MPI_Cart_get(comm, maxdims, dims, periods, coords)
MPI_Comm comm;
int maxdims;
int *dims;
int *periods;
int *coords;
{
  rst_event(MPI_CART_GET_IN);
  int returnVal = PMPI_Cart_get(comm, maxdims, dims, periods, coords);
  rst_event(MPI_CART_GET_OUT);
  return returnVal;
}

int MPI_Cart_map(comm_old, ndims, dims, periods, newrank)
MPI_Comm comm_old;
int ndims;
const int *dims;
const int *periods;
int *newrank;
{
  rst_event(MPI_CART_MAP_IN);
  int returnVal = PMPI_Cart_map(comm_old, ndims, dims, periods, newrank);
  rst_event(MPI_CART_MAP_OUT);
  return returnVal;
}

int MPI_Cart_rank(comm, coords, rank)
MPI_Comm comm;
const int *coords;
int *rank;
{
  rst_event(MPI_CART_RANK_IN);
  int returnVal = PMPI_Cart_rank(comm, coords, rank);
  rst_event(MPI_CART_RANK_OUT);
  return returnVal;
}

int MPI_Cart_shift(comm, direction, displ, source, dest)
MPI_Comm comm;
int direction;
int displ;
int *source;
int *dest;
{
  rst_event(MPI_CART_SHIFT_IN);
  int returnVal = PMPI_Cart_shift(comm, direction, displ, source, dest);
  rst_event(MPI_CART_SHIFT_OUT);
  return returnVal;
}

int MPI_Cart_sub(comm, remain_dims, comm_new)
MPI_Comm comm;
const int *remain_dims;
MPI_Comm *comm_new;
{
  rst_event(MPI_CART_SUB_IN);
  int returnVal = PMPI_Cart_sub(comm, remain_dims, comm_new);
  rst_event(MPI_CART_SUB_OUT);
  return returnVal;
}

int MPI_Cartdim_get(comm, ndims)
MPI_Comm comm;
int *ndims;
{
  rst_event(MPI_CARTDIM_GET_IN);
  int returnVal = PMPI_Cartdim_get(comm, ndims);
  rst_event(MPI_CARTDIM_GET_OUT);
  return returnVal;
}

int MPI_Dims_create(nnodes, ndims, dims)
int nnodes;
int ndims;
int *dims;
{
  rst_event(MPI_DIMS_CREATE_IN);
  int returnVal = PMPI_Dims_create(nnodes, ndims, dims);
  rst_event(MPI_DIMS_CREATE_OUT);
  return returnVal;
}

int MPI_Graph_create(comm_old, nnodes, index, edges, reorder, comm_graph)
MPI_Comm comm_old;
int nnodes;
const int *index;
const int *edges;
int reorder;
MPI_Comm *comm_graph;
{
  rst_event(MPI_GRAPH_CREATE_IN);
  int returnVal =
      PMPI_Graph_create(comm_old, nnodes, index, edges, reorder,
                        comm_graph);
  rst_event(MPI_GRAPH_CREATE_OUT);
  return returnVal;
}

int MPI_Graph_get(comm, maxindex, maxedges, index, edges)
MPI_Comm comm;
int maxindex;
int maxedges;
int *index;
int *edges;
{
  rst_event(MPI_GRAPH_GET_IN);
  int returnVal = PMPI_Graph_get(comm, maxindex, maxedges, index, edges);
  rst_event(MPI_GRAPH_GET_OUT);
  return returnVal;
}

int MPI_Graph_map(comm_old, nnodes, index, edges, newrank)
MPI_Comm comm_old;
int nnodes;
const int *index;
const int *edges;
int *newrank;
{
  rst_event(MPI_GRAPH_MAP_IN);
  int returnVal = PMPI_Graph_map(comm_old, nnodes, index, edges, newrank);
  rst_event(MPI_GRAPH_MAP_OUT);
  return returnVal;
}

int MPI_Graph_neighbors(comm, rank, maxneighbors, neighbors)
MPI_Comm comm;
int rank;
int maxneighbors;
int *neighbors;
{
  rst_event(MPI_GRAPH_NEIGHBORS_IN);
  int returnVal =
      PMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);
  rst_event(MPI_GRAPH_NEIGHBORS_OUT);
  return returnVal;
}

int MPI_Graph_neighbors_count(comm, rank, nneighbors)
MPI_Comm comm;
int rank;
int *nneighbors;
{
  rst_event(MPI_GRAPH_NEIGHBORS_COUNT_IN);
  int returnVal = PMPI_Graph_neighbors_count(comm, rank, nneighbors);
  rst_event(MPI_GRAPH_NEIGHBORS_COUNT_OUT);
  return returnVal;
}

int MPI_Graphdims_get(comm, nnodes, nedges)
MPI_Comm comm;
int *nnodes;
int *nedges;
{
  rst_event(MPI_GRAPHDIMS_GET_IN);
  int returnVal = PMPI_Graphdims_get(comm, nnodes, nedges);
  rst_event(MPI_GRAPHDIMS_GET_OUT);
  return returnVal;
}

int MPI_Topo_test(comm, top_type)
MPI_Comm comm;
int *top_type;
{
  rst_event(MPI_TOPO_TEST_IN);
  int returnVal = PMPI_Topo_test(comm, top_type);
  rst_event(MPI_TOPO_TEST_OUT);
  return returnVal;
}

double MPI_Wtime()
{
  rst_event(MPI_WTIME_IN);
  double returnVal = PMPI_Wtime();
  rst_event(MPI_WTIME_OUT);
  return returnVal;
}
