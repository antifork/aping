/* Maturity level symbols:

   _ : not implemented
   i : incomplete
   a : alpha   module
   b : beta    module
   * : working module 
*/

#define IF_LOADER(x)  if ( icmp_loader_vector[x] != NULL )
#define IF_DISSECT(x) if ( icmp_dissect_vector[x] != NULL )


#define SETUP_MATURITY() 						\
{									\
int _i;									\
for (_i=0;_i<64;_i++)							\
  {									\
   maturity_level[_i][0]=' ';						\
   maturity_level[_i][1]=' ';						\
  } 									\
}

#define LOAD_MATURITY()                         				\
{                                               				\
int _i;                                         				\
for (_i=0;_i<64;_i++)                          				\
  {                                             				\
   IF_LOADER(_i) {(*icmp_loader_vector [_i]) ((packet *)maturity_void, NULL);} 	\
   IF_DISSECT(_i){(*icmp_dissect_vector[_i]) ((packet *)maturity_void);} 	\
  }                                             				\
}

#define SET_LOADER_LEVEL(c)					\
{								\
 if ( p == (packet *)maturity_void )  				\
    {								\
      maturity_level[_dissect_type][0]=(c);			\
      return;							\
    }								\
}

#define SET_DISSECT_LEVEL(c)					\
{                                               		\
 if ( p == (packet *)maturity_void )           			\
   {                                         			\
     maturity_level[_dissect_type][1]=(c);			\
     return;                                 			\
   }                                         			\
}                           

