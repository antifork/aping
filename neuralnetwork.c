/*
gcc -DTRY_NN -DVERBOSE -Wall -pedantic -o neuralnetwork neuralnetwork.c -g3 -lm
to test as standalone with verbosity and debug

manca parte della backpropagation

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
	NN_VERBOSE("::link_layers() \r\n");

	layer_a->next    = layer_b;
	layer_b->previous= layer_a;
        NN_VERBOSE2(" -linked %p with %p\r\n",layer_a,layer_b);



	NN_VERBOSE(" +forward propagation link\r\n");
	for(neuro_a=layer_a->first,counter1=0;counter1!=layer_a->size;counter1++,neuro_a=neuro_a->next)
        /*for_each(neuron) in layer_a */
	{
		for(neuro_b=layer_b->first , link=neuro_a->fp_np , counter2=0 ; counter2!=layer_b->size ;counter2++, neuro_b=neuro_b->next)
	        /* for_each(neuron) in layer_b */
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
        /*for_each(neuron) in layer_b */
	{
		for(neuro_a=layer_a->first , link=neuro_b->bp_np , counter2=0 ; counter2!=layer_a->size ;counter2++, neuro_a=neuro_a->next)
                /* for_each(neuron) in layer_a */
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
void status_nn(NN* nn,FILE*fp)
{
	NEURO* p_neuro;
	LAYER* p_layer;
	LINK * p_link;
	u_32 counter,counter2,counter3;
	NN_VERBOSE("::dump_status()\r\n");

	fprintf(fp,"<?xml version=\"1.0\"?>\n<nn size=\"%u\" life=\"%u\" alpha=\"%f\" gain=\"%f\" error=\"%f\">\n",nn->size,nn->life,nn->alpha,nn->gain,nn->error);
	for(p_layer=nn->first,counter=0;counter!=nn->size;counter++,p_layer=p_layer->next)
	/* for_each(layer) */
	{
		u_32 nextlayersize=(p_layer->next)?p_layer->next->size:0;
		fprintf(fp,"\t<layer size=\"%u\">\n",p_layer->size);
		for(p_neuro=p_layer->first,counter2=0;counter2!=p_layer->size;counter2++,p_neuro=p_neuro->next)
		/* for_each(neuron) */
		{
			fprintf(fp,"\t\t<neuron state=\"%f\" output=\"%f\" error=\"%f\" >\n",p_neuro->state,p_neuro->output,p_neuro->error);
			for(p_link=p_neuro->fp_np,counter3=0;counter3!=nextlayersize;p_link++,counter3++)
			/* for_each(link)*/
			{
				fprintf(fp,"\t\t\t<link weight=\"%f\" weightsave=\"%f\" deltaweight=\"%f\" />\n",p_link->weight,p_link->weightsave,p_link->deltaweight);
			}
			fprintf(fp,"\t\t</neuron>\n");
		}
		fprintf(fp,"\t</layer>\n");
	}
		
	fprintf(fp,"</nn>\n");
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

	for(nn->size=size, l_tmp= nn->first , size=0;size!=nn->size;size++)
	{
		NN_VERBOSE3("alloc_layer(%u,%u,%u);\r\n",pn[size],(size)?pn[size-1]:0,(size!=nn->size-1)?pn[size+1]:0);
		l_tmp=alloc_layer(pn[size],(size)?pn[size-1]:0,(size!=nn->size-1)?pn[size+1]:0);
		if(!size)
			nn->first=l_tmp;
		else
			l_prev->next=l_tmp;
		l_prev=l_tmp;
	}
	nn->last=l_tmp;

	NN_VERBOSE("starting to link\r\n");
	for( l_prev=nn->first,l_tmp=nn->first->next , size=0 ; size!=nn->size-1 ; ++size)
	/*for_each(layer) except last*/
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

        NN_VERBOSE ("::save_nn()\r\n");

        sizeofhash   =sizeof(u_32);
        NN_VERBOSE1(" +%u bytes for hash\r\n",sizeofhash);

	sizeofnn     =sizeof(u_16)+sizeof(u_32)+4*sizeof(double);
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
        NN_VERBOSE1("nn->size=%u\r\n",nn->size);

        COPY_AND_SEEK(pnn,nn->life ,sizeof(u_32));
        NN_VERBOSE1("nn->life=%u\r\n",nn->life);        
        
        COPY_AND_SEEK(pnn,nn->alpha,sizeof(double));
	NN_VERBOSE1("nn->alpha=%f\r\n",nn->alpha);
        
        COPY_AND_SEEK(pnn,nn->eta  ,sizeof(double));
	NN_VERBOSE1("nn->eta=%f\r\n",nn->eta);
        
        COPY_AND_SEEK(pnn,nn->gain ,sizeof(double));
	NN_VERBOSE1("nn->gain=%f\r\n",nn->gain);

        COPY_AND_SEEK(pnn,nn->error,sizeof(double));
        NN_VERBOSE1("nn->error=%f\r\n",nn->error);

        NN_VERBOSE(" -saving layers\r\n");
        for(currentlayer=nn->first, counter=0;counter!=nn->size;counter++)
        /* for_each(layer) */
        {
                COPY_AND_SEEK(pnn,currentlayer->size ,sizeof(u_32));
                currentlayer=currentlayer->next;
        }
        NN_VERBOSE(" -layers saved\r\n");


        NN_VERBOSE(" +saving neurons\r\n");
        for(currentlayer=nn->first, counter=0;counter!=nn->size;counter++)
        /* for_each(layer) */
        {
                u_32 counter2;
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
                u_32 counter2,nextlayersize;
                NEURO* currentneuro;

                nextlayersize=currentlayer->next->size;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                /* for_each(neuron) */
                {
                        u_32 linkscounter;
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
                int counter2,prevlayersize;
                NEURO* currentneuro;

                prevlayersize=currentlayer->previous->size;
                for(currentneuro=currentlayer->first,counter2=0;counter2!=currentlayer->size;counter2++)
                /* for_each(neuron) */
                {
                        u_32 linkscounter;
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


NN* load_nn(char* filename)
{
	struct stat file_stat;
	FILE  * fp;
	NN    * nn;
	LAYER * layer;
	NEURO * neuro;
        LINK  * link;
	u_16    pivot16;
	u_32  * p_in_layers;	
	u_32 pivot32 , counter , counter2 , linkscounter , smartpeoplecheck;
	char* fnnbuffer,*buffer;


	NN_VERBOSE("load_nn();\r\n");

	if(stat(filename,&file_stat)!=0)
	{
                fprintf(stderr, "error in stat(\"%s\",&file_stat);\r\n",filename);
		exit(EXIT_FAILURE);
	}
	printf("file size= %u\r\n",(unsigned int)file_stat.st_size);
	buffer=fnnbuffer=(char*)b_malloc(file_stat.st_size);


	if((fp = fopen(filename,"rb"))==NULL)
	{
                fprintf(stderr, "Error opening file %s \r\n",filename);
		exit(EXIT_FAILURE);
	}
	fread(fnnbuffer,file_stat.st_size,1,fp);
	fclose(fp);

	
	NN_VERBOSE(" +crc();\r\n");
	memcpy(&pivot32,buffer,sizeof(u_32));
	if(pivot32!=eth_crc(file_stat.st_size-4,buffer+4))
	{
                fprintf(stderr, "CRC Error in file %s \r\n",filename);
		exit(EXIT_FAILURE);		
	}
	NN_VERBOSE(" +crc(); done\r\n");
	
	memcpy(&pivot16 , buffer+sizeof(u_32) , sizeof(u_16)); /*size in layers*/

	p_in_layers =(u_32*)  b_malloc(pivot16*sizeof(u_32));

	/*pointing buffer to first layersize */
	for(buffer=fnnbuffer+sizeof(u_32)+sizeof(u_32)+sizeof(u_16)+4*sizeof(double), counter=0;counter!=pivot16;counter++)
	{
		RESTORE_AND_SEEK(p_in_layers[counter],buffer,sizeof(u_32));
		NN_VERBOSE2("layer #%u with size %u;" , counter+1 , p_in_layers[counter]);
	}
	NN_VERBOSE1("%u layers found\r\n",pivot16);


	/* _SMART_PEOPLE_CHECK_ */
	for(counter=0,counter2=0;counter2!=pivot16;counter2++ )counter+=p_in_layers[counter2];
	for(linkscounter=0 , counter2=0;linkscounter!=pivot16-1;linkscounter++)
		counter2+= p_in_layers[linkscounter] * p_in_layers[linkscounter+1] *2 ;
	/*                   crc                        nn                          layers                     neurons              links  */
	smartpeoplecheck=sizeof(u_32)+ 4*sizeof(double)+sizeof(u_16)+sizeof(u_32) + pivot16*sizeof(u_32) +  counter*3*sizeof(double)+ counter2*3*sizeof(double);
	if(smartpeoplecheck!=file_stat.st_size)
	{
		fprintf(stderr,"filesize=%d guess=%d. not so smart.\r\n",(int)file_stat.st_size,smartpeoplecheck);
		exit(EXIT_FAILURE);
	}


	nn=create_nn(pivot16,p_in_layers);

	for(layer=nn->first, counter=0;counter!=pivot16;counter++)
	{
		layer->size =p_in_layers[counter];
		NN_VERBOSE2(" layer%03u->size=%u\r\n" , counter+1 , p_in_layers[counter] );
		layer=layer->next;
	}
	free(p_in_layers);
	
	NN_VERBOSE(" +copying neurons\r\n");
	for(layer=nn->first, counter=0 ; counter!=nn->size ; counter++, layer=layer->next )
	/* for_each(layer) */
	{
		for(neuro=layer->first, counter2=0;counter2!=layer->size;counter2++,neuro=neuro->next)
		/* for_each neuron */
		{
		
			RESTORE_AND_SEEK(neuro->state,buffer,sizeof(double));
			NN_VERBOSE3("\r\n  layer%03u->neuro%03u->state =%f\r\n",counter+1 , counter2+1 ,  neuro->state);

			RESTORE_AND_SEEK(neuro->output,buffer,sizeof(double));
			NN_VERBOSE3("  layer%03u->neuro%03u->output=%f\r\n",counter+1 , counter2+1 ,  neuro->output);			

			RESTORE_AND_SEEK(neuro->error,buffer,sizeof(double));
			NN_VERBOSE3("  layer%03u->neuro%03u->error =%f\r\n",counter+1 , counter2+1 ,  neuro->error);		
		}
	}
	NN_VERBOSE(" +neurons copied\r\n");

	NN_VERBOSE(" +copying fwd links\r\n");
	for(layer=nn->first, counter=0 ; counter!=nn->size-1 ; counter++, layer=layer->next )
	/* for_each(layer) except last */
	{
                u_32 nextlayersize;
                nextlayersize=layer->next->size;		

		for(neuro=layer->first, counter2=0;counter2!=layer->size;counter2++,neuro=neuro->next)
		/* for_each neuron */
		{
                        for(link=neuro->fp_np, linkscounter=0;linkscounter!=nextlayersize;linkscounter++,link++)
                        /* for_each(link) */
                        {
				RESTORE_AND_SEEK(link->weight,buffer,sizeof(double));
				NN_VERBOSE4("\r\n  layer%03u->neuro%03u->fwdlink%03u->weight     =%f\r\n",counter+1 , counter2+1 , linkscounter+1 , link->weight);
				
				RESTORE_AND_SEEK(link->weightsave,buffer,sizeof(double));
				NN_VERBOSE4("  layer%03u->neuro%03u->fwdlink%03u->weightsave =%f\r\n",counter+1 , counter2+1 , linkscounter+1,  link->weightsave);
				
				RESTORE_AND_SEEK(link->deltaweight,buffer,sizeof(double));
				NN_VERBOSE4("  layer%03u->neuro%03u->fwdlink%03u->deltaweight=%f\r\n",counter+1 , counter2+1 , linkscounter+1 ,  link->deltaweight);
                                
                        }
		}
	}
	NN_VERBOSE(" +fwd links copied\r\n");

	NN_VERBOSE(" +copying bwd links\r\n");
	for(layer=nn->first->next , counter=1 ; counter!=nn->size ; counter++, layer=layer->next )
	/* for_each(layer) except first */
	{
                u_32 prevlayersize;
                prevlayersize=layer->previous->size;		

		for(neuro=layer->first, counter2=0; counter2!=layer->size ;counter2++,neuro=neuro->next)
		/* for_each neuron */
		{
                        for(link=neuro->bp_np, linkscounter=0;linkscounter!=prevlayersize;linkscounter++,link++)
                        /* for_each(link) */
                        {
				RESTORE_AND_SEEK(link->weight    ,buffer ,sizeof(double));
				NN_VERBOSE4("\r\n  layer%03u->neuro%03u->bwdlink%03u->weight     =%f\r\n",counter+1 , counter2+1 , linkscounter+1 , link->weight);
				
				RESTORE_AND_SEEK(link->weightsave,buffer ,sizeof(double));
				NN_VERBOSE4("  layer%03u->neuro%03u->bwdlink%03u->weightsave =%f\r\n",counter+1 , counter2+1 , linkscounter+1 , link->weightsave);

				RESTORE_AND_SEEK(link->deltaweight,buffer,sizeof(double));
				NN_VERBOSE4("  layer%03u->neuro%03u->bwdlink%03u->deltaweight=%f\r\n",counter+1 , counter2+1 , linkscounter+1 , link->deltaweight);
                        }
		}
	}
	NN_VERBOSE(" +bwd links copied\r\n");

	free(fnnbuffer);
	free(p_in_layers);
	NN_VERBOSE("load_nn(); done\r\n");
	
	return nn;
}



void randomize_nn(NN*nn)
{
	LAYER* c_layer;
	NEURO* c_neuro;
	LINK * c_link;
	u_32 counter , counter2 , counter3;

  	for(c_layer=nn->first, 	counter=0;counter!=nn->size;counter++,c_layer=c_layer->next)
  	{
  		u_32 nextlayersize=(c_layer->next)?c_layer->next->size:0;
  		for(c_neuro=c_layer->first, counter2=0;c_layer->size!=counter2;counter2++,c_neuro=c_neuro->next)
  			for(c_link=c_neuro->fp_np,counter3=0;counter3!=nextlayersize;counter3++,c_link++)
  				c_link->weight= ((double) rand() / RAND_MAX) -.5;
 	}
  
}


void fire_nn(NN* nn, double* input, double* expected,double* output)
{
	LAYER* c_layer;
	NEURO* c_neuro;
	LINK * c_link ; 
	
	u_32 counter , counter2, counter3 ;
	NN_VERBOSE("fire_nn();\r\n");
	for( c_layer=nn->first , counter=0; counter!=nn->size ;counter++ , c_layer=c_layer->next)
	/*for_each(layer)*/
	{
		u_32 nextlayersize=(c_layer->next)?c_layer->next->size:0;
		NN_VERBOSE1("layer %03u\r\n",counter+1);
		for(c_neuro=c_layer->first, counter2=0; counter2!=c_layer->size ;counter2++, c_neuro=c_neuro->next)
		/*for_each(neuron)*/
		{
			NN_VERBOSE1("neuron %03u\r\nlinks: ",counter2+1);
			for(c_link=c_neuro->fp_np, counter3=0;counter3!=nextlayersize ;c_link++ , counter3++)
			/*for_each(link)*/
			{
				NN_VERBOSE1("%03u ",counter3+1);
				c_link->linkedneuro->state += c_link->weight * c_neuro->output; /* output del neurone in basso */ 
			}
			NN_VERBOSE("\r\n");
			/*since we have computed output for c_layer in a previus for_each(layer) cicle we can calculate output and zero all neurons->state */
			/*if we are not in the first layer (burp)*/
			if(counter)
			{
				
				NN_VERBOSE1("%f state\r\n",c_neuro->state);
				c_neuro->output = 1 / ( 1 + exp( - nn->gain * c_neuro->state ) );
				c_neuro->state  = 0;
				NN_VERBOSE1("output: %f \r\n",c_neuro->output);				
			}
		}
	}
	if(output) /* we won't use ouput if NULL is passed */
	{
		for(c_neuro=nn->last->first, counter=0;counter!=nn->last->size;counter++, c_neuro=c_neuro->next)
		{
			output[counter]=c_neuro->output;
		}
	}
	if(expected) /* we won't backpropagate if NULL is passed */
	{
		// back_propagate(expected);
		
	}
	NN_VERBOSE("fire_nn(); done\r\n");
}

int feed_from_file( NN*nn, char* filename  , FEEDCALLBACK* feedcallback)
{
	FILE* fp;
	double* input;
	double* output;	
	double* expected;
	u_32 sizeofinput;
	u_32 sizeofexpected;
	u_32 counter,counter2;
	LAYER * l_tmp;
	char buffer[100];
	char t_crlf;
	
	NN_VERBOSE("feedfromfile();\r\n");
	sizeofinput=nn->first->size;
	NN_VERBOSE1("sizeofinput  = %u\r\n",sizeofinput );	

	for(l_tmp=nn->first, counter=0;counter!=nn->size-1;counter++)l_tmp=l_tmp->next;
	sizeofexpected = l_tmp->size;

	NN_VERBOSE1("sizeofexpected  = %u\r\n",sizeofexpected );	

	input   =b_malloc(sizeofinput   *sizeof(double));
	output  =b_malloc(sizeofexpected*sizeof(double));
	expected=b_malloc(sizeofexpected*sizeof(double));

	
	if((fp=fopen(filename,"rt"))==NULL)
	{
		fprintf(stderr,"cannot open file\r\n");
		exit(EXIT_FAILURE);
	}
	
	for(;;)
	{
		for(counter=0;counter!=sizeofinput;counter++)
		/*read input*/
		{
			for(counter2=0;;counter2++)
			{
				if(counter>97)
				{
					fprintf(stderr,"too long double in %s \r\n",filename);
					exit(EXIT_FAILURE);
				}
				t_crlf=getc(fp);
				if(t_crlf==EOF && counter2==0 && counter==0)
				{
					goto END;
				}
				
				if(t_crlf==EOF && counter2==0 && counter==0)
				{
					fprintf(stderr,"Unexpected EOF in %s\r\n",filename);
					exit(EXIT_FAILURE);				
				}

				if(t_crlf=='\r')t_crlf=getc(fp);
				if(t_crlf=='\n')break;
				if(t_crlf==',')break;			
				buffer[counter2]=t_crlf;
				buffer[counter2+1]=0;
			}
			NN_VERBOSE2("--> %03u) %s\r\n",counter+1, buffer);
			input[counter]=strtod(buffer,NULL);
			NN_VERBOSE1("--> %f\r\n",strtod(buffer,NULL));
			//strtod() e push in input;
		}
		for(counter=0;counter!=sizeofexpected;counter++)
		/* read expected */
		{
			for(counter2=0;;counter2++)
			{
				if(counter>97)
				{
					fprintf(stderr,"too long double in %s \r\n",filename);
					exit(EXIT_FAILURE);
				}
				t_crlf=getc(fp);
				if(t_crlf==EOF)
				{
					fprintf(stderr,"Unexpected EOF in %s\r\n",filename);
					exit(EXIT_FAILURE);				
				} 
				if(t_crlf=='\r')t_crlf=getc(fp);
				if(t_crlf=='\n')break;
				if(t_crlf==',')break;			
				buffer[counter2]=t_crlf;
				buffer[counter2+1]=0;
			}
			NN_VERBOSE2("<-- %03u) %s\r\n",counter+1, buffer);
			expected[counter]=strtod(buffer,NULL);
			NN_VERBOSE1("<-- %f\r\n",expected[counter]);
		}
		fire_nn(nn,input,expected,output);
		if(feedcallback)
			feedcallback(nn,input,expected,output);
			
	}
END:
	fclose(fp);
	free(input);
	free(output);
	free(expected);
	NN_VERBOSE("feedfromfile(); done\r\n");	
	return 1;
}

#ifdef TRY_NN
int main(int zz,char**zzz) /* hello world */
{
	NN* nn;
	u_32 p_in_layers[4]={16,8,4,2};
	FILE* fp;
	double input[]=
	{
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6, .7 , .8 , .9 , 1.0,
		 .1, .2, .3, .4, .5 , .6
	};

	/* nn=load_nn("saved_mind.nn"); */
	nn=create_nn(4,p_in_layers);
	SET_ALPHA(nn,0.80);
	SET_ETA  (nn,0.28);
	SET_GAIN (nn,1.00);
	randomize_nn(nn);
	/* feed_from_file(nn,"prot.txt", NULL); */

	fire_nn(nn,input,NULL,NULL);
	save_nn(nn,"saved_mind.nn");
	fp=fopen("nn.xml","wt");
	status_nn(nn,fp);
	free_nn(nn);
	return 0;
}
#endif
