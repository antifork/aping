#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifdef VERBOSE
 #define NN_VERBOSE(a) \
 	printf(a);
 #define NN_VERBOSE1(a,b) \
 	printf(a,b);
 #define NN_VERBOSE2(a,b,c) \
 	printf(a,b,c);
 #define NN_VERBOSE3(a,b,c,d) \
 	printf(a,b,c,d);
#else 
 #define NN_VERBOSE(a) ;
 #define NN_VERBOSE1(a,b) ;
 #define NN_VERBOSE2(a,b,c) ;
 #define NN_VERBOSE3(a,b,c,d) ;
#endif

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
 struct NEURO* next;         /* link to a neuron in the next or prevoius layer*/
 double        weight;       /* link weight                                   */
 double        weightsave;   /* saved weights for no training                 */
 double        deltaweight;  /* last weight deltas for momentum               */
}LINK;

typedef struct LAYER
{
	struct NEURO* first;        /* first perceptron in this layer    */
	struct LAYER* next;         /* next layer                        */
	struct LAYER* previous;     /* previous layer                    */
	u_32   	      size;         /* fear 4 bytes for each layer? pfui */
}LAYER;

typedef struct NN
{
	struct LAYER* first;
	u_16   size;          /* size of the net (in layers)         */
	u_32   life;          /* life of the net                     */   
	double alpha;         /* momentum factor                     */
        double eta;           /* learning rate                       */
        double gain;          /* gain of sigmoid function            */
        double error;         /* error sum of the net                */
	
}NN;


static u_32 eth_crc(int length, u_8 *data);
void* b_malloc( size_t size);
void* b_calloc(size_t items, size_t size);
LAYER* alloc_layer(u_32 size, u_32 psize,u_32 nsize);
void link_layers(LAYER* a, LAYER *b);
void layer_status(LAYER* layer);
void free_layer(LAYER* lay_a);
void free_nn(NN* nn);
NN* create_nn(u_32 size,u_32*pn);
int save_nn(NN*nn,char*filename);
#endif /* NEURAL_NETWORK_H */

