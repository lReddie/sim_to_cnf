   
#include <stdio.h>
#include "global.h"
#include "miscell.h"

extern char *filename;


void resetall()
{
  wptr w;
  int i;
  int f=1;
  tw = 0;
  tc = 1;
  labeliter = 1;
  for(i = 0; i< HASHSIZE; i++){
    for(w = hash_wlist[i]; w; w=w->hnext){
      w->label = 0;
      w->repeat = 0;
    }
  }
  return;
}

void resetrepeat()
{
  wptr w;
  int i;;
  for(i = 0; i< HASHSIZE; i++){
    for(w = hash_wlist[i]; w; w=w->hnext){
      w->repeat = 0;
    }
  }
  return;
}


void do_cnf(int output_max, char *infile, int justifiable)
{
  int j;
  int endj;
  wptr w;
  if (ncktout > output_max) {endj=output_max;}
  else {endj=ncktout;}
  for(j= 0; j < endj; j++)
    { 
      fprintf(stdout,"Output: %d\n\n",j);
      resetall();    
      w = cktout[j];
      branchwire(w);
      write_header(j, justifiable);
      resetrepeat();
      printbranch(w, j, justifiable);
    }
  return;
}


void write_header(int numoutput, int justifiable)
{
    FILE *out,*fopen();
    char ufile[800];
    int numlit = tw;   //number of liters
    int numclaus = tc;   //number of clauses
    sprintf(ufile,"%s_outnum%d_just%d.cnf",filename, numoutput,justifiable);
    fprintf(stdout, "writing to %s\n", ufile);
    out = fopen(ufile,"w");
    fprintf(out,"p cnf %d ",tw); 
    fprintf(out,"%d\n", tc);
    fprintf(out,"%d 0\n",justifiable);
    fclose(out);
    return;
}



void branchwire(wptr cur_wire)
{
  nptr cur_node;
  wptr back_wire;
  int f;
  int zed;
  if( (cur_wire->repeat == 1)){ 
    return;
  }
  tw++;   
  cur_wire->label = labeliter;
  fprintf(stdout,"current wire: %s %d\n",cur_wire->name, cur_wire->label); 
  labeliter++;
  cur_wire->repeat = 1;

  cur_node = cur_wire->inode[0];
  if (cur_node->nin==0){
    fprintf(stdout,"return dummygate\n");
    return;
  }
  tc =tc +cur_node->nin +1;
  fprintf(stdout,"current node: %s\n", cur_node->name); 
 
  for(f = 0; f < cur_node->nin; f++){
    back_wire = cur_node->iwire[f];
    branchwire(back_wire);
  }
  return;
}


void printbranch(wptr cur_wire, int numoutput, int justifiable)
{
  nptr cur_node;
  wptr back_wire;
  int f;
  int zed;
  if( (cur_wire->repeat == 1)){ 
    return;
  }
 
  cur_wire->repeat = 1;
  cur_node = cur_wire->inode[0];
  if (cur_node->nin==0){
    return;
  }
  printclause(cur_node, numoutput, justifiable);
  for(f = 0; f < cur_node->nin; f++){
    back_wire = cur_node->iwire[f];
    printbranch(back_wire, numoutput, justifiable);
  }
  return;
}


void printclause(nptr n, int numoutput, int justifiable)
{

  FILE *out;//,*fopen();
  char ufile[800];
  sprintf(ufile,"%s_outnum%d_just%d.cnf",filename, numoutput, justifiable); 
  out = fopen(ufile,"a");
  int j;
   switch(n->type){
      case AND :
	for (j = 0; j < n->nin; j++){
	  fprintf(out, "%d ", n->iwire[j]->label*-1);
	}
	fprintf(out, "%d 0\n", n->owire[0]->label);
	for (j = 0; j < n->nin; j++){
	  fprintf(out,"%d %d 0\n", n->iwire[j]->label, n->owire[0]->label*-1);
	}
	break;
      case NAND :
	for (j = 0; j < n->nin; j++){
	  fprintf(out, "%d ", n->iwire[j]->label*-1);
	}
	fprintf(out, "%d 0\n", n->owire[0]->label*-1);
	for (j = 0; j < n->nin; j++){
	  fprintf(out,"%d %d 0\n", n->iwire[j]->label, n->owire[0]->label);
	}
	break;
      case OR :
	for (j = 0; j < n->nin; j++){
	  fprintf(out, "%d ", n->iwire[j]->label);
      	}
	fprintf(out, "%d 0\n", n->owire[0]->label*-1);
	for (j = 0; j < n->nin; j++){
	  fprintf(out, "%d %d 0\n", n->iwire[j]->label*-1, n->owire[0]->label);
	}	
	break;
      case NOR :
	for (j = 0; j < n->nin; j++){
	  fprintf(out, "%d ", n->iwire[j]->label);
	}
	fprintf(out, "%d 0\n", n->owire[0]->label);
	for (j = 0; j < n->nin; j++){
	  fprintf(out,"%d %d 0\n", n->iwire[j]->label*-1, n->owire[0]->label*-1);
	}
	break;
      case NOT :
	fprintf(out,"%d %d 0\n",n->iwire[0]->label*-1, n->owire[0]->label*-1);
	fprintf(out,"%d %d 0\n", n->iwire[0]->label, n->owire[0]->label);
	break;
   }
   fclose(out);
return;
}
