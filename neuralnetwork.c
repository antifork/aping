/*
 gcc -DTRY_NN -Wall -pedantic -o neuralnetwork neuralnetwork.c 
    to test as standalone
 gcc -DVERBOSE -Wall -pedantic -o neuralnetwork neuralnetwork.c 
    to log what happens
*/

#include "neuralnetwork.h"

/*----------------------------------------------------------------------------*/
/* name:    eth_crc                                                           */
/* purpose: an ethernet crc_32 used to check saved files                      */
/*----------------------------------------------------------------------------*/
static unsigned const poly= 0x04c11db7U;
static u_32 eth_crc(int length, u_8 *data)
{
	int bit,crc = -1;
	u_8 current_octet;
	while (--length >= 0)
	{
		current_octet = *data++;
		for (bit = 0; bit < 8; bit++, current_octet >>= 1)
		crc=(crc<<1)^((crc<0)^(current_octet &1)?poly:0);
	}
 	return crc;
}
/*----------------------------------------------------------------------------*/
/* name:    b_malloc                                                          */
/* purpose: wraps malloc (my mom says i have to)                              */
/*----------------------------------------------------------------------------*/
void* b_malloc(size_t size)
{
	void *a;
	a=calloc(1,size);
	if(a)
  		return a;
	fprintf(stderr,"++ERROR: Can't malloc()!!!\r\nDeath\r\n");
	exit(EXIT_FAILURE);
        return (void*)0xfaded;
}

/*----------------------------------------------------------------------------*/
/* name:    b_calloc                                                          */
/* purpose: wraps calloc                                                      */
/*----------------------------------------------------------------------------*/
void* b_calloc(size_t items, size_t size){
	void *a;
	a=calloc(items,size);
	if(a)
		return a;
	fprintf(stderr,"++ERROR: Can't calloc()!!!\r\nDeath\r\n");
 	exit(EXIT_FAILURE);
        return (void*)0xfaded;
}

/*----------------------------------------------------------------------------*/
/* name:    alloc_layer                                                       */
/* purpose: allocate a layer,its perceptrons and links                        */
/* params : size = number of perceptrons on this layer                        */
/*          nsize= number of perceptrons in next layer                        */
/*          psize= number of perceptrons in previous layer                    */
/*----------------------------------------------------------------------------*/
LAYER* alloc_layer(u_32 size, u_32 psize,u_32 nsize)
{
	LAYER *l_temp;
	NEURO *n_temp;
	NEURO *oldn_temp=0;
	u_32 csize;
	NN_VERBOSE3("::alloc_layer(size=%d,psize=%d,nsize=%d)\r\n",size,psize,nsize);
	l_temp=(LAYER*)b_malloc(sizeof(LAYER));
	l_temp->size=size;
	/*alloc perceptrons*/
	for(csize=0;csize!=size;csize++)
	{
		n_temp=(NEURO*)b_malloc(sizeof(NEURO));
		if(!csize) /* linking first neuron to layer */
			l_temp->first=n_temp;
		NN_VERBOSE1(" -allocated 1 preceptron in layer #%d\r\n",size);
		if(nsize) /* if there is a next_layer we alloc forward prop links */
			n_temp->fp_np=(LINK*)b_calloc(nsize,sizeof(LINK));
		NN_VERBOSE1("  -allocated #%2d links (fwd propagation)\r\n",nsize);
		if(psize)
			n_temp->bp_np=(LINK*)b_calloc(psize,sizeof(LINK));
		NN_VERBOSE1("  -allocated #%2d links (bwd propagation)\r\n",psize);
		if(oldn_temp)
			oldn_temp->next=n_temp;
		oldn_temp=n_temp;
	}
	NN_VERBOSE("::alloc_layer() done\r\n");
	return l_temp;
}

/*----------------------------------------------------------------------------*/
/* name: link_layers                                                          */
/* purpose: links two layers                                                  */
/*----------------------------------------------------------------------------*/
void link_layers(LAYER* a, LAYER *b)
{
	NEURO* n_tmp;
	NEURO* b_tmp;
	LINK* bck;
	u_32 size_tmp,size2_tmp;
	NN_VERBOSE("::link_layers()");
	a->next=b;
	b->previous=a;
	/* forward propagation links */
	n_tmp=a->first;
	for(size_tmp=0;size_tmp!=a->size;size_tmp++)
	{
		b_tmp=b->first;
		/* make x links where x is the size of the layer b */
		for(bck=n_tmp->fp_np,size2_tmp=0;size2_tmp!=b->size;size2_tmp++)
		{
			bck->next=b_tmp;
			bck++;
			b_tmp=b_tmp->next;
			NN_VERBOSE2("   -fwd link: linked perceptron"
				" #%2d (a) with #%2d (b) w=1\r\n",size_tmp+1,size2_tmp+1);
		}
		NN_VERBOSE1("  -forward propagation links for perceptron #%2d linked\r\n",size_tmp+1);
		n_tmp=n_tmp->next;
	}
	NN_VERBOSE(" -forward propagation link done\r\n");
	n_tmp=b->first;
	for(size_tmp=0;size_tmp!=b->size;size_tmp++)
	{
		b_tmp=a->first;
		for(bck=n_tmp->bp_np,size2_tmp=0;size2_tmp!=a->size;size2_tmp++)
		{
			bck->next=b_tmp;
			bck->weight=0;
			bck=(LINK*)(((char*)bck)+sizeof(LINK));
			b_tmp=b_tmp->next;
			NN_VERBOSE2("   -bwd link: linked perceptron"
				" #%2d (b) with #%2d \r\n",size_tmp+1,size2_tmp+1);
		}
		NN_VERBOSE1("  -backpropagation links for perceptron #%2d linked\r\n",size_tmp+1);
		n_tmp=n_tmp->next;
	}
	NN_VERBOSE(" -backpropagation link done\r\n");
	NN_VERBOSE("::link_layers() done\r\n");
}

/*----------------------------------------------------------------------------*/
/*name:    layer_status                                                       */
/*purpose: dumps the status of a layer, its perceptrons, its link             */
/*----------------------------------------------------------------------------*/
void layer_status(LAYER* layer)
{
	NEURO* a;
	LINK* b;
	u_32 lsize;
	printf("::dump_status()\r\n");
	for(a=layer->first;a;a=a->next)
	{
		printf(" -Perceptron Status----------------------\r\n");
		printf("  -Next perceptron  : 0x%08X \r\n",(unsigned int)a->next);
		printf("  -Perceptron state : %f \r\n",a->state);
		printf(" ----------------------------------------\r\n");
		/* for each perceptron in the next layer                      */
		if(layer->next)
		{
			for(b=a->fp_np,lsize=0;lsize!=layer->next->size;lsize++)
			{
				printf("  -fp link address  : 0x%08X\r\n",(unsigned int)b);
				printf("   -fp neuro address: 0x%08X\r\n",(unsigned int)b->next);
				printf("   -link weight     : %f\r\n",b->weight);
				b=(LINK*)(((char*)b)+8);
			}
			printf(" ----------------------------------------\r\n");
		}
		if(layer->previous)
		{
			for(b=a->bp_np,lsize=0;lsize!=layer->previous->size;lsize++)
			/* for_each perceptron in the previous layer */
			{
				printf("  -bp link address  : 0x%08X \r\n",(unsigned int)b);
				printf("   -bp neuro address: 0x%08X \r\n",(unsigned int)b->next);
				printf("   -link weight     : %f \r\n",b->weight);
				b=(LINK*)(((char*)b)+8);
			}
			printf(" ----------------------------------------\r\n");
		}
	}
	printf("::dump_status() done\r\n");
}

/*----------------------------------------------------------------------------*/
/*name   : free_layer                                                         */
/*purpose: free memory used by a layer                                        */
/*----------------------------------------------------------------------------*/
void free_layer(LAYER* lay_a)
{
	NEURO *neuro,*neuro2;
	for(neuro=lay_a->first,neuro2=lay_a->first;neuro;neuro=neuro2)
	{
		neuro2=neuro->next;
		if(lay_a->next)
			free(neuro->fp_np);
		if(lay_a->previous)
			free(neuro->bp_np);
		free(neuro);
		NN_VERBOSE("::free_perceptron() done\r\n");
	}
	free(lay_a);
 	NN_VERBOSE("::free_layer() done\r\n");
}

/*----------------------------------------------------------------------------*/
/* name    : free_nn                                                          */
/* purpose : free memory used by all layers                                   */
/*----------------------------------------------------------------------------*/
void free_nn(NN* nn)
{
	LAYER *layer=0;
	LAYER *n_layer=0;
	u_32 size;
	NN_VERBOSE("::free_nn()\r\n");
	for(layer=nn->first,size=nn->size;size;size--)
	{
		n_layer=layer->next;
		free_layer(layer);
		layer=n_layer;
		NN_VERBOSE1(" -snuff snuff one less #%2d remaining\r\n",size-1);
	}
	free(nn);
	NN_VERBOSE("::free_nn() done\r\n");
}

/*----------------------------------------------------------------------------*/
/* name    : create_nn                                                        */
/* purpose : neural network constructor                                       */
/* params  : size= number of layers                                           */
/*           *pn= array containing number of perceptrons for layers           */
/*----------------------------------------------------------------------------*/
NN* create_nn(u_32 size,u_32*pn)
{
	LAYER* l_tmp;
	LAYER* l_prev;
	NN* nn;
	if(size<2)
	{
		fprintf(stderr,"Ok, Nice joke\r\n");
		exit(EXIT_FAILURE);
	}
	nn=(NN*)b_malloc(sizeof(NN));
	nn->size=size;
	for(l_tmp= nn->first , size=0;size!=nn->size;size++)
	{			
		l_tmp=alloc_layer(pn[size],(size)?pn[size-1]:0,(size!=nn->size-1)?pn[size+1]:0);	
		if(!size)
			nn->first=l_tmp;
		else
			l_prev->next=l_tmp;
		l_prev=l_tmp;
	}
	NN_VERBOSE("starting to link\r\n");
	for( l_prev=nn->first,l_tmp=nn->first->next , size=0 ; size!=nn->size-1 ; ++size)
	{
		link_layers(l_prev,l_tmp);
		l_prev=l_prev->next;
		l_tmp=l_tmp->next;		
	}
	return nn;
}
/*----------------------------------------------------------------------------*/
/* name    : save_nn                                                          */
/* purpose : save neural network                                              */
/*----------------------------------------------------------------------------*/
/*
        structure of the saved file:
         crc  (u_32)

         nn->size  u_16
         nn->life  u_32
         nn->alpha double
         nn->eta   double
         nn->gain  double
         nn->error double

         layer->size (u_32)  (found nn->size times)
         ....

         neuron->state
         neuron->output
         neuron->error
        ...

        fplink->weight
        fplink->weightsave
        fplink->deltaweight
        ....

        bplink->weight
        bplink->weightsave
        bplink->deltaweight
        ....

*/

int save_nn(NN*nn,char*filename)
{
	char* pnnp,*pnn;
        u_32 sizeofhash;
	u_32 sizeofnn;
	u_32 sizeoflayers;
	u_32 sizeofneurons;
	u_32 sizeoflinks;
        u_32 totalsize;
	u_32 counter;
        u_32 crc;
        FILE*fp;
	LAYER* currentlayer;

        NN_VERBOSE("::save_nn()");
        sizeofhash   =sizeof(u_32);
        NN_VERBOSE1(" -%d bytes for hash\r\n",sizeofhash);
	sizeofnn     =sizeof(sizeof(u_16)+sizeof(u_32)+4*sizeof(double));
        NN_VERBOSE1(" -%d bytes for nn\r\n",sizeofnn);
	sizeoflayers =nn->size*sizeof(u_32);
        NN_VERBOSE1(" -%d bytes for layers\r\n",sizeoflayers);
	for(currentlayer=nn->first , sizeofneurons=0 , counter=0;counter!=nn->size;counter++)
	{
		sizeofneurons+= currentlayer->size;
		currentlayer =  currentlayer->next;
	}
        sizeofneurons *= 3*sizeof(double);
	NN_VERBOSE1(" -%d bytes for neurons\r\n",sizeofneurons);

	for(currentlayer=nn->first , sizeoflinks=0 , counter=1;counter!=nn->size;counter++)
	{
		sizeoflinks += currentlayer->size * currentlayer->next->size;
		currentlayer =  currentlayer->next;
	}
        sizeoflinks*= 2*3*sizeof(double);
        NN_VERBOSE1(" -%d bytes for links (3*sizeof(double)*2) (back and forward propagation )\r\n",sizeoflinks);
        totalsize=sizeofhash+sizeofnn+sizeoflayers+sizeofneurons+sizeoflinks;
        NN_VERBOSE1(" -%d to be stored \r\n",totalsize);
	pnnp=pnn=(char*)b_malloc(totalsize);

        /* saving nn */
        pnn+=sizeof(u_32);

        memcpy(pnn,&(nn->size),sizeof(u_16));
        pnn+=sizeof(u_16);

        memcpy(pnn,&(nn->life),sizeof(u_32));
        pnn+=sizeof(u_32);

        memcpy(pnn,&(nn->alpha),sizeof(double));
        pnn+=sizeof(double);

        memcpy(pnn,&(nn->eta),sizeof(double));
        pnn+=sizeof(double);

        memcpy(pnn,&(nn->gain),sizeof(double));
        pnn+=sizeof(double);

        memcpy(pnn,&(nn->error),sizeof(double));
        pnn+=sizeof(double);
        /* nn saved*/

        /* saving layer */
        for(currentlayer=nn->first, counter=0;counter!=nn->size;counter++)
        /* for_each(layer) */
        {
                memcpy(pnn,&(currentlayer->size),sizeof(u_32));
                pnn+=sizeof(u_32);
                currentlayer=currentlayer->next;
        }
        NN_VERBOSE(" -layers saved");
        /* layer saved */

        /* saving neurons */
        for(currentlayer=nn->first, counter=0;counter!=nn->size;counter++)
        /* for_each(layer) */
        {
                int counter2;
                NEURO* currentneuro;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                /* for_each(neuron) */
                {
                        memcpy(pnn,&(currentneuro->state),sizeof(double));
                        pnn+=sizeof(double);
                        memcpy(pnn,&(currentneuro->output),sizeof(double));
                        pnn+=sizeof(double);
                        memcpy(pnn,&(currentneuro->error),sizeof(double));
                        pnn+=sizeof(double);
                        currentneuro=currentneuro->next;
                }
                currentlayer=currentlayer->next;
                NN_VERBOSE1(" +saved layer # %d\r\n",counter+1);
        }
        /*neurons saved */

        /*forward propagation*/
        for(currentlayer=nn->first, counter=0;counter!=nn->size-1;counter++)
        /* for_each(layer) except last */
        {
                int counter2;
                NEURO* currentneuro;
                int nextlayersize;

                nextlayersize=currentlayer->next->size;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                /* for_each(neuron) */
                {
                        int linkscounter;
                        LINK* currentlink;
                        for(currentlink=currentneuro->fp_np, linkscounter=0;linkscounter!=nextlayersize;linkscounter++)
                        /* for_each(neuron) */
                        {
                                memcpy(pnn,&(currentlink->weight     ),sizeof(double));
                                pnn+=sizeof(double);
                                memcpy(pnn,&(currentlink->weightsave ),sizeof(double));
                                pnn+=sizeof(double);
                                memcpy(pnn,&(currentlink->deltaweight),sizeof(double));
                                pnn+=sizeof(double);

                                currentlink++;
                        }
                        currentneuro=currentneuro->next;
                }
                currentlayer=currentlayer->next;
                NN_VERBOSE1(" +saved forward links for layer #%d\r\n",counter+1);
        }
        /*forward propagation*/

        /*backpropagation*/
        NN_VERBOSE1("for_each(layer)\r\n (%u layers)",nn->size);
        for(currentlayer=nn->first->next, counter=1;counter!=nn->size;counter++)
        // for_each(layer) except first
        {
                int counter2;
                NEURO* currentneuro;
                int prevlayersize;

                prevlayersize=currentlayer->previous->size;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                // for_each(neuron)
                {
                        int linkscounter;
                        LINK* currentlink;
                        for(currentlink=currentneuro->bp_np , linkscounter=0;linkscounter!=prevlayersize;linkscounter++)
                        //for_each(neuron)
                        {
                                memcpy(pnn,&(currentlink->weight     ),sizeof(double));
                                pnn+=sizeof(double);
                                memcpy(pnn,&(currentlink->weightsave ),sizeof(double));
                                pnn+=sizeof(double);
                                memcpy(pnn,&(currentlink->deltaweight),sizeof(double));
                                pnn+=sizeof(double);

                                currentlink++;
                        }
                        currentneuro=currentneuro->next;
                }
                currentlayer=currentlayer->next;
                NN_VERBOSE1(" +saved backp links for layer # %d\r\n",counter+1);
        }
        /*backpropagation*/
        /* checksum */
        pnn=pnnp+sizeof(u_32);
        NN_VERBOSE1("::eth_crc(%u,pnn)\r\n",totalsize-sizeof(u_32));
        crc= eth_crc(totalsize-sizeof(u_32), pnn);
        NN_VERBOSE1("::eth_crc(%u,pnn)\r\n",totalsize-sizeof(u_32));        
        memcpy(pnnp,&crc,sizeof(u_32));
        NN_VERBOSE(" *ONE SHOT\r\n");		
        if ((fp = fopen(filename, "wb"))== NULL)
        {
                fprintf(stderr, "Cannot open output file %s\n",filename);
                return 0;
        }
        fwrite(pnnp,totalsize,1,fp);
        fclose(fp);
        NN_VERBOSE(" *ONE KILL\r\n");		        
        NN_VERBOSE("::save_nn() done\r\n");
        return 1;
}


#ifdef TRY_NN
int main() /* hello world */
{
	u_32 p_in_layers[4]={12,6,4,2};
        /* 32768 + 12288 2496 312 +72 24 8 = 47968*/
	NN* nn;
	nn=create_nn(4,p_in_layers);
	/*
	double* fire_nn(int nofinputs, double* input);
	backpropagate_nn(int neuronsinlastlayer,double* expected);
	*/
	save_nn(nn,"saved_mind.nn");
	layer_status(nn->first);
	free_nn(nn);
	return 0;
}
#endif
