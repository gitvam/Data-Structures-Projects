
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


int counter;

void initialize(){
    Species_root = NULL;
    Homo_Sapiens_root = NULL;
    counter=0;
    continents[0] = (struct ContinentTree *)malloc(sizeof(struct ContinentTree));
    continents[1] = (struct ContinentTree *)malloc(sizeof(struct ContinentTree));
    continents[2] = (struct ContinentTree *)malloc(sizeof(struct ContinentTree));
    continents[3] = (struct ContinentTree *)malloc(sizeof(struct ContinentTree));
    continents[4] = (struct ContinentTree *)malloc(sizeof(struct ContinentTree));
    for(int i=0; i<5; i++){
        continents[i]->population_root = NULL;
        continents[i]->sentinel = (struct ContinentPopulation *)malloc(sizeof(struct ContinentPopulation));
        continents[i]->sentinel->gid = -1;
        continents[i]->sentinel->rc = continents[i]->sentinel->lc = NULL;
        continents[i]->population_root = continents[i]->sentinel;
    }

}

void postorder(struct Species *root)
{
    if (root == NULL)return;
    postorder(root->lc);
    postorder(root->rc);
    printf("<%d> ", root->sid);
}

void inorder(struct Population *root)
{
    if (root == NULL)return;
    inorder(root->lc);
    printf("<%d> ", root->gid);
    inorder(root->rc);
}

void merge_print_help(struct Population *root)
{
    if (root == NULL)return;
    merge_print_help(root->lc);
    printf("\t<%d><%d> \n", root->gid,root->cid);
    merge_print_help(root->rc);
}

void merge_print(struct Species *root)
{
    if (root == NULL)return;
    merge_print(root->lc);
    merge_print(root->rc);
    printf(" <%d> \n", root->sid);
    if(root->population_root!=NULL) {
        merge_print_help(root->population_root);
    }
}

struct Species *newNode(int item)
{
    struct Species *temp =  (struct Species *)malloc(sizeof(struct Species));
    temp->sid = item;
    temp->lc = temp->rc = NULL;
    temp->population_root = NULL;
    return temp;
}

struct Population *newPopulation(int gid,int sid,int cid)
{
    struct Population *temp =  (struct Population *)malloc(sizeof(struct Population));
    temp->gid = gid;
    temp->sid = sid;
    temp->cid = cid;
    temp->parent=NULL;
    temp->lc = temp->rc = NULL;
    return temp;
}

struct Species * insert_species_help(struct Species *root,int key){
    if (root == NULL) return newNode(key);
    if(key>root->sid){
        struct Species* new = newNode(key);
        new->lc = root;
        root = new;
        return new;
    }
    else{
        if(root->lc!=NULL && key > root->lc->sid){
            root->rc = insert_species_help(root->rc, key);
        }else{
            root->lc  = insert_species_help(root->lc, key);
        }
    }
    return root;
}

int insert_species(int key){
    Species_root = insert_species_help(Species_root,key);
    if(Species_root == NULL)return 0;
    printf("S<sid>\n\t");
    postorder(Species_root);
    printf("\nDONE\n");
    return 1;
}

struct Species* search_species(struct Species* root, int sid)
{
    if (root == NULL || root->sid == sid)return root;
    struct Species *res1 = search_species(root->lc, sid);
    if(res1!=NULL)return res1;
    struct Species *res2 =search_species(root->rc, sid);
    return res2;
}
struct Population* findParent(struct Population* node,int sid)
{
    if(node==NULL)return NULL;
    findParent(node->lc,sid);
    if(node->lc!= NULL || node->lc!= NULL){
    if(node->lc->sid == sid || node->rc->sid == sid){
      return node;
      }
    }
    findParent(node->rc,sid);
}
struct Population* insert_population_help(struct Population* node,int gid, int sid, int cid){

     if (node == NULL) {
         struct Population* new = newPopulation(gid,sid,cid);
         node = new;
         if(findParent(node,sid)!=NULL)node->parent = findParent(node,sid);
         return node;
     }
     if (gid < node->gid) {
        node->lc = insert_population_help(node->lc, gid, sid, cid);
     } else if (gid > node->gid) {
         node->rc = insert_population_help(node->rc, gid, sid, cid);
     }
    return node;
}

int insert_population(int gid, int sid, int cid){
    struct Species* temp = search_species(Species_root,sid);
		if(temp==NULL)return 0;
    temp -> population_root = insert_population_help(temp->population_root,gid,sid,cid);
    if(temp->population_root == NULL)return 0;
    printf("G<gid><sid><cid>\n");
    merge_print(Species_root);
    printf("DONE\n");
    return 1;
}

void mergeArrays(struct Population **arr1, struct Population **arr2, int index1, int index2, struct Population **arr3)
{
    int i = 0, j = 0, k = 0;
    while (i < index1 && j < index2)
    {
        if (arr1[i]->gid < arr2[j]->gid)arr3[k++] = arr1[i++];
        else arr3[k++] = arr2[j++];
    }
    if (i < index1) {  while (i < index1) arr3[k++] = arr1[i++]; }
    if (j < index2) {  while (j < index2) arr3[k++] = arr2[j++]; }
}

int count(struct Population *node){
    int c = 1;
    if (node == NULL)return 0;
    else{
     c += count(node->lc);
     c += count(node->rc);
     return c;
    }
}

int AddToArray(struct Population *node, struct Population **arr, int i)
{
    if(node == NULL)return i;
    if(node->lc != NULL)i = AddToArray(node->lc, arr, i);
    arr[i] = node;
    i++;
    if(node->rc != NULL)i = AddToArray(node->rc, arr, i);
    return i;
}

struct Population *lca_help(struct Population* root, int gid1, int gid2)
{
    if (root == NULL) return NULL;
    if (root->gid > gid1 && root->gid > gid2)return lca_help(root->lc, gid1, gid2); /*an ta gids einai mikrotera apo root paei aristera*/
    if (root->gid < gid1 && root->gid < gid2)return lca_help(root->rc, gid1, gid2); /*an ta gids einai mikrotera apo root paei deksia*/
    return root;
}

int lowest_common_ancestor(int sid, int gid1, int gid2){
    struct Species* spec = search_species(Species_root,sid);
    if(spec == NULL)return 0;
    struct Population*lca = lca_help(spec->population_root,gid1,gid2);
    if(lca==NULL)return 0;
    printf("L <sid> <gid1> <gid2>\n\tLowest Common Ancestor: <%d>\nDONE\n",lca->gid);
    return 1;
}

struct ContinentPopulation* insert_cointinent_population(struct ContinentPopulation* node,int gid,int cid){
/*h insert gia ta continent populations, in-order taksinomimena*/
    if (node == NULL) {
        struct ContinentPopulation* new = (struct ContinentPopulation *)malloc(sizeof(struct ContinentPopulation));
        new -> gid = gid;
        new ->rc = new -> lc = NULL;

        if(cid==0 && continents[0]->population_root==continents[0]->sentinel)continents[0]->population_root=new;
        if(cid==1 && continents[1]->population_root==continents[1]->sentinel)continents[1]->population_root=new;
        if(cid==2 && continents[2]->population_root==continents[2]->sentinel)continents[2]->population_root=new;
        if(cid==3 && continents[3]->population_root==continents[3]->sentinel)continents[3]->population_root=new;
        if(cid==4 && continents[4]->population_root==continents[4]->sentinel)continents[4]->population_root=new;
        node = new;
        return node;
    }
    if (gid < node->gid) {
        node->lc = insert_cointinent_population(node->lc, gid, cid);
    } else if (gid > node->gid) {
        node->rc = insert_cointinent_population(node->rc, gid, cid);
    }
    return node;
}

int dist2(struct Population* node){ /*trexoume ta populations kai ta kathgoriopoioume*/
    if(node==NULL)return 0;
    if(node->cid==0){
        insert_cointinent_population(continents[0]->population_root,node->gid,node->cid);
    }
    if(node->cid==1){
        insert_cointinent_population(continents[1]->population_root,node->gid,node->cid);
    }
    if(node->cid==2){
        insert_cointinent_population(continents[2]->population_root,node->gid,node->cid);
    }
    if(node->cid==3){
        insert_cointinent_population(continents[3]->population_root,node->gid,node->cid);
    }
    if(node->cid==4){
        insert_cointinent_population(continents[4]->population_root,node->gid,node->cid);
    }
    dist2(node->lc);
    dist2(node->rc);

}

int dist1(struct Species* root){  /*trexoume ta species kai kaloume thn dist2 gia na treksei kai ola ta populations*/
    if(root==NULL)return 0;
    if(root->population_root!=NULL){
        dist2(root->population_root);
    }
    dist1(root->lc);
    dist1(root->rc);
    return 1;
}

void dist4(struct ContinentPopulation* root){ /*to print gia to kathe stoixeio tou kathe continent*/
    if(root==NULL) return;
    dist4(root->lc);
    printf("<%d> ",root->gid);
    dist4(root->rc);
}

int distribute(){
    dist1(Species_root);
    printf("D\n");
    for(int i=0; i<5; i++){
        printf("Continents %d: ",i);
        dist4(continents[i]->population_root);
        printf("\n");
    }
    printf("DONE\n");
    return 1;
}

struct Population *DeletePop(struct Population *root, int gid) {

    if (root == NULL) return root;
    if (root->gid==gid){
        if (root->lc == NULL){
            struct Population *temp = root->rc;
            return temp;
        }
        else if (root->rc == NULL){
            struct Population *temp = root->lc;
            return temp;
        }

        struct Population* temp=root->rc;
        while (temp && temp->lc != NULL)temp = temp->lc;
        root->gid= temp->gid;
        root->cid = temp->cid;
        root->sid = temp->sid;
        root->rc = DeletePop(root->rc, temp->gid);
    }
    else if (root->gid<gid) root->rc = DeletePop(root->rc, gid);
    else root->lc = DeletePop(root->lc,gid);
    return root;
}

int delete_species_help(struct Species** root, int key){
        if(*root==NULL) return 0;   //not found
        if((*root)->sid==key) {
            if((*root)->lc==NULL && (*root)->rc==NULL) {
                *root=NULL;
                return 1;
            }
            else {
                if((*root)->lc==NULL) {
                    *root=(*root)->rc;
                    return 1;
                }
                if((*root)->rc==NULL) {
                    *root=(*root)->lc;
                    return 1;
                }
                else if((*root)->rc!=NULL && (*root)->lc!=NULL) {
                    *root=(*root)->rc;
                }
            }
        }
    delete_species_help(&(*root)->lc, key);
    delete_species_help(&(*root)->rc, key);
}

struct Species* minspec(struct Species* node) { /*vriskoume to mikrotero gia na to kanoume delete sthn delete_species*/
    struct Species* current = node;
    while (current->lc != NULL)current = current->lc;
    return(current);
}

void deletexx(struct Population* node)
{
    if (node == NULL)return;
    deletexx(node->lc);
    deletexx(node->rc);
    node = NULL;
}

void delete_print_help(struct Population *root)
{
    if (root == NULL)return;
    delete_print_help(root->lc);
    printf("<%d>", root->gid);
    delete_print_help(root->rc);
}

void delete_print(struct Species *root){
    if (root == NULL)return;
    delete_print(root->lc);
    printf(" <%d> \n", root->sid);
    if(root->population_root!=NULL) {
        printf("\t");
        delete_print_help(root->population_root);
        printf("\n");
    }
    delete_print(root->rc);
}

struct ContinentPopulation* deletefromcontinent(struct ContinentPopulation *root, int gid) {
    if (root == NULL) return root;
    if (root->gid==gid){
        if (root->lc == NULL){
            struct ContinentPopulation *temp = root->rc;
            return temp;
        }
        else if (root->rc == NULL){
            struct ContinentPopulation *temp = root->lc;
            return temp;
        }

        struct ContinentPopulation* temp=root->rc;
        while (temp && temp->lc != NULL)temp = temp->lc;
        root->gid= temp->gid;
        root->rc = deletefromcontinent(root->rc, temp->gid);
    }
    else if (root->gid<gid) root->rc = deletefromcontinent(root->rc, gid);
    else root->lc = deletefromcontinent(root->lc,gid);
    return root;
}

void find_gid_fromsid(struct Population* node){
    if(node==NULL)return;
    if(node->cid==0){
        continents[0]->population_root = deletefromcontinent(continents[0]->population_root,node->gid);
    }
    if(node->cid==1){
        continents[1]->population_root = deletefromcontinent(continents[1]->population_root,node->gid);
    }
    if(node->cid==2){
        continents[2]->population_root = deletefromcontinent(continents[2]->population_root,node->gid);
    }
    if(node->cid==3){
        continents[3]->population_root = deletefromcontinent(continents[3]->population_root,node->gid);
    }
    if(node->cid==4){
        continents[4]->population_root = deletefromcontinent(continents[4]->population_root,node->gid);
    }
    find_gid_fromsid(node->lc);
    find_gid_fromsid(node->rc);
}

int delete_species(){
    struct Species* efuges = minspec(Species_root);
    if(efuges == NULL)return 0;
    find_gid_fromsid(efuges->population_root); //delete gia continent
    deletexx(efuges->population_root);
    delete_species_help(&Species_root,efuges->sid);
    printf("F\n");
    delete_print(Species_root);
    printf("CONTINENTS\n");
    for(int i=0; i<5; i++){
        printf("Continents %d: ",i);
        dist4(continents[i]->population_root);
        printf("\n");
    }
    printf("DONE\n");
    return 1;
}

int delete_population(int gid, int sid){
    struct Species* tmp = search_species(Species_root,sid);
    if(tmp==NULL)return 0;
    continents[tmp->population_root->cid]->population_root=deletefromcontinent(continents[tmp->population_root->cid]->population_root,gid); /*delete from continent*/
    tmp->population_root = DeletePop(tmp->population_root,gid);
    printf("K<gid><sid>\nSPECIES\n <%d>\n\t",sid);
    inorder(tmp->population_root);
    printf("\nCONTINENTS\n");
    for(int i=0; i<5; i++){
        printf("Continents %d: ",i);
        dist4(continents[i]->population_root);
        printf("\n");
    }
    printf("\nDONE\n");
    return 1;
}

int merge_species(int sid1, int sid2, int sid3){
    struct Species* root1 = search_species(Species_root,sid1);
    struct Species* root2 = search_species(Species_root,sid2);
    if(root1 == NULL || root2 == NULL) return 0; /*den mporei na kanei merge an den uparxei ena apo ta duo*/
    int  root1_count = count(root1->population_root);
    int  root2_count = count(root2->population_root);
    int merged_size = root1_count + root2_count;
    struct Population** arr1 = calloc(root1_count, sizeof(struct Population));
    struct Population** arr2 = calloc(root2_count, sizeof(struct Population));
    struct Population** merged_array = calloc(merged_size, sizeof(struct Population));
    AddToArray(root1->population_root,arr1,0);
    AddToArray(root2->population_root,arr2,0);
    mergeArrays(arr1,arr2,root1_count,root2_count,merged_array);
    insert_species(sid3);  /*ftiaxnoume neo eidos, gia na to gemisoume sti sunexeia */
    struct Species* root3 = search_species(Species_root,sid3);
    for(int i=0; i<merged_size; i++){ /* inserting all the elements of the sorted array to a new tree*/
        insert_population(merged_array[i]->gid,sid3,merged_array[i]->cid);
    }
    delete_species_help(&Species_root,sid1);
    delete_species_help(&Species_root,sid2);
    printf("M <sid1><sid2><sid3>\n");
    merge_print(Species_root);
    printf("DONE\n");
    return 1;
}

struct HomoSapiens* insert_homosapiens(struct HomoSapiens* node,struct Population* pop, int sid){

    if (node == NULL) {
        struct HomoSapiens* new = (struct HomoSapiens *)malloc(sizeof(struct HomoSapiens));
        new->rc = new->lc =  NULL;
        new->sid = sid;
        new->next=NULL;
        new->population_root = pop;
        node = new;
        return node;
    }
    if (sid < node->sid) {
        node->lc = insert_homosapiens(node->lc, pop,sid);
    } else if (sid >= node->sid) {
        node->rc = insert_homosapiens(node->rc,pop,sid);
    }
    return node;
}

struct HomoSapiens *Homo_Head=NULL;
void linkLeafs(struct HomoSapiens*root){
    if(root==NULL)return;
    linkLeafs(root->lc);
    linkLeafs(root->rc);
    if(root->lc == NULL && root->rc ==NULL){
        if (Homo_Head == NULL){
            Homo_Head = root;
            return;
        }
        struct HomoSapiens* last = Homo_Head;
        while (last->next != NULL)last = last->next;
        last->next = root;
    }
}

void preorder_species_evolution(struct Species*root){
    if(root==NULL)return;
    Homo_Sapiens_root = insert_homosapiens(Homo_Sapiens_root, root->population_root, root->sid);
    preorder_species_evolution(root->lc);
    preorder_species_evolution(root->rc);
}

void print_homo_sapiensxx(struct Population* root){
    if(root==NULL)return;
    print_homo_sapiensxx(root->lc);
    printf("<%d,%d>",root->gid,root->sid);
    print_homo_sapiensxx(root->rc);
}

void help_print_homosapiens(struct HomoSapiens* root){
    if(root==NULL)return;
    help_print_homosapiens(root->lc);
    if(root->population_root!=NULL)printf("[");
    print_homo_sapiensxx(root->population_root);
    if(root->population_root!=NULL)printf("]");
    help_print_homosapiens(root->rc);
}

int evolution(){
    if(Species_root == NULL)return 0;
    preorder_species_evolution(Species_root);
    linkLeafs(Homo_Sapiens_root);
    printf("E\n\tHomo Sapiens:");
    help_print_homosapiens(Homo_Sapiens_root);
    printf("\nDONE\n");
    Species_root = NULL; /*efoson zhteitai na adeiazei to species*/
    return 1;
}

int species_statistics(){
    int num=0; /*den kanw peraiterw elegxo gia ton an uparxei ksana to sid, giati an uparxei 2 fores, den tha einai fullo kai tis 2*/
    struct HomoSapiens*tmp = Homo_Head;
    printf("N\n\tHomoSapiens: ");
    while(tmp!=NULL){
        printf("<%d> ",tmp->sid);
        num++;
        tmp = tmp->next;
    }
    printf("\n\tHomoSapiens species: <%d>\nDONE\n",num);
    return 1;
}

struct HomoSapiens* search_species2(struct HomoSapiens* root, int sid){
    if (root == NULL || root->sid == sid)return root;
    if (root->sid < sid)return search_species2(root->rc, sid);
    return search_species2(root->lc, sid);
}
int pop_num = 0;
void count_sapiens_populations(struct Population* root){
    if(root==NULL){
        return;
    }
    count_sapiens_populations(root->lc);
    pop_num++;
    count_sapiens_populations(root->rc);
}

int population_statistics(int sid){
    struct HomoSapiens* ekei = search_species2(Homo_Sapiens_root,sid);
    if(ekei==NULL)return 0; /*den to vrike*/
    count_sapiens_populations(ekei->population_root);
    printf("J<sid>\n\tHomo Sapiens populations: <%d>\nDONE\n",pop_num);
    pop_num=0;
    return 1;
}

int print_species(){
    printf("P\n\t");
    postorder(Species_root);
    printf("\nDONE\n");
    return 1;
}

int print_populations(){
    printf("X\n");
    merge_print(Species_root);
    printf("DONE\n");
    return 1;
}

int print_continents(){
    for(int i=0; i<5; i++){
        printf("Continents %d: ",i);
        dist4(continents[i]->population_root);
        printf("\n");
    }
    printf("DONE\n");
    return 1;
}

int print_homo_sapiens(){
    printf("W\n\t");
    help_print_homosapiens(Homo_Sapiens_root);
    printf("\nDONE\n");
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

				/* Lowest Common Ancestor
				 * L <sid> <gid1> <gid2> */
			 case 'L':
				{
					int sid, gid1, gid2;

					sscanf(buff, "%c %d %d %d", &event, &sid, &gid1, &gid2);
					DPRINT("%c %d %d %d\n", event, sid, gid1, gid2);

					if (lowest_common_ancestor(sid, gid1, gid2)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid, gid1, gid2);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid, gid1, gid2);
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
				 * F */
			case 'F':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c \n", event);

					if (delete_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
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
