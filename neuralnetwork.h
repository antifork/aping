#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef VERBOSE
 #define NN_VERBOSE(a) \
 	printf(a);
 #define NN_VERBOSE1(a,b) \
 	printf(a,b);
 #define NN_VERBOSE2(a,b,c) \
 	printf(a,b,c);
 #define NN_VERBOSE3(a,b,c,d) \
 	printf(a,b,c,d);
 #define NN_VERBOSE4(a,b,c,d,e) \
 	printf(a,b,c,d,e);
 	
#else 
 #define NN_VERBOSE(a) ;
 #define NN_VERBOSE1(a,b) ;
 #define NN_VERBOSE2(a,b,c) ;
 #define NN_VERBOSE3(a,b,c,d) ;
 #define NN_VERBOSE4(a,b,c,d,e) ; 
#endif

#define SET_ALPHA(nn,val) nn->alpha=val
#define SET_GAIN(nn,val)  nn->gain=val
#define SET_ETA(nn,val)   nn->eta=val

#define COPY_AND_SEEK(dest,src,size) \
		do{ \
                        memcpy(dest,&(src),size); \
                        dest+=size; \
		}while(0)

#define RESTORE_AND_SEEK(dest,src,size) \
		do{ \
                        memcpy(&(dest),src,size); \
                        src+=size; \
		}while(0)


typedef unsigned int   u_32;
typedef unsigned short u_16;
typedef unsigned char  u_8;

struct LINK;

typedef struct NEURO
{
	struct LINK*  fp_np;        /* forward propagation link pointer  */
	struct LINK*  bp_np;        /* backpropagation link pointer      */
	struct NEURO* next;         /* next neuron in the same layer     */
	double        state;        /* hic! neuron state                 */
	double        output;
	double        error;         
}NEURO;

typedef struct LINK
{
 struct NEURO* linkedneuro;  /* link to a neuron in the next or prevoius layer*/
 double        weight;       /* link weight                                   */
 double        weightsave;   /* saved weights for no training                 */
 double        deltaweight;  /* last weight deltas for momentum               */
}LINK;

typedef struct LAYER
{
	struct NEURO* first;        /* first perceptron in this layer    */
	struct LAYER* next;         /* next layer                        */
	struct LAYER* previous;     /* previous layer                    */
	u_32   	      size;         /* size of layer in neurons          */
}LAYER;

typedef struct NN
{
	struct LAYER* first;
	struct LAYER* last ;
	u_16   size;          /* size of the net (in layers)         */
	u_32   life;          /* life of the net                     */   
	double alpha;         /* momentum factor                     */
        double eta;           /* learning rate                       */
        double gain;          /* gain of sigmoid function            */
        double error;         /* error sum of the net                */
	
}NN;


typedef void(FEEDCALLBACK)(NN*,double*,double*,double*);

static u_32 eth_crc( int , char* );
void*       b_malloc( size_t );
void*       b_calloc( size_t , size_t );
LAYER*      alloc_layer( u_32 , u_32 , u_32 );
void        link_layers( LAYER* , LAYER *);
void        status_nn( NN*, FILE*);
void        free_layer( LAYER* );
void        free_nn( NN* );
NN*         create_nn( u_32 ,u_32* );
int         save_nn( NN* ,char* );
void     fire_nn(NN*, double* , double* , double*);
int         feed_from_file( NN* ,char*  ,  FEEDCALLBACK* );
#endif /* NEURAL_NETWORK_H */
