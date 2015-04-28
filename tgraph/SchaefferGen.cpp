/******************************************************************
*                                                                 *  
*     This graph generator is provided by Gilles Schaeffer        * 
*                     Gilles.Schaeffer@loria.fr                   * 
*                                                                 *
*******************************************************************/
#include <TAXI/Tbase.h> 
#include <TAXI/Tmessage.h> 
#include <TAXI/color.h> 
#include <TAXI/graphs.h> 
#include <TAXI/random.h> 

#define pmRandom randomGet

//PMdef.c
#define PMTRUE -1
#define PMFALSE 0

#define MSK 'a'
#define BLACK  2
#define BLACK2 3
#define WHITE  4
#define ROOT   6
#define FREE   8
#define FREE2  9
#define CLSD   10
#define COFREE 12

#define VIRT  14
#define INNER 16
#define OUTER 18
#define DLTD  20

#define FORCED 40
#define DONE   42

#define printvf if (Meth->verbose) printf
#define printwf if (Meth->verbose == PMTRUE+PMTRUE) printf

/* les aretes sont toutes pareilles, aussi bien pour les arbres
   que pour les cartes */

typedef struct e /* The data type used for edges */
{
  struct v *from;          /* vertex where the edge starts */
  struct v *face;          /* face on the right side of the edge
		      note: only valid if addFace() called */
  struct e *prev;    /* previous edge in clockwise direction */
  struct e *next;    /* previous edge in clockwise direction */
  struct e *oppo;    /* the edge that is inverse to this one */
  long mark;
  short type;
  long label;
} pm_edge;

#define Next oppo->next   // Pour faciliter les deplacements
#define Prev prev->oppo   // dans la carte duale.
#define Oppo oppo

typedef struct v /* data type for vertices and faces*/
{
  pm_edge *root;
  struct v *next;
  long mark;
  short type;
  long label;
} pm_vertex;
    

typedef struct pmmap /* data type for root and info */
{
  pm_edge *root;
  long e, v, f, i;
}pmMap;

/* une carte ou un arbre sont constitues de sommets, d'aretes et de faces
   prises dans un des tableaux correspondant, qui servent de "reservoir":
   la gestion est faite manuellement dans ces tableaux, en particulier
   parce qu'on sait toujours d'avance de quelle place on aura besoin. */


typedef struct st /* data type for stack */
{
  pm_edge **s;
  long pos;
}pmStck;


/******************************/
/* These data types serve for the command flow */
/******************************/
			       
typedef struct pmsize /* Data type for map type and size */
{
  char m, b;     /* map and basic map type */
  long e, v, f;  /* edges, vertices, faces */
  long r, g, d;  /* red and black vertices, max degree */
  long t;        /* tolerence on e */
  long *dgArr;   /* pt on vertex list */
} pmSize;

typedef struct pmmethod /* Data type for methods */
{
  char core, pic; 
  long seed;
  char verbose;
} pmMethod;

typedef struct pmmemory  /* Data type for memory requirements */
{ 
  char dTree;
  long sTree, rTree, gTree, sWrd, sEdge, sVtx, sLeaf;
} pmMemory;

typedef struct pmoutput /* Data type for output */
{
  char format, transform;
  char map, dual;
} pmOutput;

typedef struct pmstats  /* Data type for stats */
{
  long nb;        /* number of maps generated */
  char stats, loop, core, dist, facedeg;
} pmStats;




/****************************************/
/* data utility functions */
/****************************************/

/* gestion memoire */
void pmMemoryFault(void){
  printf("\nAllocation failure\n");
  exit(3);
}
/* gestion du mot */
void pmCreateWrd(long n, char **Wrd)
{
  *Wrd=(char *)calloc(n+1,sizeof(char));
  if (*Wrd == NULL) pmMemoryFault();
}
void pmFreeWrd(char *Wrd)
{
  free(Wrd);
}

/* gestion des sommets */
pm_vertex *pmVtxSet; long pmNxtVtxNbr=-1;

void pmCreateVtx(long n)
{
  pmVtxSet=(pm_vertex *)calloc(n,sizeof(pm_vertex));
  if (pmVtxSet == NULL) pmMemoryFault();
  pmNxtVtxNbr=0;
}

void pmFreeVtx()
{
  free(pmVtxSet);
  pmNxtVtxNbr=-1;
}

pm_vertex *pmNewVtx(pm_edge *Edge)
{
  pm_vertex *NxtVtx;
  NxtVtx=pmVtxSet+pmNxtVtxNbr++;
  NxtVtx->root=Edge;
  if (Edge != NULL){
    Edge->from=NxtVtx;
    //    NxtVtx->degree=1;
  }else{
    //    NxtVtx->degree=0;
  }return NxtVtx;
}

pm_vertex *pmNewFace(pm_edge *Edge)
{
  pm_vertex *NxtVtx;
  NxtVtx=pmVtxSet+pmNxtVtxNbr++;
  NxtVtx->root=Edge;
  if (Edge != NULL){
    Edge->face=NxtVtx;
    //    NxtVtx->degree=1;
  }else{
    //    NxtVtx->degree=0;
  }return NxtVtx;
}

/* gestion des aretes */
pm_edge *pmEdgeSet;  long pmNxtEdgeNbr=-1;

void pmCreateEdge(long n)
{
  pmEdgeSet=(pm_edge *)calloc(n,sizeof(pm_edge));
  if (pmEdgeSet == NULL) pmMemoryFault();
  pmNxtEdgeNbr=0;
}

void pmFreeEdge()
{
  free(pmEdgeSet);
  pmNxtEdgeNbr=-1;
}

pm_edge *pmEmptyEdge()
{
  return (pmEdgeSet+pmNxtEdgeNbr++);
}

pm_edge *pmNewEdge(pm_vertex *from,
	      pm_edge *prev, pm_edge *next, pm_edge *oppo,
	      short type)
{
  pm_edge *NxtEdge;
  NxtEdge=pmEdgeSet+pmNxtEdgeNbr++;
  NxtEdge->from=from;
  NxtEdge->prev=prev;
  NxtEdge->next=next;
  NxtEdge->oppo=oppo;
  NxtEdge->type=type;
  return NxtEdge;
}

/* gestion de la pile */
void pmCreateStck(long n, pmStck *Stack)
{
  Stack->s=(pm_edge **)calloc(n,sizeof(pm_edge *));
  if (Stack->s == NULL) pmMemoryFault();
  Stack->pos=0;
}

void pmFreeStck(pmStck Stack)
{
  free(Stack.s);
}

void pmStckIn(pm_edge *e, pmStck *Stack)
{
  Stack->s[Stack->pos++]=e;
}

pm_edge *pmStckOut(pmStck *Stack)
{
  if (Stack->pos == 0)
    return NULL;
  else
    return Stack->s[--(Stack->pos)];
}

/* gestion des marques */
long pmAbsMark=1;
long pmAbsLabel=1;

long pmNewMark()
{
  pmAbsMark++;
  if (pmAbsMark == 0) pmAbsMark++;
  return pmAbsMark;
}
long pmCurMark()
{
  return pmAbsMark;
}
long pmNewLabel()
{
  pmAbsLabel++;
  if (pmAbsLabel == 0) pmAbsLabel++;
  return pmAbsLabel;
}


/* gestion des sous composantes */
pm_edge **pmBloc;
pm_edge **pmPost;
pm_edge **pmSeed;
pm_edge **pmComp;

long pmBlocBeg, pmBlocEnd, pmCompBeg, pmCompEnd,
  pmPostBeg, pmPostEnd, pmSeedBeg, pmSeedEnd;

void pmCreateBloc(long n)
{
  pmBloc=(pm_edge **)calloc(n,sizeof(pm_edge *));
  if (pmBloc == NULL) pmMemoryFault();
  pmBlocBeg=-1;
  pmBlocEnd=-1;
}
int pmIsBloc(){
  if (pmBlocBeg == pmBlocEnd) return(PMFALSE);
  else return(PMTRUE);
}
void pmFreeBloc()
{
  free(pmBloc);
  pmBlocBeg=-1;
  pmBlocEnd=-1;
}
void pmNewBloc(pm_edge *e)
{
  pmBloc[++pmBlocEnd]=e;
}
pm_edge *pmNextBloc(void)
{
  if (pmBlocBeg == pmBlocEnd)
    return NULL;
  else
    return pmBloc[++pmBlocBeg];
}
  
void pmCreateComp(long n)
{
  pmComp=(pm_edge **)calloc(n,sizeof(pm_edge *));
  if (pmComp == NULL) pmMemoryFault();
  pmCompBeg=-1;
  pmCompEnd=-1;
}
int pmIsComp(){
  if (pmCompBeg == pmCompEnd) return(PMFALSE);
  else return(PMTRUE);
}
void pmFreeComp()
{
  free(pmComp);
  pmCompBeg=-1;
  pmCompEnd=-1;
}
void pmFirstComp(void)
{
  pmCompBeg = -1;
}
void pmNewComp(pm_edge *e)
{
  pmComp[++pmCompEnd]=e;
}
pm_edge *pmNextComp(void)
{
  if (pmCompBeg == pmCompEnd)
    return NULL;
  else
    return pmComp[++pmCompBeg];
}



void pmCreatePost(long n)
{
  pmPost=(pm_edge **)calloc(n,sizeof(pm_edge *));
  if (pmPost == NULL) pmMemoryFault();
  pmPostBeg=-1;
  pmPostEnd=-1;
}
int pmIsPost(){
  if (pmPostBeg == pmPostEnd) return(PMFALSE);
  else return(PMTRUE);
}
void pmResetPost()
{
  pmPostBeg=-1;
  pmPostEnd=-1;
}
void pmFreePost()
{
  free(pmPost);
  pmPostBeg=-1;
  pmPostEnd=-1;
}
void pmNewPost(pm_edge *e)
{
  pmPost[++pmPostEnd]=e;
}
pm_edge *pmNextPost(void) 
{
  if (pmPostBeg == pmPostEnd)
    return NULL;
  else
    return pmPost[++pmPostBeg];
}

void pmCopyPostSeed(void)
{
  pmSeedBeg = -1;
  pmSeedEnd = -1;
  while(pmPostBeg < pmPostEnd)
    pmSeed[++pmSeedEnd] = pmPost[++pmPostBeg];
  pmPostBeg = -1;
  pmPostEnd = -1;
}

  
void pmCreateSeed(long n)
{
  pmSeed=(pm_edge **)calloc(n,sizeof(pm_edge *));
  if (pmSeed == NULL) pmMemoryFault();
  pmSeedBeg=-1;
  pmSeedEnd=-1;
}
int pmIsSeed(){
  if (pmSeedBeg == pmSeedEnd) return(PMFALSE);
  else return(PMTRUE);
}
void pmFreeSeed()
{
  free(pmSeed);
  pmSeedBeg=-1;
  pmSeedEnd=-1;
}
void pmFirstSeed(void)
{
  pmSeedBeg = -1;
}
void pmNewSeed(pm_edge *e)
{
  pmSeed[++pmSeedEnd]=e;
}
pm_edge *pmNextSeed(void)
{
  if (pmSeedBeg == pmSeedEnd)
    return NULL;
  else
    return pmSeed[++pmSeedBeg];
}




/******************************/
/* this function initialize the random generator */
/******************************/
int pmInitRND(pmMethod *){
  //srand48(Meth->seed); printvf("# Seed: %ld\n",Meth->seed);
  return(PMTRUE);
}

/******************************/
/* this function generate random integers between 1,n */
/******************************/
/*
long pmRandom(long n)
  {
  if (n>0) 
    return lrand48()%n+1;
  else return 1; 
}
*/


//**************************************************************************
//PMconjugation.c
/* tirage d'un mot et calcul a la volee de sa factorisation
 *
 * long luka1 (long n,        nombre de noeuds
 *             long k,        l'arite des noeuds (si 0 => variable)
 *             char LkWrd[])  le mot resultat
 * renvoie la position de depart du mot conjugue dans LkWrd.
 * il faut que le tableau LkWrd soit assez grand !
 * Les lettres sont 'a'+k pour l'arite k.
 */

long pmLuka1 (long n, long k, char *LkWrd )
{
  /* ici on fait un unranking tout bete. */
  /* au fur et a mesure on calcule la hauteur */
  long s,l=k*n+1;
  long start=0, min=-1, h=-1;
  LkWrd[l]='\0';
  for (s=l-1; s>=0; s--){
    if (pmRandom(s+1)<=n) {
      LkWrd[s]=(char)(k+'a'); n--;
      h-=k-1;
    }else {
      LkWrd[s]=0+'a';
      h++;
    }
    if (h<=min) { min=h; start=s; }
  }
  return start;
}

/*
 * long luka2 (long l,        la longueur du mot ; ie somme des DgArr[k]
 *	       long DgArr[],  table des degres dans le cas variable
 *             char LkWrd[])  le mot resultat
 * renvoie la position de lecture du debut du mot conjugue.
 *
 * il faut que le tableau LkWrd soit assez grand !
 * il faut aussi que la somme des degres des noeuds soit
 * egale a l.
 */

long pmLuka2 (long l, long DgArr[], char LkWrd[] )
{
  /* cette fonction utilise le tirage d'une permutation
     aleatoire en temps lineaire pour trier un tableau
     contenant les lettres. La permutation est construite
     via sa table d'inversion, par transpositions successives. */

  char c;
  long i,k=0;
  long start=0, min=0, h=0;
  /* construction du mot trie */
  for (i=0 ; i<l; i++){
    while (!(DgArr[k]--)) k++;
    LkWrd[i]='a'+(char)k;
  }    
  /* permutation */
  /* pour i de 1 a l on multiplie par (i,k) avec k<=i */
  for (i=0 ; i<l; i++){
    k=pmRandom(i+1)-1;
    c=LkWrd[k];
    LkWrd[k]=LkWrd[i];
    LkWrd[i]=c;
  }
  /* ici on ne peut pas facilement calculer le point de conjugaison
     on the fly donc on le fait apres coup */
  for (i=0 ; i<l; i++){
    h+=LkWrd[i]-'a'-1;
    if (h<min) { min=h; start=i+1; }
  }
  return start%l;
}

/* tirage d'un mot bicolore, par chottin
 *
 * long luka3 (long i,        nombre de noeuds blancs
 *             long j,        nombre de noeuds noirs
 *             char LkWrd[])  le mot resultat
 * renvoie la position de depart du mot conjugue dans LkWrd.
 * il faut que le tableau LkWrd soit assez grand !
 * Attention : chaque lettre est de la forme = (B(b*)a(b*)a)*A
 *    et son arite totale est le nombre de b moins 1.
 */

long pmLuka3 (long i, long j, char LkWrd[] )
{
  /* cette fonction utilise l'algo de chottin, tel que decrit dans
     ma these p151-154. */
  int v;
  long s=i+2*j,t=2*i+j-1,l=3*(i+j)+1,p;
  long start=0, min=0, h=0;
  for (p=0; p<l; p++, s--){
    if (pmRandom(s)<=i) {
      LkWrd[p]='B'; i--;
      for (v=0 ; v<2; v++){
	while (pmRandom(t--)<=j){
	  j--;
	  LkWrd[++p]='b';
	  h+=2;
	} 
	p++;
	LkWrd[p]='a';
      }
    }else {
      LkWrd[p]='A';
      h--;
    }
    if (h<min) { min=h; start=p+1; }
  }
  LkWrd[l+1]='\0';
  return start%l;
}






/* ici on prend les mots de lukacievicz et on construit les
   arbres associes. */

/*
 * Cette fonction construit l'arbre associe a un
 * code de lukacievicz normal quelconque. 
 *
 *
 * pm_edge *luka2tree(                 renvoie le 1/2 brin racine
 *                 long st,         position du conjugue dans
 *                 char LkWrd[])    le mot de lukacievicz
 *
 *
 * Attention cet arbre est enracine sur une 1/2 arete supplementaire
 * non donnee par le code.
 *
 * Attention encore, les feuilles ne sont pas crees :
 * il y a des 1/2 brins.  
 *
 */

pm_edge *pmLuka2tree(long st, char LkWrd[])
{
  pm_edge *Root, *Cur1, *Cur2;
  pm_vertex *Vtx;
  int arity;
  int i;
  
  Root = pmEmptyEdge();
  Root->type = ROOT;

  /* creation du sommet racine */

  arity = LkWrd[st]-MSK;

  Vtx = pmNewVtx(Root);
  Cur1 = Root;
  while (arity--){
    Cur1->next = pmNewEdge(Vtx,Cur1,NULL,NULL,BLACK);
    Cur1       = Cur1->next;
  }
  Cur1->next = Root;
  Root->prev = Cur1;
  Cur1       = Root->next;

  /* creation des autres sommets */
  i=st+1;
  if (!LkWrd[i]) i=0;
  while(i!=st){
    arity = LkWrd[i]-MSK;
    if (arity) {
      Cur1->type = INNER;
      Cur1->oppo = pmNewEdge(NULL,NULL,NULL,Cur1,INNER);
      Cur1       = Cur1->oppo;
      Vtx        = pmNewVtx(Cur1);
      Cur2       = Cur1;
      while (arity--){
	Cur2->next = pmNewEdge(Vtx,Cur2,NULL,NULL,BLACK);
	Cur2       = Cur2->next;
      }
      Cur2->next = Cur1;
      Cur1->prev = Cur2;
      Cur1       = Cur1->next;
    }else{
      Cur1 = Cur1->next;
      while (Cur1->oppo != NULL)
	Cur1=Cur1->oppo->next;
    }
    i++;
    if (!LkWrd[i]) i=0;
  }
  return Root;
}

/* Cette fonction construit l'arbre bicolore associe a un
   code de Chottin. */

pm_edge *pmChottin2tree(long st, char LkWrd[])
{
  pm_edge *Cur0, *Cur1, *Cur2;
  pm_vertex *Vtx;
  long mark = pmNewMark();
  int i=st;
  pm_edge Factice1,Factice2;
  /* pour amorcer et areter on utilise deux arete factice */
  Factice1.next = &Factice2;
  Factice1.prev = &Factice2;
  Factice2.next = &Factice1;
  Factice2.prev = &Factice1;
  Factice1.oppo = NULL;
  Cur0 = &Factice2;
  
  do{
    Cur1 = Cur0;
    /* Reconstitution d'une "lettre" */
    while(LkWrd[i++]!='A'){
      if (!LkWrd[i]) i=0;
      Cur1->type = INNER;
      Cur1->oppo = pmNewEdge(NULL,NULL,NULL,Cur1,INNER);
      Cur1       = Cur1->oppo;
      Vtx        = pmNewVtx(Cur1);
      Cur1->next = pmNewEdge(Vtx,Cur1,NULL,NULL,BLACK2);
      Cur1->prev = pmNewEdge(Vtx,NULL,Cur1,NULL,BLACK2);
      Cur1->next->next = pmNewEdge(Vtx,Cur1->next,Cur1->prev,NULL,BLACK);
      Cur1->prev->prev = Cur1->next->next;
      /* fils gauche */
      Cur2 = Cur1->next;
      while (LkWrd[i++]!='a'){
	if (!LkWrd[i]) i=0;
	Cur2->type = INNER;
	Cur2->oppo = pmNewEdge(NULL,NULL,NULL,Cur2,INNER);
	Cur2       = Cur2->oppo;
	Vtx        = pmNewVtx(Cur2);
	Cur2->next = pmNewEdge(Vtx,Cur2,NULL,NULL,BLACK);
	Cur2->prev = pmNewEdge(Vtx,NULL,Cur2,NULL,BLACK);
	Cur2->next->next = pmNewEdge(Vtx,Cur2->next,Cur2->prev,NULL,BLACK2);
	Cur2->prev->prev = Cur2->next->next;
	Cur2 = Cur2->next->next;
      }
      if (!LkWrd[i]) i=0;
      Cur2->mark = mark; //CLSD;
      /* fils droit */
      Cur2 = Cur1->prev;
      while (LkWrd[i++]!='a'){
	if (!LkWrd[i]) i=0;
	Cur2->type = INNER;
	Cur2->oppo = pmNewEdge(NULL,NULL,NULL,Cur2,INNER);
	Cur2       = Cur2->oppo;
	Vtx        = pmNewVtx(Cur2);
	Cur2->next = pmNewEdge(Vtx,Cur2,NULL,NULL,BLACK);
	Cur2->prev = pmNewEdge(Vtx,NULL,Cur2,NULL,BLACK);
	Cur2->next->next = pmNewEdge(Vtx,Cur2->next,Cur2->prev,NULL,BLACK2);
	Cur2->prev->prev = Cur2->next->next;
	Cur2 = Cur2->next->next;
      }
      if (!LkWrd[i]) i=0;
      Cur2->mark = mark; //CLSD;
      /* la suite de la lettre*/
      Cur1 = Cur1->next->next;
    }
    if (!LkWrd[i]) i=0;
    Cur1->mark = mark; //CLSD;

    /* on avance jusqu'a la prochaine feuille ouverte */
    Cur0 = Cur0->prev;
    do{
      Cur0 = Cur0->next;
      while(Cur0->oppo != NULL)
	Cur0 = Cur0->oppo->next;
    }while(Cur0->mark == mark);//CLSD);
  }while(Cur0 != &Factice1);
  Factice2.oppo->oppo = NULL;
  Factice2.oppo->type = ROOT;
  return Factice2.oppo;
}






/* Ici on ajoute les bourgeons de toutes sortes de facons */


/* arbre binaire -> bourgeons pour cartes planaires normale
 *   1 bourgeon par sommets:
 *      3 possibilites au hasard,
 *        sauf si le sommet porte une indication, auquel cas
 *        apres sa racine locale.
 */

void pmSpring1(pm_edge *Root)
{
  pm_edge *Cur1, *Cur2, *Cur3;
  Cur2 = NULL;
  pm_vertex *Vtx;
  
  Cur1 = Root->next;
  while (Cur1 != Root){
    Vtx = Cur1->from;
    switch (Vtx->type){
    case DONE: break;
    case FORCED:
      Vtx->type  = DONE;
      Cur2       = Cur1->from->root;
      Cur3       = pmNewEdge(Vtx,Cur2,Cur2->next,NULL,WHITE);
      Cur2->next->prev = Cur3;
      Cur2->next = Cur3;
      break;
    default:
      Vtx->type  = DONE;
      switch (pmRandom(3)){
      case 1: Cur2 = Cur1; break;
      case 2: Cur2 = Cur1->next; break;
      case 3: Cur2 = Cur1->prev; 
      }
      Cur3       = pmNewEdge(Vtx,Cur2,Cur2->next,NULL,WHITE);
      Cur2->next->prev = Cur3;
      Cur2->next = Cur3;
    }
    if (Cur1->oppo != NULL)
      Cur1 = Cur1->oppo;
    Cur1 = Cur1->next;
  }
}

/* arbre binaire -> bourgeons pour cartes bicubiques
 *   1 bourgeons par aretes internes
 *      2 possibilites au hasard
 *   je ne sais pas ce que voudrait dire de forcer l'une des deux
 *   mais a tout hasard, la possibilite est laissee
 */
void pmSpring2(pm_edge *Root)
{
  pm_edge *Cur1, *Cur2, *Cur3;
  pm_vertex *Vtx;
  
  Cur1 = Root->next;
  while (Cur1 != Root){
    if (Cur1->oppo != NULL){
      if (Cur1->oppo->from->type != DONE &&
	  Cur1->from->type       != DONE){
	Cur2       = pmNewEdge(NULL,NULL,NULL,Cur1,INNER);
        Vtx        = pmNewVtx(Cur2);
	Vtx->type  = DONE;
	Cur3       = pmNewEdge(Vtx,NULL,NULL,Cur1->oppo,INNER);
	Cur1->oppo->type = INNER;
	Cur1->oppo->oppo = Cur3;
	Cur1->type       = INNER;
	Cur1->oppo       = Cur2;
	if (Cur1->type == FORCED){
	  Cur2->prev = Cur3;
	  Cur3->next = Cur2;
	  Cur2->next = pmNewEdge(Vtx,Cur2,Cur3,NULL,WHITE);
	  Cur3->prev = Cur2->next;
	}else if (Cur3->oppo->type == FORCED){
	  Cur2->next = Cur3;
	  Cur3->prev = Cur2;
	  Cur3->next = pmNewEdge(Vtx,Cur3,Cur2,NULL,WHITE);
	  Cur2->prev = Cur3->next;
	}else if (pmRandom(2)==1){
	  Cur2->prev = Cur3;
	  Cur3->next = Cur2;
	  Cur2->next = pmNewEdge(Vtx,Cur2,Cur3,NULL,WHITE);
	  Cur3->prev = Cur2->next;
	}else{
	  Cur2->next = Cur3;
	  Cur3->prev = Cur2;
	  Cur3->next = pmNewEdge(Vtx,Cur3,Cur2,NULL,WHITE);
	  Cur2->prev = Cur3->next;
	}	  
	Cur1=Cur3;
      }
      Cur1 = Cur1->oppo;
    }
    Cur1 = Cur1->next;
  }
}

/* arbres ternaires -> bourgeons pour non separables
 * 1 bourgeon avant chaque demi arete non terminale
 * Pas de choix.
 */ 

void pmSpring3(pm_edge *Root)
{
  pm_edge *Cur1, *Cur2;

  Cur1 = Root->next;
  while (Cur1 != Root){
    if (Cur1->oppo != NULL){
      Cur2       = pmNewEdge(Cur1->from,Cur1->prev,Cur1,NULL,WHITE);
      Cur1->prev->next = Cur2;
      Cur1->prev       = Cur2;
      Cur1->type       = VIRT;
      if (Cur1 == Cur1->from->root) Cur1->from->root=Cur2;
      Cur1->oppo->type = VIRT;
      Cur1             = Cur1->oppo;
    }
    Cur1 = Cur1->next;
  }
}

/* arbres ternaires -> bourgeons pour cubiques
 * deux bourgeons par sommets, diametralement opposes.
 * 2 choix -> possibiliter de forcer.
 */


void pmSpring4(pm_edge *Root)
{
  pm_edge *Cur1, *Cur2, *Cur3, *Cur4, *Cur5, *Cur6;
  Cur2 = NULL;
  pm_vertex *Vtx;
  pm_edge C;
  pm_vertex V;

  C.oppo = Root;
  C.from = &V;
  V.type = DONE;
  Cur1 = &C;

  while (Cur1 != Root){
    Vtx = Cur1->from;
    if (Vtx->type != DONE){
      if (Vtx->type == FORCED)
	Cur2 = Vtx->root;
      else {
	switch (pmRandom(2)){
	case 1: Cur2 = Cur1; break;
	case 2: Cur2 = Cur1->next;
	}      
      }
      Vtx->type  = DONE;
      Vtx->root  = Cur2;
      Cur3       = pmNewEdge(Vtx,Cur2,NULL,NULL,WHITE);
      Cur4       = pmNewEdge(Vtx,Cur3,Cur2->prev,NULL,VIRT);
      Cur5       = pmNewEdge(NULL,Cur2->next->next,NULL,NULL,WHITE);
      Vtx        = pmNewVtx(Cur5);
      Vtx->type  = DONE;
      Cur6       = pmNewEdge(Vtx,Cur5,Cur2->next,Cur4,VIRT);
      Cur3->next       = Cur4;
      Cur5->next       = Cur6;
      Cur3->prev->next = Cur3;
      Cur4->next->prev = Cur4;
      Cur5->prev->next = Cur5;
      Cur6->next->prev = Cur6;
      Cur4->oppo       = Cur6;
      Cur5->prev->from = Vtx;
      Cur6->next->from = Vtx;
    }
    if (Cur1->oppo != NULL)
      Cur1 = Cur1->oppo;
    Cur1 = Cur1->next;
  }
}


/* arbre binaire -> bourgeons pour cartes bicubiques
 *   2 bourgeons par aretes internes
 *      3 possibilites au hasard
 */
void pmSpring5(pm_edge *Root)
{
  pm_edge *Cur1, *Cur2, *Cur3;
  pm_vertex *Vtx;

  Cur1 = Root->next;
  while (Cur1 != Root){
    if (Cur1->oppo != NULL){
      if (Cur1->oppo->from->type != DONE &&
	  Cur1->from->type       != DONE){
	Cur2       = pmNewEdge(NULL,NULL,NULL,Cur1,INNER);
        Vtx        = pmNewVtx(Cur2);
	Vtx->type  = DONE;
	Cur3       = pmNewEdge(Vtx,NULL,NULL,Cur1->oppo,INNER);
	Cur1->oppo->type = INNER;
	Cur1->oppo->oppo = Cur3;
	Cur1->type       = INNER;
	Cur1->oppo       = Cur2;
	switch (pmRandom(3)){
	case 1:
	  Cur2->prev = Cur3;
	  Cur3->next = Cur2;
	  Cur2->next = pmNewEdge(Vtx,Cur2,NULL,NULL,WHITE);
	  Cur3->prev = pmNewEdge(Vtx,Cur2->next,Cur3,NULL,WHITE);
	  Cur2->next->next = Cur3->prev;
	  break;
	case 2:
	  Cur2->next = pmNewEdge(Vtx,Cur2,Cur3,NULL,WHITE);
	  Cur3->prev = Cur2->next;
	  Cur3->next = pmNewEdge(Vtx,Cur3,Cur2,NULL,WHITE);
	  Cur2->prev = Cur3->next;
	  break;
	case 3:
	  Cur2->next = Cur3;
	  Cur3->prev = Cur2;
	  Cur3->next = pmNewEdge(Vtx,Cur3,NULL,NULL,WHITE);
	  Cur2->prev = pmNewEdge(Vtx,Cur3->next,Cur2,NULL,WHITE);
	  Cur3->next->next = Cur2->prev;
	  break;
	}  
	Cur1=Cur3;
      }
      Cur1 = Cur1->oppo;
    }
    Cur1 = Cur1->next;
  }
}


/* ici on fait la cloture des differents types d'arbres.
   les nouvelles aretes sont marquees OUTER.
   En meme temps, dans l'ordre postfixe, on elimine les aretes VIRT.
 */


pm_edge *pmBalance(pm_edge *Root)
{
  pm_edge *Cur1;
  pm_edge *Free = Root;
  long h = 0, min = 0;

  Cur1 = Root->next;
  while (Cur1 != Root){
    if (Cur1->oppo != NULL){
      Cur1 = Cur1->oppo;
    }else{
      switch (Cur1->type){
      case WHITE : h++; break;
      case BLACK :
      case BLACK2: h--; break;
      default    :break;
      }
      if (h<min){ min = h; Free = Cur1;}
    }
    Cur1 = Cur1->next;
  }
  return Free;
}

pm_edge *pmClosure(pm_edge *Free, pmStck *Stk)
{
  pm_edge *Cur1;
  pm_edge *Root;
  pm_vertex *Vtx;
  long deg=1;

  Free->oppo = pmNewEdge(NULL,NULL,NULL,Free,COFREE);
  if (Free->type == BLACK2)  
    Free->type = FREE2;
  else 
    Free->type = FREE;
  Root       = Free->oppo;
  Vtx        = pmNewVtx(Root);
  
  Cur1 = Free->next;
  while (Cur1 != Free){
    if (Cur1->oppo != NULL){
      Cur1 = Cur1->oppo;
      if (Cur1->type == VIRT){
	if (Cur1->oppo->type != VIRT){
	  Cur1->oppo->next->prev = Cur1->oppo->prev;
	  Cur1->oppo->prev->next = Cur1->oppo->next;
	  Cur1->next->prev = Cur1->prev;
	  Cur1->prev->next = Cur1->next;
	  Cur1->type = DLTD;
	}else
	  Cur1->type = DLTD;
      }
    }else{
      switch (Cur1->type){
      case WHITE :
	pmStckIn(Cur1, Stk); break;
      case BLACK :
      case BLACK2:
      case ROOT  :
	Cur1->oppo = pmStckOut(Stk); 
	if (Cur1->oppo == NULL){
	  if (Cur1->type == BLACK2)
	    Cur1->type = FREE2;
	  else 
	    Cur1->type = FREE;
	  Cur1->oppo = pmNewEdge(Vtx,NULL,Root,Cur1,COFREE);
	  Root->prev = Cur1->oppo;
	  Root       = Root->prev;
	  deg++;
	}
	else{
	  Cur1->type = OUTER;
	  Cur1->oppo->oppo = Cur1;
	  Cur1->oppo->type = OUTER;
	}
	break;
      default    :break;
      }
    }
    Cur1 = Cur1->next;
  }
  Free->oppo->next = Root;
  Root->prev       = Free->oppo;
  deg = pmRandom(deg);
  while (deg--) Root = Root->next;
  while (Root->oppo->type == FREE2) Root = Root->next;
  return Root;
}

/* dans certains cas on veut se debarrasser du sommet de degre 2 */

pm_edge *pmSuppress(pm_edge *Root)
{
  if (Root->next == Root->prev){
    Root->oppo->oppo = Root->next->oppo;
    Root->next->oppo->oppo = Root->oppo;
    Root->type = DLTD;
    Root->next->type = DLTD;
    Root->from->type = DLTD;
    Root = Root->next->oppo;
  }
  return Root;
}
//**************************************************************************
//PMenlight.c

/* ici on passe au dual, aux triangulations ou autres variantes. */

long pmLabelCanon(pm_edge *Root)
{
  pm_edge *Cur1, *Cur2;
  long mark;
  long vtx=1, edge=1;

  mark = pmNewMark();

  Root->from->mark = mark;
  Root->from->label = vtx++;
  Cur1 = Root->prev;
  do {
    Cur1 = Cur1->next;
    if (Cur1->oppo->from->mark != mark){
      Cur1->label       = edge;
      Cur1->oppo->label = -edge;
      edge++; 
    }else if (Cur1->oppo->mark != mark){
      Cur1->label       = edge;
      Cur1->oppo->label = -edge;
      edge++;
      Cur1->mark = mark;
    }
  }while (Cur1 != Root->prev);
  Root->mark = mark;

  if (Root->oppo->from->mark != mark)
    Cur1 = Root->oppo->next;
  else
    Cur1 = Root->next;
  while (Cur1 != Root){
    if (Cur1->from->mark != mark){
      Cur1->from->mark  = mark;
      Cur1->from->label = vtx++;
      Cur2 = Cur1->prev;
      do {
	Cur2 = Cur2->next;
	if (Cur2->oppo->from->mark != mark){
	  Cur2->label       = edge;
	  Cur2->oppo->label = -edge;
	  edge++;
	}else if (Cur2->oppo->from == Cur2->from &&
		  Cur2->oppo->mark != mark){
	  Cur2->label       = edge;
	  Cur2->oppo->label = -edge;
	  edge++;
	  Cur2->mark = mark;
	}
      }while (Cur2 != Cur1->prev);
    }
   if ((Cur1->oppo->mark == mark && Cur1->oppo->from != Cur1->from) ||
	Cur1->oppo->from->mark != mark){
      Cur1->mark = mark;
      Cur1       = Cur1->oppo;
    }
   Cur1 = Cur1->next;
  }
  return(edge);
}

// chain vertices

long pmChainVtx(pm_edge *Root)
{
  pm_edge *Cur1;
  long mark;
  pm_vertex *Vtx;
  long nb=1;

  mark = pmNewMark();

  Root->from->mark = mark;          
  Vtx = Root->from;
  Cur1 = Root;
  do{
    if (Cur1->oppo->mark == mark){
      Cur1 = Cur1->oppo;
    }else
    if (Cur1->oppo->from->mark != mark){
      Cur1->mark = mark;
      Cur1 = Cur1->oppo;
      Cur1->from->mark  = mark;
      Vtx->next = Cur1->from;
      Vtx = Cur1->from;
      nb++;
    }
    Cur1 = Cur1->next;
  }while (Cur1 != Root);
  Vtx->next = NULL;
  return(nb);
}

// chainage des faces lorsqu'elles existent

long pmChainFaces(pm_edge *Root)
{
  pm_edge *Cur1;
  long mark;
  pm_vertex *Face;
  long nb=1;

  mark = pmNewMark();

  Root->face->mark = mark;          
  Face = Root->face;
  Cur1 = Root;
  do{
    if (Cur1->oppo->mark == mark){
      Cur1 = Cur1->oppo;
    }else
    if (Cur1->oppo->face->mark != mark){
      Cur1->mark = mark;
      Cur1 = Cur1->oppo;
      Cur1->face->mark  = mark;
      Face->next = Cur1->face;
      Face = Cur1->face;
      nb++;
    }
    Cur1 = Cur1->Next;
  }while (Cur1 != Root);
  Face->next = NULL;
  return(nb);
}

// creation et chainage des faces d'une carte.

void pmMakeaFace(pm_edge *C1, pm_vertex *Fce, long mark, long *nbf){
  pm_edge *C2;
  Fce->mark = mark;
  Fce->label= ++(*nbf);
  C2 = C1->oppo->next;
  while(C2 != C1){
    C2->face = Fce;
    C2 = C2->oppo->next;
  }
}

long pmAddFaces(pm_edge *Root)
{
  pm_edge *Cur1;
  long mark;
  pm_vertex *Face;
  long nbf=0;
  
  mark = pmNewMark();
  
  Face = pmNewFace(Root);
  Cur1 = Root;
  pmMakeaFace(Cur1, Face, mark, &nbf);
  do{
    if (Cur1->oppo->mark == mark){
      Cur1 = Cur1->oppo;
    }else
    if (Cur1->oppo->face == NULL ||
	Cur1->oppo->face->mark != mark){
      Cur1->mark = mark;
      Cur1 = Cur1->oppo;
      Face->next = pmNewFace(Cur1);
      Face       = Face->next;
      pmMakeaFace(Cur1, Face, mark, &nbf);
    }
    Cur1 = Cur1->oppo->next;
  }while (Cur1 != Root);
  Face->next = NULL;
  return(nbf);
}



void pmLabelFaces(pm_vertex *Face){
  int i=1;
  for (; Face->next != NULL; Face = Face->next)
    Face->label = i++;
  Face->label =i;
}
  

void pmClearLblFace(pm_vertex *Face)
{
  do{
    Face->label = 0;
    Face = Face->next;
  }while(Face != NULL);
}
void pmClearLblVtx(pm_vertex *Vtx)
{
  do{
    Vtx->label = 0;
    Vtx = Vtx->next;
  }while(Vtx != NULL);
}





long pmBicolorFaces(pm_edge *Root)
{
  pm_edge *Cur1;
  long mark, nb=1;
  short t=1;
  
  mark = pmNewMark();
  
  Root->face->mark = mark;
  Root->face->type = t;
  Cur1 = Root;
  do{
    if (Cur1->oppo->mark == mark){
      Cur1 = Cur1->oppo;
      t = (t%2) +1;
    }else
    if (Cur1->oppo->face->mark != mark){
      Cur1->mark = mark;
      Cur1 = Cur1->oppo;
      t = (t%2) +1;
      Cur1->face->mark  = mark;
      Cur1->face->type = t;
      if (t%2) nb++;
    }
    Cur1 = Cur1->Next;
  }while (Cur1 != Root);
  return(nb);
}



void pmEdgeMap(pmMap *Map)
{
  pm_edge *Cur1;
  pm_edge *Root = Map->root;
  pm_vertex *Face = Root->face;
  pm_vertex *Vtx  = Root->from;
  short t = Face->type;

  //  bicolorFaces(Root);

  Root->prev = Root->Next;
  Root->from = Root->face;
  Cur1 = Root->Next;
  while(Cur1 != Root){
    Cur1->prev = Cur1->Next;
    Cur1->from->root = Cur1;
    Cur1->from = Cur1->face;
    Cur1 = Cur1->Next;
  }
  while(Face->next != NULL){
    Face = Face->next;
    if (Face->type == t){
      Face->root->prev = Face->root->Next;
      Face->root->from->root = Face->root;
      Face->root->from = Face->root->face;
      Cur1 = Face->root->Next;
      while(Cur1 != Face->root){
	Cur1->prev = Cur1->Next;
	Cur1->from->root = Cur1;
	Cur1->from = Cur1->face;
	Cur1 = Cur1->Next;
      }
    }
  }

  Vtx->type = DLTD;
  Root->face = Root->oppo->face;
  Root->face->root = Root;
  Root->oppo = Root->next->next;
  Root->next->next->face = Root->next->next->oppo->face;
  Root->next->next->face->root = Root->next->next;
  Root->next->next->oppo = Root;
  Root->next->type = DLTD; Root->next->next->next->type = DLTD;
  while(Vtx->next != NULL){
    Vtx = Vtx->next;
    Vtx->type = DLTD;
    Cur1 = Vtx->root;
    Cur1->face = Cur1->oppo->face;
    Cur1->face->root = Cur1;
    Cur1->oppo = Cur1->next->next;
    Cur1->next->next->face = Cur1->next->next->oppo->face;
    Cur1->next->next->face->root = Cur1->next->next;
    Cur1->next->next->oppo = Cur1;
    Cur1->next->type = DLTD; Cur1->next->next->next->type = DLTD;
  }
  
  Face = Root->from;
  Cur1 = Root->prev;
  Cur1->next = Root;
  while (Cur1 != Root){
    Cur1->prev->next = Cur1;
    Cur1 = Cur1->prev;
  }
  while (Face->next != NULL){
    Face = Face->next;
    if (Face->type == t){
      Cur1 = Face->root->prev;
      Cur1->next = Face->root;
      while (Cur1 != Face->root){
	Cur1->prev->next = Cur1;
	Cur1 = Cur1->prev;
      }
    }
  }
  Map->e = pmLabelCanon(Root);
  Map->v = pmChainVtx(Root);
  Map->f = pmChainFaces(Root);
  pmLabelFaces(Root->face);
}
    
//**************************************************************************
//PMextract.c

/* ici les procedures d'extraction :
   en fait c'est toujours la meme, on elimine les cycles
   maximaux de longueur k, 1<=k<=4.
*/


/**********************************************************************/
// elimination des 4-cocycles dans une 4-reguliere sans 2-cocycle


pm_edge *pmVide4cocycle(pm_edge *Root, pm_edge *Cot1, pm_edge *Cot2, pm_edge *Cot3)
{
  pm_edge *Curr, *Cur1, *Cur2, *Cur3, *Cur4,
       *Inn1, *Inn2, *Inn3, *Inn4;

  pm_vertex *Vtx = pmNewVtx(NULL);

  Cur1 = pmNewEdge(Vtx,NULL,NULL,Root->oppo,INNER); //remplace Root
  Cur2 = pmNewEdge(Vtx,NULL,Cur1,Cot1->oppo,INNER); //remplace Cot1
  Cur3 = pmNewEdge(Vtx,NULL,Cur2,Cot2->oppo,INNER);
  Cur4 = pmNewEdge(Vtx,Cur1,Cur3,Cot3->oppo,INNER);
  Vtx->root = Cur1; Vtx->label = Root->from->label;
  Cur1->next = Cur4;
  Cur1->prev = Cur2;
  Cur2->prev = Cur3;
  Cur3->prev = Cur4;
  Cur1->label = Root->label;
  Cur2->label = Cot1->label;
  Cur3->label = Cot2->label;
  Cur4->label = Cot3->label;
  // maintenant il faut cauteriser...
  // d'abord l'exterieur
  Vtx = pmNewVtx(NULL);
  Inn1 = pmNewEdge(Vtx,NULL,NULL,Root,INNER); // face Root
  Inn2 = pmNewEdge(Vtx,Inn1,NULL,Cot1,INNER);  
  Inn3 = pmNewEdge(Vtx,Inn2,NULL,Cot2,INNER);
  Inn4 = pmNewEdge(Vtx,Inn3,Inn1,Cot3,INNER);
  Vtx->root = Inn1; Vtx->label = Root->from->label+1;
  Inn1->prev = Inn4; Inn1->next = Inn2;
  Inn2->next = Inn3; Inn3->next = Inn4;
  Inn1->label = Root->oppo->label;
  Inn2->label = Cot1->oppo->label;
  Inn3->label = Cot2->oppo->label;
  Inn4->label = Cot3->oppo->label;
  Inn1->face = Root->oppo->face;
  Inn2->face = Cot1->oppo->face;
  Inn3->face = Cot2->oppo->face;
  Inn4->face = Cot3->oppo->face;
  Root->oppo = Inn1; Cot1->oppo = Inn2;
  Cot2->oppo = Inn3; Cot3->oppo = Inn4;
  Root->face->root = Root; Cot1->face->root = Cot1;
  Cot2->face->root = Cot2; Cot3->face->root = Cot3;
  // puis l'interieur
  Cur1->oppo->oppo = Cur1;
  Cur2->oppo->oppo = Cur2;
  Cur3->oppo->oppo = Cur3;
  Cur4->oppo->oppo = Cur4;
  Cur1->face = pmNewFace(Cur1); Cur1->face->label = Root->face->label;
  for(Curr = Cur1->Next; Curr != Cur1; Curr = Curr->Next)
    Curr->face = Cur1->face;
  Cur2->face = pmNewFace(Cur2); Cur2->face->label = Cot1->face->label;
  for(Curr = Cur2->Next; Curr != Cur2; Curr = Curr->Next)
    Curr->face = Cur2->face;
  Cur3->face = pmNewFace(Cur3); Cur3->face->label = Cot2->face->label;
  for(Curr = Cur3->Next; Curr != Cur3; Curr = Curr->Next)
    Curr->face = Cur3->face;
  Cur4->face = pmNewFace(Cur4); Cur4->face->label = Cot3->face->label;
  for(Curr = Cur4->Next; Curr != Cur4; Curr = Curr->Next)
    Curr->face = Cur4->face;

  return(Cur1);
}


// on commence par une procedure pour decomposer en somme horiz.
// si la composante est indecomposable on renvoie PMTRUE, sinon PMFALSE
// et les composantes resultantes sont ajoutees dans Fut

int pmInSum(pm_edge *Root){
  pm_edge *Edge, *Edg1, *Edg2;
  long mark = pmNewMark();
  short indec = PMTRUE;

  Edg1 = Root;
  Edg2 = Root->next;
  
  // marquage des faces etiquetees 2 et detection des diagonales
  for(Edge = Root->Next;
      Edge != Root->Prev;
      Edge = Edge->Next){
    Edge->oppo->face->mark = mark;
    Edge->oppo->face->root = Edge->Oppo;
  }
      
  for(Edge = Root->next->oppo->Prev;
      Edge != Root->next->next;
      Edge = Edge->Prev){
    if (Edge->Oppo->face->mark == mark){
      // diagonal detected
      pmNewBloc(pmVide4cocycle(Edg1, Edge->oppo->face->root, Edge, Edg2));
      Edg1 = Edge->oppo->face->root->oppo;
      Edg2 = Edge->oppo;
      indec = PMFALSE;
    } else
      Edge->oppo->face->mark = mark;
  }
  if (!indec)
    pmNewBloc(pmVide4cocycle(Edg1, Root->prev, Root->next->next, Edg2));
  return(indec);  
}

// formerly local function

int pmCheck1(pm_edge *Edge){
    long label = pmNewLabel();
    pm_edge *Edg1, *Edg2;

    for (Edg1 = Edge->Next; Edg1->from->label == 0;
	 Edg1 = Edg1->Next){
      Edg1->oppo->face->label = label;
      Edg1->oppo->face->root  = Edg1->oppo;
    }
    for (Edg1 = Edge->oppo->Prev; Edg1->from->label == 0;
	 Edg1 = Edg1->Prev){;}
    for (; Edg1->oppo->from->label == 0;
	 Edg1 = Edg1->Next){
      for (Edg2 = Edg1->oppo->Prev;
	   Edg2->from->label == 0 && Edg2 != Edg1->oppo->Next;
	   Edg2 = Edg2->Prev){;}
      for (; Edg2 != Edg1->oppo; Edg2 = Edg2->Next)
	if (Edg2->oppo->face->label == label &&
	    Edge->oppo->from != Edg2->oppo->from &&
	    Edge->from != Edg2->from){
	  pmNewBloc(pmVide4cocycle(Edge,Edg2->oppo->face->root,Edg2,Edg1));
	  return(1);
	}
    }
    return(0);  printf("kes tu fous la dans check1 ??\n");
}

void pmC3kernel(pm_edge *Root){

  pm_edge *Edge, *Edg1;

  long level;
  short i;
  long mark = pmNewMark();


  pmResetPost();
  level = 1;
  Root->from->label = level;
  for (i=0, Edge = Root; i < 4; i++, Edge = Edge->next) 
    pmNewPost(Edge);
  while(pmIsPost()){
    pmCopyPostSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      pmCheck1(Edge);
    }
    level++;
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      if (Edge->from->label == level - 1 &&
		  //Edge->oppo->from->label == 0);  //hub supp ;
		  Edge->oppo->from->label == 0) 
      Edge->oppo->from->label = level;
    }
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      for (Edg1 = Edge->oppo->next; Edg1 != Edge->oppo; Edg1 = Edg1->next)
	  if (Edg1->from->label == level &&
	      Edg1->oppo->from->label == 0 &&
	      Edg1->mark != mark){
	    Edg1->mark = mark;
	    pmNewPost(Edg1);
	  }
    }
    //     printf("%ld -> %ld\n",SeedEnd,PostEnd);
  }
}

void pmFull2to3c(pm_edge *Root){
  pm_edge *Edge;

  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);
  
  pmNewBloc(Root);
  while(pmIsBloc()){
    Edge = pmNextBloc();
    if (Edge->oppo->from != Edge->next->next->oppo->from){
      // non trivial
      if (pmInSum(Edge) && pmInSum(Edge->next)){
	// vraiment indecomposable
	pmNewComp(Edge);    
	pmC3kernel(Edge);
      }
    }
  }
}

pm_edge *pmGet3c(pm_edge *Root){
  
  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);

  if (pmInSum(Root) && pmInSum(Root->next)){
    pmC3kernel(Root);
    return(Root);
  }else
    return(NULL);
}

/**********************************************************************/
/*************************************************************/
/**********************************************************************/

// Elimination des 2cocycles dans les cubiques

pm_edge *pmVide2cocycle(pm_edge *Root, pm_edge *Cot1)
{
  pm_edge *Curr, *Inn1, *Inn2;
    
  Inn1 = Root->oppo;
  Inn2 = Cot1->oppo;

  Inn1->oppo = Inn2;
  Inn2->oppo = Inn1;
  Root->oppo = Cot1;
  Cot1->oppo = Root;

  Inn1->face = pmNewFace(Inn1);
  Inn2->face = pmNewFace(Inn2);

  Inn1->face->label = Cot1->face->label;
  for(Curr = Inn1->Next; Curr != Inn1; Curr = Curr->Next)
    Curr->face = Inn1->face;

  Inn2->face->label = Root->face->label;
  for(Curr = Inn2->Next; Curr != Inn2; Curr = Curr->Next)
    Curr->face = Inn2->face;

  return(Inn1);  
}

// formerly local fnct
int pmCheck2(pm_edge *Edge){

  pm_edge *Edg1;

  Edge->face->root = Edge;
  for (Edg1 = Edge->oppo->Next; Edg1 != Edge->oppo; Edg1 = Edg1->Next)
    if (Edg1->oppo->face->root == Edge)
      pmNewBloc(pmVide2cocycle(Edge,Edg1));
  return(1);
}

void pmTri3kernel(pm_edge *Root){
  
  pm_edge *Edge, *Edg1;
  
  long level;
  short i;
  long mark = pmNewMark();


  pmResetPost();
  level = 1;
  Root->from->label = level;
  for (i=0, Edge = Root; i < 3; i++, Edge = Edge->next)
    pmNewPost(Edge);
  while(pmIsPost()){
    pmCopyPostSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      pmCheck2(Edge);
    }
    level++;
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      if (Edge->from->label == level - 1 &&
	  //Edge->oppo->from->label == 0); //hub
	  Edge->oppo->from->label == 0)
      Edge->oppo->from->label = level;
    }
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      for (Edg1 = Edge->oppo->next; Edg1 != Edge->oppo; Edg1 = Edg1->next)
	  if (Edg1->from->label == level &&
	      Edg1->oppo->from->label == 0 &&
	      Edg1->mark != mark){
	    Edg1->mark = mark;
	    pmNewPost(Edg1);
	  }
    }
    //     printf("%ld -> %ld\n",SeedEnd,PostEnd);
  }
}



pm_edge *pmGet3tri(pm_edge *Root){

  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);
  
  pmTri3kernel(Root);
  return(Root);
}

void pmFull2to3tri(pm_edge *Root){
  pm_edge *Edge;

  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);
  
  pmNewBloc(Root);
  while(pmIsBloc()){
    Edge = pmNextBloc();
    pmNewComp(Edge);    
    if (Edge->oppo->from != Edge->next->oppo->from ||
        Edge->oppo->from != Edge->prev->oppo->from){
      // non reduit a un triangle
      pmTri3kernel(Edge);
    }
  }
}



/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

// Elimination des 3cocycles dans les cubiques

pm_edge *pmVide3cocycle(pm_edge *Root, pm_edge *Cot1, pm_edge *Cot2)
{
  pm_edge *Curr, *Cur1, *Cur2, *Cur3,
       *Inn1, *Inn2, *Inn3;
    
  pm_vertex *Vtx = pmNewVtx(NULL);
    
  Cur1 = pmNewEdge(Vtx,NULL,NULL,Root->oppo,INNER); //remplace Root
  Cur2 = pmNewEdge(Vtx,NULL,Cur1,Cot1->oppo,INNER); //remplace Cot1
  Cur3 = pmNewEdge(Vtx,Cur1,Cur2,Cot2->oppo,INNER);
  Vtx->root = Cur1; Vtx->label = Root->from->label;
  Cur1->next = Cur3;
  Cur1->prev = Cur2;
  Cur2->prev = Cur3;
  Cur1->label = Root->label;
  Cur2->label = Cot1->label;
  Cur3->label = Cot2->label;
  // maintenant il faut cauteriser...
  // d'abord l'exterieur
  Vtx = pmNewVtx(NULL);
  Inn1 = pmNewEdge(Vtx,NULL,NULL,Root,INNER); // face Root
  Inn2 = pmNewEdge(Vtx,Inn1,NULL,Cot1,INNER);
  Inn3 = pmNewEdge(Vtx,Inn2,Inn1,Cot2,INNER);
  Vtx->root = Inn1; Vtx->label = Root->from->label+1;
  Inn1->prev = Inn3; Inn1->next = Inn2; Inn2->next = Inn3; 
  Inn1->label = Root->oppo->label;
  Inn2->label = Cot1->oppo->label;
  Inn3->label = Cot2->oppo->label;
  Inn1->face = Root->oppo->face;
  Inn2->face = Cot1->oppo->face;
  Inn3->face = Cot2->oppo->face;
  Root->oppo = Inn1; Cot1->oppo = Inn2; Cot2->oppo = Inn3;
  // puis l'interieur
  Cur1->oppo->oppo = Cur1;
  Cur2->oppo->oppo = Cur2;
  Cur3->oppo->oppo = Cur3;
  Cur1->face = pmNewFace(Cur1); Cur1->face->label = Root->face->label;
  for(Curr = Cur1->Next; Curr != Cur1; Curr = Curr->Next)
    Curr->face = Cur1->face;
  Cur2->face = pmNewFace(Cur2); Cur2->face->label = Cot1->face->label;
  for(Curr = Cur2->Next; Curr != Cur2; Curr = Curr->Next)
    Curr->face = Cur2->face;
  Cur3->face = pmNewFace(Cur3); Cur3->face->label = Cot2->face->label;
  for(Curr = Cur3->Next; Curr != Cur3; Curr = Curr->Next)
    Curr->face = Cur3->face;

  return(Cur1);  
}

// formerly local fct
int pmCheck3(pm_edge *Edge){
    long label = pmNewLabel();
    pm_edge *Edg1;

    Edge->face->root = Edge;
    for (Edg1 = Edge->oppo->Next; Edg1 != Edge->oppo; Edg1 = Edg1->Next)
      if (Edg1->oppo->face->root == Edge)
	pmNewBloc(pmVide2cocycle(Edge,Edg1));
    
    for (Edg1 = Edge->Next; Edg1->from->label == 0;
	 Edg1 = Edg1->Next){
      Edg1->oppo->face->label = label;
      Edg1->oppo->face->root  = Edg1->oppo;
    }
    for (Edg1 = Edge->oppo->Prev; Edg1->from->label == 0;
	 Edg1 = Edg1->Prev){;}
    for (; Edg1 != Edge->oppo->Prev; Edg1 = Edg1->Next)
      if (Edg1->oppo->face->label == label &&
	  Edge->from != Edg1->from){
	pmNewBloc(pmVide3cocycle(Edge,Edg1->oppo->face->root,Edg1));
	return(1);
      }
    return(0);  printf("kes tu fous la dans check3 ??\n");
}

void pmTri4kernel(pm_edge *Root){
  
  pm_edge *Edge, *Edg1;
  
  long level;
  short i;
  long mark = pmNewMark();

  pmResetPost();
  level = 1;
  Root->from->label = level;
  for (i=0, Edge = Root; i < 3; i++, Edge = Edge->next) 
    pmNewPost(Edge);
  while(pmIsPost()){
    pmCopyPostSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      pmCheck3(Edge);
    }
    level++;
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      if (Edge->from->label == level - 1 &&
	  //Edge->oppo->from->label == 0); hub
	  Edge->oppo->from->label == 0)
      Edge->oppo->from->label = level;
    }
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      for (Edg1 = Edge->oppo->next; Edg1 != Edge->oppo; Edg1 = Edg1->next)
	  if (Edg1->from->label == level &&
	      Edg1->oppo->from->label == 0 &&
	      Edg1->mark != mark){
	    Edg1->mark = mark;
	    pmNewPost(Edg1);
	  }
    }
    //     printf("%ld -> %ld\n",SeedEnd,PostEnd);
  }
}


void pmFull2to4tri(pm_edge *Root){
  pm_edge *Edge;
  
  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);

  pmNewBloc(Root);
  while(pmIsBloc()){
    Edge = pmNextBloc();
    pmNewComp(Edge);
    if (Edge->oppo->from != Edge->next->oppo->from ||
        Edge->oppo->from != Edge->prev->oppo->from){
      // non reduit a un triangle
      //      pmNewComp(Edge);
      pmTri4kernel(Edge);
    }
  }
}



pm_edge *pmGet4tri(pm_edge *Root){
  
  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);

  pmTri4kernel(Root);
  return(Root);
}





/**********************************************************************/
/**********************************************************************/
/**********************************************************************/



// Elimination des 2cocycles des 4regular

pm_edge *pmVide2cocycle4r(pm_edge *Root, pm_edge *Cot1)
{
  pm_edge *Curr, *Inn1, *Inn2;

  Inn1 = Root->oppo;
  Inn2 = Cot1->oppo;

  Inn1->oppo = Inn2;
  Inn2->oppo = Inn1;
  Root->oppo = Cot1;
  Cot1->oppo = Root;

  Inn1->face = pmNewFace(Inn1); 
  Inn2->face = pmNewFace(Inn2);

  Inn1->face->label = Cot1->face->label;
  for(Curr = Inn1->Next; Curr != Inn1; Curr = Curr->Next)
    Curr->face = Inn1->face;

  Inn2->face->label = Root->face->label;
  for(Curr = Inn2->Next; Curr != Inn2; Curr = Curr->Next)
    Curr->face = Inn2->face;
  
  return(Inn1);
}




int pmCheck4(pm_edge *Edge){

  pm_edge *Edg1;
  
  Edge->face->root = Edge;
  for (Edg1 = Edge->oppo->Next; Edg1 != Edge->oppo; Edg1 = Edg1->Next)
    if (Edg1->oppo->face->root == Edge)
      pmNewBloc(pmVide2cocycle4r(Edge,Edg1));
  return(1);
}


void pmSimplekernel(pm_edge *Root){
  
  pm_edge *Edge, *Edg1;
  
  long level;
  short i;
  long mark = pmNewMark();

  pmResetPost();
  level = 1;
  Root->from->label = level;
  for (i=0, Edge = Root; i < 4; i++, Edge = Edge->next) 
    pmNewPost(Edge);
  while(pmIsPost()){
    pmCopyPostSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      pmCheck4(Edge);
    }
    level++;
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      if (Edge->from->label == level - 1 &&
	  //hub Edge->oppo->from->label == 0);
	  Edge->oppo->from->label == 0)
      Edge->oppo->from->label = level;
    }
    pmFirstSeed();
    while(pmIsSeed()){
      Edge = pmNextSeed();
      for (Edg1 = Edge->oppo->next; Edg1 != Edge->oppo; Edg1 = Edg1->next)
	  if (Edg1->from->label == level &&
	      Edg1->oppo->from->label == 0 &&
	      Edg1->mark != mark){
	    Edg1->mark = mark;
	    pmNewPost(Edg1);
	  }
    }
    //     printf("%ld -> %ld\n",SeedEnd,PostEnd);
  }
}



pm_edge *pmGetsimple4r(pm_edge *Root){

  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);
  
  pmSimplekernel(Root);
  return(Root);
}

void pmFull4rtosimple(pm_edge *Root){
  pm_edge *Edge;

  pmClearLblFace(Root->face);
  pmClearLblVtx(Root->from);
  
  pmNewBloc(Root);
  while(pmIsBloc()){
    Edge = pmNextBloc();
    pmNewComp(Edge);    
    if (Edge->from != Edge->oppo->from ||
        Edge->next->next->from != Edge->next->next->oppo->from){
      // non reduit a un sommet et 2 boucles
      //      pmNewComp(Edge);    
      pmSimplekernel(Edge);
    }
  }
}


/******************************/
/* This function parses the command line */
/******************************/
int pmParseArgs(int argc, char *argv[],
		pmSize *Size, pmMethod *Meth,
		pmOutput *Outp)
{
  long param;
  char modifier;
  long i,j;

  /* Size preinit */
  Size->m = 5; Size->m = 5;
  Size->e = 0; Size->v = 0; Size->f = 0;
  Size->r = 0; Size->g = 0; Size->d = 0;
  Size->t = -1; Size->dgArr = NULL;
  /* Meth preinit */
  Meth->core = 0; Meth->pic = 0;
  Meth->seed = 1;
  Meth->verbose = 0;

  /* main loop to parse args */

  for (i = 1; i < argc; i++){

    /* numerical arguments */

    if (sscanf(argv[i],"-%c%ld", &modifier, &param) == 2){
      switch(modifier){
      case 'C': //cubic maps
	if (param == 2){ // dually 2-connected
	  printvf("# 2-edge-connected cubic maps\n");
	  Size->m = 1; Size->b = 1;
	}else if (param == 3){ // dually 3-connected
	  printvf("# 3-edge-connected cubic maps\n");
	  Size->m = 2; Size->b = 1;
	}else if (param == 4){ // dually 4-connected
	  printvf("# 4-edge-connected cubic maps\n");
	  Size->m = 3; Size->b = 1;
	}else{
	  fprintf(stderr,"unknown kind of cubic\n");
	  exit(2);
	}break;
      case 'Q': //4-regular maps
	if (param == 1){
	  printvf("# 2-edge-connected quartic maps\n");
	  Size->m = 4; Size->b = 4;
	}else if (param == 2){
	  printvf("# 4-edge-connected quartic maps\n");
	  Size->m = 5; Size->b = 5;
	}else if (param == 3){
	  printvf("# 6-edge-connected quartic maps\n");
	  Size->m = 6; Size->b = 5;
	}else if (param == 4){
	  printvf("# bi-quartic maps");
	  Size->m = 9; Size->b = 9;
	}else{
	  fprintf(stderr,"unknown kind of quartic\n");
	  exit(2);
	}break;
      case 'M': //general maps
	Outp->transform = 1;
	if (param == 1){
	  printvf("# general map\n");
	  Size->m = 4; Size->b = 4;
	}else if (param == 2){
	  printvf("# nonseparable\n");
	  Size->m = 5; Size->b = 5;
	}else if (param == 3){
	  printvf("# 3-connected\n");
	  Size->m = 6; Size->b = 5;
	}else if (param == 4){
	  printvf("# bipartite bicolor\n");
	  Size->m = 9; Size->b = 9;
	}else if (param == 7){
	  printvf("# simple nonseparable\n");
	  Size->m = 13; Size->b = 5;
	}else if (param == 8){
	  printvf("# loopless maps\n");
	  Size->m = 11; Size->b = 4;
	}else if (param == 9){
	  printvf("# loopless simple maps\n");
	  Size->m = 12; Size->b = 4;
	}else{
	  fprintf(stderr,"unknown kind of map\n");
	  exit(2);
	}break;
      case 'B': //bipartie
	if (param == 1){ // 1-c
	  printvf("# 2-edge-connected bipartite cubic\n");
	  Size->m = 7; Size->b = 7;
	}else if (param == 2){ // 2-c
	  printvf("# 3-edge-connected bipartite cubic\n");
	  Size->m = 8; Size->b = 7;
	}else{
	  fprintf(stderr,"unknown kind of bicubic\n");
	  exit(2);
	}break;
      case 'D': // given degrees
	if (param > 0) { // max degree
	  printvf("# eulerian with reduced degree distribution:");
	  Size->dgArr = (long *)calloc(param, sizeof(long));
	  for (j=0; j < param; j++){
	    if (sscanf(argv[++i],"%ld",Size->dgArr+j)){
	      printvf("#  %ld ", Size->dgArr[j]);
	    } else {
	      fprintf(stderr,"\nincoherent degree distribution\n");
	      exit(2);
	    }
	  }
	  printvf("# \n");
	  Size->d = param;
	  Size->m = 10; Size->b = 10;
	}

      case 'E': // number of edges
	if (param > 0){
	  printvf("# Number of edges: %ld\n",param);
	  Size->e = param;
	}break;
      case 'V': // number of vertices
	if (param > 0){
	  printvf("# Number of vertices: %ld\n",param);
	  Size->v = param;
	}break;

      case 'I': // error allowed on size
	if (param >=0){
	  printvf("# error allowed on size: %ld\n",param);
	  Size->t = param;
	}break;

      break;

      case 'X': // given seed of random generator
	Meth->seed = param;
	break;
      default :
	printvf("# unused arg -%c%ld\n", modifier, param);
	break;
      }
    }
  }
  return(PMTRUE);
}


//**************************************************************************
//PMplanmap.c
/******************************/
/* This function checks and completes parameters with defaults */
/******************************/
int pmSetParameters(pmSize *Size, pmMethod *Meth)
{
  long i;

  /* Default parameters */

  if (Size->m == 0) {                 // type of maps generated 
    Size->m = 5; Size->b = 5;          // Q2 : 2-c quartic
    printvf("# 2-c quartic by default\n");
  }

  // compute face, vertice and edge numbers...

  if (Size->e + Size->v + Size->f + Size->r + Size->g == 0 &&
      Size->dgArr == NULL){
    fprintf(stderr,"Size must be given somehow\n");
    return(PMFALSE);
  }

  switch(Size->m){

  case 1:                          // all type of cubic maps
  case 2:
  case 3:
  case 7:
  case 8:
    if (Size->v){
      if (Size->v % 2 == 0){
	Size->e = 3 * Size->v / 2;
	Size->f  = Size->e + 2 - Size->v;
      }else{
	fprintf(stderr,"nb vertex must be even for cubic\n");
	return(PMFALSE);
      }
    }else if (Size->f){
      Size->e = 3 * Size->f - 6;
      Size->v = 2 * Size->e / 3;
    }else if (Size->e){
      if (Size->e % 3 == 0){
	Size->f = Size->e / 3 - 2;
	Size->v = 2*Size->e / 3;
      }else{
	fprintf(stderr,"nb edges must be multiple of three for bicubic\n");
	return(PMFALSE);
      }
    }else{
      fprintf(stderr,"degrees or colors not available for cubic\n");
      return(PMFALSE);
    }break;

  case 9:                     // all type of quartic
    if (Size->v % 2 != 0){
      fprintf(stderr,"vtx number must be even for bi-quartic\n");
      return(PMFALSE);
    }// no break here !
  case 4:
    if (Size->g || Size->r){
      fprintf(stderr,"color control only for 2-c or 3-c quartic\n");
      return(PMFALSE);
    }// no break here !
  case 5:
  case 6: 
    if (Size->g && Size->r){
      Size->f = Size->g + Size->r;
      Size->v = Size->f - 2;
      Size->e = 2 * Size->v;
    }else if (Size->v){
      Size->f = Size->v + 2;
      Size->e = 2 * Size->v;
    }else if (Size->e){
      if (Size->e % 2 == 0){
	Size->v = Size->e / 2;
	Size->f = Size->v + 2;
      }else{
	fprintf(stderr,"nb edges must be even for quartic\n");
	return(PMFALSE);
      }
    }else if (Size->f){
      Size->v = Size->f - 2;
      Size->e = 2 * Size->v;
    }
    if (Size->g) Size->r = Size->f - Size->g;
    if (Size->r) Size->g = Size->f - Size->r;
    if (Size->g < 0 || Size->r < 0){
      fprintf(stderr,"not enough faces for colors \n");
      return(PMFALSE);
    }break;

  case 10:                         // eulerian
    if (Size->g || Size->r){
      fprintf(stderr,"color control not implemented for eulerian\n");
      return(PMFALSE);
    }else{
      Size->e = 0; 
      Size->v = 0;
      for (i=0; i<Size->d; i++){
	Size->v = Size->v + Size->dgArr[i];
	Size->e = Size->e + (i+1) * Size->dgArr[i];
      }
      Size->e = Size->e / 2;
      Size->f = Size->e + 2 - Size->v;
    }
  }
  printvf("# Edges: %ld ; Faces: %ld ; Vtx: %ld ; Greens: %ld ; Reds: %ld\n",
	  Size->e, Size->f, Size->v, Size->g, Size->r);
  
  if (Size->m == 2 || Size->m == 3 || // extraction methods for core maps
      Size->m == 6 || Size->m == 8) {     
    if (!Meth->core) Meth->core = 2;  // largest component enabled
    if (!Meth->pic)  Meth->pic  = 1;  // pic optimization enabled
    if (Size->t == -1)                // default tolerence n^(2/3)
      Size->t = (long) exp(2*log((double)Size->v)/3);
    printvf("# Size interval: %ld, %ld\n",
	    Size->v - Size->t, Size->v + Size->t);
  }
  

  return(PMTRUE);
}


/******************************/
/* this function pre-compute the basic memory requirements */
/******************************/
int pmMemoryInit(pmSize *S, pmMethod *Meth, pmMemory *M)
{

  switch(S->m){

  case 1:                            // cubic
  case 2:
  case 3:
    M->dTree = 3;                    // uses a ternary tree T
                                     // nb nodes in T
    if (S->m == 1)                   //    for dually 2-c
      M->sTree = S->v / 2;              
    else if (S->m == 2){             //    for dually 3-c
      if (Meth->pic == 1) M->sTree = (long) ((S->v)+2-0.77*exp(2*log((double)S->v)/3));
      else M->sTree = S->v;
    }else if (S->m == 3){             //    for dually 4-c
      if (Meth->pic == 1) M->sTree = (long) (2*(S->v)+2-0.77*exp(2*log((double)S->v)/3));
      else M->sTree = S->v * 2;
    }
    M->sWrd  = 3 * M->sTree + 1;     // nb letters in word
    M->sEdge = 8 * M->sTree + 2;     // nb half edges used
    M->sVtx  = 4 * M->sTree + 2;     // nb vertices+faces
    M->sLeaf = 2 * M->sTree + 2;     // nb leaf in stack for balance
    break;

  case 4:                            // quartic
    M->dTree = 2;                    // uses a binary tree T
    M->sTree = S->v;                 // nb nodes in T
    M->sWrd  = 2* M->sTree + 1;      // nb letters in word
    M->sEdge = 4* M->sTree + 2;      // nb half edges used
    M->sVtx  = 2* M->sTree + 3;      // nb vtx+faces
    M->sLeaf = 2* M->sTree + 2;      // nb leaf in stack for balance
    break;
    
  case 5:                                 // quartic 2-c and 3-c
  case 6:
    M->dTree = 3;                         // uses a ternary tree T
                                          // nb nodes in T
    if (S->m == 5){                       //   for 2-c
      if (S->r){                          //     with color control
	M->rTree = S->r - 1;
	M->gTree = S->g - 2;
      }else M->rTree = M->gTree = 0;
      M->sTree = S->v - 1;                //     without 
    }else if (S->m == 6){                 //   for 3-c
      if (Meth->pic == 1) M->sTree =(long)(3*(S->v)-1.22*exp(2*log(3.*S->v)/3)); 
      else M->sTree = 3 * S->v;
      if (S->r){                          //   with color control
	M->rTree = S->r * 3;                 //   (pic correction non ready)
	M->gTree = S->g * 3;
	M->sTree = S->v * 3;
      }else M->rTree = M->gTree = 0;
    }
    if (S->r) M->sWrd = 6 * M->sTree + 3; // nb letters in word
    else      M->sWrd = 3 * M->sTree + 1;
    M->sEdge = 6 * M->sTree + 2;          // nb half edges used
    M->sVtx  = 2 * M->sTree + 4;          // nb vertices+faces used
    M->sLeaf = 4 * M->sTree;              // nb leaf in stack for balance
    break;

  case 7:                               // bicubic
  case 8:
    M->dTree = 2;                       // uses a binary tree T
                                        // nb nodes in T
    if (S->m == 7)                      //   for 1-c
      M->sTree = S->v / 2; 
    else if (S->m == 8){                //   for 2-c
      if (Meth->pic == 1) 
	M->sTree = (long) (9*(S->v)/5+2-0.77*exp(2*log(9.*S->v/5)/3));
      else M->sTree = 9*(S->v)/5+2;
    }
    M->sWrd  = 2 * M->sTree + 1;        // nb letters in word
    M->sEdge = 6 * M->sTree;            // nb half edges used
    M->sVtx  = 3 * M->sTree + 2;        // nb vertices+faces 
    M->sLeaf = 2 * M->sTree + 1;        // nb leaf in stack for balance
    break;
  case 9:                               //biquartic
    M->dTree = 3;                       // uses a ternary tree T
    M->sTree = S->v / 2;
    M->sWrd  = 3 * M->sTree + 1;
    M->sEdge = 8 * M->sTree;
    M->sVtx  = 4 * M->sTree + 2;
    M->sLeaf = 4 * M->sTree + 1;
    break;
  default:
    fprintf(stderr,"unknown type of map %d", (int) S->m);
    return(PMFALSE);
  }
  printvf("# Size of tree: %ld\n", M->sTree);
  printvf("# Memory       : %ld vtx, %ld edgs\n", M->sVtx, M->sEdge);

  return(PMTRUE);
}

/******************************/
/* this function extends the number of edges created in case of extraction */
/******************************/
int pmExtendMemory(pmSize *S, pmMethod *Meth, pmMemory *M, char OtherReason)
{
  char map = S->m;
  printvf("# Memory (extd) map:%d:\n",S->m); 
  if (OtherReason == PMTRUE &&
      (map == 1 || map == 2 || map == 4 || map == 5 || map == 7)) map++;

  switch(map){                   // cubic 2c
  case 2:
    M->sVtx  = 2 * M->sVtx;
    break;
  case 3:                        // cubic 3c
    M->sEdge = 8 * M->sEdge;
    M->sVtx  = 8 * M->sVtx;
    break;
  case 5:                        // quartic 2-c
    M->sVtx  = 2 * M->sVtx;
    break;
  case 6:                        // quartic 3-c
    M->sEdge = 8 * M->sEdge;
    M->sVtx  = 8 * M->sVtx;
    break;
  case 8:                        // bicubic
    M->sVtx  = 2 * M->sVtx;
    break;
  }
  printvf("# Memory (extd): %ld vtx, %ld edgs\n", 
	  M->sVtx, M->sEdge);

  return(PMTRUE);
}

/******************************/
/* This function uses conjugation of tree to sample a map */
/******************************/
int pmTreeConjugation(pmSize *S, pmMemory *M,
		      pmMap *Map)
{
  long pos;
  char *Wrd;
  pmStck Stack;
  pm_edge *Root;
    

  pmCreateWrd(M->sWrd, &Wrd);     // allocation of the word

  if (S->b == 5 && M->rTree)      // generation of a random word
    pos = pmLuka3(M->rTree, M->gTree, Wrd);
  else                          
    pos = pmLuka1(M->sTree, M->dTree, Wrd);


  pmCreateEdge(M->sEdge);         // allocation of edges 
  pmCreateVtx(M->sVtx);           // and vertices in global variables

  if (S->b == 5 && M->rTree)      // transformation word -> tree
  Root = pmChottin2tree(pos, Wrd);
  else
  Root = pmLuka2tree(pos, Wrd);

  pmFreeWrd(Wrd);                   // finished with the word

  
  switch(S->b){                   // adjunction of buds
  case 1: pmSpring4(Root); break;
  case 4: pmSpring1(Root); break;
  case 5: pmSpring3(Root); break;
  case 7: pmSpring2(Root); break;
  case 9: pmSpring5(Root); break;
  }

  Root = pmBalance(Root);           // the conjugation

  pmCreateStck(M->sLeaf, &Stack);   // the closure, using a stack
  Root = pmClosure(Root, &Stack);
  Root = pmSuppress(Root);
  pmFreeStck(Stack);      

  Map->e = pmLabelCanon(Root);      // set labels and create faces
  Map->v = pmChainVtx(Root);
  Map->f = pmAddFaces(Root);
  Map->root = Root;


  return(PMTRUE);
}

/******************************/
/* this function extract the core or largest component */
/******************************/
int pmExtract(pmSize *S, pmMethod *Meth, pmMemory *M, pmMap *Map)
{
  pm_edge *Root, *maxEdge;
  long maxVnb, maxEnb;

  Root = Map->root;

  pmCreatePost(M->sEdge); pmCreateSeed(M->sEdge);   // compoment lists
  pmCreateBloc(M->sEdge); pmCreateComp(M->sEdge);

  if (Meth->core == 1){                     // core extraction method
    switch (S->m){
    case 2:
    case 8:
      Root = pmGet3tri(Root); break;
    case 3:
      Root = pmGet4tri(Root); break;
    case 6:
      Root = pmGet3c(Root); break;
    }
    if (Root != NULL){
      Map->e = pmLabelCanon(Root);
      Map->v = pmChainVtx(Root);
      Map->f = pmChainFaces(Root);
      pmLabelFaces(Map->root->face);
    }else  
      Map->v = 0;
    Map->root = Root;
  }

  if (Meth->core == 2){                      // largest component method
    switch (S->m){
    case 2:
    case 8:
      pmFull2to3tri(Root); break;            // all components are extracted
    case 3:
      pmFull2to4tri(Root); break;
    case 6:
      pmFull2to3c(Root); break;
    }

    maxVnb = 0; maxEnb = 0; maxEdge = Root;  // the largest is selected
    pmFirstComp();
    while(pmIsComp()){
      Root = pmNextComp();
      Map->e = pmLabelCanon(Root);
      Map->v = pmChainVtx(Root);
      if (Map->v >= maxVnb){
	maxVnb = Map->v;
	maxEnb = Map->e;
	maxEdge = Root;
      }
    }
    Map->v = maxVnb;
    Map->e = maxEnb;
    Map->root = maxEdge;
    Map->f = pmChainFaces(Map->root);
    pmLabelFaces(Map->root->face);
  }
  pmFreeBloc();pmFreePost();pmFreeSeed();pmFreeComp();
  return 0;
}


/******************************/
/* this function is the main sampling function */
/******************************/
int pmPlanMap(pmSize *S, pmMethod *Meth, pmMemory *M, pmMap *Map)
{
  long numTry;
  
  if ((S->m == 1 || S->m == 4 ||                    // basic families
       S->m == 5 || S->m == 7 || S->m == 9))
    pmTreeConjugation(S, M, Map);

  else if (S->m == 2 || S->m == 3 ||                // extracted families
	   S->m == 6 || S->m == 8){
    numTry = 0;
    do{
      pmTreeConjugation(S, M, Map);
      pmExtract(S, Meth, M, Map);
      if (Map->v < S->v - S->t || Map->v > S->v + S->t){
	pmFreeEdge(); pmFreeVtx();
      }
      numTry++;
    }while (Map->v < S->v - S->t || Map->v > S->v + S->t);
    printwf("# NbTry%ld = %ld; Final Size = %ld;\n",Map->i, numTry, Map->v);
  }
  
  return(PMTRUE);
}

int pmFreeMap(pmMap *Map)
{
  Map->root=NULL;
  pmFreeEdge();
  pmFreeVtx();
  return (PMTRUE);
}


//**************************************************************************
GraphContainer *GenerateSchaeffer(int n_ask,int type,int e_connectivity
                                  ,bool randomEraseMultipleEdges,bool randomUseGeneratedCir)
//n_ask must be even
  {pmSize Sizes;
  pmMethod Meth;
  pmMemory Mem;
  pmMap Map;
  pmOutput Outp;
  Outp.transform=0;

  /* Sizes preinit */
  Sizes.m = 5; 
  Sizes.e = 0; Sizes.v = 0; Sizes.f = 0;
  Sizes.r = 0; Sizes.g = 0; Sizes.d = 0;
  Sizes.t = -1; Sizes.dgArr = NULL;
  /* Meth preinit */
  Meth.core = 0; Meth.pic = 0;
  Meth.verbose = 0;
  //Allowed approximation on n,m
  Sizes.t = -1;
  // random
  randomStart();
  char t[256];
  bool loops = false;
  bool multiple = false;
  if(type == 3)// Cubic
     {Sizes.b = 1;
     if(e_connectivity == 2)      
	 {n_ask = Max(n_ask,6); n_ask = (n_ask*2)/3;n_ask += n_ask%2;
	 Sizes.m = 1;multiple = true;
	 Sizes.t = 0; 
	 }
      else if(e_connectivity == 3)
	  {n_ask = Max(n_ask,6); n_ask = (n_ask*2)/3;n_ask += n_ask%2;
	  Sizes.m = 2;
	  Sizes.t = (int)(n_ask*.01 +.5);
	  }
      else if(e_connectivity == 4)
	  {Sizes.m = 3;
	  n_ask = Max(n_ask,12); n_ask = (n_ask*2)/3;n_ask += n_ask%2;
	  Sizes.t = (int)(n_ask*.02 +.5);
	  }
      sprintf(t,"Cubic_%d",e_connectivity);
     }
  else if(type == 4)// Quadric
      {if(e_connectivity == 2)    
	  {Sizes.m = 4; Sizes.b = 4;
	  n_ask = Max(n_ask,4);n_ask /= 2;
	  Sizes.t = (int)(n_ask*.02 +.5);multiple = true;
	  sprintf(t,"4-Regular_%d",e_connectivity/2);
	  loops = true;
	  }
      else if(e_connectivity == 4)
	  {Sizes.m = 5; Sizes.b = 5;
	  n_ask = Max(n_ask,8);n_ask /= 2;
	  Sizes.t = (int)(n_ask*.04 +.5);multiple = true;
	  sprintf(t,"4-Regular_%d",e_connectivity/2);
	  }
      else if(e_connectivity == 6)
	  {Sizes.m = 6; Sizes.b = 5;
	  n_ask = Max(n_ask,12);n_ask /= 2;
	  Sizes.t = (int)(n_ask*.20 +.5);
	  sprintf(t,"4-Regular_%d",e_connectivity/2);
	  }
      else if(e_connectivity == 0)//BiQuadric
	  {Sizes.m = 9; Sizes.b = 9; 
	  n_ask = Max(n_ask,12);n_ask /= 2;
	  n_ask += n_ask%2;multiple = true;
	  sprintf(t,"4-Regular-Bipartite_%d",e_connectivity/2);
	  } 
      }
  else if(type == 2)// Bipartite
      {if(e_connectivity == 1)
	  {sprintf(t,"Bipartite");
	  Outp.transform = 1;
	  n_ask += n_ask%2;
	  Sizes.m = 9; Sizes.b = 9;multiple = true;
	  }
      else if(e_connectivity == 2)
	   {sprintf(t,"CubicBipartite_2");
	   n_ask = Max(n_ask,6);
	   n_ask = (n_ask*2)/3;
	   n_ask += n_ask%2;
	   Sizes.m = 7; Sizes.b = 7;multiple = true;
	   }
      else if(e_connectivity == 3)
	   {sprintf(t,"CubicBipartite_3");
	   n_ask = Max(n_ask,12);
	   n_ask = (n_ask*2)/3;
	   n_ask += n_ask%2;
	   Sizes.m = 8; Sizes.b = 7;Sizes.t = (int)(n_ask*.04 +.5);
	   }
      }
  else if(type == 1)// Planar
      {Outp.transform = 1;
      sprintf(t,"Planar_%d",e_connectivity);
      if(e_connectivity == 1)
	  {Sizes.m = 4; Sizes.b = 4;
	  Sizes.t = (int)(n_ask*.02 +.5);
	  loops = multiple = true;
	  }
      else if(e_connectivity == 2)
	  {Sizes.m = 5; Sizes.b = 5;
	  n_ask = Max(n_ask,2);
	  Sizes.t = (int)(n_ask*.04 +.5);
	  multiple = true;
	  }
      else if(e_connectivity == 3)
	  {Sizes.m = 6; Sizes.b = 5;
	  n_ask = Max(n_ask,6);
	  Sizes.t = (int)(n_ask*.06 +.5);
	  }
      }
  
  Sizes.v = n_ask; 
  //if(!pmInitRND(&Meth))return NULL;
  if(!pmSetParameters(&Sizes,&Meth))return NULL;
  if(!pmMemoryInit(&Sizes,&Meth,&Mem))return NULL;
  if(!pmExtendMemory(&Sizes,&Meth,&Mem,0))return NULL;
  Map.i=0;
  if(!pmPlanMap(&Sizes,&Meth,&Mem,&Map))return NULL;
  //Output transform
  if(Outp.transform)
      {pmBicolorFaces(Map.root);
      pmEdgeMap(&Map);
      }
  //Create the graph
  GraphContainer &GC = *new GraphContainer;
  pm_edge *Cur1; 
  pm_vertex *Vtx = Map.root->from;
  pm_vertex *Vtx1 = Map.root->from;
  // compute the number of vertices and edges
  long ni=0, i=0;
  while (Vtx1 != NULL)
      {i++;
      for (Cur1 = Vtx1->root->next; Cur1 != Vtx1->root; Cur1 = Cur1->next) i++;
      Vtx1 = Vtx1->next;
      ni++;
      }
  int n = (int)ni;
  int m = (int)i/2;
  // resize the graph container
  GC.setsize(n,m);
  // define the title
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char seed_txt[20];
  sprintf(seed_txt,"_%ld",randomSetSeed());
  title() = t;  title() += seed_txt;
  Prop<tvertex> vin(GC.Set(tbrin()),PROP_VIN); vin[0]=0;           vin.SetName("GEN:vin");
  Prop<tbrin> cir(GC.Set(tbrin()),PROP_CIR); cir[0]=0;             cir.SetName("GEN:cir"); 
  Prop<tbrin> acir(GC.Set(tbrin()),PROP_ACIR); acir[0]=0;          acir.SetName("GEN:acir"); 
  Prop<tbrin> pb(GC.Set(tvertex()),PROP_PBRIN); pb.clear();        pb.SetName("GEN:pbrin");  
  Prop1<int> maptype(GC.Set(),PROP_MAPTYPE);
  maptype() = PROP_MAPTYPE_ARBITRARY;
  Prop1<tbrin> extbrin(GC.Set(),PROP_EXTBRIN); extbrin()=1;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);         vcoord.SetName("GEN:vcoord");         
  Prop<int> vlabel(GC.PV(),PROP_LABEL);           vlabel.SetName("GEN:vlabel");
  Prop<int> elabel(GC.PE(),PROP_LABEL);           elabel.SetName("GEN:elabel");
  // compute the labels and coordinates
  vlabel[0]=0;
  double angle = 2.*acos(-1.)/n;
  for (tvertex v=1; v<=n; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(cos(angle*(v()-1)),sin(angle*(v()-1)));
    }
  for (tedge e=0; e<=m; e++)
    elabel[e]=e();
  // compute vin, cir, acir
  tbrin b = 0;
  Cur1 = Vtx->root; 
  while(Cur1 != Vtx->root->prev)
      {if(Cur1->label > 0)
	  {b=Cur1->label;
	  vin[b] = (int)Cur1->from->label;  vin[-b]  = (int)Cur1->oppo->from->label;
	  cir[b] = (int)Cur1->next->label; acir[b]=(int)Cur1->prev->label;
	  cir[-b] = (int)Cur1->oppo->next->label; acir[-b]=(int)Cur1->oppo->prev->label;
	  }
      Cur1 = Cur1->next; 
      }

  if(Cur1->label > 0)
    {b=Cur1->label;
      vin[b] = (int)Cur1->from->label;  vin[-b]  = (int)Cur1->oppo->from->label;
      cir[b] = (int)Cur1->next->label; acir[b]=(int)Cur1->prev->label;
      cir[-b] = (int)Cur1->oppo->next->label; acir[-b]=(int)Cur1->oppo->prev->label;
      }

  while(Vtx->next != NULL)
      {Vtx = Vtx->next;
      Cur1 = Vtx->root; 
      while(Cur1 != Vtx->root->prev)
	  {if(Cur1->label > 0)
	      {b=Cur1->label;
	      vin[b] = (int)Cur1->from->label;  vin[-b]  = (int)Cur1->oppo->from->label;
	      cir[b] = (int)Cur1->next->label; acir[b]=(int)Cur1->prev->label;
	      cir[-b] = (int)Cur1->oppo->next->label; acir[-b]=(int)Cur1->oppo->prev->label;
	      }
	  Cur1 = Cur1->next; 
	  }
      if(Cur1->label > 0) 
          {b=Cur1->label;
          vin[b] = (int)Cur1->from->label;  vin[-b]  = (int)Cur1->oppo->from->label;
          cir[b] = (int)Cur1->next->label; acir[b]=(int)Cur1->prev->label;
          cir[-b] = (int)Cur1->oppo->next->label; acir[-b]=(int)Cur1->oppo->prev->label;
          }
      }

  // construct pbrin
  for (b=-m; b<=m; b++)
    if ((b!=0) && (pb[vin[b]]==0)) pb[vin[b]]=b;
  pmFreeMap(&Map);
  randomEnd();

  // construct a topological graph
  if(!randomUseGeneratedCir)
      {GC.Set(tbrin()).erase(PROP_CIR); GC.Set(tbrin()).erase(PROP_ACIR);
      GC.Set(tvertex()).erase(PROP_PBRIN);
      }

  TopologicalGraph TG(GC);
  int erased  = 0;
  if(!loops){Prop1<int> numloops(TG.Set(),PROP_NLOOPS);numloops() = 0;}
  if(randomEraseMultipleEdges && multiple)
      erased = TG.Simplify();
  else if(loops)
      erased = TG.RemoveLoops();
  if(debug())LogPrintf("<GEN:(%ld) n:%d m:%d erased:%d>\n",randomSetSeed(),TG.nv(),TG.ne(),erased);
  if(randomUseGeneratedCir)
       {if (TG.ComputeGenus() != 0) setPigaleError(-1,"Bad genus for random map (Schaeffer)");
       else TG.planarMap() = 1;
       }
  
  return &GC;
  }









