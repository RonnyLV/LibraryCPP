#include <stddef.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>

#include <stdarg.h>

#include "Menu.cpp"

#define member_size(type, member) sizeof(((type *)0)->member)

/*
--------------
Ronalds Breikšs P2-4
--------------
PRAKSE - 5.praktiskais darbs (STRUKTURAS UN APVIENOJUMI)
--------------
Uzdevums:
        Izveidot dinamisko masivu no strukturam. Katra struktura
sastav no, minumums, 4 laukiem, dazhi no tiem var but doti.
Uz nodosanas bridi masiva jabut 5 ierakstiem.
        Jabut iespejai:
                1) Pievienot ierakstus
                2) Meklet pec pamatvienibas
                3) Dzest ierakstus
                4) Apskatit DB saturu
                5) Izpildit individualo uzdevumu
                6) Sakartot ierakstus pec pamatvienibas
------------------------------------------
INDIVIDUALAIS UZDEVUMS:
        3.variants
                Pamatvieniba: NOSAUKUMS
--------------
Uzdevums: Atrast visas kada autora gramatas (autoru ievada lietotajs)
--------------
*/
/**
*	Structure type definitions
*/
struct book_rec_type{ 							//  Struktura ar informaciju par gramatam
    unsigned short int 	id;
    char 			    title[20];
    char 			    author[15];
    char 			    publishing_house[20];
    unsigned short int  year;
};

unsigned short int	    g_count; 					// Ierakstu skaits
FILE			       *g_file;					// Fails
book_rec_type 	       *g_book_rec_pt;				// Norade uz strukturu ( datubazes veidosanai )
unsigned short int      g_max_id = 0;

bool                    C_DEBUG = false;
FILE                   *C_LOG_FILE = fopen("library.log", "a+");
/**
*	Function Header declarations
*/
int ShowAllRecords( void ); 						// Apskatam DB
int AddRec( void ); 						// Pievienoshanas funkcija
int DelRec( void ); 						// Dzeshanas funkcija
int FindRec( void ); 						// Atlasa pa 1
int individ ( void ); 						// Individualaa uzdevuma funkcija
int SaveDB( void ); 						// Saglabashanas funkcija

int ReadDB( void );

int printline(const char *format, ...);
int debug(const char *format, ...);
int ERR_MEM_ALLOC( void );
int askForConfirmation();
int removeRecord(int indexToRemove);
int searchElement( size_t offset, /* size_t memb_size,*/ int *target, const char* p_type );
/**
	Main function
*/
int main( int argc, char** argv ) {

    for (int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            C_DEBUG = true;
    }

    debug("START");

    g_file = fopen( "db.dat", "a+b" );
    fread( &g_count, sizeof( g_count ), 1, g_file); // Nolasa ierakstu daudzumu

    fread( &g_max_id, sizeof( g_max_id ), 1, g_file); // Nolasa maks. id

    debug("counter %hu", g_count);

    if(g_count > 0){
        debug("main calloc");
        g_book_rec_pt = ( struct book_rec_type * ) calloc(g_count, sizeof( struct book_rec_type ));

        if ( ( g_book_rec_pt == NULL )) {
            return ERR_MEM_ALLOC();
        }

    	{
    		int i; // Cikla iterators
    		
            debug("iterate records");

    		fread( g_book_rec_pt, sizeof(struct book_rec_type), g_count, g_file); // Tiek nolasiti ieraksti no faila
            debug("read in records",g_count);            
    	}
    }
    fclose(g_file);

    debug("Size of array %d", sizeof(*g_book_rec_pt));

	{
		Menu *MyMenu = new Menu;
		struct MenuLine MenuLines[] = {
			{"View All Records",&ShowAllRecords},
			{"Add a Record",&AddRec},
			{"Delete a Record",&DelRec},
			{"Find a Book By Title",&FindRec},
			{"Do something",&individ},
			{"Save Changes",&SaveDB}
		};
		if(MyMenu->setLines( MenuLines, sizeof( MenuLines ) / sizeof( struct MenuLine ) ) == 0)
			MyMenu->getMenu();
		else
			return EXIT_FAILURE;
	}    
    free( g_book_rec_pt );
	return EXIT_SUCCESS;
}

/* Datubazes ReadDB */
int ShowAllRecords( void ){
    ReadDB();
    getchar();
    return EXIT_SUCCESS;
}

int ReadDB( void ) {
    int i; /* Cikls */

    printf("ID");
    printf("\tNosaukums");
    printf("\tAutors");
    printf("\tIzdevnieciba ");
    printf("\tGads\n");
    printf("-------------------------------------------------------------------\n");
    for (i = 0; i < g_count; i++) { /* Skatamies DB saturu */
        printf("%hd", g_book_rec_pt[i].id);
        printf("\t%s", g_book_rec_pt[i].title);
        printf("\t%s", g_book_rec_pt[i].author);
        printf("\t%s", g_book_rec_pt[i].publishing_house);
        printf("\t%hd\n", g_book_rec_pt[i].year);
    }
    printf("-------------------------------------------------------------------\n");
    
    return EXIT_SUCCESS;
}

/* Funkcija ierakstu pievienosanai */
int AddRec(void) {    
    book_rec_type tmp_rec;

    system("clear");

    debug("adding records");
    
    printf("Nosaukums: ");
    scanf("%s",  tmp_rec.title);
    __fpurge(stdin);
    printf("Autors: ");
    scanf("%s",  tmp_rec.author);
    __fpurge(stdin);
    printf("Izdevnieciba: ");
    scanf("%s",  tmp_rec.publishing_house);
    __fpurge(stdin);
    printf("Gads: ");
    do 
    {       
        scanf("%hu", & tmp_rec.year);
        __fpurge(stdin);
    } while (tmp_rec.year < 1);    

    if (g_count == 0) { // Ja ir tuksha, tad vnk ieraksta
        debug("adding first record");
        debug("size of array", sizeof(*g_book_rec_pt));
        g_book_rec_pt = (book_rec_type *) (calloc(++g_count, sizeof(book_rec_type))); //Iedodam vietu pirmajam ierakstam
    } 
    else // Ja nav, tad sakarto alfabeta seciba
    {   
        g_book_rec_pt = ( struct book_rec_type * ) realloc(g_book_rec_pt, (++g_count) * sizeof( struct book_rec_type)); // iedalam papildus atmiņu jaunajkam ierakstam
        if ((g_book_rec_pt == NULL)) {
            return ERR_MEM_ALLOC();
        }           
    }    
    g_book_rec_pt[g_count-1] = tmp_rec;
    g_book_rec_pt[g_count-1].id = ++g_max_id;

    return EXIT_SUCCESS;
}

/* Funkcija ierakstu dzesanai */
int DelRec(void) {
    int i; /* Cikls */
    short * id; /* Dinamiskais masivs ar ID */
    short error; // Parbauda vai tika ievadits pareizs ID
    short tmp, tmp2;    
    int  inputId;

    system("clear");

    ReadDB();

    printline("Enter ID of book you want to delete:"); 
    scanf("%d",&inputId);
	__fpurge(stdin);
    {
        int my_rec, v_yes_no;;
        
        while(
            (my_rec = 
                searchElement(  offsetof( struct book_rec_type, id)
                                //, member_size( struct book_rec_type, id)
                                , (int*)&inputId, 
                                "USINT" )
            ) >= 0){
                printline("Are You sure You want to delete this record? [Y/n] ");
                v_yes_no = askForConfirmation();                
                if(v_yes_no == 1)
                    if(removeRecord(my_rec) == EXIT_SUCCESS)
                        printline("The record has been successfully removed");
        }
    }
    getchar();
    return EXIT_SUCCESS;
}
/* Atlasa pa vienam */
int FindRec(void) {
    int i; // Cikls   
    unsigned short v_found = 0;
    char inputName[15]; // Lietotaja inputs

    system("clear");

    ReadDB();

    printline("Ievadiet gramatas nosaukumu: ");
    scanf("%s",  inputName);
	__fpurge(stdin);        

    {
        int my_rec;
        while(
            (my_rec = 
                searchElement(  offsetof( struct book_rec_type, title)
                                //, member_size( struct book_rec_type, title)
                                , (int*)inputName, "CHAR" )
            ) >= 0){
                //DARI KKO AR IERAKSTU (:
        }
    }

    getchar();
    return EXIT_SUCCESS;
}

/* Individualais uzdevums - konkretai atrashanai */
int individ(void) {    
    char inputName[20]; // Izdevn Nosauk salidzinashanai

    system("clear");

    ReadDB();

    printline("Ievadiet gramatas autoru: ");    
    scanf("%s",  inputName);
	__fpurge(stdin);

    {
        int my_rec;
        while(
            (my_rec = 
                searchElement(  offsetof( struct book_rec_type, author)
                                //, member_size( struct book_rec_type, author)
                                , (int*)inputName, "CHAR" )
            ) >= 0){
                //DARI KKO AR IERAKSTU (:
        }
    }

    getchar();
    return EXIT_SUCCESS;
}

/* Funkcija datu saglabasanai */
int SaveDB(void) {
    int i; // Cikls    

    g_file = fopen("db.dat", "wb");
    fwrite( & g_count, sizeof(g_count), 1, g_file); /* Jauno elementu skaitu saglabajam faila */

    debug("G_COUNT: %d", g_count);

    fwrite( &g_max_id, sizeof(g_max_id), 1, g_file); /* Jauno elementu skaitu saglabajam faila */
    
    fwrite( & g_book_rec_pt[i], sizeof(book_rec_type), g_count, g_file);        
    
    fclose(g_file); /* Aizveram failu */

    printline("Tava DB tika veiksmigi saglabata!");

    getchar();
    return EXIT_SUCCESS;
}

int printline(const char *format, ...){
    va_list ap;
    int v_return;

    va_start(ap, format);
    v_return = vprintf(format, ap);
    printf("\n");
    va_end(ap);
    return v_return;
}

int debug(const char *format, ...){
    va_list ap;
    int v_return = EXIT_SUCCESS;
    if(C_DEBUG){
        va_start(ap,format);
        fprintf(C_LOG_FILE, "\n");
        v_return = vfprintf(C_LOG_FILE, format, ap);        
        vprintf(format, ap);
        va_end(ap);
    }

    return v_return;
}

int ERR_MEM_ALLOC( void ){
    char    ERR_MSG[] = "ERROR while allocating space!";

    printf("%s", ERR_MSG);
    getchar();
    return EXIT_FAILURE;
}

int askForConfirmation(){
    int v_return = 0;
    char v_yes_no;

    while(v_return == 0){
         scanf("%c", &v_yes_no);
        __fpurge(stdin);
        if(v_yes_no == 'Y' || v_yes_no == 'y')
            v_return = 1;
        else if(v_yes_no == 'N' || v_yes_no == 'n')
            v_return = -1;
        else
            printline("Please enter a valid input Y or N!");
    }
    return v_return;
}

int removeRecord(int indexToRemove)
{        
    memmove(&g_book_rec_pt[indexToRemove], &g_book_rec_pt[indexToRemove+1], (g_count - indexToRemove - 1) * sizeof(book_rec_type)); //Bīdam atmiņas blokus
    g_count--;
    g_book_rec_pt = (book_rec_type *) realloc(g_book_rec_pt,g_count* sizeof(struct book_rec_type));

    if ( ( g_book_rec_pt == NULL )) {
        return ERR_MEM_ALLOC();
    }

    return EXIT_SUCCESS;
}

int searchElement( size_t offset, /*size_t memb_size,*/ int *target, const char* p_type ){
    static int rec_nr = 0;
    static int v_found = 0;

    for (; rec_nr < g_count; rec_nr++) { // Tiek mekletas visas izdevniecibas
        if (
                (p_type == "CHAR" && strcmp(((char*)target), ((char*)((book_rec_type *)&g_book_rec_pt[rec_nr])+offset)) == 0)
                ||
                (p_type == "INT" && (((int)(*(int*)((book_rec_type *)&g_book_rec_pt[rec_nr])+offset)) == *target))
                ||
                (p_type == "USINT" && (((unsigned short int)(*(int*)((book_rec_type *)&g_book_rec_pt[rec_nr])+offset)) == *target))
            ) { 
            printline("ID: %hd\n", g_book_rec_pt[rec_nr].id);
            printline("Nosaukums: %s\n", g_book_rec_pt[rec_nr].title);
            printline("Autors: %s\n", g_book_rec_pt[rec_nr].author);
            printline("Izdevnieciba: %s\n", g_book_rec_pt[rec_nr].publishing_house);
            printline("Gads: %hd\n\n", g_book_rec_pt[rec_nr].year);
            printline("-----\n\n");
            v_found++;
            return rec_nr++;
        }
    }

    if(v_found == 0){
        printline("The record could not be found!");
    }
    else{
        if(v_found == 1)
            printline("One record totally found.");
        else
            printline("%d record totally found.", v_found);
        v_found = 0;
    }

    rec_nr = 0;
    
    return -1;
}