#include <stddef.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>

#include <time.h>

#include <stdarg.h>

#include "Menu.cpp"

enum DataType
{
    INT,
    FLOAT,
    STRING,
    UNSIGNED_SHORT_INT,
    CHAR,
};

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
struct book_rec_type{ 							// Struktura ar informaciju par gramatam
    unsigned short int 	id;                     // Ieraksta ID
    char 			    title[20];              // Nosaukums
    char 			    author[15];             // Autors
    char 			    publishing_house[20];   // Izdēvniecība
    unsigned short int  year;                   // Izlaiduma gads
};

unsigned short int	    g_count; 				// Ierakstu skaits
FILE			       *g_file;					// Fails
book_rec_type 	       *g_book_rec_pt;			// Pointeris uz masīvu no struktūŗas
unsigned short int      g_max_id = 0;           // ID sekvence

bool                    C_DEBUG = false;        // Debugot vai nē
/**
*	Function Header declarations
*/
int ShowAllRecords( void );                     // Apskatam DB
int AddRec( void );                             // Pievienoshanas funkcija
int DelRec( void );                             // Dzeshanas funkcija
int FindRec( void );                            // Atlasa pa 1
int individ ( void );                           // Individualaa uzdevuma funkcija
int SaveDB( void );                             // Saglabashanas funkcija

int ReadDB( void );                             // Masīva izvadīšana

int printline(const char *format, ...);         // Izvade ar newline beigās
int debug(const char *format, ...);             // Debugošanas procedūŗa
int ERR_MEM_ALLOC( void );                      // Procedūra atmiņas iedalīšanas kļūdas gadījumam
int askForConfirmation();                       // Darbības aptsiprināšanas procedūra
int removeRecord(int indexToRemove);            // Ieraksta dzēšana no masīva

                                                // Ieraksta meklēšana masīvā
int searchElement( size_t offset, void *target, DataType p_type );
/**
	Main function
*/
int main( int argc, char** argv ) {

    // Vai slēgt debugošanu
    for (int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            C_DEBUG = true;
    }

    debug("START");

    // Atveram failu papildināšanai (lai izveidotu failu, ja tas neeksistē) binārajā režīmā
    g_file = fopen( "db.dat", "a+b" );

    fread( &g_count, sizeof( g_count ), 1, g_file); // Nolasa ierakstu daudzumu

    fread( &g_max_id, sizeof( g_max_id ), 1, g_file); // Nolasa id sekvenci

    debug("Records count %hu", g_count);

    if(g_count > 0){
        debug("main calloc");

        // Iedalam atmiņu visiem ierakstiem, kurus satur fails
        g_book_rec_pt = ( struct book_rec_type * ) calloc(g_count, sizeof( struct book_rec_type ));

        if ( ( g_book_rec_pt == NULL )) {
            exit(ERR_MEM_ALLOC());
        }

        //Ielasam ierakstus masīvā
    	fread( g_book_rec_pt, sizeof(struct book_rec_type), g_count, g_file); // Tiek nolasiti ieraksti no faila
        debug("read in records",g_count);
    }
    fclose(g_file); // Aizveram failu

    debug("Size of array %d", sizeof(*g_book_rec_pt));

	{
        // Inicializējam galveno izvēlni izmantojo Menu.h
		Menu *MyMenu = new Menu;
		struct MenuLine MenuLines[] = {
			{"View All Records",&ShowAllRecords},
			{"Add a Record",&AddRec},
			{"Delete a Record",&DelRec},
			{"Find a Book By Title",&FindRec},
			{"Do something",&individ},
			{"Save Changes",&SaveDB}
		};
		if(MyMenu->setLines( MenuLines, sizeof( MenuLines ) / sizeof( struct MenuLine ) ) == 0){
			MyMenu->getMenu(); // Izvadam izvēlni
        }
		else
			return EXIT_FAILURE;
	}
    free( g_book_rec_pt ); //Atbrīvojam masīva atmiņu
	return EXIT_SUCCESS;
}

/* Datubazes ReadDB */
int ShowAllRecords( void ){
    ReadDB(); // Izvadam visus ierakstus
    getchar();
    return EXIT_SUCCESS;
}

int ReadDB( void ) {
    int i; // Cikla skaitītājs

    if(g_count == 0)
    {
        printline("Record list is empty!");
        return EXIT_FAILURE;
    }
    else
    {
        printf("ID");
        printf("\tTitle");
        printf("\tAuthor");
        printf("\tPublishing house");
        printf("\tYear\n");
        printf("-------------------------------------------------------------------\n");
        for (i = 0; i < g_count; i++) { // Izvadam ieraksta datus
            printf("%hd", g_book_rec_pt[i].id);
            printf("\t%s", g_book_rec_pt[i].title);
            printf("\t%s", g_book_rec_pt[i].author);
            printf("\t%s", g_book_rec_pt[i].publishing_house);
            printf("\t%hd\n", g_book_rec_pt[i].year);
        }
        printf("-------------------------------------------------------------------\n");

        return EXIT_SUCCESS;
    }
}

/* Funkcija ierakstu pievienosanai */
int AddRec(void) {
    book_rec_type tmp_rec;
    debug("adding records");

    // Aizpildam ieraksta datus
    printf("Title: ");
    scanf("%s",  tmp_rec.title);
    __fpurge(stdin);
    printf("Author: ");
    scanf("%s",  tmp_rec.author);
    __fpurge(stdin);
    printf("Publ. house: ");
    scanf("%s",  tmp_rec.publishing_house);
    __fpurge(stdin);
    printf("Year: ");
    {
        time_t theTime = time(NULL);
        struct tm *aTime = localtime(&theTime);

        int year = aTime->tm_year + 1900;
        do
        {
            scanf("%hu", & tmp_rec.year);
            __fpurge(stdin);
        } while ((tmp_rec.year < 1 || tmp_rec.year > year) && printf("Enter a valid year!") >= 0);
    }

    if (g_count == 0) {
        debug("adding first record");
        debug("size of array", sizeof(*g_book_rec_pt));
        g_book_rec_pt = (book_rec_type *) (calloc(++g_count, sizeof(book_rec_type))); //Iedodam vietu pirmajam ierakstam
    }
    else
    {
        // Iedalam vietu jaunajam elementam
        g_book_rec_pt = ( struct book_rec_type * ) realloc(g_book_rec_pt, (++g_count) * sizeof( struct book_rec_type)); // iedalam papildus atmiņu jaunajkam ierakstam
        if ((g_book_rec_pt == NULL)) {
            exit(ERR_MEM_ALLOC());
        }
    }
    g_book_rec_pt[g_count-1] = tmp_rec; // piesaistam jauno ierakstu masīvam
    g_book_rec_pt[g_count-1].id = ++g_max_id; // Inkrementējam ID sekvenci un piesaistam id laukam

    return EXIT_SUCCESS;
}

/* Funkcija ierakstu dzesanai */
int DelRec(void) {
    unsigned short int  inputId; // Lietotāja inputs

    if(ReadDB() == EXIT_SUCCESS){ // Izvadam visus ierakstus
        printline("Enter ID of book you want to delete:");
        scanf("%hu",&inputId);
    	__fpurge(stdin);
        {
            int my_rec, v_yes_no;;

            while(
                //Ejam cauri visiem atrastajiem ierakstiem (bet vajadzētu būt tikai vienam)
                (my_rec =
                    searchElement(  offsetof( struct book_rec_type, id)
                                    //, member_size( struct book_rec_type, id)
                                    , (int*)&inputId,
                                    UNSIGNED_SHORT_INT )
                ) >= 0){
                    // Prasam vai dzēst ierakstu
                    v_yes_no = askForConfirmation();
                    if(v_yes_no == 1)
                        //Dzēšam ierakstu
                        if(removeRecord(my_rec) == EXIT_SUCCESS)
                            printline("The record has been successfully removed");
            }
        }
    }

    getchar();
    return EXIT_SUCCESS;
}
/* Atlasa pa vienam */
int FindRec(void) {
    char inputName[15]; // Lietotaja inputs

    if(ReadDB() == EXIT_SUCCESS){ // Izvadam visus ierakstus
        printline("Input book's title: ");
        scanf("%s",  inputName);
    	__fpurge(stdin);

        {
            int my_rec;
            while(
                // Ejam cauri visiem atrastajiem ierakstiem
                (my_rec =
                    searchElement(  offsetof( struct book_rec_type, title)
                                    //, member_size( struct book_rec_type, title)
                                    , (int*)inputName, STRING )
                ) >= 0){
                    //DARI KKO AR IERAKSTU (:
            }
        }
    }
    getchar();
    return EXIT_SUCCESS;
}

/**
    Individualais uzdevums - konkretai atrashanai pēc autora
*/
int individ(void) {
    char inputName[20]; //Salīdzināmā vērtīBa

    if(ReadDB() == EXIT_SUCCESS){ // Izvadam visus ierakstus
        printline("Input book's author: ");
        scanf("%s",  inputName);
    	__fpurge(stdin);

        {
            int my_rec;
            while(
                // Meklējam ierakstus, kamēr neatgriež negatīvu skaitli
                (my_rec =
                    searchElement(  offsetof( struct book_rec_type, author)
                                    //, member_size( struct book_rec_type, author)
                                    , (int*)inputName, STRING )
                ) >= 0){
                    //DARI KKO AR IERAKSTU (:
            }
        }
    }
    getchar();
    return EXIT_SUCCESS;
}

/* Funkcija datu saglabasanai */
int SaveDB(void) {

    g_file = fopen("db.dat", "wb"); // Atveram failu rakstīšanai binārajā režīmā
    fwrite( & g_count, sizeof(g_count), 1, g_file); // Jauno elementu skaitu saglabajam faila
    debug("G_COUNT: %d", g_count);

    fwrite( &g_max_id, sizeof(g_max_id), 1, g_file); // Jauno elementu skaitu saglabajam faila
    debug("G_MAX_ID: %d", g_max_id);

    fwrite( g_book_rec_pt, sizeof(book_rec_type), g_count, g_file); // Ierakstam visus ierakstus no masīva failā

    fclose(g_file); // Aizveram failu

    printline("Database has been successfully saved.");

    getchar();
    return EXIT_SUCCESS;
}

/**
    Procedūra izvadei ar newline galā
*/
int printline(const char *format, ...){
    va_list ap;
    int v_return = EXIT_SUCCESS;

    va_start(ap, format);
    v_return = vprintf(format, ap);
    printf("\n");
    va_end(ap);
    return v_return;
}

/**
    Programma debuggošanas procedūra

    Lai strādātu, jāpadod -v --verbose parametrs palaišanas brīdī
    ./library -v [--verbose]
*/
int debug(const char *format, ...){
    va_list ap;
    FILE    *C_LOG_FILE = fopen("library.log", "a+"); // Debugošanas fails

    int v_return = EXIT_SUCCESS;

    if(C_DEBUG){
        va_start(ap,format);
        fprintf(C_LOG_FILE, "\n");
        v_return = vfprintf(C_LOG_FILE, format, ap);
        va_end(ap);
    }

    return v_return;
}

/**
    Procedūre Atmiņas iedalīšanas kļūdas gadījumā
*/
int ERR_MEM_ALLOC( void ){
    char    ERR_MSG[] = "ERROR while allocating space!";

    printf("%s", ERR_MSG);
    getchar();
    return EXIT_FAILURE;
}

/**
    Pieprasam apstiprinājumu
    Atgriež 1, ja Y vai y, atgriež -1, ja N vai n
*/
int askForConfirmation( void ){
    int v_return = EXIT_SUCCESS;
    char v_yes_no;

    printline("Are You sure You want to delete this record? [Y/n] ");

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
    memmove(&g_book_rec_pt[indexToRemove], &g_book_rec_pt[indexToRemove+1], (g_count - indexToRemove - 1) * sizeof(book_rec_type)); //Bīdam atmiņas blokus, lai "dzēstu"
    g_count--; //Globālo ierakstu skaitītāju dekrementējam
    //g_book_rec_pt = (book_rec_type *) realloc(g_book_rec_pt,g_count* sizeof(struct book_rec_type));

    if ( ( g_book_rec_pt == NULL )) {
        exit(ERR_MEM_ALLOC());
    }

    return EXIT_SUCCESS;
}

/**
    Atgriežam -1, ja funkcija ir beigusi savu darbību
*/
int searchElement( size_t offset, /*size_t memb_size,*/ void *target, DataType p_type ){
    static int rec_nr = 0; //Elementa indexa skaitītājs
    static int v_found = 0; //Atrasto ierakstu skaitītājs

    for (; rec_nr < g_count; rec_nr++) { // Tiek mekletas visas izdevniecibas
        //Veicam vērtību salīdzīnāšanu izmantojot pointeru aritmētiku un tipu kāstošanu
        /**
            Padotā parametra konstante:
                "CHAR" nozīmē, ka pārbaudāmais elements tiks kāstots kā pointeris uz "char" masīvu
                "INT" nozīmē, ka pārbaudāmais elements tiks kāstots kā "int"
                "USINT" nozīmē, ka pārbaudāmais elements tiks kāstots kā "unsigned short int"
        */
        if (
                (p_type == STRING && strcmp(((char*)target), ((char*)((book_rec_type *)&g_book_rec_pt[rec_nr])+offset)) == 0)
                ||
                (p_type == INT && (((int)(*(int*)((book_rec_type *)&g_book_rec_pt[rec_nr])+offset)) == *(int*)target))
                ||
                (p_type == UNSIGNED_SHORT_INT && (((unsigned short int)(*(int*)((book_rec_type *)&g_book_rec_pt[rec_nr])+offset)) == *(unsigned short int *)target))
            ) {
            printline("ID: %hd\n", g_book_rec_pt[rec_nr].id);
            printline("Title: %s\n", g_book_rec_pt[rec_nr].title);
            printline("Author: %s\n", g_book_rec_pt[rec_nr].author);
            printline("publishing House: %s\n", g_book_rec_pt[rec_nr].publishing_house);
            printline("Year: %hd\n\n", g_book_rec_pt[rec_nr].year);
            printline("-----\n\n");
            v_found++;
            return rec_nr++;
        }
    }
    //Neviens ieraksts netika atrasts
    if(v_found == 0){
        printline("The record could not be found!");
    }
    //Atrasts tika vismaz 1 ieraksts
    else
    {
        if(v_found == 1) //Ja atrasts tikai viens ieraksts
            printline("One record totally found.");
        else //Atrasti vairāki ieraksti
            printline("%d record totally found.", v_found);

        v_found = 0; //Resetot ierakstu skaitītāju
    }

    rec_nr = 0; //Resetot ierakstu skaitītāju

    return -1;
}
