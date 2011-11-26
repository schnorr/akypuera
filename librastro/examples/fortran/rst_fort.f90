      PROGRAM hellotrace
        IMPLICIT NONE
        INTEGER*4 :: a
        REAL :: b
        CHARACTER(20) :: s
        INTEGER*8 :: id1,id2


        INTEGER*1 :: int8    !c  8 bits signed int
        INTEGER*2 :: int16   !w  16 signed int
        INTEGER*4 :: int32   !i  32 signed int
        INTEGER*8 :: int64   !l 64 signed int

        REAL      :: a_float !f 32bit floating point
        REAL*8    :: a_double !d 64bit floating point
        CHARACTER(20) :: a_string !s , variable lenght string (non null
                                  !terminated on fortran)

        INTEGER*2, PARAMETER :: EVTTYPE        = 1
        INTEGER*2, PARAMETER :: EVTTYPE_INT8   = 10
        INTEGER*2, PARAMETER :: EVTTYPE_INT16  = 11
        INTEGER*2, PARAMETER :: EVTTYPE_INT32  = 12
        INTEGER*2, PARAMETER :: EVTTYPE_INT64  = 13
        INTEGER*2, PARAMETER :: EVTTYPE_FLOAT  = 14
        INTEGER*2, PARAMETER :: EVTTYPE_DOUBLE = 15
        INTEGER*2, PARAMETER :: EVTTYPE_STR    = 16

        int8      = 'a'
        int16     = 16
        int32     = 32
        int64     = 64
        a_float   = 4.4
        a_double  = 8.8
        a_string  = "string"
        
        a = 1
        b = 2.5
        s = "teste"
        id1 = 10
        id2 = 20

        WRITE (*,*) "Init"
        CALL rst_init_f(id1,id2)

        WRITE (*,*) "Calling event INT8"
        CALL rst_event_c_f(EVTTYPE_INT8,  int8)
        
        WRITE (*,*) "Calling event INT16"
        CALL rst_event_w_f(EVTTYPE_INT16,  int16)
        
        WRITE (*,*) "Calling event INT32"
        CALL rst_event_i_f(EVTTYPE_INT32,  int32)

        WRITE (*,*) "Calling event INT64"
        CALL rst_event_l_f(EVTTYPE_INT64,  int64)

        WRITE (*,*) "Calling event FLOAT"
        CALL rst_event_f_f(EVTTYPE_FLOAT,  a_float)

        WRITE (*,*) "Calling event DOUBLE"
        CALL rst_event_d_f(EVTTYPE_DOUBLE,  a_double)

        WRITE (*,*) "Calling event STR"
        CALL rst_event_s_f(EVTTYPE_STR,  a_string//CHAR(0))

        do 10 a=1,20
          b = 2000/float(2**a)
          WRITE (*,*) "event"
          CALL rst_event_ifs_f(EVTTYPE, a, b, s//CHAR(0))
    10  continue



        WRITE (*,*) "finalize"
        CALL rst_finalize_f()

        WRITE (*,*) "Get the hell outahere"
      END PROGRAM hellotrace
