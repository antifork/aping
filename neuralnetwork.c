/*
 gcc -DTRY_NN -DVERBOSE -Wall -pedantic -o neuralnetwork neuralnetwork.c -g
    to test as standalone with verbosity and debug
*/

#include "neuralnetwork.h"

/*----------------------------------------------------------------------------*/
/* name:    eth_crc                                                           */
/* purpose: an ethernet crc_32 used to check saved files                      */
/*----------------------------------------------------------------------------*/
static unsigned const poly= 0x04c11db7U;
static u_32 eth_crc(int length, char *data)
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
	NN_VERBOSE3("::alloc_layer(size=%u,psize=%u,nsize=%u)\r\n",size,psize,nsize);
	l_temp=(LAYER*)b_malloc(sizeof(LAYER));
	l_temp->size=size;
	/*alloc perceptrons*/
	for(csize=0;csize!=size;csize++)
	{
		n_temp=(NEURO*)b_malloc(sizeof(NEURO));
		if(!csize) /* linking first neuron to layer */
			l_temp->first=n_temp;
		NN_VERBOSE1(" -allocated 1 preceptron in layer #%u\r\n",size);
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
void link_layers(LAYER* layer_a, LAYER *layer_b)
{
	NEURO* neuro_a;
	NEURO* neuro_b;
	LINK* link;
	u_32 counter1,counter2;
	NN_VERBOSE("::link_layers()");

	layer_a->next    = layer_b;
	layer_b->previous= layer_a;
        NN_VERBOSE2(" -linked %p with %p",layer_a,layer_b);



	NN_VERBOSE(" +forward propagation link\r\n");
	for(neuro_a=layer_a->first,counter1=0;counter1!=layer_a->size;counter1++,neuro_a=neuro_a->next)
        //for_each(neuron) in layer_a
	{
		for(neuro_b=layer_b->first , link=neuro_a->fp_np , counter2=0 ; counter2!=layer_b->size ;counter2++, neuro_b=neuro_b->next)
                //for_each(neuron) in layer_b
		{
			link->linkedneuro=neuro_b;
			link++;
			NN_VERBOSE2("   -fwd link: linked perceptron #%2d (a) with #%2d (b)\r\n",counter1+1,counter2+1);
		}
		NN_VERBOSE1("  -fwd propagation links for perceptron #%2d linked\r\n",counter1+1);
	}
	NN_VERBOSE(" +forward propagation link done\r\n");



	NN_VERBOSE(" +backward propagation link\r\n");
	for(neuro_b=layer_b->first,counter1=0;counter1!=layer_b->size;counter1++,neuro_b=neuro_b->next)
        //for_each(neuron) in layer_b
	{
		for(neuro_a=layer_a->first , link=neuro_b->bp_np , counter2=0 ; counter2!=layer_a->size ;counter2++, neuro_a=neuro_a->next)
                //for_each(neuron) in layer_a
		{
			link->linkedneuro=neuro_a;
			link++;
			NN_VERBOSE2("   -bwd link: linked perceptron #%2d (a) with #%2d (b)\r\n",counter1+1,counter2+1);
		}
		NN_VERBOSE1("  -bwd propagation links for perceptron #%2d linked\r\n",counter1+1);
	}
	NN_VERBOSE(" -backward propagation link done\r\n");


	NN_VERBOSE("::link_layers() done\r\n");
}

/*----------------------------------------------------------------------------*/
/*name:    layer_status                                                       */
/*purpose: dumps the status of a layer, its perceptrons, its link             */
/*----------------------------------------------------------------------------*/
void layer_status(LAYER* layer)
{
	NEURO* neuro;
	LINK * link;
	u_32 lsize;

	printf("::dump_status()\r\n");

	for(neuro=layer->first;neuro;neuro=neuro->next)
	{
		printf(" -Perceptron Status----------------------\r\n");
		printf("  -Next perceptron  : 0x%p \r\n",neuro->next);
		printf("  -Perceptron state : %f   \r\n",neuro->state);
		printf(" ----------------------------------------\r\n");

		if(layer->next)
		{
			for(link=neuro->fp_np,lsize=0;lsize!=layer->next->size;lsize++)
                        /* for_each(perceptron)in the next layer */
			{
				printf("  -fp link address  : 0x%p\r\n",link);
				printf("   -linked neuro    : 0x%p\r\n",link->linkedneuro);
				printf("   -link weight     : %f  \r\n",link->weight);
				link++;
			}
			printf(" ----------------------------------------\r\n");
		}
		if(layer->previous)
		{
			for(link=neuro->bp_np,lsize=0;lsize!=layer->previous->size;lsize++)
			/* for_each(perceptron)in the previous layer */
			{
				printf("  -bp link address  : 0x%p \r\n",link);
				printf("   -bp neuro address: 0x%p \r\n",link->linkedneuro);
				printf("   -link weight     : %f   \r\n",link->weight);
				link++;
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
 	NN_VERBOSE("::free_layer(); \r\n");

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
		NN_VERBOSE1(" -one less #%2d remaining\r\n",size-1);
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
        NN_VERBOSE("create_nn();\r\n");
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
		l_tmp =l_tmp->next;
	}
        NN_VERBOSE("create_nn(); done\r\n");
	return nn;
}
/*----------------------------------------------------------------------------*/
/* name    : save_nn                                                          */
/* purpose : save neural network                                              */
/*----------------------------------------------------------------------------*/
/*
        structure of the saved file:
         crc  (u_32)

         nn->size             u_16
         nn->life             u_32
         nn->alpha          double
         nn->eta            double
         nn->gain           double
         nn->error          double

         layer->size          u_32  (found nn->size times)
         ....

         neuron->state      double 
         neuron->output     double 
         neuron->error      double 
        ...

        fplink->weight      double 
        fplink->weightsave  double 
        fplink->deltaweight double 
        ....

        bplink->weight      double 
        bplink->weightsave  double 
        bplink->deltaweight double 
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

        NN_VERBOSE ("::save_nn()");

        sizeofhash   =sizeof(u_32);
        NN_VERBOSE1(" +%u bytes for hash\r\n",sizeofhash);

	sizeofnn     =sizeof(sizeof(u_16)+sizeof(u_32)+4*sizeof(double));
        NN_VERBOSE1(" +%u bytes for nn\r\n",sizeofnn);

	sizeoflayers =nn->size*sizeof(u_32);
        NN_VERBOSE1(" +%u bytes for layers\r\n",sizeoflayers);


	for(currentlayer=nn->first , sizeofneurons=0 , counter=0;counter!=nn->size;counter++)
	{
		sizeofneurons+= currentlayer->size;
		currentlayer =  currentlayer->next;
	}
        sizeofneurons *= 3*sizeof(double);
	NN_VERBOSE1(" +%u bytes for neurons\r\n",sizeofneurons);


	for(currentlayer=nn->first , sizeoflinks=0 , counter=1;counter!=nn->size;counter++)
	{
		sizeoflinks += currentlayer->size * currentlayer->next->size;
		currentlayer =  currentlayer->next;
	}
        sizeoflinks*= 2*3*sizeof(double);
        NN_VERBOSE1(" +%u bytes for links (3*sizeof(double)*2) (back and forward propagation )\r\n",sizeoflinks);


        totalsize=sizeofhash+sizeofnn+sizeoflayers+sizeofneurons+sizeoflinks;
        NN_VERBOSE1(" +%u to be stored \r\n",totalsize);


	pnnp=pnn=(char*)b_malloc(totalsize);

        pnn+=sizeof(u_32); /* first 4 bytes for crc */
        COPY_AND_SEEK(pnn,nn->size ,sizeof(u_16));
        COPY_AND_SEEK(pnn,nn->life ,sizeof(u_32));
        COPY_AND_SEEK(pnn,nn->alpha,sizeof(double));
        COPY_AND_SEEK(pnn,nn->eta  ,sizeof(double));
        COPY_AND_SEEK(pnn,nn->gain ,sizeof(double));
        COPY_AND_SEEK(pnn,nn->error,sizeof(double));


        NN_VERBOSE(" -saving layers");
        for(currentlayer=nn->first, counter=0;counter!=nn->size;counter++)
        /* for_each(layer) */
        {
                COPY_AND_SEEK(pnn,currentlayer->size,sizeof(u_32));
                currentlayer=currentlayer->next;
        }
        NN_VERBOSE(" -layers saved");


        NN_VERBOSE(" +saving neurons\r\n");
        for(currentlayer=nn->first, counter=0;counter!=nn->size;counter++)
        /* for_each(layer) */
        {
                int counter2;
                NEURO* currentneuro;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                /* for_each(neuron) */
                {
                        COPY_AND_SEEK(pnn,currentneuro->state ,sizeof(double));
                        COPY_AND_SEEK(pnn,currentneuro->output,sizeof(double));
                        COPY_AND_SEEK(pnn,currentneuro->error ,sizeof(double));
                        currentneuro=currentneuro->next;
                }
                currentlayer=currentlayer->next;
        }
        NN_VERBOSE(" +neurons saved\r\n");

        NN_VERBOSE(" +saving forward propagation links\r\n");
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
                                COPY_AND_SEEK(pnn,currentlink->weight     ,sizeof(double));
                                COPY_AND_SEEK(pnn,currentlink->weightsave ,sizeof(double));
                                COPY_AND_SEEK(pnn,currentlink->deltaweight,sizeof(double));
                                currentlink++;
                        }
                        currentneuro=currentneuro->next;
                }
                currentlayer=currentlayer->next;
                NN_VERBOSE1(" +saved forward links for layer #%u\r\n",counter+1);
        }
        NN_VERBOSE(" +forward propagation links saved\r\n");

        NN_VERBOSE(" +saving backward propagation links\r\n");
        for(currentlayer=nn->first->next, counter=1;counter!=nn->size;counter++)
        /* for_each(layer) except first */
        {
                int counter2;
                NEURO* currentneuro;
                int prevlayersize;

                prevlayersize=currentlayer->previous->size;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                /* for_each(neuron) */
                {
                        int linkscounter;
                        LINK* currentlink;
                        for(currentlink=currentneuro->bp_np , linkscounter=0;linkscounter!=prevlayersize;linkscounter++)
                        /* for_each(neuron) */
                        {
                                COPY_AND_SEEK(pnn,currentlink->weight     ,sizeof(double));
                                COPY_AND_SEEK(pnn,currentlink->weightsave ,sizeof(double));
                                COPY_AND_SEEK(pnn,currentlink->deltaweight,sizeof(double));
                                currentlink++;
                        }
                        currentneuro=currentneuro->next;
                }
                currentlayer=currentlayer->next;
                NN_VERBOSE1(" +saved backp links for layer # %u\r\n",counter+1);
        }
        NN_VERBOSE(" +backward propagation links saved\r\n");

        /* checksum */
        pnn=pnnp+sizeof(u_32);
        NN_VERBOSE1("::eth_crc(%u,pnn)\r\n",totalsize-sizeof(u_32));
        crc= eth_crc(totalsize-sizeof(u_32), pnn);
        memcpy(pnnp,&crc,sizeof(u_32));
        NN_VERBOSE2("::eth_crc(%u,pnn) = %u \r\n",totalsize-sizeof(u_32),crc);


        NN_VERBOSE(" +one shot\r\n");
        if ((fp = fopen(filename, "wb"))== NULL)
        {
                fprintf(stderr, "Cannot open output file %s\n",filename);
                return 0;
        }
        fwrite(pnnp,totalsize,1,fp);
        fclose(fp);
        NN_VERBOSE(" +one kill\r\n");


        NN_VERBOSE("::save_nn() done\r\n");
        free(pnnp);
        return 1;
}


#ifdef TRY_NN
int main(int zz,char**zzz) /* hello world */
{
	u_32 p_in_layers[6]={256,128,64,32,8,2};
        /* 32768 + 12288 2496 312 +72 24 8 = 47968*/
	NN* nn;
	nn=create_nn(6,p_in_layers);
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
