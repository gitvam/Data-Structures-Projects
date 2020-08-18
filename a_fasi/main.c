
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Darwin.h"

#define BUFFER_SIZE 1024  /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints
 * or comment to disable it */
#define DEBUG

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */

/*global dhlwseis*/
int globalcid, globalsid;

/*arxh me tis dikes mou sunarthseis*/
struct Population *
SortedMerge (struct Population *a, struct Population *b)
{
  struct Population *result = NULL;

  if (a == NULL)
    return (b);
  else if (b == NULL)
    return (a);

  else if(a == NULL && b == NULL)return NULL;

  if (a->gid <= b->gid)
    {
      result = a;
      result->next = SortedMerge (a->next, b);
    }
  else
    {
      result = b;
      result->next = SortedMerge (a, b->next);
    }
  return (result);
}

void
MoveNode (struct Population **telptr, struct Population **arxptr)
{
  struct Population *newNode = *arxptr;
  *arxptr = newNode->next;
  newNode->next = *telptr;
  *telptr = newNode;
}


void
delete_population_gid (struct Species *head, int gid)
{
  struct Species *temp_species = head;
  struct Population *temp_population = temp_species->Population_head;
  struct Population *prev;

  if (temp_population != NULL && temp_population->gid == gid)
    {
      temp_species->Population_head = temp_population->next;
      free (temp_population);
      return;
    }

  while (temp_population != NULL && temp_population->gid != gid)
    {
      prev = temp_population;
      temp_population = temp_population->next;
    }

  if (temp_population == NULL)
    return;
  prev->next = temp_population->next;
  free (temp_population);
}

void
delete_population_from_continent (struct Species *head, int gid)
{
  struct Population *temp = head->Population_head;
  while (temp!=NULL&&temp->gid != gid)
    temp = temp->next;
  if(temp==NULL)return;
  globalcid = temp->cid;
  struct ContinentPopulation *tempaki = continents[temp->cid]->populationList;
  struct ContinentPopulation *prev;

  if (tempaki->gid != -1 && tempaki->gid == gid)
    {

      continents[temp->cid]->populationList = tempaki->next;
      free (tempaki);
      return;
    }

  while (tempaki->gid != -1 && tempaki->gid != gid)
    {
      prev = tempaki;
      tempaki = tempaki->next;
    }

  if (tempaki->gid == -1)
    return;
  prev->next = tempaki->next;
  free (tempaki);
}

void
deleteNode2 (int key)
{
  struct Species *temp = Species_tail;
  if (temp != NULL && temp->sid == key)
    {
      Species_tail = temp->prev;
      temp->prev->next = NULL;
      free (temp);
      return;
    }

  while (temp != NULL && temp->sid != key)
    temp = temp->prev;

  if (temp == Species_head)
    {
      Species_head = temp->next;
      temp->next->prev = NULL;
      free (temp);
      return;
    }

  if (temp == NULL)
    return;

  temp->prev->next = temp->next;
  temp->next->prev = temp->prev;

  free (temp);

}

/*telos me tis dikes mou sunarthseis*/

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize (void)
{
	Species_head = NULL;
	Species_tail = NULL;
	struct ContinentPopulation *sentinel0 =
		(struct ContinentPopulation *)
		malloc (sizeof (struct ContinentPopulation));
	struct ContinentPopulation *sentinel1 =
		(struct ContinentPopulation *)
		malloc (sizeof (struct ContinentPopulation));
	struct ContinentPopulation *sentinel2 =
		(struct ContinentPopulation *)
		malloc (sizeof (struct ContinentPopulation));
	struct ContinentPopulation *sentinel3 =
		(struct ContinentPopulation *)
		malloc (sizeof (struct ContinentPopulation));
	struct ContinentPopulation *sentinel4 =
		(struct ContinentPopulation *)
		malloc (sizeof (struct ContinentPopulation));
	sentinel0->gid = sentinel1->gid = sentinel2->gid = sentinel3->gid =
		sentinel4->gid = -1;

	continents[0] =
		(struct ContinentList *) malloc (sizeof (struct ContinentList));;
	continents[1] =
		(struct ContinentList *) malloc (sizeof (struct ContinentList));;
	continents[2] =
		(struct ContinentList *) malloc (sizeof (struct ContinentList));;
	continents[3] =
		(struct ContinentList *) malloc (sizeof (struct ContinentList));;
	continents[4] =
		(struct ContinentList *) malloc (sizeof (struct ContinentList));;

	continents[0]->sentinel = continents[0]->populationList = sentinel0;
	continents[1]->sentinel = continents[1]->populationList = sentinel1;
	continents[2]->sentinel = continents[2]->populationList = sentinel2;
	continents[3]->sentinel = continents[3]->populationList = sentinel3;
	continents[4]->sentinel = continents[4]->populationList = sentinel4;

	return 1;
}

/**
 * @brief insert new species in Species' list with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_species (int sid)
{
	struct Species * newNode =
		(struct Species * ) malloc(sizeof(struct Species));
	newNode -> sid = sid;
	newNode -> Population_head = NULL;
	newNode -> Population_tail = NULL;
	struct Species * current;

	if (Species_head == NULL) {
		Species_head = newNode;
		newNode -> prev = NULL;
		newNode -> next = NULL;
	} else if ((Species_head) -> sid >= newNode -> sid) {
		newNode -> next = Species_head;
		newNode -> next -> prev = newNode;
		Species_head = newNode;
	} else {
		current = Species_head;
		while (current -> next != NULL && current -> next -> sid < newNode -> sid) {
			current = current -> next;
		}
		newNode -> next = current -> next;

		if (current -> next != NULL) {
			newNode -> next -> prev = newNode;
		}

		current -> next = newNode;
		newNode -> prev = current;
	}
		current = Species_head;
		printf("S<sid>\n ");
		while (current != NULL) {
		printf("<%d>", current->sid);
		if (current->next != NULL)
			printf (",");
		else
		{
		  Species_tail = current;
		}
		current = current->next;
	}
	printf ("\nDONE\n");
	return 1;
}

/**
 * @brief insert new population with ID <gid> in Species' with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_population(int gid, int sid, int cid){
	struct Population *newNode =
    (struct Population *) malloc (sizeof (struct Population));
  struct Species *species_temp = Species_head;
  struct Population *population_temp;
  newNode->gid = gid;
  newNode->cid = cid;
  newNode->sid = sid;
  newNode->next = NULL;
  while (species_temp->sid != sid)
    species_temp = species_temp->next;


  if (species_temp->Population_head == NULL
      || (species_temp->Population_head)->gid >= newNode->gid)
    {
      newNode->next = species_temp->Population_head;
      species_temp->Population_head = newNode;

    }
  else
    {
      population_temp = species_temp->Population_head;
      while (population_temp->next != NULL
	     && population_temp->next->gid < newNode->gid)
	{
	  population_temp = population_temp->next;
	}

      newNode->next = population_temp->next;
      population_temp->next = newNode;
    }

  struct Species *print_species = Species_head;
  struct Population *print_population;
  while (print_species != NULL)
    {
      print_population = print_species->Population_head;
      printf ("<%d>\n ", print_species->sid);
      while (print_population != NULL)
	{
	  printf ("<%d><%d>", print_population->gid, print_population->cid);
	  if (print_population->next == NULL)
	    {
	      print_species->Population_tail = print_population;
	      printf ("\n");
	    }
	  else
	    {
	      printf (",");
	    }
	  print_population = print_population->next;
	}

      print_species = print_species->next;
    }

  printf ("DONE\n");
	return 1;
}

/**
 * @brief merge species with IDs <sid1> and <sid2> into new species with ID <sid3>
 *
 * @return 1 on success
 *         0 on failure
 */
int merge_species(int sid1, int sid2, int sid3){
	struct Species *temp1 = Species_head;
  struct Species *temp2 = Species_head;
  struct Species *temp3 = Species_head;
  struct Species *temp_delete = Species_head;
  struct Species *temp_delete2 = Species_head;
  while (temp1!=NULL&&temp1->sid != sid1)
    temp1 = temp1->next;
  while (temp2!=NULL&&temp2->sid != sid2)
    temp2 = temp2->next;

  insert_species (sid3);
  while (temp3!=NULL&&temp3->sid != sid3)
    temp3 = temp3->next;

  struct Population *result = NULL;
  result = SortedMerge (temp1->Population_head, temp2->Population_head);
  if(temp3!=NULL)temp3->Population_head = result;

  deleteNode2 (sid1);
  deleteNode2 (sid2);

  struct Species *print_species = Species_head;
  struct Population *print_population;
  while (print_species != NULL)
    {
      print_population = print_species->Population_head;
      printf ("<%d>\n", print_species->sid);
      while (print_population != NULL)
	{
	  printf (" <%d><%d>\n", print_population->gid,
		  print_population->cid);
    if(print_population->next==NULL)print_species->Population_tail = print_population;
	  print_population = print_population->next;
	}
      print_species = print_species->next;
    }
  printf ("DONE\n");
	return 1;
}

/**
 * @brief Distribute species' population in continents array
 *
 * @return 1 on success
 *         0 on failure
 */
int distribute(){
	struct Species *print_species = Species_head;
  struct Population *print_population;
  while (print_species != NULL)
    {
      print_population = print_species->Population_head;
      while (print_population != NULL)
	{
	  //to neo node gia to kathena
	  struct ContinentPopulation *node =
	    (struct ContinentPopulation *)
	    malloc (sizeof (struct ContinentPopulation));
	  node->gid = print_population->gid;
	  if (print_population->cid == 0)
	    {
	      node->next = continents[0]->populationList;
	      continents[0]->populationList = node;
	    }
	  else if (print_population->cid == 1)
	    {
	      node->next = continents[1]->populationList;
	      continents[1]->populationList = node;
	    }
	  else if (print_population->cid == 2)
	    {
	      node->next = continents[2]->populationList;
	      continents[2]->populationList = node;
	    }
	  else if (print_population->cid == 3)
	    {
	      node->next = continents[3]->populationList;
	      continents[3]->populationList = node;
	    }
	  else if (print_population->cid == 4)
	    {
	      node->next = continents[4]->populationList;
	      continents[4]->populationList = node;
	    }
	  print_population = print_population->next;
	}
      print_species = print_species->next;
    }

  struct ContinentPopulation *temp = continents[0]->populationList;
  printf ("D\n");
  printf ("Continent0: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[1]->populationList;
  printf ("Continent1: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[2]->populationList;
  printf ("Continent2: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[3]->populationList;
  printf ("Continent3: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[4]->populationList;
  printf ("Continent4: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\nDONE\n");
	return 1;
}

/**
 * @brief delete population with ID <gid> from species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_population(int gid, int sid){

	struct Species *temp_species = Species_head;
  while (temp_species->sid != sid)
    temp_species = temp_species->next;
  delete_population_from_continent (temp_species, gid);
  delete_population_gid (temp_species, gid);

  printf ("K <gid> <sid>\nSPECIES\n ");
  struct Population *temp = temp_species->Population_head;
  while (temp != NULL)
    {
      printf ("<%d>", temp->gid);
      if (temp->next != NULL)
	printf (",");
      temp = temp->next;
    }
  printf ("\nCONTINENTS\n Continent%d: ", globalcid);

  struct ContinentPopulation *last_print =
    continents[globalcid]->populationList;
  while (last_print->gid != -1)
    {
      printf ("<%d>", last_print->gid);
      if (last_print->next->gid != -1)
	printf (",");
      last_print = last_print->next;
    }
  printf ("\nDONE\n");
	return 1;
}

/**
 * @brief delete species with ID <sid> and its populations
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_species(int sid){
	struct Species *temp_species = Species_head;
  struct Population *temp_population;
  while (temp_species->sid != sid)
    temp_species = temp_species->next;
  temp_population = temp_species->Population_head;
  while (temp_population != NULL)
    {
      delete_population_from_continent (temp_species, temp_population->gid);
      temp_population = temp_population->next;
    }
  deleteNode2 (sid);
  struct Species *print_species = Species_head;
  struct Population *print_population;
  while (print_species != NULL)
    {
      print_population = print_species->Population_head;
      printf ("<%d>\n", print_species->sid);
      while (print_population != NULL)
	{
	  printf (" <%d><%d>\n", print_population->gid,
		  print_population->cid);
	  print_population = print_population->next;
	}
      print_species = print_species->next;
    }
  struct ContinentPopulation *temp = continents[0]->populationList;
  printf ("D\n");
  printf ("Continent0: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[1]->populationList;
  printf ("Continent1: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[2]->populationList;
  printf ("Continent2: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[3]->populationList;
  printf ("Continent3: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[4]->populationList;
  printf ("Continent4: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\nDONE\n");
	return 1;
}

/**
 * @brief Remaining species evolve into homo sapiens.
 *
 * @return 1 on success
 *         0 on failure
 */
int evolution(){
     struct Species *temp = Species_head;
     struct Population *tail=NULL;
     struct Population* new = NULL;
     hs_L = NULL;

     while (temp != NULL) {

         if(temp->Population_head!=NULL){
             new = temp->Population_head;
             if(hs_L==NULL)hs_L=new;
             tail = temp->Population_tail;
             if(temp->next!=NULL) {
                 new->next_species = temp->next->Population_head;
                 tail->next = new->next_species;
             }else if(temp->next==NULL){
                 new->next_species = NULL;
                 tail->next = NULL;
             }
         }

         temp = temp->next;
     }

     struct Population *print_temp_species = hs_L;
     struct Population *print_temp = hs_L;
     printf ("E\n  Homo Sapiens:[");

     while (print_temp != NULL)
     {

         if (print_temp->next != NULL)
         {
             if (print_temp->sid != print_temp->next->sid)
                 printf ("<%d,%d>] [", print_temp->gid, print_temp->sid);
             else
                 printf ("<%d,%d>,", print_temp->gid, print_temp->sid);
         }
         else {
             printf("<%d,%d>", print_temp->gid, print_temp->sid);
         }

         print_temp = print_temp->next;
     }

     printf ("]\nDONE\n");
     return 1;
}

/**
 * @brief Gather statistics on species from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int species_statistics(){
	int num = 0;
  struct Population *statistics = hs_L;
  while (statistics != NULL)
    {
      num++;
      statistics = statistics->next_species;
    }
  printf ("N\n Homo Sapiens species: <%d>\nDONE\n", num);
	return 1;
}

/**
 * @brief Gather statistics on population belonging to species with ID <sid> from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int population_statistics(int sid){
	struct Population *prwto = hs_L;
  struct Population *deutero;
  int num = 0;

  while (prwto!=NULL && prwto->sid != sid)
    {
      prwto = prwto->next_species;
    }
  deutero = prwto;
  while (deutero!=NULL && deutero != prwto->next_species)
    {
      num++;
      deutero = deutero->next;
    }

  printf ("J<sid>\n Homo Sapiens populations: <%d>\nDONE\n", num);
	return 1;
}

/**
 * @brief Print species' list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_species(){
	struct Species *current = Species_head;
  printf ("P\n ");
  while (current != NULL)
    {
      printf ("<%d>", current->sid);
      if (current->next != NULL)
	printf (",");
      else
	{
	  Species_tail = current;
	}
      current = current->next;
    }
  printf ("\nDONE\n");
	return 1;
}

/**
 * @brief Print species' list with population lists
 *
 * @return 1 on success
 *         0 on failure
 */
int print_populations(){
	struct Species *print_species = Species_head;
  struct Population *print_population;
  printf ("X\n");
  while (print_species != NULL)
    {
      print_population = print_species->Population_head;
      printf ("<%d>\n ", print_species->sid);
      while (print_population != print_species->Population_tail)
	     {
	        printf ("<%d><%d>", print_population->gid, print_population->cid);

	         if (print_population->next != print_species->Population_tail)
	          printf (",");

	         print_population = print_population->next;
           if(print_population==print_species->Population_tail)printf (",<%d><%d>\n", print_population->gid, print_population->cid);
	     }
      print_species = print_species->next;
    }



  printf ("DONE\n");
	return 1;
}

/**
 * @brief Print continents array with each continent's population list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_continents(){
	struct ContinentPopulation *temp = continents[0]->populationList;

  printf ("Continent0: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[1]->populationList;
  printf ("Continent1: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[2]->populationList;
  printf ("Continent2: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[3]->populationList;
  printf ("Continent3: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\n");
  temp = continents[4]->populationList;
  printf ("Continent4: ");
  while (temp->gid != -1)
    {
      printf ("<%d>", temp->gid);
      if (temp->next->gid != -1)
	printf (",");
      temp = temp->next;
    }
  printf ("\nDONE\n");
	return 1;
}

/**
 * @brief Print homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_homo_sapiens(){
	struct Population *print_temp_species = hs_L;
  struct Population *print_temp = hs_L;
  printf ("W\n  Homo Sapiens:[");
  while (print_temp != NULL)
    {

      if (print_temp->next != NULL)
	{
	  if (print_temp->sid != print_temp->next->sid)
	    printf ("<%d,%d>] [", print_temp->gid, print_temp->sid);
	  else
	    printf ("<%d,%d>,", print_temp->gid, print_temp->sid);
	}
      else
	printf ("<%d,%d>", print_temp->gid, print_temp->sid);

      print_temp = print_temp->next;
    }

  printf ("]\nDONE\n");
	return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_all(void)
{

	return 1;
}


/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char** argv)
{
	FILE *fin = NULL;
	char buff[BUFFER_SIZE], event;

	/* Check command buff arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if (( fin = fopen(argv[1], "r") ) == NULL ) {
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();

	/* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin)) {

		DPRINT("\n>>> Event: %s", buff);

		switch(buff[0]) {

			/* Comment */
			case '#':
				break;

				/* Insert Species
				 * S <sid> */
			case 'S':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (insert_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Insert population
				 * G <gid> <sid> <cid> */
			case 'G':
				{
					int gid, sid, cid;

					sscanf(buff, "%c %d %d %d", &event, &gid, &sid, &cid);
					DPRINT("%c %d %d %d\n", event, gid, sid, cid);

					if (insert_population(gid, sid, cid)) {
						DPRINT("%c %d %d %d succeeded\n", event, gid, sid, cid);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, gid, sid, cid);
					}

					break;
				}

				/* Merge species
				 * M <sid1> <sid2> <sid3> */
			case 'M':
				{
					int sid1, sid2, sid3;

					sscanf(buff, "%c %d %d %d", &event, &sid1, &sid2, &sid3);
					DPRINT("%c %d %d %d\n", event, sid1, sid2, sid3);

					if (merge_species(sid1, sid2, sid3)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid1, sid2, sid3);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid1, sid2, sid3);
					}

					break;
				}

				/* Distribute species
				 * D */
			case 'D':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (distribute()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Delete population
				 * K <gid> <sid> */
			case 'K':
				{
					int gid, sid;

					sscanf(buff, "%c %d %d", &event, &gid, &sid);
					DPRINT("%c %d %d\n", event, gid, sid);

					if (delete_population(gid, sid)) {
						DPRINT("%c %d %d succeeded\n", event, gid, sid);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, gid, sid);
					}

					break;
				}

				/* Delete species
				 * F <sid> */
			case 'F':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (delete_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Evolution to homo sapiens
				 * E */
			case 'E':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (evolution()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Species' statistics
				 * N */
			case 'N':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (species_statistics()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Population statistics
				 * J <sid> */
			case 'J':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (population_statistics(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Print species
				 * P */
			case 'P':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print populations
				 * X */
			case 'X':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_populations()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print continents
				 * C */
			case 'C':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_continents()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print homo sapiens
				 * W */
			case 'W':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_homo_sapiens()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Empty line */
			case '\n':
				break;

				/* Ignore everything else */
			default:
				DPRINT("Ignoring buff: %s \n", buff);

				break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
