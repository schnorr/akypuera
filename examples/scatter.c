#include <mpi.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCATTER_TAG 42

static void datatype_copy(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype) {
  int count, sendsize, recvsize;

  if(recvcount > 0) {
    MPI_Type_size(sendtype, &sendsize);
    MPI_Type_size(recvtype, &recvsize);
    sendcount *= sendsize;
    recvcount *= recvsize;
    count = sendcount < recvcount ? sendcount : recvcount;
    memcpy(recvbuf, sendbuf, count);
  }
}

static void binomial_tree_scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
   int rank, size, relrank, bytes, count, subcount, mask, src, dst, next;
   int sendsize, recvsize;
   int tmpsize = 0;
   void* tmpbuf = NULL;
   MPI_Status status;
   MPI_Request* requests;

   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   if ((rank == root && sendcount == 0) || (rank != root && recvcount == 0)) {
      return;
   }
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Type_size(sendtype, &sendsize);
   MPI_Type_size(recvtype, &recvsize);
   relrank = rank >= root ? rank - root : rank - root + size;
   if (rank == root) {
      bytes = sendsize * sendcount;
   } else {
      bytes = recvsize * recvcount;
   }
   count = 0;
   if (relrank && !(relrank % 2)) {
      tmpsize = (bytes * size) / 2;
      tmpbuf = calloc(tmpsize, sizeof(char));
   }
   if (rank == root) {
      if (root != 0) {
         tmpsize = bytes * size;
         tmpbuf = calloc(tmpsize, sizeof(char));
         datatype_copy((char*)sendbuf + sendsize * sendcount * rank, sendcount * (size - rank), sendtype, tmpbuf, bytes * (size - rank), MPI_BYTE);
         datatype_copy(sendbuf, sendcount * rank, sendtype, (char*)tmpbuf + bytes * (size - rank), bytes * rank, MPI_BYTE);
         count = bytes * size;
      } else {
         count = sendcount * size;
      }
   }
   mask = 0x1;
   while (mask < size) {
      if (relrank & mask) {
         src = rank >= mask ? rank - mask : rank - mask + size;
         if (relrank % 2) {
            MPI_Recv(recvbuf, recvcount, recvtype, src, SCATTER_TAG, comm, &status);
         } else {
            MPI_Recv(tmpbuf, tmpsize, MPI_BYTE, src, SCATTER_TAG, comm, &status);
            MPI_Get_count(&status, MPI_BYTE, &count);
         }
         break;
      }
      mask <<= 1;
   }
   next = (int)ceil(log(size) / log(2));
   requests = (MPI_Request*)calloc(next, sizeof(MPI_Request));
   next = 0;
   mask >>= 1;
   while (mask > 0) {
      if (relrank + mask < size) {
         dst = (rank + mask) % size;
         if (rank == root && root == 0) {
            subcount = count - sendcount * mask;
            MPI_Isend((char*)sendbuf + sendsize * sendcount * mask, subcount, sendtype, dst, SCATTER_TAG, comm, &requests[next]);
            next++;
         } else {
            subcount = count - bytes * mask;
            MPI_Isend((char*)tmpbuf + bytes * mask, subcount, MPI_BYTE, dst, SCATTER_TAG, comm, &requests[next]);
            next++;
         }
         count -= subcount;
      }
      mask >>= 1;
   }
   MPI_Waitall(next, requests, MPI_STATUS_IGNORE);
   free(requests);
   if (rank == root && root == 0) {
      datatype_copy(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype);
   } else if (!(relrank % 2)) {
      datatype_copy(tmpbuf, bytes, MPI_BYTE, recvbuf, recvcount, recvtype);
   }
   if (tmpbuf) {
      free(tmpbuf);
   }
}

int main(int argc, char** argv) {
   int rank, size, msgsize;
   char* send;
   char* recv;
   double start, stop;

   MPI_Init(&argc, &argv);

   if(argc != 2) {
      fprintf(stderr, "Usage: %s msgsize\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   msgsize = atoi(argv[1]);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   if (rank == 0) {
      send = (char*)calloc(size, msgsize * sizeof(char));
      recv = (char*)calloc(1, msgsize * sizeof(char));
   } else {
      send = NULL;
      recv = (char*)calloc(1, msgsize * sizeof(char));
   }
   start = MPI_Wtime();
   binomial_tree_scatter(send, msgsize, MPI_CHAR, recv, msgsize, MPI_CHAR, 0, MPI_COMM_WORLD);
   stop = MPI_Wtime();
   fprintf(stdout, "%f for rank %d\n", stop - start, rank);
   free(recv);
   free(send);
   MPI_Finalize();
   return 0;
}
